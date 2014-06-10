//
//  StartScene.h
//  Titan
//
//  Created by Yunjie.Zhang on 14-6-10.
//
//

#ifndef __Titan__StartScene__
#define __Titan__StartScene__

#include "cocos2d.h"

class StartScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC( StartScene );
};

#endif /* defined(__Titan__StartScene__) */
