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

    //
    auto label = Label::createWithTTF("Option", "fonts/Arial.ttf", 24);
    //
    label->setPosition(Vec2(vSize.width / 2, vSize.height - 100));
    this->addChild(label, 0);
    //

    // character
    auto character = Sprite::create("testcharactersheet.png", Rect(67, 100, 67, 100));
    //
    character->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
    character->setScale(2.0f);
    this->addChild(character, 0);
    //

    // button1
    auto button1 = MenuItemImage::create("plp0.png", "plp1.png",
        [&](Ref* sender) {
            auto scene = StartScene::createScene();
            Director::getInstance()->replaceScene(scene);
        });
    //
    button1->setScale(0.2f);
    button1->setPosition(Vec2(vSize.width / 2, 120));
    //

    // menu
    auto menu = Menu::create(button1, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    //

    return true;
}