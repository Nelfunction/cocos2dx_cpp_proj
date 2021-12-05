#ifndef __CD_SCENE_H__
#define __CD_SCENE_H__

#include "cocos2d.h"

class CD : public cocos2d::Scene
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();
	virtual void onEnterTransitionDidFinish();

	cocos2d::Sprite* CD::makeSprite(const char* path, cocos2d::Vec2 position);

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	void onMouseUp(cocos2d::Event* event);
	void CD::webRemove(Node* node);
	// implement the "static create()" method manually
	CREATE_FUNC(CD);

private:
	cocos2d::PhysicsWorld *sceneWorld;

	void SetPhysicsWorld(cocos2d::PhysicsWorld *world) { sceneWorld = world; };
	bool onContactBegin(cocos2d::PhysicsContact &
	);

};

#endif // __CD_SCENE_H__
