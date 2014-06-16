//
//  BattleScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/11.
//
//
#include "BattleBase.h"
#include "BattleScene.h"

USING_NS_CC;

BattleScene::BattleScene():
mWorld(NULL),
_debugDraw(NULL)
{
    
}

void BattleScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();

    this->initPhysics();

    scheduleUpdate();
}

void BattleScene::initPhysics()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, BATTLE_GRAVITY);
    mWorld = new b2World(gravity);

    _debugDraw = new GLESDebugDraw(PTM_RATIO);
    mWorld->SetDebugDraw(_debugDraw);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_aabbBit;
    flags += b2Draw::e_pairBit;
    flags += b2Draw::e_centerOfMassBit;
    _debugDraw->SetFlags(flags);

    {
        Sprite *ballSprite = CCSprite::create("circle.png");
        ballSprite->setPosition(Vec2(150.0f, 150.0f));
        ballSprite->setScale(2.0f);
        this->addChild(ballSprite);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.userData = ballSprite;
        bodyDef.position.Set(150.0f/PTM_RATIO, 150.0f/PTM_RATIO);

        b2Body *body = mWorld->CreateBody(&bodyDef);

        b2CircleShape circle;
        circle.m_radius = 18.0f / PTM_RATIO;

        b2FixtureDef fixtureDef;
        fixtureDef.shape        = &circle;
        fixtureDef.density      = 0.4f;
        fixtureDef.friction     = 0.5f;

        body->CreateFixture(&fixtureDef);
    }
    
    {
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        boxBodyDef.position.Set(150.0f/PTM_RATIO, 100.0f/PTM_RATIO);
        b2Body *boxBody = mWorld->CreateBody(&boxBodyDef);
    
        b2PolygonShape rect;
        rect.SetAsBox(100.0f/PTM_RATIO, 25.0f/PTM_RATIO);
    
        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape       = &rect;
        boxFixtureDef.density     = 0.4f;
        boxFixtureDef.friction    = 0.5f;
        boxFixtureDef.restitution = 0.6f;
        boxBody->CreateFixture(&boxFixtureDef);
    }
}

void BattleScene::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated)
{
    Layer::draw(renderer, transform, transformUpdated);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    mWorld->DrawDebugData();
    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void BattleScene::update(float dt)
{
    mWorld->Step(dt, 10, 10);

    b2Body *body = mWorld->GetBodyList();
    for (; NULL != body; body = body->GetNext())
    {
        if (NULL != body->GetUserData())
        {
            Sprite* sprite = (Sprite*)body->GetUserData();
            sprite->setPosition(body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO);
            sprite->setRotation( -1.0f * CC_RADIANS_TO_DEGREES(body->GetAngle()) );
        }
    }
}

BattleScene::~BattleScene()
{
    CC_SAFE_DELETE(_debugDraw);
    CC_SAFE_DELETE(mWorld);
}