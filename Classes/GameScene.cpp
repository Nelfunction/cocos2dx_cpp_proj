#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "StartScene.h"

USING_NS_CC;

RepeatForever* move;
int pressLeft = 0;
int pressRight = 0;
bool moving = false;

bool web_connected = false;
bool web_alive = false;
PhysicsJointLimit* webjoint; // 거미줄 물리 joint

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
    auto edgeBody = PhysicsBody::createEdgeBox(vSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
    //
    edgeNode->addComponent(edgeBody);
    edgeNode->setPosition(Point(vSize.width / 2, vSize.height / 2));
    this->addChild(edgeNode);
    //

    scheduleUpdate();
    return true;
}

void GameScene::onEnterTransitionDidFinish() {

    auto director = Director::getInstance();
    auto vSize = director->getVisibleSize();

    // bg color
    auto bg = cocos2d::LayerColor::create(Color4B(53, 53, 53, 255));
    this->addChild(bg);
    //

    // label
    auto label = Label::createWithTTF("Main Gameplay", "fonts/Arial.ttf", 24);
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
    button1->setScale(0.2f);
    button1->setPosition(Vec2(500, 600));
    //

    // menu
    auto menu = Menu::create(button1, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    //

    //
    auto sprite = Sprite::create("spider.png");
    auto texture = sprite->getTexture();
    auto animation = Animation::create();
    animation->setDelayPerUnit(0.0625f);
    for (int i = 0; i < 8; i++)
    {
        int column = i % 4;
        int row = i / 5;
        animation->addSpriteFrameWithTexture(texture,
            Rect(column * 100, row * 100, 100, 100));
    }

    // 메인 캐릭터
    auto spider = Sprite::create("spider.png", Rect(0, 0, 100, 100));
    spider->setTag(1);
    spider->setName("spider");
    spider->setPosition(Point(240, 160));
    this->addChild(spider);

    auto animate = Animate::create(animation);
    move = RepeatForever::create(animate);
    move->retain();

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    eventListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, spider);
    //


    // 물리 실체 주기
    //auto spider = (Sprite*)this->getChildByTag(1);
    auto spiderBody = PhysicsBody::createBox(Size(70, 50), PhysicsMaterial(0, 1, 0));
    spider->setPhysicsBody(spiderBody);
    //this->addChild(spider);

    spiderBody->setCategoryBitmask(1);
    spiderBody->setCollisionBitmask(2);
    spiderBody->setContactTestBitmask(true);
    spiderBody->setRotationEnable(false);

    // 더미 물체
    auto spriteB = makeSprite("CloseNormal.png", Point(vSize.width / 2 - 200, vSize.height / 2 + 200));
    auto spriteC = makeSprite("CloseNormal.png", Point(vSize.width / 2 - 200, vSize.height / 2 + 200));

    this->addChild(spriteB);
    this->addChild(spriteC);

    // 거미줄 연결
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    // 마우스 이벤트
    auto Mouse = EventListenerMouse::create();
    Mouse->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

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
        this->removeChild(b->getOwner());
        web_connected = true;
    }

    if (((4 == a->getCollisionBitmask() && 1 == b->getCollisionBitmask())) && !web_connected)
    {
        webjoint = PhysicsJointLimit::construct(spider->getPhysicsBody(), b, Point::ZERO, Point::ZERO, 30.0f, spider->getPosition().getDistance(b->getPosition()));
        this->getScene()->getPhysicsWorld()->addJoint(webjoint);
        this->removeChild(a->getOwner());
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

    if (!web_alive)
    {

        web_alive = true;
        auto spriteB = Sprite::create("CloseNormal.png");
        spriteB->addComponent(PhysicsBody::createBox(spriteB->getContentSize(), PhysicsMaterial(0, 1, 0)));
        spriteB->setPosition(spider->getPosition());
        spriteB->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);


        auto scaleAmmo = spriteB->getContentSize().width / spriteB->getPosition().getDistance(ClickPoint);
        spriteB->setScaleX(1.0f / scaleAmmo);

        spriteB->setRotation(getAngle(spider->getPosition(), ClickPoint));

        auto scaling = Sequence::create(
            ScaleTo::create(0.1f, (1.0f / scaleAmmo), spriteB->getScaleY() * 0.1), CallFuncN::create(CC_CALLBACK_1(GameScene::webRemove, this)),
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

void GameScene::webRemove(Node* node) //거미줄 사라지게함
{
    node->removeFromParent();
    web_alive = false;
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto spider = (Sprite*)this->getChildByTag(1);
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        pressLeft = pressRight + 1;
        if (!moving) {
            moving = true;
            spider->runAction(move);
            spider->setFlipX(true);
        }
        break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        pressRight = pressLeft + 1;
        if (!moving) {
            moving = true;
            spider->runAction(move);
            spider->setFlipX(false);
        }
        break;
    case EventKeyboard::KeyCode::KEY_SPACE:

        break;
    case EventKeyboard::KeyCode::KEY_Z:
        if (web_connected)
        {
            webjoint->removeFormWorld();
            web_connected = false;
            web_alive = false;
        }
        break;
    }
};
void GameScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto spider = (Sprite*)getChildByTag(1);
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        pressLeft = 0;
        if (!pressRight) {
            moving = false;
            spider->stopAction(move);
        }
        else
            spider->setFlipX(false);
        break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        pressRight = 0;
        if (!pressLeft) {
            moving = false;
            spider->stopAction(move);
        }
        else
            spider->setFlipX(true);
        break;
    }
};

void GameScene::update(float delta)
{
    if (moving) {
        if (pressLeft > 0 && (pressRight == 0 || pressLeft < pressRight))
        {
            auto spider = getChildByTag(1);
            spider->setPositionX(spider->getPositionX() - 120.0f * delta);
        }
        else if (pressRight > 0 && (pressLeft==0 || pressLeft > pressRight))
        {
            auto spider = getChildByTag(1);
            spider->setPositionX(spider->getPositionX() + 120.0f * delta);
        }
    }

}