//
//  BattleWorld.h
//  Titan
//
//  Created by zhang yunjie on 2014/06/17.
//
//

#ifndef __Titan__BattleWorld__
#define __Titan__BattleWorld__

#include "cocos2d.h"
#include "Box2D.h"

#include "BattleBase.h"

class BattleWorld
{
public:
    static BattleWorld* getInstance()
    {
        if (NULL == _singleton)
        {
            _singleton = new BattleWorld();
        }
        return _singleton;
    }

    BattleWorld(){};
    virtual ~BattleWorld(){};

    b2World* createB2World(b2Vec2 gravity = b2Vec2(0.0f, BATTLE_GRAVITY));

    b2Body* createB2Body(void* pUserData, b2BodyType bodyType, cocos2d::Vec2 pos, const b2Shape& shape, float density, b2World* pWorld);
    b2Body* createB2BodyWithSprite(cocos2d::Sprite* sprite, b2BodyType bodyType, const b2Shape& shape, float density, b2World* pWorld);

    void marchingSquares(cocos2d::Image* image, std::vector<cocos2d::Vec2>& pointVector);
    void RDP(std::vector<cocos2d::Vec2> &v, float epsilon, std::vector<cocos2d::Vec2> &resultVector);

    struct Pixel{
        cocos2d::Color4B color = cocos2d::Color4B(0, 0, 0, 0);
        cocos2d::Vec2    position = cocos2d::Vec2::ZERO;
    };

    Pixel getPixel(cocos2d::Image* image, int x, int y);

private:
    static BattleWorld* _singleton;
    cocos2d::Image* _image;
    int tolerance;

    float findPerpendicularDisatance(cocos2d::Vec2 point, cocos2d::Vec2 p1, cocos2d::Vec2 p);
    cocos2d::Vec2* getStartingPixel();
    int getSquareValue(int px, int py);
};

#endif /* defined(__Titan__BattleWorld__) */
