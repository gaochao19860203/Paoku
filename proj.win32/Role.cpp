#include "Role.h"

Role::Role():xSpeed(5),ySpeed(15),constXspeed(xSpeed),constYspeed(ySpeed), accelerate(0.94){
}

Role::~Role(){
}

bool Role::init(){
	return true;
}

void Role::extraInit(){
	
	CCTMXObjectGroup* group = this->getGameMap()->objectGroupNamed("Role");
	CCDictionary* dic = group->objectNamed("player");
	float x = dic->valueForKey("x")->floatValue();
	float y = dic->valueForKey("y")->floatValue();

	mSprite = CCSprite::create("CloseNormal.png");
	mSprite->setAnchorPoint( CCPointZero );
	mSprite->setPosition( ccp(x, y) );

	this->getGameMap()->addChild( mSprite, ROLE_ZORDER );

	this->setCurrentRoleState(ROLE_RUN);

	this->setMapPointByRole();

	this->schedule( schedule_selector( Role::role_update ), 0.02f );

	this->schedule( schedule_selector( Role::camera_update ), 0.02f );
}

Role* Role::createWithTMX( CCTMXTiledMap* _map ){
	Role* role = new Role();

	if(role && role->init()){
		role->setGameMap(_map);
		role->extraInit();
		role->autorelease();
		return role;
	}

	CC_SAFE_DELETE(role);
	return NULL;
}

void Role::role_update( float dt ){
	role_logic();
}

// 设置镜头的视角
void Role::setMapPointByRole(){
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCSize mapTiledNum = this->getGameMap()->getMapSize();
	CCSize tiledSize = this->getGameMap()->getTileSize();
	CCSize mapSize = CCSizeMake( mapTiledNum.width * tiledSize.width, mapTiledNum.height * tiledSize.height );
	
	CCPoint rolePoint = mSprite->getPosition();
	
	float x = rolePoint.x > visibleSize.width/4 ? rolePoint.x : visibleSize.width/4;
	x = x < mapSize.width - visibleSize.width*.8 ? x : mapSize.width - visibleSize.width*.8;
	
	float y = rolePoint.y > visibleSize.height/3 ? rolePoint.y : visibleSize.height/3;
	y = y < mapSize.height/2 ? y : mapSize.height/2;

	CCPoint descPoint = ccp( x, y );  //目标点
	CCPoint centerPoint = ccp( visibleSize.width/5, visibleSize.height/4 );
	CCPoint rltPoint = ccpSub( centerPoint, descPoint );
	this->getGameMap()->setPosition(rltPoint);
}

void Role::camera_update( float dt ){
	this->setMapPointByRole();
}

/***

用于与山体碰撞的函数

***/
bool Role::isCollWithCollArea( COLL_STATE _collState ){
	bool bFlag = false;

	CCTMXObjectGroup* collGroup = this->getGameMap()->objectGroupNamed("coll");
	CCObject* obj = NULL;
	CCARRAY_FOREACH( collGroup->getObjects(), obj ){
		CCDictionary* dic = (CCDictionary*)obj;

		float x = dic->valueForKey("x")->floatValue();
		float y = dic->valueForKey("y")->floatValue();
		float width = dic->valueForKey("width")->floatValue();
		float height = dic->valueForKey("height")->floatValue();

		CCRect box = CCRectMake( x, y, width, height );

		switch( _collState ){
			case COLL_TOP:
				bFlag = this->isCollisionWithTop( box );
				break;
			case COLL_BOTTOM:
				bFlag = this->isCollisionWithBottom( box );
				break;
			case COLL_LEFT:
				bFlag = this->isCollisionWithLeft( box );
				break;
			case COLL_RIGHT:
				bFlag = this->isCollisionWithRight( box );
				break;
			default:
				break;
		}

		if(bFlag){
			this->fixColl( _collState, box );
		}

	}
	
	return bFlag;
}

bool Role::isCollisionWithTop( CCRect box ){
	//获取人物上边框中间点
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMidX(), roleBox.getMaxY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithBottom( CCRect box ){
	//获取人物下边框中间点
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMidX(), roleBox.getMinY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithLeft( CCRect box ){
	//获取人物左边框中间点
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMinX(), roleBox.getMidY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithRight( CCRect box ){
	//获取人物右边框中间点
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMaxX(), roleBox.getMidY() );

	return box.containsPoint(rolePoint);
}

void Role::runLogic(){
	/**
		先判断向右方向的碰撞
		再判断向上方向的碰撞


		1. 获取人物的位置点
		2. 结合速度去修改位置点
		3. 传给新的人物位置
	**/

	if(this->isCollWithCollArea( COLL_RIGHT )){
		// 碰到墙了，状态转换为stand_by
		this->setCurrentRoleState( ROLE_STAND_BY );
		CCLOG("%s \n", "convert to stand by");
		return;
	}

	if( !this->isCollWithCollArea( COLL_BOTTOM ) ){
		// 如果不与地面接触，状态转换为落下状态
		this->setCurrentRoleState( ROLE_JUMP_DOWN );
		CCLOG("%s \n", "convert to jump down");
		return;
	}

	CCPoint preRolePoint = this->getCurrentRolePoint();

	CCPoint targetRolePoint = ccp( preRolePoint.x + xSpeed, preRolePoint.y );

	this->setCurrentRolePoint( targetRolePoint );

}

/**
	跳起状态
**/
void Role::jumpUpLogic(){
	if( ySpeed<=0 ){
		// 转为下落状态
		ySpeed = 0;
		this->setCurrentRoleState( ROLE_JUMP_DOWN );
		CCLOG("%s \n", "jumpUpLogic turn to jump down");
		return;
	}
	CCPoint roleCurrentPoint = this->getCurrentRolePoint();

	float xValue = xSpeed + roleCurrentPoint.x;
	ySpeed -= accelerate;
	float yValue = ySpeed + roleCurrentPoint.y;

	this->setCurrentRolePoint( ccp(xValue, yValue) );
}

/**
落下状态
**/
void Role::jumpDownLogic(){
	/**
		1.判断与地面（Y方向）的碰撞顺序
		2.判断与X方向

	**/
	if(this->isCollWithCollArea( COLL_BOTTOM )){
		ySpeed = constYspeed;
		this->setCurrentRoleState( ROLE_RUN );
		
		return;
	}
	CCPoint roleCurrentPoint = this->getCurrentRolePoint();
	ySpeed += accelerate;
	if(ySpeed>=15){
		ySpeed = 15;
	}
	CCPoint rlt = ccp( xSpeed, -ySpeed );
	this->setCurrentRolePoint( roleCurrentPoint + rlt );
}

void Role::role_logic(){
	//DLog::showLog("asdflkajsdlfk");
	//CCLOG("%s \n", "hahah");
	switch(this->getCurrentRoleState()){
		case ROLE_INVAILD_STATE:
			break;
		case ROLE_RUN:
			runLogic();
			break;
		case ROLE_JUMP_UP:
			jumpUpLogic();
			break;
		case ROLE_JUMP_DOWN:
			jumpDownLogic();
			break;
		case ROLE_STAND_BY:
			break;
		default:
			break;
	}
}

/***
	修复碰撞
***/
void Role::fixColl( COLL_STATE _collState, CCRect box ){
	/***
		1.算出相交的矩形
		2.根据方向将人物拉回到应用的位置上
	**/
	CCRect roleBox = mSprite->boundingBox();
	
	float maxX = roleBox.getMaxX() > box.getMaxX() ? box.getMaxX():roleBox.getMaxX();
	float minX = roleBox.getMinX() > box.getMinX() ? roleBox.getMinX():box.getMinX();

	float maxY = roleBox.getMaxY() > box.getMaxY() ? box.getMaxY():roleBox.getMaxY();
	float minY = roleBox.getMinY() > box.getMinY() ? roleBox.getMinY():box.getMinY();

	float width = maxX - minX;
	float height = maxY - minY;

	CCRect inBox = CCRectMake(minX, maxY, width, height);

	if( _collState == COLL_BOTTOM ){
		mSprite->setPositionY( mSprite->getPositionY() + height );
		return;
	}

	if( _collState == COLL_TOP ){
		mSprite->setPositionY( mSprite->getPositionY() - height );
		return;
	}

	if( _collState == COLL_LEFT ){
		mSprite->setPositionX(mSprite->getPositionX() + width);
		return;
	}

	if( _collState == COLL_RIGHT ){
		mSprite->setPositionX(mSprite->getPositionX() - width);
		return;
	}

}

void Role::setCurrentRolePoint(CCPoint currentRolePoint){
	mSprite->setPosition( currentRolePoint );
}

CCPoint Role::getCurrentRolePoint(){
	return mSprite->getPosition();
}
