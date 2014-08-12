//
//  b2SeparatorTestScene.h
//  Titan
//
//  Created by zhang yunjie on 2014/08/12.
//
//

#ifndef __Titan__b2SeparatorTestScene__
#define __Titan__b2SeparatorTestScene__

#include "cocos2d.h"
#include "Box2D.h"
#include "GLES-Render.h"

#include "TitanScene.h"

USING_NS_TT;

class b2SeparatorTestScene : public TTScene<b2SeparatorTestScene>
{
public:
    b2SeparatorTestScene();
    ~b2SeparatorTestScene();

    virtual void initScene();
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    
    void initTouch();
    void initPhysics();
    void initDebugMenu();
    
    void update(float dt);
    void updateScene();
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:
    void debugBtnCallBack(cocos2d::Ref* pSender);
    
private:
    enum kTag
    {
        kTagDebugMenu = 1,
    };

    enum kZOrder
    {
        kZOrderMenu = 1,
    };

    cocos2d::Size mWinSize;
    cocos2d::EventListenerTouchOneByOne* mpTouchEventListener;
    
    b2World *mWorld;
    GLESDebugDraw *mDebugDraw;
};

#endif /* defined(__Titan__b2SeparatorTestScene__) */
