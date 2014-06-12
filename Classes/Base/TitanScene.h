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
static TTSceneBaseScene* scene() \
{ \
    auto _scene = TTSceneBaseScene::create(); \
    auto _node = __TYPE__::create(); \
    _scene->addChild(_node, MAIN_LAYER); \
    _scene->setTTScene(dynamic_cast<TTSceneBaseLayer*>(_node)); \
    return _scene; \
}

#define PHY_SCENE_FUNC(__TYPE__) \
static TTPhysicsBaseScene* scene() \
{ \
    auto _scene = dynamic_cast<TTPhysicsBaseScene*>(TTSceneBaseScene::createWithPhysics()); \
    _scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL); \
    auto _node = __TYPE__::create(); \
    _node->setPhyWorld(_scene->getPhysicsWorld()); \
    _scene->addChild(_node, MAIN_LAYER); \
    _scene->setTTScene(dynamic_cast<TTSceneBaseLayer*>(_node)); \
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

#pragma mark - BaseScene

class TTSceneBaseScene : public cocos2d::Scene
{
    TTSceneBaseLayer* scene;

protected:
    TTSceneBaseScene():scene(NULL){}

public:
    void setTTScene(TTSceneBaseLayer* _scene){scene=_scene;}
    TTSceneBaseLayer* getTTScene(){return scene;}

    CREATE_FUNC(TTSceneBaseScene);
};

class TTPhysicsBaseScene : public TTSceneBaseScene
{
public:
    CREATE_FUNC(TTPhysicsBaseScene);
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

        initScene();

        setInterrupt(true);
        return _singleton? true : false;
    }
    
    // 返回主层所属的场景
    TTSceneBaseScene* getScene()
    {
        return dynamic_cast<TTSceneBaseScene*>(this->getParent());
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
};

template<class T_c> T_c* TTScene<T_c>::_singleton = NULL;

template<class T_c> class TTPhysicsScene : public TTScene<T_c>
{
    void setPhysicsWorld(cocos2d::PhysicsWorld* world){mWorld = world;}

private:
    cocos2d::PhysicsWorld* mWorld;

    PHY_SCENE_FUNC(T_c);
};

NS_TT_END

#endif /* defined(__Titan__TitanScene__) */
