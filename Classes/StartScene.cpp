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
    auto back = Sprite::create("background.png");
    //
    back->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
    auto back_size = back->getContentSize();
    back->setScale(vSize.height / back_size.height, vSize.height / back_size.height);
    this->addChild(back, 0);
    //

    // title
    auto title = Sprite::create("title.png");
    title->setPosition(Vec2(vSize.width / 2, vSize.height - 120));
    this->addChild(title, 0);
    //

    // button1
    auto label1 = Label::createWithTTF("PLAY", "fonts/arial.ttf", 40);
    auto button1 = MenuItemLabel::create(label1, CC_CALLBACK_1(StartScene::onClickButton1, this));
    //
    button1->setPosition(Vec2(vSize.width / 2, 220));
    //

    // button2
    auto label2 = Label::createWithTTF("INFO", "fonts/arial.ttf", 40);
    auto button2 = MenuItemLabel::create(label2, CC_CALLBACK_1(StartScene::onClickButton2, this));
    //
    button2->setPosition(Vec2(vSize.width / 2, 160));
    //

    // button3
    auto label3 = Label::createWithTTF("QUIT", "fonts/arial.ttf", 40);
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
    auto repeat_move = RepeatForever::create(animate_move);
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
    auto animate_atk = Animate::create(animation_atk);
    auto repeat_atk = RepeatForever::create(animate_atk);
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
    auto animate_jump = Animate::create(animation_jump);
    auto repeat_jump = RepeatForever::create(animate_jump);
    //

    // 
    auto spider1 = Sprite::create("spider.png", Rect(0, 0, 100, 100));
    spider1->setPosition(Vec2(vSize.width / 2 - 100, vSize.height - 120));
    this->addChild(spider1);
    spider1->runAction(repeat_move);
    // 
    auto spider2 = Sprite::create("spider.png", Rect(0, 0, 100, 100));
    spider2->setPosition(Vec2(vSize.width / 2, vSize.height - 120));
    this->addChild(spider2);
    spider2->runAction(repeat_atk);
    // 
    auto spider3 = Sprite::create("spider.png", Rect(0, 0, 100, 100));
    spider3->setPosition(Vec2(vSize.width / 2 + 100, vSize.height - 120));
    this->addChild(spider3);
    spider3->runAction(repeat_jump);
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