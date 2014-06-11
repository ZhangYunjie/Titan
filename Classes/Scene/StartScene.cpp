//
//  StartScene.cpp
//  Titan
//
//  Created by Yunjie.Zhang on 14-6-10.
//
//
#include "GameDefine.h"

#include "StartScene.h"

USING_NS_CC;

void StartScene::initScene()
{
    mWinSize = Director::getInstance()->getWinSize();

    showStartButton();
}

#pragma mark - Draw Scene

void StartScene::showStartButton()
{
    auto startLabel = Label::createWithTTF("START", FONT_ARIAL, 14);
    auto menuItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(StartScene::startCallback, this));
    menuItem->setTag(kTagStartButton);

    auto menu = Menu::create();
    menu->addChild(menuItem);
    menu->setPosition( mWinSize / 2.0f );

    this->addChild(menu, kZOrderMenu);
}

#pragma mark - Callback

void StartScene::startCallback(Ref *sender)
{

}
