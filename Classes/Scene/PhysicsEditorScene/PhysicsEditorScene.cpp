//
//  PhysicsEditorScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/07/22.
//
//
#include "GB2ShapeCache-x.h"
#include "TitanBase.h"
#include "BattleBase.h"

#include "PhysicsEditorScene.h"

USING_NS_CC;

#pragma mark - INITLIZATION

PhysicsEditorScene::PhysicsEditorScene():
mWorld(NULL),
mDebugDraw(NULL),
mpTouchEventListener(NULL),
mpRender(NULL)
{

}

void PhysicsEditorScene::initScene()
{
    // load physics shapes
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("charactor.plist");

    mWinSize = Director::getInstance()->getWinSize();

    // Define the gravity vector.
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);

    // Do we want to let bodies sleep
    bool doSleep = true;

    // Construct a world object, which will hold and simulate the rigid bodies.
    mWorld = new b2World(gravity);
    mWorld->SetAllowSleeping(doSleep);
    mWorld->SetContinuousPhysics(true);

    mDebugDraw = new GLESDebugDraw(PTM_RATIO);
    mWorld->SetDebugDraw(mDebugDraw);

    uint32 flags = DEBUG_DRAW_NONE;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_jointBit;
    mDebugDraw->SetFlags(flags);

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(mWinSize.width/PTM_RATIO_2, mWinSize.height/PTM_RATIO_2);

    // Call the body factory which allocate memory for the ground body
    // form a pool and creates the ground box shape ( also from a pool).
    // The body is als added to the world.
    b2Body* groundBody = mWorld->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2PolygonShape groundBox;

    // bottom
    groundBox.SetAsBox(mWinSize.width/PTM_RATIO_2, 0, b2Vec2(0, -mWinSize.height/PTM_RATIO_2), 0);
    groundBody->CreateFixture(&groundBox, 0);

    // top
    groundBox.SetAsBox(mWinSize.width/PTM_RATIO_2, 0, b2Vec2(0, mWinSize.height/PTM_RATIO_2), 0);
    groundBody->CreateFixture(&groundBox, 0);

    // left
    groundBox.SetAsBox(0, mWinSize.height/PTM_RATIO_2, b2Vec2(-mWinSize.width/PTM_RATIO_2, 0), 0);
    groundBody->CreateFixture(&groundBox, 0);

    // right
    groundBox.SetAsBox(0, mWinSize.height/PTM_RATIO_2, b2Vec2(mWinSize.width/PTM_RATIO_2, 0), 0);
    groundBody->CreateFixture(&groundBox, 0);

    // Set up sprite
    mpRender = RenderTexture::create(mWinSize.width, mWinSize.height);
    mpRender->setPosition(Vec2(mWinSize.width/2, mWinSize.height/2));
    this->addChild(mpRender, 10);

    auto fgSprite = Sprite::create("charactor_1.png");
    fgSprite->setPosition((mWinSize.width + 100.0f) / 2.0f, mWinSize.height / 2.0f);
    fgSprite->setTag(kTagCharacter);
    fgSprite->retain();
    mpRender->beginWithClear(0, 0, 0, 0);
    fgSprite->visit();
    mpRender->end();

    // 没有变化的坐标转换？？？
    // OpenGL坐标系原点再屏幕左下角，X轴向右，y轴向上
    Vec2 location = Director::getInstance()->convertToGL(fgSprite->getPosition());
    addNewSpriteWithCoords(location, fgSprite);

    initTouch();

    scheduleUpdate();
}


void PhysicsEditorScene::addNewSpriteWithCoords(Vec2 location, Sprite *sprite)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    bodyDef.position.Set(location.x/PTM_RATIO, location.y/PTM_RATIO);
    bodyDef.userData = sprite;
    b2Body *body = mWorld->CreateBody(&bodyDef);

    // add the fixture definitions to the body

    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, "charactor_1");
    
    // (0.5, 0.5)
    sprite->setAnchorPoint(sc->anchorPointForShape("charactor_1"));
}

void PhysicsEditorScene::initTouch()
{
    if ( NULL != mpTouchEventListener ) return;

    CCLOG("===== PhysicsEditor#initTouch =====");

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    mpTouchEventListener = EventListenerTouchOneByOne::create();
    mpTouchEventListener->setSwallowTouches(true); //　设置不向下传递触摸事件
    mpTouchEventListener->onTouchBegan = CC_CALLBACK_2(PhysicsEditorScene::onTouchBegan, this);
    mpTouchEventListener->onTouchMoved = CC_CALLBACK_2(PhysicsEditorScene::onTouchMoved, this);
    mpTouchEventListener->onTouchEnded = CC_CALLBACK_2(PhysicsEditorScene::onTouchEnded, this);
    mpTouchEventListener->onTouchCancelled = CC_CALLBACK_2(PhysicsEditorScene::onTouchCancelled, this);
    dispatcher->addEventListenerWithSceneGraphPriority(mpTouchEventListener, this);
}

#pragma mark - UPDATE

void PhysicsEditorScene::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    mWorld->DrawDebugData();
    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void PhysicsEditorScene::update(float dt)
{
    mWorld->Step(dt, 10, 10);

    updateScene();
}

void PhysicsEditorScene::updateScene()
{
    b2Body *body = mWorld->GetBodyList();
    for (; NULL != body; body = body->GetNext())
    {
        if ( NULL != body->GetUserData() )
        {
            Sprite* sprite = (Sprite*)body->GetUserData();
            int tag = sprite->getTag();
            if (tag == kTagCharacter)
            {
                // 问题：当不可以根据物体的位置来设置画布的位置（of cource, 因为两个根本不是一个东西）
                // 难点：对于可破坏地形，无法移动
                //mpRender->setPosition(body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO);
                //mpRender->setRotation( -1.0f * CC_RADIANS_TO_DEGREES(body->GetAngle()));
            }
        }
    }
}

void PhysicsEditorScene::showBombEffect(Vec2 point)
{
    auto hole = Sprite::create("img1.png");
    hole->setPosition(point);
    BlendFunc cbl = { GL_ZERO, GL_ONE_MINUS_SRC_ALPHA};
    hole->setBlendFunc(cbl);

    auto effect = Sprite::create();
    effect->setPosition(point);
    BlendFunc ebl = {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
    effect->setBlendFunc(ebl);

    mpRender->begin();
    hole->visit();
    effect->visit();
    mpRender->end();
}

#pragma mark - TOUCH

bool PhysicsEditorScene::onTouchBegan(Touch* touch, Event* event)
{
    Point touchPoint = touch->getLocation();
    showBombEffect(touchPoint);
    return false;
}

void PhysicsEditorScene::onTouchMoved(Touch* touch, Event* event)
{

}

void PhysicsEditorScene::onTouchEnded(Touch* touch, Event *event)
{

}

void PhysicsEditorScene::onTouchCancelled(Touch* touch, Event *event)
{

}

#pragma mark - DISTRUCTOR

PhysicsEditorScene::~PhysicsEditorScene()
{
    CC_SAFE_DELETE(mDebugDraw);
    CC_SAFE_DELETE(mWorld);
}
