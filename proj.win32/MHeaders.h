#ifndef CescPaokuGame_MHeaders_h
#define CescPaokuGame_MHeaders_h

#include "cocos2d.h"
#include "DLog.h"

const int ROLE_ZORDER = 10;

USING_NS_CC;

inline CCPoint operator +(CCPoint p1, CCPoint p2){
	return ccp(p1.x + p2.x, p1.y + p2.y);
}

inline CCPoint operator -(CCPoint p1, CCPoint p2){
	return ccp(p1.x - p2.x, p1.y - p2.y);
}

#endif