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

USING_NS_CC;

void StartScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();

    auto bgSprite = Sprite::create("bg.png");
    bgSprite->setPosition( mWinSize / 2.0f );
    this->addChild(bgSprite);

    showStartButton();
}

#pragma mark - Draw Scene

void StartScene::showStartButton()
{
    TTFConfig ttfConfig(FONT_ARIAL, 40.0f);
    auto startLabel = Label::createWithTTF(ttfConfig, "START");
    startLabel->setColor(Color3B::WHITE);
    auto menuItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(StartScene::startCallback, this));
    menuItem->setTag(kTagStartButton);
    menuItem->setPosition( mWinSize / 2.0f );

    auto menu = Menu::create();
    menu->addChild(menuItem);
    menu->setPosition(Vec2::ZERO);

    this->addChild(menu, kZOrderMenu);
}

#pragma mark - Callback

void StartScene::startCallback(Ref *sender)
{
    // TODO: test
    replaceScene<BattleScene>();
}
