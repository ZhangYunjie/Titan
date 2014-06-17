#include <base/bisqueBase.h>
#include <base/data/BQ159/BQ159Util.h>
#include <base/data/BQ159/BQ_md159.h>
#include <base/data/BQUUID.h>
#include <base/data/BQF/BQF.h>
#include <base/platform/BQLock.h>
#include <base/platform/helper/BQ_independence.h>
#include <base/util/BQSensitive.h>

#include "vfs_drv_hm40.h"
#include "vfs_drv_hm40_private.h"
using namespace bisqueBase;
using namespace bisqueBase::Data;


#ifndef HMDB_VFS_HM40_BUFFERSZ
#   define HMDB_VFS_HM40_BUFFERSZ 8192
#endif

#define MAX_MATRIX_LOOP (2048*1024)

// これはグローバルで持つしかないのだ (SQLite の仕様)
static volatile bool g_IkemenIsRegisterd = false;
static BQ159::BisqueKey* g_Ikemen40Key = NULL;


typedef struct hm40_context hm40_context;
struct hm40_context {
    sqlite3_file base;
    IO::Stream* fd;
    int nWriteOffet;
    
    char *aBuffer;
    int nBuffer;
    sqlite3_int64 iBufferOfst;
    util::VariableBuffer* VB;
    BQ159::BisqueKey* bqKey;
    hmdb_ikmnvfs_header_t header;
};

// fill
BQ_C_INLINE bq_byte*
VFS_HM40_Fill(bq_byte *pData,
              bq_uint length)
{
    BQ_UUID* rand = BQ_UUID::self();
    rand->seedInternal();
    
 	bq_byte *p = pData;
	bq_uint i = 0;
  	for (i = 0; i < length; i++ ) {
		*p = (bq_byte)(rand->genSysInt32() % 0xff);
		p++;
	}
	   
	return pData;
}

// エンコード
BQ_C_INLINE bq_byte*
VFS_HM40_EncodeMatrix(
                const hmdb_ikmnvfs_matrix_t* _Mat,
                bq_byte *pData,
                bq_uint length)
{
	bq_byte *p = pData;
	
	bq_uint i = 0;
	for (i = 0; i < length; i++ ) {
		*p = _Mat->primary[*p];
		p++;
	}
    
	return pData;
}

// デコード
BQ_C_INLINE bq_byte*
VFS_HM40_DecodeMatrix(
                const hmdb_ikmnvfs_matrix_t* _Mat,
                bq_byte *pData,
                bq_uint length)
{
 	bq_byte *p = pData;
	bq_uint i = 0;
  	for (i = 0; i < length; i++ ) {
		*p = _Mat->secondary[*p];
		p++;
	}
	return pData;
}


BQ_C_INLINE BQ_RESULT
VFS_HM40_CreateMatrix(hmdb_ikmnvfs_matrix_t *_Mat, bool _UseDefault)
{
	int current = 1;
	bool bNext = false;
    BQ_UUID* rand = BQ_UUID::self();
    rand->seedInternal();
    
    _Mat->paunter = (bq_byte*)BQ_sensitive_alloc(BQ_ALIGN(HMDB_MOD_IKMNVFS_KEY_SIZ_PK, 32));
	memset(_Mat->paunter, 0x00, HMDB_MOD_IKMNVFS_KEY_SIZ_PK);
    _Mat->primary = _Mat->paunter;
    _Mat->secondary = &_Mat->paunter[HMDB_MOD_IKMNVFS_KEY_SIZ];

    rand->seed();
    _Mat->primary[0] = 0x00;
    bq_uint loops = 0;
    

	// プライマリの生成
	while(1) {
        bq_byte g = (bq_byte)(rand->genSysInt32() % 256);
		_Mat->primary[current] = g;
        
        if ((loops++ > MAX_MATRIX_LOOP) || (true == _UseDefault)) {
            memcpy(_Mat->primary, HM40_INITIAL_VECTOR_P, BQ_CONST_ARRAY_SIZE(HM40_INITIAL_VECTOR_P));
            break;
        }
        
		bNext = true;
		// 重複チェック
		for (int i = 0; i < current; i++) {
			// 重複しやがった
			if( _Mat->primary[i] == _Mat->primary[current] ) {
				bNext = false;
				break;
			}
		}
        
		// 次へ進んで良いか
		if (true == bNext) {
			current++;
			// 最後まで作った
			if( current > 255 ) {
				break;
			}
		}
	}
    
	// 逆リスト作成
	for (int i = 0; i <= 255; i++) {
		_Mat->secondary[ _Mat->primary[i] ] = i;
	}
    
	// 検証
	bq_byte check[256];
	memset( check, 0xBB, 256 );
	for (int i = 0; i <= 255; i++) {
        check[i] = i;
	}
	// エンコードする
	VFS_HM40_EncodeMatrix(_Mat, check, BQ_CONST_ARRAY_SIZE(check));
	// デコードしる
	VFS_HM40_DecodeMatrix(_Mat, check, BQ_CONST_ARRAY_SIZE(check));
    
	BQ_INDEPENDENCE_LOG_TRACE("VFS_HM40_CreateMatrix", "---- Mat  -----");
    
	// チェック
	for (int i = 0; i <= 255; i++) {
		if (i != check[i]) {
			BQ_INDEPENDENCE_LOG_TRACE("VFS_HM40_CreateMatrix", "failed T=%02X F=%02X\n", i, check[i]);
            return BQ_E_FAIL;
		}
	}
	BQ_INDEPENDENCE_LOG_TRACE("VFS_HM40_CreateMatrix", "---- done -----" );
    return BQ_S_OK;
}

static int
VFS_HM40_write(
            hm40_context *c,
            const void *zBuf,
            int iAmt)
{
   
    c->VB->clear(false);
    c->VB->append(zBuf, iAmt);
    
    bq_byte *addr = VFS_HM40_EncodeMatrix(&c->header.matrix, (bq_byte*)c->VB->get(), iAmt);
    
    int wc = c->fd->write(addr, iAmt);
    // TODO: WC のチェックを入れる
    return wc;
}

static int
VFS_HM40_read(
        hm40_context *c,
        void *zBuf,
        int iAmt)
{
    c->VB->clear(false);
    c->VB->expand(iAmt);
    
    int rc = c->fd->read(c->VB->get(), iAmt);
    bq_byte *addr = VFS_HM40_DecodeMatrix(&c->header.matrix, (bq_byte*)c->VB->get(), rc);
    
    memcpy(zBuf, addr, rc);
    return rc;
}


static int
VFS_HM40_WriteFile(
                hm40_context *p,                /* イケメソコンテキスト */
                const void *zBuf,               /* イケメソバッファー */
                int iAmt,                       /* イケメソバッファーサイズ */
                sqlite_int64 iOfst              /* オフセット(ブロック固定に使えそうで微妙に使えない) */
)
{
    size_t nWrite;
    
    bq_int64 ofst;
    
    iOfst += p->nWriteOffet;
    ofst = p->fd->seek(iOfst, IO::BQ_SEEK_SET);
    if (ofst != iOfst) {
        return SQLITE_IOERR_WRITE;
    }
    
    nWrite = VFS_HM40_write(p, zBuf, iAmt);
    if (nWrite != iAmt) {
        return SQLITE_IOERR_WRITE;
    }
    
    return SQLITE_OK;
}


static int
VFS_HM40_FlushBuffer(hm40_context *p)
{
    int rc = SQLITE_OK;
    if (p->nBuffer) {
        rc = VFS_HM40_WriteFile(p, p->aBuffer, p->nBuffer, p->iBufferOfst);
        p->nBuffer = 0;
    }
    return rc;
}

static int
VFS_HM40_Close(sqlite3_file *pFile)
{
    int rc = 0;
    hm40_context *p = (hm40_context*)pFile;
    
    rc = VFS_HM40_FlushBuffer(p);
    sqlite3_free(p->aBuffer);
    BQCloseStream(&p->fd);
    BQ_DESTRUCT(p->VB);
    BQ_DESTRUCT(p->bqKey);
    BQ_SAFE_FREE(p->header.matrix.paunter);
    return rc;
}


static int
VFS_HM40_Read(
            sqlite3_file *pFile,  /* イケメソコンテキスト */
            void *zBuf,             /* イケメソバッファー */
            int iAmt,               /* イケメソバッファーサイズ */
            sqlite_int64 iOfst      /* オフセット(ブロック固定に使えそうで微妙に使えない) */
            )
{
    hm40_context *p = (hm40_context*)pFile;
    off_t ofst;
    int nRead;
    int rc;
    
    IO::Stream* fd = p->fd;
    
    rc = VFS_HM40_FlushBuffer(p);
    if (SQLITE_OK != rc){
        return rc;
    }
    
    iOfst += p->nWriteOffet;
    ofst = fd->seek(iOfst, IO::BQ_SEEK_SET);
    if (ofst != iOfst ) {
        return SQLITE_IOERR_READ;
    }
    
    nRead = VFS_HM40_read(p, zBuf, iAmt);
    if (nRead == iAmt) {
        return SQLITE_OK;
    }
    else if (nRead >= 0) {
        return SQLITE_IOERR_SHORT_READ;
    }
    
    return SQLITE_IOERR_READ;
}

static int
VFS_HM40_Write(
            sqlite3_file *pFile,    /* イケメソコンテキスト */
            const void *zBuf,       /* イケメソバッファー */
            int iAmt,               /* イケメソバッファーサイズ */
            sqlite_int64 iOfst      /* オフセット(ブロック固定に使えそうで微妙に使えない) */
            )
{
    hm40_context *p = (hm40_context*)pFile;
    
    if (NULL == p->aBuffer){
        return VFS_HM40_WriteFile(p, zBuf, iAmt, iOfst);
    }
    
    char *z = (char *)zBuf;
    int n = iAmt;
    sqlite3_int64 i = iOfst;
    
    while (n > 0) {
        int nCopy;
        if ((HMDB_VFS_HM40_BUFFERSZ == p->nBuffer) ||
            (i != p->iBufferOfst+p->nBuffer)) {
            int rc = VFS_HM40_FlushBuffer(p);
            if (SQLITE_OK != rc){
                return rc;
            }
        }
        BQ_INDEPENDENCE_CENSOR(0 == p->nBuffer || p->iBufferOfst+p->nBuffer == i);
        p->iBufferOfst = i - p->nBuffer;
        
        /* Copy as much data as possible into the buffer. */
        nCopy = HMDB_VFS_HM40_BUFFERSZ - p->nBuffer;
        if (nCopy > n){
            nCopy = n;
        }
        memcpy(&p->aBuffer[p->nBuffer], z, nCopy);
        p->nBuffer += nCopy;
        
        n -= nCopy;
        i += nCopy;
        z += nCopy;
    }

    return SQLITE_OK;
}


// やらない
static int
VFS_HM40_Truncate(sqlite3_file *pFile, sqlite_int64 size){
    return SQLITE_OK;
}


static int
VFS_HM40_Sync(sqlite3_file *pFile, int flags){
    hm40_context *p = (hm40_context*)pFile;
    int rc;
    
    rc = VFS_HM40_FlushBuffer(p);
    if (SQLITE_OK != rc){
        return rc;
    }
    // TODO: sync 入れた方が良さげ
    
    return SQLITE_OK;
}

static int
VFS_HM40_FileSize(sqlite3_file *pFile, sqlite_int64 *pSize) {
    hm40_context *p = (hm40_context*)pFile;
    int rc;                         /* Return code from fstat() call */   
    IO::Stream* fd = p->fd;
    
    rc = VFS_HM40_FlushBuffer(p);
    if (SQLITE_OK != rc){
        return rc;
    }
    
    *pSize = fd->getLength() - p->nWriteOffet;
    
    return SQLITE_OK;
}

// シングルプロセス前提なので lock はやらない
static int
VFS_HM40_Lock(sqlite3_file *pFile, int eLock){
    return SQLITE_OK;
}
// シングルプロセス前提なので unlock はやならい
static int
VFS_HM40_Unlock(sqlite3_file *pFile, int eLock){
    return SQLITE_OK;
}
// シングルプ(ry
static int
VFS_HM40_CheckReservedLock(sqlite3_file *pFile, int *pResOut){
    *pResOut = 0;
    return SQLITE_OK;
}

// やらない
static int
VFS_HM40_FileControl(sqlite3_file *pFile, int op, void *pArg){
    return SQLITE_OK;
}

// 128bit 固定(NTY Burst みたいなのを入れるときは可変にするが、SQLite側の都合を考えると微妙)
static int
VFS_HM40_SectorSize(sqlite3_file *pFile){
    return 16;
}

// いらない
static int
VFS_HM40_DeviceCharacteristics(sqlite3_file *pFile){
    return 0;
}

// オーポン
static int VFS_HM40_Open(
                    sqlite3_vfs *pVfs,              /* VFS */
                    const char *zName,              /* NULL ならトランザクションらしい */
                    sqlite3_file *pFile,            /* 親イケメンコンテキスト */
                    int flags,                      /* SQLITE_OPEN_XXX の引数 */
                    int *pOutFlags                  /* flags を投げ返せばいいらしい */
                    )
{
    // sqlite3_io_methods を埋める
    static const sqlite3_io_methods kHm40_mtds = {
        1,                            /* iVersion */
        VFS_HM40_Close,                    /* xClose */
        VFS_HM40_Read,                     /* xRead */
        VFS_HM40_Write,                    /* xWrite */
        VFS_HM40_Truncate,                 /* xTruncate */
        VFS_HM40_Sync,                     /* xSync */
        VFS_HM40_FileSize,                 /* xFileSize */
        VFS_HM40_Lock,                     /* xLock */
        VFS_HM40_Unlock,                   /* xUnlock */
        VFS_HM40_CheckReservedLock,        /* xCheckReservedLock */
        VFS_HM40_FileControl,              /* xFileControl */
        VFS_HM40_SectorSize,               /* xSectorSize */
        VFS_HM40_DeviceCharacteristics     /* xDeviceCharacteristics */
    };
    
    hm40_context *p = (hm40_context*)pFile;
    int oflags = 0;
    char *aBuf = 0;
    
    if (BQ_FAILED(HK_is_usable_hm40())) {
        BQ_INDEPENDENCE_LOG_FATAL("VFS_HM40_Open", "module IS NOT INITIALIZED!!");
        return SQLITE_IOERR;
    }
    
    if (0 == zName) {
        return SQLITE_IOERR;
    }
    
    BQ_INDEPENDENCE_LOG_TRACE("VFS_HM40_Open", "open@D <%s>", zName);
    
    if (flags & SQLITE_OPEN_MAIN_JOURNAL ){
        aBuf = (char *)sqlite3_malloc(HMDB_VFS_HM40_BUFFERSZ);
        if (!aBuf){
            return SQLITE_NOMEM;
        }
    }
    else {
        BQ_INDEPENDENCE_LOG_TRACE("VFS_HM40_Open", "open@J <%s>", zName);
    }
    
    if (flags & SQLITE_OPEN_EXCLUSIVE) {
        oflags |= BQ_IO_UND_TRUNCATE;
    }
    if (flags & SQLITE_OPEN_CREATE) {
        oflags |= BQ_IO_UND_CREATE;
    }
    if (flags & SQLITE_OPEN_READONLY) {
        oflags |= BQ_IO_UND_READ;
    }
    if (flags & SQLITE_OPEN_READWRITE) {
        if (!IO::Directory::exists(zName) || flags & SQLITE_OPEN_MAIN_JOURNAL) {
            oflags = BQ_IO_READWRITE;
        }
        else {
            oflags = BQ_IO_EXIST_READWRITE;
        }
    }
    
    memset(p, 0, sizeof(hm40_context));
    p->VB = new util::VariableBuffer();
    p->nWriteOffet = HMDB_MOD_IKMNVFS_HEADER_SIZE;
    p->fd = IO::File::open(zName, (IO::FILE_MODE)oflags);

    p->bqKey = new BQ159::BisqueKey(g_Ikemen40Key->get());
    
    if (NULL == p->fd){
        sqlite3_free(aBuf);
        return SQLITE_CANTOPEN;
    }
    
    if (oflags & BQ_IO_UND_CREATE) {
        memset(&p->header, 0x00, sizeof(hmdb_ikmnvfs_header_t));
        hmdb_ikmnvfs_header_t *hd = &p->header;
        bq_uint32* d = (bq_uint32*)hd->fourcc;
        d[0] = HMDB_MOD_IKMNVFS_FOURCC_M1;
        d[1] = HMDB_MOD_IKMNVFS_FOURCC_M2;
        d[2] = HMDB_MOD_IKMNVFS_FOURCC_M3;
        d[3] = HMDB_MOD_IKMNVFS_FOURCC_M4;
        
        if (BQ_FAILED(VFS_HM40_CreateMatrix(&hd->matrix, (flags & SQLITE_OPEN_MAIN_JOURNAL) ? true : false))) {
            return SQLITE_CANTOPEN;
        }
        // ここ BQF ヘッダーに変える
        util::VariableBuffer vb;
        vb.expand(BQ_ALIGN(HMDB_MOD_IKMNVFS_KEY_SIZ*2, 32));
        BQ_MD159_encrypt_ex(p->bqKey->get(), (const bq_byte*)hd->matrix.paunter, vb.plunder<bq_byte*>(), HMDB_MOD_IKMNVFS_KEY_SIZ_PK);
        p->fd->write(hd->fourcc, 16);
        p->fd->write(vb.plunder<bq_byte*>(), HMDB_MOD_IKMNVFS_KEY_SIZ_PK);
        
    }
    else {
        hmdb_ikmnvfs_header_t *hd = &p->header;
        hd->matrix.paunter = (bq_byte*)malloc(BQ_ALIGN(HMDB_MOD_IKMNVFS_KEY_SIZ_PK, 32));
        // ここ BQF ヘッダーに変える
        p->fd->read(hd->fourcc, 16);
        util::VariableBuffer vb;
        vb.expand(BQ_ALIGN(HMDB_MOD_IKMNVFS_KEY_SIZ_PK, 32));
        p->fd->read(vb.plunder<bq_byte*>(), HMDB_MOD_IKMNVFS_KEY_SIZ_PK);
        BQ_MD159_decrypt_ex(p->bqKey->get(), vb.plunder<const bq_byte*>(), (bq_byte*)hd->matrix.paunter, HMDB_MOD_IKMNVFS_KEY_SIZ_PK);
        
        hd->matrix.primary = hd->matrix.paunter;
        hd->matrix.secondary = hd->matrix.paunter + HMDB_MOD_IKMNVFS_KEY_SIZ;
    }
    
    p->fd->seek(p->nWriteOffet, IO::BQ_SEEK_SET);
    p->aBuffer = aBuf;
    
    if (pOutFlags){
        *pOutFlags = flags;
    }
    p->base.pMethods = &kHm40_mtds;
    return SQLITE_OK;
}


// ぢゃーなるを消す
static int
VFS_HM40_Delete(sqlite3_vfs *pVfs, const char *zPath, int dirSync)
{
    IO::Directory::remove(zPath);
    return SQLITE_OK;// : SQLITE_IOERR_DELETE;
}


// 要は、有るか無いか
static int
VFS_HM40_Access(
              sqlite3_vfs *pVfs,
              const char *zPath,
              int flags,
              int *pResOut
              )
{
    
    *pResOut = (true == IO::Directory::exists(zPath));
    return SQLITE_OK;
}

// フルパスを返せばいいらしい(.journalファイルにも使われる)
static int
VFS_HM40_FullPathname(
                    sqlite3_vfs *pVfs,
                    const char *zPath,
                    int nPathOut,
                    char *zPathOut
                    )
{

    // TODO: オンボロイド用にパス調整しよう
    IO::Path::lookupPathName(zPathOut, zPath, nPathOut);
    zPathOut[nPathOut-1] = '\0';
    return SQLITE_OK;
}

// ----------------------------------------------------------------------------
// だいなみっくろーど関連(スマホでは使わない)
//
static void *
VFS_HM40_DlOpen(sqlite3_vfs *pVfs, const char *zPath)
{
    return 0;
}

static void
VFS_HM40_DlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg)
{
    sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
    zErrMsg[nByte-1] = '\0';
}

static void
(*VFS_HM40_DlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void)
{
    return 0;
}

static void
VFS_HM40_DlClose(sqlite3_vfs *pVfs, void *pHandle)
{
    return;
}
// ----------------------------------------------------------------------------

// ランダム fill(重いのでやらない)
static int
VFS_HM40_Randomness(sqlite3_vfs *pVfs, int nByte, char *zByte)
{
    VFS_HM40_Fill((bq_byte*)zByte, nByte);
    return SQLITE_OK;
}

// sleep
static int
VFS_HM40_Sleep(sqlite3_vfs *pVfs, int nMicro)
{
    BQ_concurrent_milliwait(nMicro * 1000);
    return nMicro;
}

// 2038年問題がある気がするが、SQLite の仕様なので doubleで返す
static int
VFS_HM40_CurrentTime(sqlite3_vfs *pVfs, double *pTime)
{
    time_t t = BQ_time_now();
    *pTime = t/86400.0 + 2440587.5; 
    return SQLITE_OK;
}

// VFSハンドル
sqlite3_vfs*
sqlite3_vfs_hm40(void)
{

    static sqlite3_vfs mtz = {
        1,                            /* iVersion */
        sizeof(hm40_context),             /* szOsFile */
        BQ_MAX_PATH_NAME,                  /* mxPathname */
        0,                            /* pNext */
        HK_IKMNVFS_NAME,        /* zName */
        0,                            /* pAppData */
        VFS_HM40_Open,                     /* xOpen */
        VFS_HM40_Delete,                   /* xDelete */
        VFS_HM40_Access,                   /* xAccess */
        VFS_HM40_FullPathname,             /* xFullPathname */
        VFS_HM40_DlOpen,                   /* xDlOpen */
        VFS_HM40_DlError,                  /* xDlError */
        VFS_HM40_DlSym,                    /* xDlSym */
        VFS_HM40_DlClose,                  /* xDlClose */
        VFS_HM40_Randomness,               /* xRandomness */
        VFS_HM40_Sleep,                    /* xSleep */
        VFS_HM40_CurrentTime,              /* xCurrentTime */
    };
    return &mtz;
}

void
HK_register_vfs_hm40(void)
{
    if (g_IkemenIsRegisterd) {
        return;
    }
    g_IkemenIsRegisterd = true;
    sqlite3_vfs_register(sqlite3_vfs_hm40(), 1);
}

void
HK_unregister_vfs_hm40(void)
{
    if (!g_IkemenIsRegisterd) {
        return;
    }
    g_IkemenIsRegisterd = false;
    sqlite3_vfs_unregister(sqlite3_vfs_hm40());
}

//
// モジウル外関数(HMDB から直呼び出し想定)
//

BQ_RESULT
HK_register_hm40_key(bisqueBase::Data::BQ159::BisqueKey* _Key)
{
    BQ_DESTRUCT(g_Ikemen40Key);
    if (NULL == _Key) {
        return BQ_S_OK;
    }
    
    g_Ikemen40Key = new BQ159::BisqueKey(_Key->get());
    if(true != g_Ikemen40Key->validate()) {
        BQ_DESTRUCT(g_Ikemen40Key);
        return BQ_E_FAIL;
    }
    return BQ_S_OK;
}

BQ_RESULT
HK_is_usable_hm40(void)
{
    if (true != g_IkemenIsRegisterd) {
        return BQ_E_FAIL;
    }
    
    if ((NULL == g_Ikemen40Key) ||
        (true != g_Ikemen40Key->validate())) {
        return BQ_E_FAIL;
    }
    
    return BQ_S_OK;
}

bq_uint
HK_detect_hm40_header(SovietString _Path)
{
    if (!IO::Directory::exists(_Path)) {
        return 0;
    }
    IO::Stream* f = IO::File::open(_Path, IO::BQ_FILE_RO);
    if (NULL == f) {
        return 0;
    }
    
    // ここは BQF ヘッダーに変える
    
    bq_uint mark[2];
    if (sizeof(mark) != f->read(&mark, sizeof(mark))) {
        BQCloseStream(&f);
        return 0;
    }
    BQCloseStream(&f);
    
    if (HMDB_MOD_IKMNVFS_FOURCC_M1 != mark[0]) {
        return 0;
    }
    return mark[1];
}

