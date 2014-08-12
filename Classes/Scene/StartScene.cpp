//
//  StartScene.cpp
//  Titan
//
//  Created by Yunjie.Zhang on 14-6-10.
//
//
#include "GameDefine.h"

#include "StartScene.h"
#include "BattleScene.h"
#include "PhysicsEditorScene.h"
#include "b2SeparatorTestScene.h"

USING_NS_CC;

void StartScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();

    showStartButton();
}

#pragma mark - Draw Scene

void StartScene::showStartButton()
{
    // 使用marching squares构造box2d
    TTFConfig ttfConfig(FONT_ARIAL, 20.0f);
    auto startLabel = Label::createWithTTF(ttfConfig, "Marching Squares");
    startLabel->setColor(Color3B::WHITE);
    auto menuItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(StartScene::btnCallback, this));
    menuItem->setTag(kTagStartButton);
    menuItem->setPosition( Vec2(mWinSize.width/2.0f, mWinSize.height/2.0f - 30.0f) );

    // 使用physics editor构造box2d
    auto physicsEditorLabel = Label::createWithTTF(ttfConfig, "Physics Editor");
    physicsEditorLabel->setColor(Color3B::WHITE);
    auto physicsEditorItem = MenuItemLabel::create(physicsEditorLabel, CC_CALLBACK_1(StartScene::btnCallback, this));
    physicsEditorItem->setTag(kTagPhysicsEditorButton);
    physicsEditorItem->setPosition(Vec2(mWinSize.width/2.0f, mWinSize.height/2.0f));

    // 使用poly2tri
    auto poly2triLabel = Label::createWithTTF(ttfConfig, "poly2tri");
    poly2triLabel->setColor(Color3B::WHITE);
    auto poly2triItem = MenuItemLabel::create(poly2triLabel, CC_CALLBACK_1(StartScene::btnCallback, this));
    poly2triItem->setTag(kTagPoly2triButton);
    poly2triItem->setPosition(Vec2(mWinSize.width/2.0f, mWinSize.height/2.0f + 30.0f));
    
    // b2Separator Test
    auto b2SeparatorLabel = Label::createWithTTF(ttfConfig, "b2Separator");
    b2SeparatorLabel->setColor(Color3B::WHITE);
    auto b2SeparatorItem = MenuItemLabel::create(b2SeparatorLabel, CC_CALLBACK_1(StartScene::btnCallback, this));
    b2SeparatorItem->setTag(kTagb2Separator);
    b2SeparatorItem->setPosition(Vec2(mWinSize.width/2.0f, mWinSize.height/2.0f -60.0f));

    auto menu = Menu::create();
    menu->addChild(menuItem);
    menu->addChild(physicsEditorItem);
    menu->addChild(poly2triItem);
    menu->addChild(b2SeparatorItem);
    menu->setPosition(Vec2::ZERO);

    this->addChild(menu, kZOrderMenu);
}

#pragma mark - Callback

void StartScene::btnCallback(Ref *sender)
{
    Node* node = dynamic_cast<Node*>(sender);
    kTag tag = (kTag)node->getTag();
    switch (tag)
    {
        case kTagStartButton:
            replaceScene<BattleScene>();
            break;


        case kTagPhysicsEditorButton:
            replaceScene<PhysicsEditorScene>();
            break;

        case kTagb2Separator:
            replaceScene<b2SeparatorTestScene>();
            break;

        default:
            break;
    }
}
