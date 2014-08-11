//
//  BattleScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/11.
//
//
#include "ShaderSprite.h"
#include "b2Separator.h"
#include "poly2tri.h"

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

    // addBackground();

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

    auto fgSprite = Sprite::create("tileC2.png");
    fgSprite->setPosition(mWinSize / 2.0f);
    mpRender->beginWithClear(0, 0, 0, 0);
    fgSprite->visit();
    mpRender->end();

    Image *img = new Image();
    img->initWithImageFile("tileC2.png");

    std::vector<Vec2> pointVector;
    std::vector<Vec2> marchingVector;

    // 对图片进行边界顶点化
    // 并通过RDP算法减少顶点个数
    float epsilon = 0.2f; // 阈值
    BattleWorld::getInstance()->marchingSquares(img, pointVector);
    BattleWorld::getInstance()->RDP(pointVector, epsilon, marchingVector);

    // 清空临时vector
    pointVector.clear();
    std::vector<Vec2>().swap(pointVector);

    // 由于生成顶点的锚点位置在左下角
    // 而cocos2dx的精灵锚点位置在中心
    // 因此将所有顶点位置移动半幅
    /*
    for (int i = 0; i < marchingVector.size(); i++ )
    {
        marchingVector.at(i).x -= img->getWidth() / 2.0f;
        marchingVector.at(i).y -= img->getHeight() / 2.0f;
        marchingVector.at(i).y *= -1;
    }
     */

    b2BodyDef* bodyDef = new b2BodyDef();
    bodyDef->type = b2_staticBody;
    bodyDef->position.Set(mWinSize.width/PTM_RATIO_2, mWinSize.height/PTM_RATIO_2);
    b2Body *body = mWorld->CreateBody(bodyDef);

    b2FixtureDef* fixtureDef = new b2FixtureDef();
    fixtureDef->restitution = 0.4;
    fixtureDef->friction = 0.2;
    fixtureDef->density = 4;

    std::vector<b2Vec2> vec;
    bool doSample = false;
    if (marchingVector.size() > 100)
    {
        doSample = true;
    }
    for(int i = marchingVector.size() - 1; i >= 0; i--)
    {
        if (i%10 == 0 || !doSample)
        {
            vec.push_back(b2Vec2(marchingVector[i].x/PTM_RATIO_2, marchingVector[i].y/PTM_RATIO_2));
        }
    }
    marchingVector.clear();
    std::vector<Vec2>().swap(marchingVector);

    // b2Separator用于将顶点组成面片
    b2Separator* sep = new b2Separator();
    if (sep->validate(vec) == 0)
    {
        CCLOG("Yey! Those vertices are goood to go");
        sep->separator(body, fixtureDef, &vec, PTM_RATIO_2);
    }
    else
    {
        CCLOG("I guess you messed something up! Error %d", sep->validate(vec));
    }
    vec.clear();
    std::vector<b2Vec2>().swap(vec);

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
    mpTouchEventListener = EventListenerTouchOneByOne::create();
    mpTouchEventListener->setSwallowTouches(true);
    mpTouchEventListener->onTouchBegan     = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
    mpTouchEventListener->onTouchMoved     = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
    mpTouchEventListener->onTouchEnded     = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
    mpTouchEventListener->onTouchCancelled = CC_CALLBACK_2(BattleScene::onTouchCancelled, this);
    dispatcher->addEventListenerWithSceneGraphPriority(mpTouchEventListener, this);
}

#pragma mark - UPDATE

void BattleScene::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);

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
            sprite->setRotation( -1.0f * CC_RADIANS_TO_DEGREES(body->GetAngle()));
        }
    }
}

void BattleScene::showBombEffect(Vec2 point)
{
    auto hole = Sprite::create("img1.png");
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

bool BattleScene::onTouchBegan(Touch* touch, Event* event)
{
    Point touchPoint = touch->getLocation();
    showBombEffect(touchPoint);
    return false;
}

void BattleScene::onTouchMoved(Touch* touch, Event* event)
{

}

void BattleScene::onTouchEnded(Touch* touch, Event* event)
{

}

void BattleScene::onTouchCancelled(Touch* touch, Event* event)
{

}

#pragma mark - CALLBACK

/*
 @brief 开关debug视图
 */
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
