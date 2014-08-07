//
//  PhysicsEditorScene.h
//  Titan
//
//  Created by zhang yunjie on 2014/07/22.
//
//

#ifndef __Titan__PhysicsEditorScene__
#define __Titan__PhysicsEditorScene__

#include "cocos2d.h"
#include "Box2D.h"
#include "GLES-Render.h"

#include "TitanScene.h"

USING_NS_TT;

class PhysicsEditorScene : public TTScene<PhysicsEditorScene>
{
public:
    PhysicsEditorScene();
    ~PhysicsEditorScene();

    virtual void initScene();
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    void initTouch();
    void initDebugMenu();
    void initBox2DGround();

    void update(float dt);
    void updateScene();

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    void showBombEffect(cocos2d::Vec2 point);
    void addNewCharactor();
    void addNewSpriteWithCoords(cocos2d::Vec2 location, cocos2d::Sprite *sprite);

    void debugBtnCallBack(cocos2d::Ref* pSender);

private:
    
    enum kTag
    {
        kTagCharacter = 1,
        kTagDebugMenu,
    };
    
    enum kZOrder
    {
        kZOrderBackground = 1,
        kZOrderMenu,
    };

    cocos2d::Size mWinSize;
    cocos2d::RenderTexture *mpRender;
    cocos2d::EventListenerTouchOneByOne* mpTouchEventListener;
    
    b2World* mWorld;
    GLESDebugDraw *mDebugDraw;
};

#endif /* defined(__Titan__PhysicsEditorScene__) */
