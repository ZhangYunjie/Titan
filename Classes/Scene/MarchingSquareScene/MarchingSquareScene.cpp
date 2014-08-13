//
//  MarchingSquareScene.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/08/13.
//
//

#include "MarchingSquare.h"
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

    auto sprite = Sprite::create("img4.png");
    sprite->setPosition(mWinSize / 2.0f);
    this->addChild(sprite);

    auto marchingSquare = new MarchingSquare();
    auto image = new Image();
    image->initWithImageFile("charactor_1.png");
    marchingSquare->marchingSquares(image, pointVector);
}

#pragma mark - UPDATE

void MarchingSquareScene::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);

    DrawPrimitives::setDrawColor4F(0.5f, 0.5f, 0.0f, 0.5f);
    for (std::vector<Vec2>::iterator it = pointVector.begin(); it!= pointVector.end(); it++)
    {
        DrawPrimitives::drawSolidCircle((*it) / 2.0f + mWinSize / 2.0f, 2.0f,  CC_DEGREES_TO_RADIANS(90), 50);
    }
}

#pragma mark - DISTRUCTOR

MarchingSquareScene::~MarchingSquareScene()
{
    
}