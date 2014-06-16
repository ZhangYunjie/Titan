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

#include "TitanScene.h"

USING_NS_TT;

class  BattleScene :
public TTPhysicsScene<BattleScene>
{
public:
    CREATE_FUNC( BattleScene );

    BattleScene();
    ~BattleScene();
    
    virtual void initScene();

    void initPhysicsWorld();
    void initMenu();

private:
    void btnDebugDrawCallback(cocos2d::Object* pSender);

private:
    enum kTag
    {
        kTagCharacter = 1,
    };

    enum kZOrder
    {
        kZOrderMenu = 1,
    };

    cocos2d::Size mWinSize;
};

#endif /* defined(__Titan__BattleScene__) */
