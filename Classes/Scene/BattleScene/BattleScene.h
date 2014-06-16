//
//  BattleScene.h
//  Titan
//
//  Created by zhang yunjie on 2014/06/11.
//
//

#ifndef __Titan__BattleScene__
#define __Titan__BattleScene__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "GLES-Render.h"

#include "TitanScene.h"

USING_NS_TT;

class  BattleScene :
public TTScene<BattleScene>,
public b2ContactListener
{
public:
    CREATE_FUNC( BattleScene );

    BattleScene();
    ~BattleScene();
    
    virtual void initScene();
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated) override;

    void initPhysics();
    void update(float dt);

private:

private:
    enum kTag
    {
        kTagCharacter = 1,
    };

    cocos2d::Size mWinSize;

    b2World *mWorld;
    GLESDebugDraw *_debugDraw;
};

#endif /* defined(__Titan__BattleScene__) */
