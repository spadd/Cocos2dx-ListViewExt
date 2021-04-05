

#ifndef __TableViewExt_SCENE_H__
#define __TableViewExt_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;

class ITableView {
public:
	virtual Size sizeSource(int index)=0;
	virtual Node* loadSource(int index)=0;
	virtual void unloadSource(int index)=0;
};

class TableViewExt : Ref
{
private:
	ListView* self;
	float _checkOffsetX;
	float _checkOffsetY;
	float _checkWidth;
	float _checkHeight;
	float _innerP;
	int _headIndex;
	int _tailIndex;
	int nil;
	ITableView* _impl;
	
	Node* _loadSource(int index);

	void _jumpTo(Ref* i,int index);
public:
	TableViewExt();
	~TableViewExt();

	Layout* createDefaultWidget();
	void initDefaultItems(int total);
	void insertRow(int index);
	void deleteRow(int index);
	void jumpTo(int index);

	bool checkInView(Node* item);
	void scrolling();
	void attachTo(ListView* lv, ITableView* impl);

	void performWithDelay(CallFunc* func);
};

#endif // __TableViewExt_SCENE_H__
