#ifndef _VFS_DRV_HM40_H_8C698ACA_2F62_4EF4_F959_DDA02A92A89F_
#define _VFS_DRV_HM40_H_8C698ACA_2F62_4EF4_F959_DDA02A92A89F_

#include <base/bisqueBase.h>
#include <base/data/BQ159/BQ159Util.h>
#include <sqlite3.h>

//
// VFS_HM40 ドライバー
// イケメンDB (HMDB) に組み込むことを想定、難読化アルゴリズムはアクセス頻度が激しいので
// ステート操作可能なストリーム暗号であるところの BQ40 を使用
//


BQ_DECLARE_BEGIN

/** 
 VFS キー
 */
#define HK_IKMNVFS_NAME "HM40v1"

/**
 VFS ファクトリー
 */
static sqlite3_vfs* sqlite3_vfs_hm40(void);

/**
 SQLite から呼ばれるモジウル登録関数
 */
BQ_EXTERN void HK_register_vfs_hm40(void);

/**
 SQLite から呼ばれるモジウル解除関数
 */
BQ_EXTERN void HK_unregister_vfs_hm40(void);

/**
 キー設定
 @param[in] キー
 @return 成功したら BQ_S_OK
 */
BQ_EXTERN BQ_RESULT HK_register_hm40_key(bisqueBase::Data::BQ159::BisqueKey *_Key);

/**
 使える？
 @return 使えるなら BQ_S_OK
 */
BQ_EXTERN BQ_RESULT HK_is_usable_hm40(void);

#define HMDB_MOD_IKMNVFS_FOURCC_M1 BQ_PACK32(0x4e, 0x4d, 0x4b, 0x49)
#define HMDB_MOD_IKMNVFS_FOURCC_M2 BQ_PACK32(0x00, 0x00, 0x00, 0x01)
#define HMDB_MOD_IKMNVFS_FOURCC_M3 BQ_PACK32(0x00, 0x00, 0x00, 0x00)
#define HMDB_MOD_IKMNVFS_FOURCC_M4 BQ_PACK32(0x00, 0x00, 0x00, 0x00)

/**
 VFS_HM40 フォーマットチェッカー
 @param[in] ファイル名
 @return VFS_HM40 フォーマットなら1以上のフォーマットバージョン、違ったら零
 */
BQ_EXTERN bq_uint HK_detect_hm40_header(SovietString _Path);




#define HMDB_MOD_IKMNVFS_KEY_SIZ 256
#define HMDB_MOD_IKMNVFS_KEY_SIZ_PK (HMDB_MOD_IKMNVFS_KEY_SIZ*2)


// デカいのでやめる
typedef struct
tagHMDB_IKMN_MATRIX {
    bq_byte *paunter;
    bq_byte *primary;
    bq_byte *secondary;
}hmdb_ikmnvfs_matrix_t;

// BQF に変える
typedef struct
tagHMDB_IKMNVFS_HEADER_T{
    bq_byte fourcc[16];
    hmdb_ikmnvfs_matrix_t matrix;
}hmdb_ikmnvfs_header_t;

#define HMDB_MOD_IKMNVFS_HEADER_SIZE (16+HMDB_MOD_IKMNVFS_KEY_SIZ_PK);

BQ_DECLARE_END

#endif /* _VFS_DRV_HM40_H_8C698ACA_2F62_4EF4_F959_DDA02A92A89F_ */

