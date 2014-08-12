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

#include "TitanScene.h"

USING_NS_TT;

class  StartScene :
public TTScene<StartScene>
{
public:
    CREATE_FUNC( StartScene );

    virtual void initScene();

private:
    void showStartButton();

    void btnCallback(Ref* sender);

private:
    cocos2d::Size mWinSize;

    enum kTag
    {
        kTagStartButton = 1,
        kTagPhysicsEditorButton,
        kTagPoly2triButton,
        kTagb2Separator,
    };

    enum kZOrder
    {
        kZOrderMenu = 1,
    };
};

#endif /* defined(__Titan__StartScene__) */
