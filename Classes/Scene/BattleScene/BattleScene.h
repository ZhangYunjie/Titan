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
#include "Box2D.h"

#include "TitanScene.h"

USING_NS_TT;

class  BattleScene :
public TTPhysicsScene<BattleScene>
{
public:
    CREATE_FUNC( BattleScene );
    
    virtual void initScene();
    
private:
    
private:
    cocos2d::Size mWinSize;
    
    enum kTag
    {
        kTagStartButton = 1,
    };
    
    enum kZOrder
    {
        kZOrderMenu = 1,
    };
};

#endif /* defined(__Titan__BattleScene__) */
