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

b2Body* BattleWorld::createB2Body(void* pUserData, b2BodyType bodyType, Vec2 pos, const b2Shape& shape, float density, b2World* pWorld)
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
        result = fabs(slope * p.x - p.y + intercept) / sqrtf(powf(slope, 2) + 1);
    }
    return result;
}

void BattleWorld::marchingSquares(cocos2d::Image *image, std::vector<cocos2d::Vec2> &pointVector)
{
    pointVector.clear();

    _image = image;
    tolerance = 1;

    // get the starting point
    cocos2d::Vec2* startPoint = getStartingPixel();
    if (startPoint != NULL)
    {
        // px and py are the coordinates of the starting points
        int px = startPoint->x;
        int py = startPoint->y;

        // stepx and stepy can be -1, 0 or 1 and represent the step in pixels to reach
        // next contour point
        int stepx, stepy;
        // we also neeed to save the previous step. that's why we use prevx and prevy
        int prevx, prevy;
        // closeLoop wille be true once we traced the full contour
        bool closedLoop = false;

        while (!closedLoop)
        {
            // the core of the script is getting the 2x2 suqare value of each pixel
            int squareValue = getSquareValue(px, py);
            switch (squareValue) {
                /* going UP with these cases:

                 +---+---+   +---+---+   +---+---+
                 | 1 |   |   | 1 |   |   | 1 |   |
                 +---+---+   +---+---+   +---+---+
                 |   |   |   | 4 |   |   | 4 | 8 |
                 +---+---+   +---+---+   +---+---+

                 */
                case 1:
                case 5:
                case 13:
                    stepx = 0;
                    stepy = -1;
                    break;

                /* going DOWN with these cases:

                 +---+---+   +---+---+   +---+---+
                 |   |   |   |   | 2 |   | 1 | 2 |
                 +---+---+   +---+---+   +---+---+
                 |   | 8 |   |   | 8 |   |   | 8 |
                 +---+---+   +---+---+  +---+---+

                 */
                case 8:
                case 10:
                case 11:
                    stepx = 0;
                    stepy = 1;
                    break;

                /* going LEFT with these cases:

                 +---+---+   +---+---+   +---+---+
                 |   |   |   |   |   |   |   | 2 |
                 +---+---+   +---+---+   +---+---+
                 | 4 |   |   | 4 | 8 |   | 4 | 8 |
                 +---+---+   +---+---+  +---+---+

                 */
                case 4:
                case 12:
                case 14:
                    stepx = -1;
                    stepy = 0;
                    break;

                /* going RIGHT with these cases:

                 +---+---+   +---+---+   +---+---+
                 |   | 2 |   | 1 | 2 |   | 1 | 2 |
                 +---+---+   +---+---+   +---+---+
                 |   |   |   |   |   |   | 4 |   |
                 +---+---+   +---+---+  +---+---+

                 */
                case 2:
                case 3:
                case 7:
                    stepx = 1;
                    stepy = 0;
                    break;

                /* special saddle point case 1:

                 +---+---+
                 |   | 2 |
                 +---+---+
                 | 4 |   |
                 +---+---+

                 going LEFT if coming from UP
                 else going RIGHT

                 */
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

                /* special saddle point case 2:

                 +---+---+
                 | 1 |   |
                 +---+---+
                 |   | 8 |
                 +---+---+

                 going UP if coming from RIGHT
                 else going DOWN

                 */
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
            // moving onto next point
            px += stepx;
            py += stepy;
            // saving contour point
            pointVector.push_back(Vec2(px, py));

            prevx = stepx;
            prevy = stepy;
            // if we returned to the first point visited, the loop has finished.
            if (px == startPoint->x && py == startPoint->y)
            {
                closedLoop = true;
            }
        }
    }
}

void BattleWorld::RDP(std::vector<cocos2d::Vec2> &v, float epsilon, std::vector<cocos2d::Vec2> &resultVector)
{
    if (v.size() < 1){ return; }

    cocos2d::Vec2 firstPoint = v[0];
    cocos2d::Vec2 lastPoint = v[v.size() - 1];
    if (v.size() < 3)
    {
        resultVector = v;
        return;
    }
    int index = -1;
    float dist = 0.0f;
    for(int i = 1; i < v.size() - 1; i++)
    {
        float cDist = findPerpendicularDisatance(v[i], firstPoint, lastPoint);
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
        for (int i = 0; i< index + 1; i++) {
            l1.push_back(v.at(i));
        }
        for (int i = index; i < v.size() - 1; i++)
        {
            l2.push_back(v.at(i));
        }

        std::vector<cocos2d::Vec2> r1;
        std::vector<cocos2d::Vec2> r2;
        RDP(l1, epsilon, r1);
        RDP(l2, epsilon, r2);
        l1.clear();
        std::vector<cocos2d::Vec2>().swap(l1);
        l2.clear();
        std::vector<cocos2d::Vec2>().swap(l2);

        resultVector.clear();
        for (int i = 0; i < r1.size()-1; i++)
        {
            resultVector.push_back(r1.at(i));
        }
        r1.clear();
        std::vector<cocos2d::Vec2>().swap(r1);

        for (int i = 0; i < r2.size(); i++)
        {
            resultVector.push_back(r2.at(i));
        }
        r2.clear();
        std::vector<cocos2d::Vec2>().swap(r2);

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

            if (getPixel(_image, j, i).color.a >= tolerance)
            {
                return offsetPoint;
            }
        }
    }
    return NULL;
}

int BattleWorld::getSquareValue(int px, int py)
{
    /*

     checking the 2x2 pixel grid, assigning these values to each pixel, if not transparent

     +---+---+
     | 1 | 2 |
     +---+---+
     | 4 | 8 | <- current pixel (pX,pY)
     +---+---+

     */

    int squareValue = 0;
    // checking upper left pixel
    if ((px > 0 && py > 0) && getPixel(_image, px-1, py-1).color.a >= tolerance)
    {
        squareValue += 1;
    }
    // checking upper pixel
    if ((py > 0) && getPixel(_image, px, py-1).color.a > tolerance)
    {
        squareValue += 2;
    }
    // checking left pixel
    if ( (px > 0) && getPixel(_image, px-1, py).color.a > tolerance)
    {
        squareValue += 4;
    }
    // checking the pixel itself
    if (getPixel(_image, px, py).color.a > tolerance)
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
    _pixel.color = Color4B(0,0,0,0);

    if (x<0 || x >= mPixelW || y<0 || y >= mPixelH)
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
