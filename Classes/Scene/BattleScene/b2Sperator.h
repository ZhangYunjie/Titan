//
//  b2Sperator.h
//  Titan
//
//  Created by 张 韵杰 on 14-6-23.
//
//

#ifndef __Titan__b2Sperator__
#define __Titan__b2Sperator__

#include <vector>

#include "box2d.h"
#include "cocos2d.h"

#define MAX_VALUE 2147483647

class b2Separator
{
public:
    b2Separator(){}
    
    void separator(b2Body* body, b2FixtureDef* fixtureDef, std::vector<b2Vec2>* verticesVec, float scale=30.f);
    int validate(const std::vector<b2Vec2>& verticesVec);

private:
    void calcShapes(std::vector<b2Vec2>& vertivesVec, std::vector<std::vector<b2Vec2>> &result);
    b2Vec2* hitRay(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    b2Vec2* hitSegment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    bool isOnSegment(float px, float py, float x1, float y1, float x2, float y2);
    bool pointsMatch(float x1, float y1, float x2, float y2);
    bool isOnLine(float px, float py, float x1, float y1, float x2, float y2);
    float det(float x1, float y1, float x2, float y2, float x3, float y3);
};

#endif /* defined(__Titan__b2Sperator__) */
