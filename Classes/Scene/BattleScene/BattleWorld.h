//
//  BattleWorld.h
//  Titan
//
//  Created by zhang yunjie on 2014/06/17.
//
//

#ifndef __Titan__BattleWorld__
#define __Titan__BattleWorld__

#include "cocos2d.h"
#include "Box2D.h"

#include "BattleBase.h"

class BattleWorld
{
public:
    static BattleWorld* getInstance()
    {
        if (NULL == _singleton)
        {
            _singleton = new BattleWorld();
        }
        return _singleton;
    }
    
    BattleWorld(){};
    virtual ~BattleWorld(){};

    b2World* createB2World(b2Vec2 gravity = b2Vec2(0.0f, BATTLE_GRAVITY));
    b2Body* createB2Body(void* pUserData, b2BodyType bodyType, cocos2d::Point pos, const b2Shape& shape, float density, b2World* pWorld);
    b2Body* createB2BodyWithSprite(cocos2d::Sprite* sprite, b2BodyType bodyType, const b2Shape& shape, float density, b2World* pWorld);

private:
    static BattleWorld* _singleton;
};

#endif /* defined(__Titan__BattleWorld__) */
