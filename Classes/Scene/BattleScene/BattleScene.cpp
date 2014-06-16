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

#pragma mark - INIT

BattleScene::BattleScene()
{
    
}

void BattleScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();

    initPhysicsWorld();
    initMenu();
}

void BattleScene::initPhysicsWorld()
{
    CCLOG("===== BattleScene#initPhysicsWorld =====");

    auto edgeBody = PhysicsBody::createEdgeBox(mWinSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
    auto edgeNode = Node::create();
    edgeNode->setPosition(Vec2::ANCHOR_MIDDLE);
    edgeNode->setPhysicsBody(edgeBody);
    this->addChild(edgeNode);

    auto ballSprite = Sprite::create("circle.png");
    auto ballBody = PhysicsBody::createCircle(ballSprite->getContentSize().width/2.0f);
    ballSprite->setPhysicsBody(ballBody);
    ballSprite->setPosition(Vec2::ANCHOR_MIDDLE);
    this->addChild(ballSprite);
}

void BattleScene::initMenu()
{
    CCLOG("===== BattleScene#initMenu =====");

    auto menuItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(BattleScene::btnDebugDrawCallback, this));
    menuItem->setPosition(Vec2(mWinSize.width - 10.0f, mWinSize.height - 10.0f));
    
    auto menu = Menu::create();
    menu->addChild(menuItem);
    menu->setPosition(Vec2::ZERO);
    
    this->addChild(menu, kZOrderMenu);
}

#pragma mark - CALLBACK

void BattleScene::btnDebugDrawCallback(Object* pSender)
{
    
}

BattleScene::~BattleScene()
{
}