/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
//#include "SimpleAudioEngine.h"
//#include "ajson/ajson.hpp"

using namespace std;
//using namespace ajson;

USING_NS_CC;

//struct demo
//{
//	string hello;
//	string world;
//};
//AJSON(demo, hello, world)

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    ListView* lv = ListView::create();
    lv->setContentSize(Size(300,200));
    lv->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    lv->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    lv->setBackGroundColor(Color3B(0,100,0));
    lv->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    lv->setItemsMargin(4);
    lv->retain();
    addChild(lv);

    TableView = std::make_unique<TableViewExt>();
    TableView->attachTo(lv, this);
    TableView->initDefaultItems(50);

    TableView->jumpTo(1);
  
    //runAction(Sequence::create(
    //    DelayTime::create(0),
    //    CallFunc::create(CC_CALLBACK_0(HelloWorld::deayFunc, this)),
    //    nullptr)
    //);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::deayFunc()
{
    TableView->jumpTo(10);
}

Size HelloWorld::sizeSource(int index)
{
    return Size(300, 15);
}

Node* HelloWorld::loadSource(int index)
{
    Text* t = Text::create(std::to_string(index), "fonts/Arial.ttf", 18);
    return t;
}

void HelloWorld::unloadSource(int index)
{
    //CCLOG("do texture unload here:%d", index);
}

