//
//  BattleScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/11.
//
//
#include "ShaderSprite.h"
#include "b2Sperator.h"

#include "BattleBase.h"
#include "BattleWorld.h"
#include "BattleScene.h"

USING_NS_CC;

#pragma mark - INITLIZATION

BattleScene::BattleScene():
mWorld(NULL),
mDebugDraw(NULL),
mpTouchEventListener(NULL),
mpRender(NULL)
{

}

void BattleScene::initScene()
{
    CCLOG("===== BattleScene#initScene =====");

    mWinSize = Director::getInstance()->getWinSize();

    //addBackground();

    initPhysics();
    initDebugMenu();
    initTouch();
    
    addTerrain();

    scheduleUpdate();
}

void BattleScene::addBackground()
{
    auto bgSprite = Sprite::create("bg.png");
    bgSprite->setPosition(mWinSize / 2.0f);
    this->addChild(bgSprite, kZOrderBackground, kTagBackground);
}

void BattleScene::addTerrain()
{
    mpRender = RenderTexture::create(mWinSize.width, mWinSize.height);
    mpRender->setPosition(Vec2(mWinSize.width/2, mWinSize.height/2));
    this->addChild(mpRender, 10);
    
    auto fgSprite = Sprite::create("img2.png");
    fgSprite->setPosition(mWinSize / 2.0f);
    mpRender->begin();
    fgSprite->visit();
    mpRender->end();
    
    ///////////////////////////////////////////////

//    b2Separator* sep = new b2Separator();
//    b2BodyDef* bodyDef = new b2BodyDef();
//    bodyDef->type = b2_staticBody;
//    bodyDef->position.Set(mWinSize.width/PTM_RATIO_2, mWinSize.height/PTM_RATIO_2);
//    b2Body *body = mWorld->CreateBody(bodyDef);
//    
//    b2FixtureDef* fixtureDef;
//    fixtureDef->restitution = 0.4;
//    fixtureDef->friction = 0.2;
//    fixtureDef->density = 4;
//
//    std::vector<b2Vec2>* vec = new std::vector<b2Vec2>();
//    
//    vec->push_back(b2Vec2(-2.f, -2.f));
//    vec->push_back(b2Vec2(2.f, -2.f));
//    vec->push_back(b2Vec2(2.f, 0.f));
//    vec->push_back(b2Vec2(0.f, 0.f));
//    vec->push_back(b2Vec2(-2.f, 2.f));
//    
//    if (sep->validate(*vec) == 0)
//    {
//        sep->separator(body, fixtureDef, vec, PTM_RATIO);
//    }
//    
//    vec->clear();
//    CC_SAFE_DELETE(vec);
    
    //////////////////////////////////////////////
    
    Image *img = mpRender->newImage();
    std::vector<Vec2> pointVector;
    std::vector<Vec2> marchingVector;
    BattleWorld::getInstance()->marchingSquares(img, pointVector);
    BattleWorld::getInstance()->RDP(pointVector, 0.2, marchingVector);
    pointVector.clear();
    
    b2Separator* sep = new b2Separator();
    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->type = b2_staticBody;
    bodyDef->position.Set(mWinSize.width/PTM_RATIO_2, mWinSize.height/PTM_RATIO_2);
    b2Body *body = mWorld->CreateBody(bodyDef);
    
    b2FixtureDef* fixtureDef = new b2FixtureDef();
    fixtureDef->restitution = 0.4;
    fixtureDef->friction = 0.2;
    fixtureDef->density = 4;
    
    std::vector<b2Vec2> vec;
    for(int i = marchingVector.size() - 1; i >= 0; i--)
    {
        if (i%10 == 0)
        {
            vec.push_back(b2Vec2(marchingVector[i].x/PTM_RATIO, marchingVector[i].y/PTM_RATIO));
        }
    }
    
    if (sep->validate(vec) == 0)
    {
        sep->separator(body, fixtureDef, &vec);
    }
    
}

void BattleScene::initPhysics()
{
    CCLOG("===== BattleScene#initPhysics =====");

    mWorld = BattleWorld::getInstance()->createB2World();

    mDebugDraw = new GLESDebugDraw(PTM_RATIO);
    mDebugDraw->SetFlags(DEBUG_DRAW_ALL);

    mWorld->SetDebugDraw(mDebugDraw);

    Sprite *ballSprite = CCSprite::create("circle.png");
    ballSprite->setPosition(Vec2(150.0f, 150.0f));
    ballSprite->setScale(2.0f);
    this->addChild(ballSprite, 100);

    b2CircleShape circle;
    circle.m_radius = ballSprite->getContentSize().width * ballSprite->getScale() / PTM_RATIO_2;

    BattleWorld::getInstance()->createB2BodyWithSprite(ballSprite, b2_dynamicBody, circle, 0.4f, mWorld);

    {
        b2BodyDef boxBodyDef;
        boxBodyDef.type = b2_staticBody;
        boxBodyDef.position.Set(150.0f/PTM_RATIO, 100.0f/PTM_RATIO);
        b2Body *boxBody = mWorld->CreateBody(&boxBodyDef);

        b2PolygonShape rect;
        rect.SetAsBox(100.0f/PTM_RATIO, 25.0f/PTM_RATIO);
        
        b2ChainShape edge;
        b2Vec2 mv[3] = {b2Vec2(-50.f/PTM_RATIO, 0.f/PTM_RATIO), b2Vec2(50.f/PTM_RATIO, 0.f/PTM_RATIO), b2Vec2(100.f/PTM_RATIO, 100.f/PTM_RATIO)};
        edge.CreateChain(mv, 3);

        b2FixtureDef boxFixtureDef;
        boxFixtureDef.shape       = &edge;
        boxFixtureDef.density     = 0.4f;
        boxFixtureDef.friction    = 0.5f;
        boxFixtureDef.restitution = 0.6f;
        boxBody->CreateFixture(&boxFixtureDef);
    }
}

void BattleScene::initDebugMenu()
{
    CCLOG("===== BattleScene#initDebugMenu =====");

    auto debugBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(BattleScene::debugBtnCallback, this));
    debugBtn->setPosition(Point(mWinSize.width-20.0f, 20.0f));
    debugBtn->setTag(kTagDebugDraw);
    
    //メニューを作成
    auto debugMenu = Menu::create(debugBtn, NULL);
    debugMenu->setPosition(Point::ZERO);
    this->addChild(debugMenu, kZOrderMenu, kTagDebug);
}

void BattleScene::initTouch()
{
    if (NULL != mpTouchEventListener) return;

    CCLOG("===== BattleScene#initTouch ======");

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    mpTouchEventListener = cocos2d::EventListenerTouchOneByOne::create();
    mpTouchEventListener->setSwallowTouches(true);
    mpTouchEventListener->onTouchBegan     = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
    mpTouchEventListener->onTouchMoved     = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
    mpTouchEventListener->onTouchEnded     = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
    mpTouchEventListener->onTouchCancelled = CC_CALLBACK_2(BattleScene::onTouchCancelled, this);
    dispatcher->addEventListenerWithSceneGraphPriority(mpTouchEventListener, this);
}

#pragma mark - UPDATE

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

    updateScene();
}

void BattleScene::updateScene()
{
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

void BattleScene::showBombEffect(Vec2 point)
{
    Sprite* hole = Sprite::create("img1.png");
    hole->setPosition(point);
    BlendFunc cbl = {GL_ZERO ,GL_ONE_MINUS_SRC_ALPHA};
    hole->setBlendFunc(cbl);
    
    auto effect = Sprite::create("img3.png");
    effect->setPosition(point);
    BlendFunc ebl = {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
    effect->setBlendFunc(ebl);
    
    mpRender->begin();
    hole->visit();
    effect->visit();
    mpRender->end();
}

#pragma mark - TOUCH

bool BattleScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Point touchPoint = touch->getLocation();
    showBombEffect(touchPoint);
    return false;
}

void BattleScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void BattleScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void BattleScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

#pragma mark - CALLBACK

void BattleScene::debugBtnCallback(cocos2d::Ref* pSender)
{
    if (mDebugDraw->GetFlags())
    {
        mDebugDraw->SetFlags(DEBUG_DRAW_NONE);
    }
    else
    {
        mDebugDraw->SetFlags(DEBUG_DRAW_ALL);
    }
}

#pragma mark - DISTRUCTOR

BattleScene::~BattleScene()
{
    CC_SAFE_DELETE(mDebugDraw);
    CC_SAFE_DELETE(mWorld);
}