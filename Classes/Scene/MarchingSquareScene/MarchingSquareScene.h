//
//  MarchingSquareScene.h
//  Titan
//
//  Created by zhang yunjie on 2014/08/13.
//
//

#ifndef __Titan__MarchingSquareScene__
#define __Titan__MarchingSquareScene__

#include "cocos2d.h"

#include "TitanScene.h"

USING_NS_TT;

class MarchingSquareScene :
public TTScene<MarchingSquareScene>
{
public:
    CREATE_FUNC( MarchingSquareScene );
    
    MarchingSquareScene();
    ~MarchingSquareScene();
    
    virtual void initScene();
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

private:
    cocos2d::Size mWinSize;
    std::vector<cocos2d::Vec2> pointVector;
};

#endif /* defined(__Titan__MarchingSquareScene__) */
