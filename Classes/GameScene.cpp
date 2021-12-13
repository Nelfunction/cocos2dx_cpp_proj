#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "StartScene.h"

USING_NS_CC;

RepeatForever* repeat_move;
Animate* animate_atk;
Animate* animate_jump;
float cooldown = 0.0f;
int pressLeft = 0;
int pressRight = 0;
bool moving = false;
bool moveanimate = false;
bool jumping = false;

bool web_connected = false;
bool web_alive = false;
PhysicsJointLimit* webjoint; // 거미줄 물리 joint
Sprite* WEB;

Scene* GameScene::createScene()
{
    auto scene = GameScene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    auto layer = GameScene::create();
    layer->SetPhysicsWorld(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if (!Scene::init()){return false;}

    auto director = Director::getInstance();
    auto vSize = director->getVisibleSize();

    //물리 박스
    auto edgeNode = Node::create();
    auto edgeBody = PhysicsBody::createEdgeBox(Size(vSize.width, vSize.height-100), PHYSICSBODY_MATERIAL_DEFAULT, 3);
    //
    edgeNode->addComponent(edgeBody);
    edgeNode->setPosition(Point(vSize.width / 2, vSize.height / 2 + 50));
    this->addChild(edgeNode);
    //

    return true;
}

void GameScene::onEnterTransitionDidFinish() {

    auto director = Director::getInstance();
    auto vSize = director->getVisibleSize();
    this->getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_JOINT);
	this->getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);

    // background
    auto back = Sprite::create("gamebackground.png");
    //
    back->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
    auto back_size = back->getContentSize();
    back->setScale(vSize.height / back_size.height, vSize.height / back_size.height);
    this->addChild(back, 0);
    //

    // label
    auto label = Label::createWithTTF("Click to web shooting / Press Z to release", "fonts/Arial.ttf", 20);
    //
    label->setPosition(Vec2(vSize.width / 2, vSize.height - 100));
    this->addChild(label, 0);
    //

    // button1
    auto button1 = MenuItemImage::create("plp0.png", "plp1.png",
        [&](Ref* sender) {
            auto scene = StartScene::createScene();
            Director::getInstance()->replaceScene(scene);
        });
    //
    button1->setScale(0.1f);
    button1->setPosition(Vec2(vSize.width - 100, vSize.height - 100));
    //

    // menu
    auto menu = Menu::create(button1, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    //

    // 애니메이션
    auto sprite = Sprite::create("spider.png");
    auto texture = sprite->getTexture();
    // 이동
    auto animation_move = Animation::create();
    animation_move->setDelayPerUnit(0.0625f);
    for (int i = 0; i < 8; i++)
    {
        int column = i % 4;
        int row = i / 4;
        animation_move->addSpriteFrameWithTexture(texture,
            Rect(column * 100, row * 100, 100, 100));
    }
    auto animate_move = Animate::create(animation_move);
    repeat_move = RepeatForever::create(animate_move);
    repeat_move->retain();
    // 공격
    auto animation_atk = Animation::create();
    animation_atk->setDelayPerUnit(0.125f);
    for (int i = 8; i < 14; i++)
    {
        int column = i % 4;
        int row = i / 4;
        animation_atk->addSpriteFrameWithTexture(texture,
            Rect(column * 100, row * 100, 100, 100));
    }
    animate_atk = Animate::create(animation_atk);
    animate_atk->retain();
    // 점프
    auto animation_jump = Animation::create();
    animation_jump->setDelayPerUnit(0.125f);
    for (int i = 14; i < 19; i++)
    {
        int column = i % 4;
        int row = i / 4;
        animation_jump->addSpriteFrameWithTexture(texture,
            Rect(column * 100, row * 100, 100, 100));
    }
    animate_jump = Animate::create(animation_jump);
    animate_jump->retain();
    //

    // 메인 캐릭터
    auto spider = Sprite::create("spider.png", Rect(0, 0, 100, 100));
    spider->setTag(1);
    spider->setName("spider");
    spider->setPosition(Point(240, 160));
    this->addChild(spider);

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    eventListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, spider);
    //


    // 물리 실체
    //auto spider = (Sprite*)this->getChildByTag(1);
    auto spiderBody = PhysicsBody::createBox(Size(70, 50), PhysicsMaterial(0, 0, 0));
    spider->setPhysicsBody(spiderBody);
    //this->addChild(spider);

    spiderBody->setCategoryBitmask(1);
    spiderBody->setCollisionBitmask(2);
    spiderBody->setContactTestBitmask(true);
    spiderBody->setRotationEnable(false);

    // 더미 물체
    auto spriteB = makeSprite("companion.png", Point(vSize.width / 2 - 200, vSize.height / 2 + 200));
    auto spriteC = makeSprite("companion.png", Point(vSize.width / 2 - 150, vSize.height / 2 + 150));
    auto spriteD = makeSprite("companion.png", Point(vSize.width / 2 - 50, vSize.height / 2 + 250));
    auto spriteE = makeSprite("companion.png", Point(vSize.width / 2 + 200, vSize.height / 2));
    auto spriteF = makeSprite("companion.png", Point(vSize.width / 2 + 150, vSize.height / 2 + 50));

    this->addChild(spriteB);
    this->addChild(spriteC);
    this->addChild(spriteD);
    this->addChild(spriteE);
    this->addChild(spriteF);

    // 거미줄 연결
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    // 마우스 이벤트
    auto Mouse = EventListenerMouse::create();
    Mouse->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

    scheduleUpdate();
}

Sprite* GameScene::makeSprite(const char* path, Vec2 position) //거미줄로 끌어당길 sprite 만들어 주는 함수
{
    Sprite* S = nullptr;

    S = Sprite::create(path);

    S->addComponent(PhysicsBody::createBox(S->getContentSize(), PhysicsMaterial(0, 1, 0)));
    S->setPosition(position);
    auto SBody = S->getPhysicsBody();
    SBody->setCollisionBitmask(1);
    SBody->setContactTestBitmask(true);
    SBody->setLinearDamping(0.2f);
    return S;
}

bool GameScene::onContactBegin(cocos2d::PhysicsContact& contact) //거미줄 이벤트
{
    PhysicsBody* a = contact.getShapeA()->getBody();
    PhysicsBody* b = contact.getShapeB()->getBody();
    auto spider = (Sprite*)this->getChildByTag(1);

    // check if the bodies have collided
    if ((1 == a->getCollisionBitmask() && 4 == b->getCollisionBitmask()) && !web_connected)
    {
        webjoint = PhysicsJointLimit::construct(spider->getPhysicsBody(), a, Point::ZERO, Point::ZERO, 30.0f, a->getPosition().getDistance(spider->getPosition()));
        this->getScene()->getPhysicsWorld()->addJoint(webjoint);
		b->setTag(2);
        //this->removeChild(b->getOwner());
        web_connected = true;
    }

    if (((4 == a->getCollisionBitmask() && 1 == b->getCollisionBitmask())) && !web_connected)
    {
        webjoint = PhysicsJointLimit::construct(spider->getPhysicsBody(), b, Point::ZERO, Point::ZERO, 30.0f, spider->getPosition().getDistance(b->getPosition()));
        this->getScene()->getPhysicsWorld()->addJoint(webjoint);
        //this->removeChild(a->getOwner());
		a->setTag(2);
        web_connected = true;
    }

    return true;
}

float getAngle(cocos2d::Point vec1, cocos2d::Point vec2) // 두 점 사이의 각도 반환
{
    auto vectorFromVec1ToVec2 = vec2 - vec1;
    // the angle between two vectors
    return CC_RADIANS_TO_DEGREES(-vectorFromVec1ToVec2.getAngle());
}

void GameScene::onMouseUp(cocos2d::Event* event) // 마우스 클릭시 거미줄 생성
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 ClickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    auto isRight = mouseEvent->getMouseButton();
    auto spider = (Sprite*)this->getChildByTag(1);

    if (!web_alive&&!web_connected)
    {
		if (spider->numberOfRunningActions() > 0)
		{
			spider->stopAllActions();
		}
        if (moveanimate) {
            moveanimate = false;
            spider->stopAction(repeat_move);
        }
        spider->runAction(animate_atk);

        web_alive = true;
        WEB = Sprite::create("web.png");
		WEB->addComponent(PhysicsBody::createBox(WEB->getContentSize(), PhysicsMaterial(0, 1, 0)));
		WEB->setPosition(spider->getPosition());
		WEB->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);


        auto scaleAmmo = WEB->getContentSize().width / WEB->getPosition().getDistance(ClickPoint);
        //WEB->setScaleX(1.0f / scaleAmmo);

		WEB->setRotation(getAngle(spider->getPosition(), ClickPoint));

        auto scaling = Sequence::create(
            ScaleTo::create(0.1f, (1.0f / scaleAmmo), WEB->getScaleX() * 0.1), CallFuncN::create(CC_CALLBACK_1(GameScene::webRemove, this)),
            nullptr);
        auto spriteBodyB = WEB->getPhysicsBody();
        spriteBodyB->setGravityEnable(false);
        spriteBodyB->setCategoryBitmask(1);
        spriteBodyB->setCollisionBitmask(4);
        spriteBodyB->setContactTestBitmask(true);

		WEB->runAction(scaling);
        this->addChild(WEB);
    }
}

void GameScene::webRemove(Node* node) //거미줄 사라지게함
{
	if (!web_connected)
	{
		node->removeFromParent();
		web_alive = false;
	}
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto spider = (Sprite*)this->getChildByTag(1);
    auto spiderBody = spider->getPhysicsBody();
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_A:
        pressLeft = pressRight + 1;
        if (!moving) {
            moving = true;
            if (!moveanimate) {
                spider->runAction(repeat_move);
                moveanimate = true;
                spider->setFlipX(true);
            }
        }
        break;
    case EventKeyboard::KeyCode::KEY_D:
        pressRight = pressLeft + 1;
        if (!moving) {
            moving = true;
            if (!moveanimate) {
                spider->runAction(repeat_move);
                moveanimate = true;
                spider->setFlipX(false);
            }
        }
        break;
    case EventKeyboard::KeyCode::KEY_W:
        log("W pressed");
		if (spider->numberOfRunningActions() > 0)
		{
			spider->stopAllActions();
		}
        if (!jumping) {
            log("jumping!");
            jumping = true;
            cooldown = 0.5f;
            if (moveanimate) {
                moveanimate = false;
                spider->stopAction(repeat_move);
            }
            spider->runAction(animate_jump);
        }
        break;
    case EventKeyboard::KeyCode::KEY_Z:
        if (web_connected)
        {
            webjoint->removeFormWorld();
            web_connected = false;
            web_alive = false;
			WEB->removeFromParent();
        }
        break;
    }
};
void GameScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto spider = (Sprite*)getChildByTag(1);
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_A:
        pressLeft = 0;
        if (!pressRight) {
            moving = false;
            if (moveanimate) {
                spider->stopAction(repeat_move);
                moveanimate = false;
            }
        }
        else
            spider->setFlipX(false);
        break;
    case EventKeyboard::KeyCode::KEY_D:
        pressRight = 0;
        if (!pressLeft) {
            moving = false;
            if (moveanimate) {
                spider->stopAction(repeat_move);
                moveanimate = false;
            }
        }
        else
            spider->setFlipX(true);
        break;
    }
};

void GameScene::update(float delta)
{
    auto spider = getChildByTag(1);
    spider->getPhysicsBody()->setVelocity(Vec2(0, spider->getPhysicsBody()->getVelocity().y));
    if (moving) {
        if (!moveanimate && spider->numberOfRunningActions() == 0)
        {
            spider->runAction(repeat_move);
            moveanimate = true;
        }

        if (pressLeft > 0 && (pressRight == 0 || pressLeft < pressRight))
        {
            spider->setPositionX(spider->getPositionX() - 120.0f * delta);
        }
        else if (pressRight > 0 && (pressLeft==0 || pressLeft > pressRight))
        {
            spider->setPositionX(spider->getPositionX() + 120.0f * delta);
        }
    }

    if (jumping) {
        log("jump");
        spider->setPositionY(spider->getPositionY() + 10.0f * cooldown);
        cooldown -= delta;
        if (cooldown <= 0.0f) {
            cooldown = 0.0f;
            jumping = false;
        }
    }

	if (web_connected) //만약 거미줄이 연결되었다면 
	{

		auto posA = webjoint->getBodyA()->getPosition();
		auto posB = webjoint->getBodyB()->getPosition();
		
		
		WEB->setPosition(posA);
		
		WEB->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		
		auto scaleAmmo = WEB->getContentSize().width / WEB->getPosition().getDistance(posB);
		WEB->setScaleX(1.0f / scaleAmmo);

		WEB->setRotation(getAngle(posA, posB));

		auto spriteBodyB = WEB->getPhysicsBody();
		spriteBodyB->setGravityEnable(false);
		spriteBodyB->setCategoryBitmask(1);
		spriteBodyB->setCollisionBitmask(2);
		spriteBodyB->setContactTestBitmask(true);
		
	}
}