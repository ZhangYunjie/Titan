//
//  b2SeparatorTestScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/08/12.
//
//
#include "BattleBase.h"

#include "b2SeparatorTestScene.h"

USING_NS_CC;

#pragma mark - INITIIALIZATION

b2SeparatorTestScene::b2SeparatorTestScene():
mWorld(NULL),
mDebugDraw(NULL),
mpTouchEventListener(NULL)
{

}

void b2SeparatorTestScene::initScene()
{
    
}

void b2SeparatorTestScene::initPhysics()
{
    
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
}

void b2SeparatorTestScene::update(float dt)
{
    mWorld->Step(dt, 10, 10);

    updateScene();
}

void b2SeparatorTestScene::updateScene()
{
    b2Body *body = mWorld->GetBodyList();
    for (; NULL != body; body = body->GetNext() )
    {
        if ( NULL != body->GetUserData() )
        {
            auto sprite = (Sprite*)body->GetUserData();
            sprite->setPosition( body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO );
            sprite->setRotation( -1.0f * CC_RADIANS_TO_DEGREES( body->GetAngle() ) );
        }
    }
}

#pragma mark - TOUCH

bool b2SeparatorTestScene::onTouchBegan( Touch* touch, Event* event )
{
    return false;
}

void b2SeparatorTestScene::onTouchMoved( Touch* touch, Event* event )
{
    
}

void b2SeparatorTestScene::onTouchEnded( Touch* touch, Event* event )
{
    
}

void b2SeparatorTestScene::onTouchCancelled( Touch* touch, Event* event )
{

}

#pragma mark - CALLBACK

void b2SeparatorTestScene::debugBtnCallBack( Ref *pSender )
{
    
}

#pragma mark - DISTRUCTOR

b2SeparatorTestScene::~b2SeparatorTestScene()
{
    CC_SAFE_DELETE( mDebugDraw );
    CC_SAFE_DELETE( mWorld );
}