//
//  b2SeparatorTestScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/08/12.
//
//
#include "BattleBase.h"
#include "b2Separator.h"
#include "b2SeparatorTestScene.h"

USING_NS_CC;

#pragma mark - INITIIALIZATION

b2SeparatorTestScene::b2SeparatorTestScene():
mWorld(NULL),
mDebugDraw(NULL),
mpTouchEventListener(NULL),
mbTouched(false),
mbUnabled(false),
mPolyTag(kTagPoly)
{
    points[0] = Vec2(-60.0f, -60.0f);
    points[1] = Vec2(60.0f, -60.0f);
    points[2] = Vec2(60.0f, 0.0f);
    points[3] = Vec2(0.0f, 0.0f);
    points[4] = Vec2(-60.0f, 60.0f);
}

void b2SeparatorTestScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();
    
    initTouch();
    initDebugMenu();
    initPhysics();

    scheduleUpdate();
}

void b2SeparatorTestScene::initPhysics()
{
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

    initGround();
    initPoly();
}

void b2SeparatorTestScene::initGround()
{
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
}

void b2SeparatorTestScene::initPoly()
{
    auto bodyDef = new b2BodyDef();
    bodyDef->type = b2_dynamicBody;
    bodyDef->position.Set( mWinSize.width / PTM_RATIO_2, mWinSize.height / PTM_RATIO_2 );

    auto body = mWorld->CreateBody( bodyDef );
    body->SetUserData((void*)&mPolyTag);

    auto fixtureDef = new b2FixtureDef();
    fixtureDef->restitution = 0.4;
    fixtureDef->friction = 0.2;
    fixtureDef->density = 4;
    
    generatePoly(body, fixtureDef);
}

void b2SeparatorTestScene::initTouch()
{
    if ( NULL != mpTouchEventListener ) return;
    
    CCLOG("===== b2SeparatorTestScene#initTouch =====");

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    mpTouchEventListener = EventListenerTouchOneByOne::create();
    mpTouchEventListener->setSwallowTouches( true );
    mpTouchEventListener->onTouchBegan = CC_CALLBACK_2( b2SeparatorTestScene::onTouchBegan, this );
    mpTouchEventListener->onTouchMoved = CC_CALLBACK_2( b2SeparatorTestScene::onTouchMoved, this );
    mpTouchEventListener->onTouchEnded = CC_CALLBACK_2( b2SeparatorTestScene::onTouchEnded, this );
    mpTouchEventListener->onTouchCancelled = CC_CALLBACK_2( b2SeparatorTestScene::onTouchCancelled, this );
    dispatcher->addEventListenerWithSceneGraphPriority( mpTouchEventListener, this );
}

void b2SeparatorTestScene::initDebugMenu()
{
    auto debugBtn = MenuItemImage::create( "CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(b2SeparatorTestScene::debugBtnCallBack, this) );
    debugBtn->setPosition( Point(mWinSize.width - 20.0f, 20.0f ));
    debugBtn->setTag( kTagDebugMenu );

    auto debugMenu = Menu::create( debugBtn, NULL );
    debugMenu->setPosition( Point::ZERO );
    this->addChild( debugMenu, kZOrderMenu, kTagDebugMenu );
}

#pragma mark - UPDATE

void b2SeparatorTestScene::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw( renderer, transform, flags );

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
    Director::getInstance()->pushMatrix( MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW );
    mWorld->DrawDebugData();
    Director::getInstance()->popMatrix( MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW );

    int i = 0;
    for (; i < 5; i++)
    {
        if (mbUnabled)
        {
            DrawPrimitives::setDrawColor4F(1.0f, 0.0f, 0.0f, 0.5f);
        }
        else if (mbTouched && i == mTouchedIndex)
        {
            DrawPrimitives::setDrawColor4F(0.5f, 0.5f, 0.0f, 0.5f);
        }
        else
        {
            DrawPrimitives::setDrawColor4F(0.0f, 1.0f, 0.0f, 0.5f);
        }
        DrawPrimitives::drawSolidCircle(points[i]+mWinSize/2.0f, 5.0f,  CC_DEGREES_TO_RADIANS(90), 50);

        if (i >= 1)
        {
            if (!mbUnabled && mbTouched && i == mTouchedIndex + 1)
            {
                DrawPrimitives::setDrawColor4F(0.5f, 0.5f, 0.0f, 0.5f);
            }
            DrawPrimitives::drawLine(points[i-1]+mWinSize/2.0f, points[i]+mWinSize/2.0f);
        }
        else
        {
            if (!mbUnabled && mbTouched && mTouchedIndex == 4)
            {
                DrawPrimitives::setDrawColor4F(0.5f, 0.5f, 0.0f, 0.5f);
            }
            DrawPrimitives::drawLine(points[0]+mWinSize/2.0f, points[4]+mWinSize/2.0f);
        }
    }
}

void b2SeparatorTestScene::update(float dt)
{
    mWorld->Step(dt, 10, 10);
    mWorld->ClearForces();
    updateScene();
}

void b2SeparatorTestScene::updateScene()
{
    b2Body *body = mWorld->GetBodyList();
    for (; NULL != body; body = body->GetNext() )
    {
        if ( NULL != body->GetUserData())
        {
//            Sprite* sprite = (Sprite*)body->GetUserData();
//            if ( NULL != sprite )
//            {
//                sprite->setPosition( body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO );
//                sprite->setRotation( -1.0f * CC_RADIANS_TO_DEGREES( body->GetAngle() ) );
//            }
        }
    }
}

void b2SeparatorTestScene::generatePoly(b2Body* body, b2FixtureDef* fixtureDef)
{
    auto *separator = new b2Separator();
    
    std::vector<b2Vec2> vec;
    int i = 0;
    for (; i < 5; i++)
    {
        vec.push_back(b2Vec2(points[i].x/PTM_RATIO, points[i].y/PTM_RATIO));
    }
    
    if ( separator->validate(vec) == 0 )
    {
        CCLOG("Yey! Those vertices are good to go!");
        separator->separator(body, fixtureDef, &vec);
    }
    else
    {
        CCLOG("Oh, I guess you messed something up");
    }
    
}

#pragma mark - TOUCH

bool b2SeparatorTestScene::onTouchBegan( Touch* touch, Event* event )
{
    Point pos = touch->getLocation();
    int i = 0;
    for (; i<5; i++)
    {
        float distance = fabsf(points[i].getDistance(pos-mWinSize/2.0f));
        if ( distance < 5.0f )
        {
            mbTouched = true;
            mTouchedIndex = i;
        }
    }
    return true;
}

void b2SeparatorTestScene::onTouchMoved( Touch* touch, Event* event )
{
    if (mbTouched)
    {
        points[mTouchedIndex] = touch->getLocation() - mWinSize/2.0f;

        auto *separator = new b2Separator();
        std::vector<b2Vec2> vec;
        int i = 0;
        for (; i < 5; i++)
        {
            vec.push_back(b2Vec2(points[i].x/PTM_RATIO, points[i].y/PTM_RATIO));
        }

        if (separator->validate(vec))
        {
            mbUnabled = true;
        }
        else
        {
            mbUnabled = false;
        }
    }
}

void b2SeparatorTestScene::onTouchEnded( Touch* touch, Event* event )
{
    if (mbTouched)
    {
        mbTouched = false;
    }
}

void b2SeparatorTestScene::onTouchCancelled( Touch* touch, Event* event )
{
    if (mbTouched)
    {
        mbTouched = false;
    }
}

#pragma mark - CALLBACK

void b2SeparatorTestScene::debugBtnCallBack( Ref *pSender )
{
    b2Body *body = mWorld->GetBodyList();
    for (; NULL != body; body = body->GetNext() )
    {
        if ( NULL != body->GetUserData() )
        {
            kTag tag = *((kTag*)body->GetUserData());
            if ( tag == mPolyTag )
            {
                body->SetUserData(NULL);
                mWorld->DestroyBody(body);
            }
        }
    }
    initPoly();
}

#pragma mark - DISTRUCTOR

b2SeparatorTestScene::~b2SeparatorTestScene()
{
    CC_SAFE_DELETE( mDebugDraw );
    CC_SAFE_DELETE( mWorld );
}