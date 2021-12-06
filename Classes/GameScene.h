#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void onEnterTransitionDidFinish();

    cocos2d::Sprite* GameScene::makeSprite(const char* path, cocos2d::Vec2 position);

    // a selector callback
    void onClickButton1(cocos2d::Ref* pSender);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void update(float delta);

    void onMouseUp(cocos2d::Event* event);
    void webRemove(Node* node);

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

private:
    cocos2d::PhysicsWorld* sceneWorld;

    void SetPhysicsWorld(cocos2d::PhysicsWorld* world) { sceneWorld = world; };
    bool onContactBegin(cocos2d::PhysicsContact&);
};

#endif // __GAME_SCENE_H__