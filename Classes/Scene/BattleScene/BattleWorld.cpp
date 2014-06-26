//
//  BattleWorld.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/06/17.
//
//

#include "BattleWorld.h"

USING_NS_CC;

BattleWorld* BattleWorld::_singleton = NULL;

b2World* BattleWorld::createB2World(b2Vec2 gravity)
{
    b2World* world = new b2World(gravity);
    return world;
}

b2Body* BattleWorld::createB2Body(void* pUserData, b2BodyType bodyType, Point pos, const b2Shape& shape, float density, b2World* pWorld)
{
    if (NULL == pWorld) return NULL;

    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.userData = pUserData;
    bodyDef.position.Set(pos.x/PTM_RATIO, pos.y/PTM_RATIO);

    b2Body* body = pWorld->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.5f;

    body->CreateFixture(&fixtureDef);

    return body;
}

b2Body* BattleWorld::createB2BodyWithSprite(cocos2d::Sprite* sprite, b2BodyType bodyType, const b2Shape& shape, float density, b2World* pWorld)
{
    if (NULL == pWorld || NULL == sprite) return NULL;

    return createB2Body(dynamic_cast<void*>(sprite), bodyType, sprite->getPosition(), shape, density, pWorld);
}

float BattleWorld::findPerpendicularDisatance(cocos2d::Vec2 p, cocos2d::Vec2 p1, cocos2d::Vec2 p2)
{
    float result;
    if (p1.x == p2.x)
    {
        result = fabs(p.x - p1.x);
    }
    else
    {
        float slope = (p2.y - p1.y) / (p2.x - p1.x);
        float intercept = p1.y - (slope*p1.x);
        result = fabs(slope * p.x - p.y + intercept) / sqrt(pow(slope, 2) + 1);
    }
    return result;
}

void BattleWorld::marchingSquares(cocos2d::Image *image, std::vector<cocos2d::Vec2> &pointVector)
{
    _image = image;
    tolerance = 1;
    
    // get the starting point
    cocos2d::Vec2* startPoint = getStartingPixel();
    if (startPoint != NULL)
    {
        int px = startPoint->x;
        int py = startPoint->y;
        
        float stepx, stepy;
        float prevx, prevy;
        bool closedLoop = false;
        
        while (!closedLoop)
        {
            int squareValue = getSquareValue(px, py);
            switch (squareValue) {
                case 1:
                case 5:
                case 13:
                    stepx = 0;
                    stepy = -1;
                    break;
                    
                case 8:
                case 10:
                case 11:
                    stepx = 0;
                    stepy = 1;
                    break;
                    
                case 4:
                case 12:
                case 14:
                    stepx = -1;
                    stepy = 0;
                    break;
                    
                case 2:
                case 3:
                case 7:
                    stepx = 1;
                    stepy = 0;
                    break;
                    
                case 6:
                    if(prevx == 0 && prevy == -1)
                    {
                        stepx = -1;
                        stepy = 0;
                    }
                    else
                    {
                        stepx = 1;
                        stepy = 0;
                    }
                    break;
                
                case 9:
                    if(prevx == 1 && prevy == 0)
                    {
                        stepx = 0;
                        stepy = -1;
                    }
                    else
                    {
                        stepx = 0;
                        stepy = 1;
                    }
                    break;
                    
                default:
                    break;
            }
            px += stepx;
            py += stepy;
            
            pointVector.push_back(Vec2(px, py));
            
            prevx = stepx;
            prevy = stepy;
            
            if (px == startPoint->x && py == startPoint->y) {
                closedLoop = true;
            }
        }
    }
}

void BattleWorld::RDP(std::vector<cocos2d::Vec2> &v, float epsilon, std::vector<cocos2d::Vec2> &resultVector)
{
    cocos2d::Vec2 firstPoint = v[0];
    cocos2d::Vec2 lastPoint = v[v.size() - 1];
    if (v.size() < 3)
    {
        resultVector = v;
    }
    int index = -1;
    int dist = 0;
    for(int i = 1; i < v.size() - 1; i++)
    {
        int cDist = findPerpendicularDisatance(v[i], firstPoint, lastPoint);
        if ( cDist > dist)
        {
            dist = cDist;
            index = i;
        }
    }
    if(dist > epsilon)
    {
        std::vector<cocos2d::Vec2> l1;
        std::vector<cocos2d::Vec2> l2;
        for (int i = 0; i<index; i++) {
            l1.push_back(v.at(i));
        }
        for (int i = index; i < v.size(); i++)
        {
            l2.push_back(v.at(i));
        }
    
        std::vector<cocos2d::Vec2> r1;
        std::vector<cocos2d::Vec2> r2;
        RDP(l1, epsilon, r1);
        RDP(l2, epsilon, r2);
        l1.clear();
        l2.clear();
        
        resultVector.clear();
        for (int i = 0; i < r1.size(); i++)
        {
            resultVector.push_back(r1.at(i));
        }
        r1.clear();
        
        for (int i = 0; i < r2.size(); i++)
        {
            resultVector.push_back(r2.at(i));
        }
        r2.clear();
    }
    else
    {
        resultVector.clear();
        resultVector.push_back(firstPoint);
        resultVector.push_back(lastPoint);
    }
}

cocos2d::Vec2* BattleWorld::getStartingPixel()
{
    cocos2d::Vec2 *offsetPoint = new cocos2d::Vec2(cocos2d::Vec2::ZERO);
    for (int i = 0; i<_image->getHeight(); i++)
    {
        for (int j = 0; j<_image->getWidth(); j++)
        {
            offsetPoint->x = j;
            offsetPoint->y = i;
            
            if (getPixel(_image, j, i).color.r >= tolerance)
            {
                return offsetPoint;
            }
        }
    }
    return NULL;
}

int BattleWorld::getSquareValue(float px, float py)
{
    int squareValue = 0;
    
    if (getPixel(_image, px-1, py-1).color.a >= tolerance)
    {
        squareValue += 1;
    }
    
    if (getPixel(_image, px, py-1).color.a >= tolerance)
    {
        squareValue += 2;
    }
    
    if (getPixel(_image, px-1, py).color.a >= tolerance)
    {
        squareValue += 4;
    }
    
    if (getPixel(_image, px, py).color.a >= tolerance)
    {
        squareValue += 8;
    }
    
    return squareValue;
}

BattleWorld::Pixel BattleWorld::getPixel(Image* image, int x, int y)
{
    unsigned char * data = image->getData();
    int mPixelW = image->getWidth();
    int mPixelH = image->getHeight();
    
    Pixel _pixel;
    
    if (x<0 || x > mPixelW || y<0 || y > mPixelH)
    {
        return _pixel;
    }
    
    _pixel.color.r = data[(y * mPixelW * 4 + x * 4) + 0];
    _pixel.color.g = data[(y * mPixelW * 4 + x * 4) + 1];
    _pixel.color.b = data[(y * mPixelW * 4 + x * 4) + 2];
    _pixel.color.a = data[(y * mPixelW * 4 + x * 4) + 3];
    
    _pixel.position = cocos2d::Vec2(x, y);
    
    return _pixel;
}
