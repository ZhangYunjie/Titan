//
//  BattleWorld.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/17.
//
//

#include "BattleWorld.h"

USING_NS_CC;

BattleWorld* BattleWorld::_singleton = NULL;

b2World* BattleWorld::createB2World(b2Vec2 gravity)
{
    b2World* world = new b2World(gravity);
    return world;
}

b2Body* BattleWorld::createB2Body(void* pUserData, b2BodyType bodyType, Point pos, const b2Shape& shape, float density, b2World* pWorld)
{
    if (NULL == pWorld) return NULL;

    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.userData = pUserData;
    bodyDef.position.Set(pos.x/PTM_RATIO, pos.y/PTM_RATIO);

    b2Body* body = pWorld->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.5f;

    body->CreateFixture(&fixtureDef);

    return body;
}

b2Body* BattleWorld::createB2BodyWithSprite(cocos2d::Sprite* sprite, b2BodyType bodyType, const b2Shape& shape, float density, b2World* pWorld)
{
    if (NULL == pWorld || NULL == sprite) return NULL;

    return createB2Body(dynamic_cast<void*>(sprite), bodyType, sprite->getPosition(), shape, density, pWorld);
}
