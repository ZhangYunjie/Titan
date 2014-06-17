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

#define SCENE_FUNC(__TYPE__) \
static cocos2d::Scene* scene() \
{ \
    auto _scene = cocos2d::Scene::create(); \
    auto _node = __TYPE__::create(); \
    _scene->addChild(_node, MAIN_LAYER); \
    return _scene; \
}

#define PHY_SCENE_FUNC(__TYPE__) \
static cocos2d::Scene* scene() \
{ \
    auto _scene = cocos2d::Scene::createWithPhysics(); \
    _scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL); \
    auto _node = __TYPE__::create(); \
    _node->setPhysicsWorld(_scene->getPhysicsWorld()); \
    _scene->addChild(_node, MAIN_LAYER); \
    return _scene; \
}

NS_TT_BEGIN

#pragma mark - BaseLayer

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

#pragma mark - TTScene (场景的主层)

template<class T_c> class TTScene : public TTSceneBaseLayer
{
    bool interaction;

protected:

    static T_c* _singleton;

public:
    TTScene():interaction(false){}
    virtual ~TTScene(){}

    virtual bool init()
    {
        if(!cocos2d::LayerColor::init())
        {
            return false;
        }

        _singleton = dynamic_cast<T_c*>(this);

        mpEventListener = NULL;

        initScene();

        setInterrupt(true);
        return _singleton? true : false;
    }
    
    // 返回主层所属的场景
    cocos2d::Scene* getScene()
    {
        return dynamic_cast<cocos2d::Scene*>(this->getParent());
    }

    virtual void initScene(){}
    virtual void termScene(){}

    template<class T_cPush> static void pushScene()
    {
        _singleton->setInterrupt(false);
        _singleton->termScene();
        cocos2d::Director::getInstance()->pushScene(T_cPush::scene());
    }

    static void popScene()
    {
        _singleton->setInterrupt(false);
        _singleton->termScene();
        cocos2d::Director::getInstance()->popScene();
    }

    template<class T_cReplace> static void replaceScene()
    {
        _singleton->setInterrupt(false);
        _singleton->termScene();
        cocos2d::Director::getInstance()->replaceScene(T_cReplace::scene());
    }

    template<class T_cScene> static void topScene()
    {
        _singleton->setInterrupt(false);
        _singleton->termScene();

        // TODO: topScene実装
        // cocos2d::Director::getInstance()->topScene(T_cScene::scene());
    }

    SCENE_FUNC(T_c);
    CREATE_FUNC(T_c);

private:
    cocos2d::EventListener* mpEventListener;
};

template<class T_c> T_c* TTScene<T_c>::_singleton = NULL;

NS_TT_END

#endif /* defined(__Titan__TitanScene__) */
