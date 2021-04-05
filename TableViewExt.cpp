
#include "TableViewExt.h"

#include<algorithm>
Layout* TableViewExt::createDefaultWidget()
{
	Layout* layer = Layout::create();
	return layer;
}

void TableViewExt::initDefaultItems(int total)
{
	auto items = self->getItems();
	int oldTotal = items.size();

	//CCLOG("initDefaultItems self._headIndex=%d", _headIndex);
	//CCLOG("initDefaultItems self._tailIndex=%d", _tailIndex);

	for (int i = _headIndex; i < _tailIndex; i++)
	{
		items.at(i)->removeAllChildren();
		_impl->unloadSource(i);
	}
	_headIndex = 0;
	_tailIndex = -1;
	_innerP = nil;
	self->removeAllItems();

	CCLOG("initDefaultItems total=%d", total);

	for (int i = 0; i < total; i++)
	{
		auto widget = createDefaultWidget();
		widget->setContentSize(_impl->sizeSource(i));
		self->pushBackCustomItem(widget);
	}
}

void TableViewExt::insertRow(int index)
{
	auto widget = createDefaultWidget();
	widget->setContentSize(_impl->sizeSource(index));
	self->insertCustomItem(widget,index-1);
	if (index > _tailIndex)
	{
		return;
	}
	if (index<=_headIndex) 
	{
		index = _headIndex;
	}
	auto item = self->getItem(index-1);
	item->addChild(_loadSource(index));
	_tailIndex += 1;
}

void TableViewExt::deleteRow(int index)
{
	self->removeItem(index-1);
	if (index>_tailIndex) 
	{
		return;
	}
	if (index<_headIndex) 
	{
		_headIndex -= 1;
	}
	else 
	{
		_impl->unloadSource(index);
	}
	auto item = self->getItem(_tailIndex-1);
	if (item) 
	{
		item->addChild(_loadSource(_tailIndex));
	}
	else 
	{
		_tailIndex -= 1;
	}
}

void TableViewExt::_jumpTo(Ref* i,int index)
{
	
	ScrollView::Direction direction = self->getDirection();
	std::map<int, bool> checkTab;
	Size size = self->getContentSize();
	auto items = self->getItems();
	for (int i = _headIndex; i < _tailIndex; i++)
	{
		checkTab.insert(std::pair<int, bool>(i, false));
	}
	float const z = 0;
	auto item = items.at(index);

	//CCLOG("_jumpTo index=%d", index);
	//CCLOG("_jumpTo items.size()=%d", items.size());

	if (ScrollView::Direction::VERTICAL == direction)
	{
		float y = item->getPositionY();
		float height = item->getContentSize().height;
		float destY = size.height - y - height;

		destY = std::min(z, destY);
		self->getInnerContainer()->setPositionY(destY);
		_innerP = destY;
	}
	else
	{
		float destX = size.width - item->getPositionX() - item->getContentSize().width;
		destX = std::min(z, destX);
		self->getInnerContainer()->setPositionX(destX);
		_innerP = destX;
	}

	for (int i = index; i >= 0; i--)
	{
		if (!checkInView(items.at(i)))
		{
			break;
		}
		_headIndex = i;
	}
	for (int i = index; i < items.size(); i++)
	{
		if (!checkInView(items.at(i)))
		{
			break;
		}
		_tailIndex = i;
	}
	for (int i = _headIndex; i <= _tailIndex; i++)
	{
		if (checkTab.find(i) == checkTab.end())
		{
			items.at(i)->addChild(_loadSource(i));
		}
		checkTab[i] = true;
	}

	for (const auto& p : checkTab)
	{
		if (false == p.second)
		{
			items.at(p.first)->removeAllChildren();
			_impl->unloadSource(p.first);
		}
	}

	//CCLOG("_jumpTo self._headIndex=%d", _headIndex);
	//CCLOG("_jumpTo self._tailIndex=%d", _tailIndex);
}


void TableViewExt::jumpTo(int index)
{
	// 不能小于 0
	if (index < 0) 
	{
		index = 0;
	}
	self->stopAllActions();
	CallFuncN* func = CallFuncN::create(CC_CALLBACK_1(TableViewExt::_jumpTo, this, index));
	performWithDelay(func);
}

void TableViewExt::performWithDelay(CallFunc* func)
{
	self->runAction(Sequence::create(
		func,
		DelayTime::create(0),
		nullptr)
	);
}

Node* TableViewExt::_loadSource(int index)
{
	Node* node = _impl->loadSource(index);

	node->ignoreAnchorPointForPosition(false);
	node->setAnchorPoint(Vec2::ZERO);
	node->setPosition(Vec2::ZERO);
	return node;
}

TableViewExt::TableViewExt():nil(-999)
{
}

TableViewExt::~TableViewExt()
{
	_impl = nullptr;
	self = nullptr;
}

bool TableViewExt::checkInView(Node* item)
{
	Vec2 posA = item->convertToWorldSpace(Vec2::ZERO);
	posA = self->convertToNodeSpace(posA);
	posA.x = posA.x + _checkOffsetX;
	posA.y = posA.y + _checkOffsetY;
	Size const sizeA = item->getContentSize();
	// AABB
	float const centerXdelta = (sizeA.width + _checkWidth) / 2;
	float const centerYdelta = (sizeA.height + _checkHeight) / 2;

	//CCLOG("checkInView centerXdelta=%f", centerXdelta);
	//CCLOG("checkInView centerYdelta=%f", centerYdelta);
	//bool b1 = abs((posA.x + sizeA.width / 2) - (_checkWidth / 2)) <= centerXdelta;
	//bool b2 = abs((posA.y + sizeA.height / 2) - (_checkHeight / 2)) <= centerYdelta;
	//bool b3 = b1 && b2;
	//CCLOG("b1=%d,b2=%d,b3=%d", b1,b2,b3);

	//float i1 = abs((posA.x + sizeA.width / 2) - (_checkWidth / 2));
	//float i2 = abs((posA.y + sizeA.height / 2) - (_checkHeight / 2));
	//CCLOG("x = %f, y = %f | %f=%f", i1, i2, centerXdelta, centerYdelta);

	if (abs((posA.x + sizeA.width / 2) - (_checkWidth / 2)) <= centerXdelta && abs((posA.y + sizeA.height / 2) - (_checkHeight / 2)) <= centerYdelta)
	{
		//CCLOG("checkInView true");
		return true;
	}
	//CCLOG("checkInView false");
	return false;
}

void TableViewExt::scrolling()
{
	//CCLOG("scrolling self._headIndex=  %d", _headIndex);
	//CCLOG("scrolling self._tailIndex= %d", _tailIndex);

	if (_headIndex== nil)
	{
		for (int i = _tailIndex; i >= 0; i--)
		{
			Widget* item = self->getItem(i-1);
			if (!checkInView(item)) 
			{
				break;
			}
			if (item->getChildren().size()>0) 
			{
				break;
			}
			item->addChild(_loadSource(i));
			_headIndex = i;
		}
		//CCLOG("scrolling self._headIndex == nil ,%d", _headIndex);
		return;
	}
	if (_tailIndex == nil)
	{
		Vector<Widget*> items = self->getItems();
		for (int i = 0; i < items.size(); i++)
		{
			Widget* item = items.at(i);
			if (!checkInView(item))
			{
				break;
			}
			if (item->getChildren().size() > 0)
			{
				break;
			}
			item->addChild(_loadSource(i));
			_tailIndex = i;
		}
		//CCLOG("scrolling self._tailIndex == nil ,%d", _tailIndex);
		return;
	}

	if (_innerP==nil) 
	{
		//CCLOG("scrolling _innerP == nil");
		return;
	}

	ScrollView::Direction direction = self->getDirection();
	bool isForward = false;
	if (ScrollView::Direction::VERTICAL==direction) 
	{
		float py = self->getInnerContainer()->getPositionY();
		if (_innerP < py) 
		{
			isForward = true;
		}
		_innerP = py;
	}
	else
	{
		float px = self->getInnerContainer()->getPositionX();
		if (_innerP > px)
		{
			isForward = true;
		}
		_innerP = px;
	}

	// CCLOG("scrolling isForward = ,%d", isForward);
	Node* item;
	if (isForward) 
	{
		CCLOG("scrolling isForward = true, _tailIndex = %d", _tailIndex - 1);
		item = self->getItem(_tailIndex-1);
		if (checkInView(item)) 
		{
			do
			{
				item = self->getItem(_tailIndex);
				if (nullptr == item) 
				{
					break;
				}
				_tailIndex += 1;
				item->addChild(_loadSource(_tailIndex));
			} while (true);

			do
			{
				item = self->getItem(_headIndex-1);
				if (checkInView(item)) 
				{
					break;
				}
				item->removeAllChildren();
				_impl->unloadSource(_headIndex);
				_headIndex += 1;
			} while (true);
		}
		else 
		{
			for (int i = _headIndex; i <= _tailIndex; i++)
			{
				item = self->getItem(i-1);
				item->removeAllChildren();
				_impl->unloadSource(i);
				_headIndex = nil;
			}

			do
			{
				item = self->getItem(_tailIndex);
				if (nullptr == item)
				{
					break;
				}
				if (checkInView(item))
				{
					_tailIndex += 1;
					item->addChild(_loadSource(_tailIndex));
					if (nil == _headIndex) 
					{
						_headIndex = _tailIndex;
					}
				}
				else 
				{
					if (nil!=_headIndex) 
					{
						break;
					}
					else 
					{
						_tailIndex += 1;
					}
				}
			} while (true);
		}
	}
	else 
	{
		CCLOG("scrolling isForward = false, _headIndex = %d", _headIndex - 1);
		item = self->getItem(_headIndex - 1);
		if (checkInView(item)) 
		{
			do
			{
				item = self->getItem(_headIndex - 2);
				if (nullptr == item) 
				{
					break;
				}
				if (!checkInView(item))
				{
					break;
				}
				_headIndex -= 1;
				item->addChild(_loadSource(_headIndex));
			} while (true);

			do
			{
				item = self->getItem(_tailIndex-1);
				if (checkInView(item))
				{
					break;
				}
				item->removeAllChildren();
				_impl->unloadSource(_tailIndex);
				_tailIndex -= 1;

			} while (true);

		}
		else 
		{
			for (int i = _headIndex; i <= _tailIndex; i++)
			{
				item = self->getItem(i-1);
				item->removeAllChildren();
				_impl->unloadSource(i);
				_tailIndex = nil;
			}

			do
			{
				item = self->getItem(_headIndex - 2);
				if (nullptr == item) 
				{
					break;
				}
				if (checkInView(item)) 
				{
					_headIndex -= 1;
					item->addChild(_loadSource(_headIndex));
					if (nil == _tailIndex) 
					{
						_tailIndex = _headIndex;
					}
				}
				else 
				{
					if (nil != _tailIndex) 
					{
						break;
					}
					else 
					{
						_headIndex -= 1;
					}
				}

			} while (true);
		}
	}

}


void TableViewExt::attachTo(ListView* listview, ITableView* impl)
{
	_impl = impl;
	self = listview;

	Size const size = listview->getContentSize();
	_checkOffsetX = size.width / 2;
	_checkOffsetY = size.height / 2;
	_checkWidth = size.width * 2;
	_checkHeight = size.height * 2;

	CCLOG("_checkOffsetX = %f", _checkOffsetX);
	CCLOG("_checkOffsetY = %f", _checkOffsetY);
	CCLOG("_checkWidth = %f", _checkWidth);
	CCLOG("_checkHeight = %f", _checkHeight);

	_headIndex = 0;
	_tailIndex = -1;

	listview->addEventListener([&](Ref*, ui::ScrollView::EventType e)
		{
			switch (e)
			{
			//case cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP:
			//	break;
			//case cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
			//	break;
			//case cocos2d::ui::ScrollView::EventType::SCROLL_TO_LEFT:
			//	break;
			//case cocos2d::ui::ScrollView::EventType::SCROLL_TO_RIGHT:
			//	break;
			case cocos2d::ui::ScrollView::EventType::SCROLLING:
			case cocos2d::ui::ScrollView::EventType::BOUNCE_TOP:
			case cocos2d::ui::ScrollView::EventType::BOUNCE_BOTTOM:
			case cocos2d::ui::ScrollView::EventType::BOUNCE_LEFT:
			case cocos2d::ui::ScrollView::EventType::BOUNCE_RIGHT:
			case cocos2d::ui::ScrollView::EventType::CONTAINER_MOVED:
			case cocos2d::ui::ScrollView::EventType::SCROLLING_BEGAN:
			case cocos2d::ui::ScrollView::EventType::SCROLLING_ENDED:
			case cocos2d::ui::ScrollView::EventType::AUTOSCROLL_ENDED:
			{
				performWithDelay(CallFunc::create(CC_CALLBACK_0(TableViewExt::scrolling, this)));
			}
				break;
			default:
				break;
			}
		});
}
