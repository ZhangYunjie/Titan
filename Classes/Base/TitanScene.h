//
//  TitanScene.h
//  Titan
//
//  Created by Yunjie.Zhang on 14-6-11.
//
//

#ifndef __Titan__TitanScene__
#define __Titan__TitanScene__

#include "cocos2d.h"

#include "TitanBase.h"

#define SCENE_FUN(__TYPE__) \
static TTSceneBaseScene* scene() \
{ \
    TTSceneBaseScene* _scene = TTSceneBaseScene::create(); \
    auto _node = __TYPE__::create(); \
    _scene->addChild(_node); \
    _scene->setTitainScene(dynamic_cast<TTSceneBaseScene*>(_node)); \
    return _scene; \
}

NS_TT_BEGIN

class TTSceneBaseLayer : public cocos2d::LayerColor
{
    bool interruption;
    
protected:
    TTSceneBaseLayer():interruption(false){}
    void setInterrupt(bool _interruption){interruption = _interruption;}
    
public:
    bool getInterrupt(){return interruption;}

    virtual void callBackForApplicationDidEnterBackground(){}
	virtual void callBackForApplicationWillEnterForeground(){}
};

class TTSceneBaseScene : public cocos2d::Scene
{
    TTSceneBaseScene* scene;

protected:
    TTSceneBaseScene():scene(NULL){}

public:
    void setTTScene(TTSceneBaseLayer* _scene){scene=_scene;}
    TTSceneBaseScene* getTTScene(){return scene;}
    
    CREATE_FUNC(TTSceneBaseScene);
}

NS_TT_END

#endif /* defined(__Titan__TitanScene__) */
