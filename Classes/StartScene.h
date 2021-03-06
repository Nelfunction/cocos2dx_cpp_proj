#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"

class StartScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void onClickButton1(cocos2d::Ref* pSender);
    void onClickButton2(cocos2d::Ref* pSender);
    void onClickButton3(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(StartScene);
};

#endif // __HELLOWORLD_SCENE_H__