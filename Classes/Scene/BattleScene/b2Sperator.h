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

class b2Separator
{
public:
    b2Separator(){}
    
    void separator(b2Body body, b2FixtureDef fixtureDef, std::vector<b2Vec2> verticesVec, int scale=30)
    {
        int n = verticesVec.size(), j, m;
        std::vector<b2Vec2> vec;
        b2Vec2 figsVec[] = {};
        b2PolygonShape polygonShape;
        int i = 0;
        for (; i<verticesVec.size(); i++)
        {
            vec.push_back(b2Vec2(verticesVec.at(i).x*scale, verticesVec.at(i).y*scale));
        }
    }
};

#endif /* defined(__Titan__b2Sperator__) */
