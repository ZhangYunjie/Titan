//
//  MarchingSquareScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/08/13.
//
//
#include "GameDefine.h"
#include "MarchingSquareScene.h"

USING_NS_CC;

#pragma mark - INITILIZATION

MarchingSquareScene::MarchingSquareScene()
{

}

void MarchingSquareScene::initScene()
{
    CCLOG("===== MarchingSquareScene#initScene =====");
    
    mWinSize = Director::getInstance()->getWinSize();
    pointVector.clear();

    auto sprite = Sprite::create("tileC2.png");
    sprite->setPosition(mWinSize / 2.0f);
    this->addChild(sprite);

    marchingSquare = new MarchingSquare();
    auto image = new Image();
    image->initWithImageFile("tileC2.png");

    int imgScaleFactor = 2;
    marchingSquare->marchingSquares(image, pointVector, imgScaleFactor);

    TTFConfig ttfConfig(FONT_ARIAL, 20.0f);
    auto debugInfoLabel = Label::createWithTTF(ttfConfig, marchingSquare->getDebugInfo());
    debugInfoLabel->setTag(kTagDebugInfo);
    debugInfoLabel->setAnchorPoint(Vec2(1.0f, 1.0f));
    debugInfoLabel->setPosition(mWinSize - Size(10.0f, 10.0f));
    this->addChild(debugInfoLabel);

    scheduleUpdate();
}

#pragma mark - UPDATE

void MarchingSquareScene::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);

    DrawPrimitives::setDrawColor4F(0.5f, 0.5f, 0.0f, 0.5f);
    for (std::vector<Vec2>::iterator it = pointVector.begin(); it!= pointVector.end(); it++)
    {
        DrawPrimitives::drawSolidCircle((*it) + mWinSize / 2.0f, 2.0f,  CC_DEGREES_TO_RADIANS(90), 50);
    }
}

void MarchingSquareScene::update(float dt)
{
    auto debugInfoLabel = (Label*)this->getChildByTag(kTagDebugInfo);
    debugInfoLabel->setString(marchingSquare->getDebugInfo());
}

#pragma mark - DISTRUCTOR

MarchingSquareScene::~MarchingSquareScene()
{
    
}