#include "CDScene.h"

USING_NS_CC;

bool is_already; //이미 거미줄이 다른데 붙어있는지 확인하는 변수
bool web_exist;//거미줄이 살아있냐
PhysicsJointLimit* joint; // 거미줄 물리 joint
Sprite* mainSprite; // 메인 캐릭터

Sprite* CD::makeSprite(const char* path, Vec2 position) //거미줄로 끌어당길 sprite 만들어 주는 함수
{
	Sprite* S = nullptr;

	S = Sprite::create(path);

	S->addComponent(PhysicsBody::createBox(S->getContentSize(), PhysicsMaterial(0, 1, 0)));
	S->setPosition(position);
	auto SBody = S->getPhysicsBody();
	SBody->setCollisionBitmask(1);
	SBody->setContactTestBitmask(true);

	return S;
}


Scene* CD::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	//scene->getPhysicsWorld()->setGravity( Vec2( 0, 0 ) );
	
	// 'layer' is an autorelease object
	auto layer =CD::create();
	layer->SetPhysicsWorld(scene->getPhysicsWorld());
	
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
// on "init" you need to initialize your instance

bool CD::init()
{
	//////////////////////////////
	// 1. super init first

	is_already = false;
	web_exist = false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	auto edgeNode = Node::create();
	auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);

	edgeNode->addComponent(edgeBody);

	edgeNode->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(edgeNode);
	
	return true;
}

void CD::onEnterTransitionDidFinish() {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	this->getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_JOINT);  // joint만 보이게 만들어줌
	mainSprite = Sprite::create("HelloWorld.png");
	mainSprite->setPosition(250,100);

	auto mainSpriteBody = PhysicsBody::createBox(mainSprite->getContentSize(), PhysicsMaterial(0, 1, 0));
	mainSprite->setPhysicsBody(mainSpriteBody);
	this->addChild(mainSprite);

	mainSpriteBody->setCategoryBitmask(1);
	mainSpriteBody->setCollisionBitmask(2);

	mainSpriteBody->setContactTestBitmask(true);
	mainSpriteBody->setRotationEnable(false);


	auto spriteB = makeSprite("CloseNormal.png", Point(visibleSize.width / 2 + origin.x - 200, visibleSize.height / 2 + origin.y + 200));

	auto spriteC = makeSprite("CloseNormal.png", Point(visibleSize.width / 2 + origin.x + 200, visibleSize.height / 2 + origin.y + 200));
	
	this->addChild(spriteB);
	this->addChild(spriteC);

	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) { //키보드 입력 함수

		Vec2 loc = event->getCurrentTarget()->getPosition();
		float dis = 15.0f;
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_Z:
			if (is_already)
			{
				joint->removeFormWorld();
				is_already = false;
				web_exist = false;
			}
			break;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			event->getCurrentTarget()->setPosition(loc.x-dis, loc.y);
			
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			event->getCurrentTarget()->setPosition(loc.x+dis, loc.y);
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			event->getCurrentTarget()->setPosition(loc.x, loc.y+10*dis);
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			event->getCurrentTarget()->setPosition(loc.x, --loc.y);
			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, mainSprite);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(CD::onContactBegin, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);


	auto Mouse = EventListenerMouse::create();
	Mouse->onMouseUp = CC_CALLBACK_1(CD::onMouseUp, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

}

bool CD::onContactBegin(cocos2d::PhysicsContact &contact) //거미줄 이벤트
{
	PhysicsBody *a = contact.getShapeA()->getBody();
	PhysicsBody *b = contact.getShapeB()->getBody();

	// check if the bodies have collided
	if ((1 == a->getCollisionBitmask() && 4 == b->getCollisionBitmask())  &&!is_already )
	{
		joint = PhysicsJointLimit::construct(mainSprite->getPhysicsBody(), a, Point::ZERO, Point::ZERO, 30.0f, a->getPosition().getDistance(mainSprite->getPosition()) );
		this->getScene()->getPhysicsWorld()->addJoint(joint);
		this->removeChild(b->getOwner());
		is_already = true;
	}

	if (((4 == a->getCollisionBitmask() && 1 == b->getCollisionBitmask())) && !is_already)
	{
		joint = PhysicsJointLimit::construct(mainSprite->getPhysicsBody(), b, Point::ZERO, Point::ZERO, 30.0f, mainSprite->getPosition().getDistance(b->getPosition()));
		this->getScene()->getPhysicsWorld()->addJoint(joint);
		this->removeChild(a->getOwner());
		is_already = true;
	}

	return true;
}


float getAngleOfTwoVectors(cocos2d::Point vec1, cocos2d::Point vec2) // 두 점 사이의 각도 반환
{
	auto vectorFromVec1ToVec2 = vec2 - vec1;
	// the angle between two vectors
	return CC_RADIANS_TO_DEGREES(-vectorFromVec1ToVec2.getAngle());
}

void CD::onMouseUp(cocos2d::Event* event) // 마우스 클릭시 거미줄 생성
{
	auto mouseEvent = static_cast<EventMouse*>(event);
	Vec2 ClickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
	auto isRight = mouseEvent->getMouseButton();

	if (!web_exist)
	{
		web_exist = true;
		auto spriteB = Sprite::create("CloseNormal.png"); 
		spriteB->addComponent(PhysicsBody::createBox(spriteB->getContentSize(), PhysicsMaterial(0, 1, 0)));
		spriteB->setPosition(mainSprite->getPosition());
		spriteB->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);


		auto scaleAmmo = spriteB->getContentSize().width / spriteB->getPosition().getDistance(ClickPoint);
		//spriteB->setScaleX(1.0f / scaleAmmo);  //거미줄 발사하게함


		spriteB->setRotation(getAngleOfTwoVectors(mainSprite->getPosition(), ClickPoint));


		auto scaling = Sequence::create(
			ScaleTo::create(0.1f, (1.0f / scaleAmmo), spriteB->getScaleX()*0.1), CallFuncN::create(CC_CALLBACK_1(CD::webRemove, this)),
			nullptr);
		auto spriteBodyB = spriteB->getPhysicsBody();
		spriteBodyB->setGravityEnable(false);
		spriteBodyB->setCategoryBitmask(1);
		spriteBodyB->setCollisionBitmask(4);
		spriteBodyB->setContactTestBitmask(true);

		spriteB->runAction(scaling);
		this->addChild(spriteB);
	}
}

void CD::webRemove(Node* node) //거미줄 사라지게함
{
	
	node->removeFromParent();
	web_exist = false;
}







