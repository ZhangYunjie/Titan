//
//  ShaderSprite.h
//  Titan
//
//  Created by zhang yunjie on 2014/06/18.
//
//

#ifndef __Titan__ShaderSprite__
#define __Titan__ShaderSprite__

#include "cocos2d.h"
USING_NS_CC;

class ShaderSprite : public Sprite {
    
public:
    static ShaderSprite* create(const char* pszFileName);
    virtual bool initWithTexture(Texture2D *pTexture, const Rect& rect) override;
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated) override;

private:
    GLProgram* mpShaderProgram;
};

#endif /* defined(__Titan__ShaderSprite__) */
