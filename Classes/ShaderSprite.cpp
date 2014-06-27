//
//  ShaderSprite.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/18.
//
//

#include "ShaderSprite.h"

static CC_DLL const GLchar *transparentshader =
#include "tansparentshader.h"

ShaderSprite* ShaderSprite::create(const char *pszFileName)
{
    ShaderSprite *pRet = new ShaderSprite();
    if (pRet && pRet->initWithFile(pszFileName)) {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool ShaderSprite::initWithTexture(Texture2D *pTexture, const Rect& rect)
{
    do{
        CC_BREAK_IF(!Sprite::initWithTexture(pTexture, rect));
        
        // 加载顶点着色器和片元着色器
        mpShaderProgram = new GLProgram();
        mpShaderProgram ->initWithByteArrays(ccPositionTextureA8Color_vert, transparentshader);
        
        CHECK_GL_ERROR_DEBUG();
        
        // 启用顶点着色器的attribute变量，坐标、纹理坐标、颜色
        mpShaderProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
        mpShaderProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
        mpShaderProgram->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD);

        CHECK_GL_ERROR_DEBUG();
        
        // 自定义着色器链接
        mpShaderProgram->link();
        
        CHECK_GL_ERROR_DEBUG();
        
        // 设置移动、缩放、旋转矩阵
        mpShaderProgram->updateUniforms();
        this->setGLProgram(mpShaderProgram);
        
        CHECK_GL_ERROR_DEBUG();
        
        return true;
        
    }while(0);
    return false;
}

void ShaderSprite::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
    
    CCAssert(!_batchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");
    
    CC_NODE_DRAW_SETUP();
    
    //
	// 启用attributes变量输入，顶点坐标，纹理坐标，颜色
	//
	GL::enableVertexAttribs(  GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );
    GL::blendFunc(_blendFunc.src, _blendFunc.dst);
    
    mpShaderProgram->use();
    mpShaderProgram->setUniformsForBuiltins();
    
    // 绑定纹理到纹理槽0
    GL::bindTexture2D(_texture->getName());
    
#define kQuadSize sizeof(_quad.bl)

	long offset = (long)&_quad;
    
	// vertex
	int diff = offsetof( V3F_C4B_T2F, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
    
	// texCoods
	diff = offsetof( V3F_C4B_T2F, texCoords);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
    
	// color
	diff = offsetof( V3F_C4B_T2F, colors);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
    
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
	CHECK_GL_ERROR_DEBUG();
    
    CC_INCREMENT_GL_DRAWS(1);
    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
}