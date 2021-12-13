#include "OptionScene.h"
#include "SimpleAudioEngine.h"
#include "StartScene.h"


USING_NS_CC;

Scene* OptionScene::createScene()
{
    return OptionScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool OptionScene::init()
{
    if (!Scene::init()) { return false; }

    auto director = Director::getInstance();
    auto vSize = director->getVisibleSize();

    // bg color
    auto bg = cocos2d::LayerColor::create(Color4B(53, 53, 53, 255));
    this->addChild(bg);
    //
    // 
    //
    auto label = Label::createWithTTF("INFO", "fonts/Arial.ttf", 24);
    //
    label->setPosition(Vec2(vSize.width / 2, vSize.height - 100));
    this->addChild(label, 0);
    //

    //
    auto label2 = Label::createWithTTF("powered by - cocod2dx\nhttps://github.com/cocos2d/cocos2d-x \n\npainted by - Piskel\nhttps://github.com/piskelapp/piskel \n\nour repo \nhttps://github.com/Nelfunction/cocos2dx_cpp_proj", "fonts/Arial.ttf", 14);
    //
    label2->setPosition(Vec2(vSize.width / 2, vSize.height - 200));
    this->addChild(label2, 0);
    //

    // character
    auto character = Sprite::create("spider.png", Rect(100, 100, 100, 100));
    //
    character->setPosition(Vec2(vSize.width / 2, vSize.height / 2 - 50));
    this->addChild(character, 0);
    //

    // button1
    auto button1 = MenuItemImage::create("plp0.png", "plp1.png",
        [&](Ref* sender) {
            auto scene = StartScene::createScene();
            Director::getInstance()->replaceScene(scene);
        });
    //
    button1->setScale(0.1f);
    button1->setPosition(Vec2(vSize.width / 2, 120));
    //

    // menu
    auto menu = Menu::create(button1, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    //

    return true;
}