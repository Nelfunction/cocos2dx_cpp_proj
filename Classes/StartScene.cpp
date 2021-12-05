#include "StartScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "OptionScene.h"

USING_NS_CC;

Scene* StartScene::createScene()
{
    return StartScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool StartScene::init()
{
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto director = Director::getInstance();
    auto vSize = director->getVisibleSize();

    // background
    auto back = Sprite::create("testbackground.png");
    //
    back->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
    auto back_size = back->getContentSize();
    back->setScale(vSize.height / back_size.height, vSize.height / back_size.height);
    this->addChild(back, 0);
    //

    // title
    auto title = Sprite::create("testtitle.png");
    //
    title->setAnchorPoint(Vec2(0.5f, 1));
    title->setPosition(Vec2(vSize.width / 2, vSize.height - 30));
    title->setScale(0.5f);
    this->addChild(title, 0);
    //

    // character
    auto character = Sprite::create("testcharactersheet.png", Rect(0, 0, 67, 100));
    //
    character->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
    character->setScale(2.0f);
    this->addChild(character, 0);
    //

    // button1
    auto label1 = Label::createWithTTF("PLAY", "fonts/Marker Felt.ttf", 24);
    auto button1 = MenuItemLabel::create(label1, CC_CALLBACK_1(StartScene::onClickButton1, this));
    //
    button1->setPosition(Vec2(vSize.width / 2, 180));
    //

    // button2
    auto label2 = Label::createWithTTF("OPTION", "fonts/arial.ttf", 24);
    auto button2 = MenuItemLabel::create(label2, CC_CALLBACK_1(StartScene::onClickButton2, this));
    //
    button2->setPosition(Vec2(vSize.width / 2, 140));
    //

    // button3
    auto label3 = Label::createWithTTF("QUIT", "fonts/arial.ttf", 24);
    auto button3 = MenuItemLabel::create(label3, [&](Ref* sender) {
        Director::getInstance()->end();
        });
    //
    button3->setPosition(Vec2(vSize.width / 2, 100));
    //

    // menu
    auto menu = Menu::create(button1, button2, button3, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    //

    return true;
}


void StartScene::onClickButton1(Ref* pSender)
{
    log("onClickButton1");
    auto scene = TransitionFade::create(0.5f, GameScene::createScene());
    Director::getInstance()->replaceScene(scene);
}

void StartScene::onClickButton2(Ref* pSender)
{
    log("onClickButton2");
    auto scene = OptionScene::createScene();
    Director::getInstance()->replaceScene(scene);
}