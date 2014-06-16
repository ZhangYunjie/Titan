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

    // Touch Mode: One by one
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event){return false;}
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event){}
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event){}
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event){}

    // Touch Mode: All at once
    virtual bool onTouchesBegan(const std::vector<cocos2d::Touch*> touches, cocos2d::Event* event){return false;}
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*> touches, cocos2d::Event* event){}
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*> touches, cocos2d::Event* event){}
    virtual void onTouchesCancelled(const std::vector<cocos2d::Touch*> touches, cocos2d::Event* event){}

    void enableTouch(cocos2d::Touch::DispatchMode mode = cocos2d::Touch::DispatchMode::ONE_BY_ONE)
    {
        if (NULL != mpEventListener) { return; }

        switch (mode)
        {
            case cocos2d::Touch::DispatchMode::ONE_BY_ONE:
            {
                auto _eventListener = cocos2d::EventListenerTouchOneByOne::create();
                _eventListener->setSwallowTouches(true);  // 设置其不想下传递
                _eventListener->onTouchBegan     = CC_CALLBACK_2(T_c::onTouchBegan, this);
                _eventListener->onTouchMoved     = CC_CALLBACK_2(T_c::onTouchMoved, this);
                _eventListener->onTouchEnded     = CC_CALLBACK_2(T_c::onTouchEnded, this);
                _eventListener->onTouchCancelled = CC_CALLBACK_2(T_c::onTouchCanceled, this);

                auto _eventDipatcher = cocos2d::Director::getInstance()->getEventDispatcher();
                _eventDipatcher->addEventListenerWithSceneGraphPriority(_eventListener, this);

                mpEventListener = dynamic_cast<cocos2d::EventListener*>(_eventListener);
            }
                break;

            case cocos2d::Touch::DispatchMode::ALL_AT_ONCE:
            {
                auto _eventListener = cocos2d::EventListenerTouchAllAtOnce::create();
                _eventListener->onTouchesBegan     = CC_CALLBACK_2(T_c::onTouchesBegan, this);
                _eventListener->onTouchesMoved     = CC_CALLBACK_2(T_c::onTouchesMoved, this);
                _eventListener->onTouchesEnded     = CC_CALLBACK_2(T_c::onTouchesEnded, this);
                _eventListener->onTouchesCancelled = CC_CALLBACK_2(T_c::onTouchesCancelled, this);

                auto _eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
                _eventDispatcher->addEventListenerWithSceneGraphPriority(_eventListener, this);

                mpEventListener = dynamic_cast<cocos2d::EventListener*>(_eventListener);
            }
                break;

            default:
                break;
        }
    }

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
