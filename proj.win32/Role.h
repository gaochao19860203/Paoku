#pragma once
#include "MHeaders.h"

enum ROLE_STATE {
	ROLE_INVAILD_STATE = 0,
	ROLE_RUN,
	ROLE_JUMP_UP,
	ROLE_JUMP_DOWN,
	ROLE_STAND_BY
};

enum COLL_STATE{
	COLL_INVALID_STATE = 0,
	COLL_TOP,
	COLL_BOTTOM,
	COLL_LEFT,
	COLL_RIGHT
};

class Role : public CCNode {

public:
	Role();
	~Role();

	virtual bool init();

	static Role* createWithTMX( CCTMXTiledMap* map );

	CC_SYNTHESIZE( CCTMXTiledMap*, gameMap, GameMap );

	CC_PROPERTY( CCPoint, currentRolePoint, CurrentRolePoint);

	CC_SYNTHESIZE( ROLE_STATE, currentRoleState, CurrentRoleState );

	void role_update( float dt );

	void role_logic();

	CC_SYNTHESIZE(float, xSpeed, XSpeed);

	CC_SYNTHESIZE(float, ySpeed, YSpeed);

	CC_SYNTHESIZE(float, constXspeed, ConstXspeed);

	CC_SYNTHESIZE(float, constYspeed, ConstYspeed);

private:
	CCSprite* mSprite;

	void extraInit();

	void runLogic();

	void jumpUpLogic();

	void jumpDownLogic();

	void setMapPointByRole(); //根据人物的位置点设置镜头

	void camera_update( float dt );

	bool isCollWithCollArea( COLL_STATE _collState);	//碰撞区检测函数

	bool isCollisionWithTop( CCRect box );

	bool isCollisionWithBottom( CCRect box );

	bool isCollisionWithLeft( CCRect box );

	bool isCollisionWithRight( CCRect box );

	float accelerate;

	void fixColl( COLL_STATE _collState, CCRect box );
};

