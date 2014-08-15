//
//  MarchingSquare.h
//  Titan
//
//  Created by zhang yunjie on 2014/08/13.
//
//

#ifndef __Titan__MarchingSquare__
#define __Titan__MarchingSquare__

#include "cocos2d.h"

class MarchingSquare
{
public:
    void marchingSquares(cocos2d::Image* image, std::vector<cocos2d::Vec2>& pointVector, int scale = 1);
    std::string getDebugInfo();

private:
    cocos2d::Vec2* getStartingPixel();
    int getSquareValue(int px, int py);

    struct Pixel{
        cocos2d::Color4B color = cocos2d::Color4B(0, 0, 0, 0);
        cocos2d::Vec2    position = cocos2d::Vec2::ZERO;
    };
    
    float getAlphaValue(cocos2d::Image* image, int x, int y);

private:
    cocos2d::Image *mpImage;
    float mTolerance;
    int mScale;
};
#endif /* defined(__Titan__MarchingSquare__) */
