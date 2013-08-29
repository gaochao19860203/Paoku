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

// ���þ�ͷ���ӽ�
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

	CCPoint descPoint = ccp( x, y );  //Ŀ���
	CCPoint centerPoint = ccp( visibleSize.width/5, visibleSize.height/4 );
	CCPoint rltPoint = ccpSub( centerPoint, descPoint );
	this->getGameMap()->setPosition(rltPoint);
}

void Role::camera_update( float dt ){
	this->setMapPointByRole();
}

/***

������ɽ����ײ�ĺ���

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
	//��ȡ�����ϱ߿��м��
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMidX(), roleBox.getMaxY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithBottom( CCRect box ){
	//��ȡ�����±߿��м��
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMidX(), roleBox.getMinY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithLeft( CCRect box ){
	//��ȡ������߿��м��
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMinX(), roleBox.getMidY() );

	return box.containsPoint(rolePoint);
}

bool Role::isCollisionWithRight( CCRect box ){
	//��ȡ�����ұ߿��м��
	CCRect roleBox = mSprite->boundingBox();
	CCPoint rolePoint = ccp( roleBox.getMaxX(), roleBox.getMidY() );

	return box.containsPoint(rolePoint);
}

void Role::runLogic(){
	/**
		���ж����ҷ������ײ
		���ж����Ϸ������ײ


		1. ��ȡ�����λ�õ�
		2. ����ٶ�ȥ�޸�λ�õ�
		3. �����µ�����λ��
	**/

	if(this->isCollWithCollArea( COLL_RIGHT )){
		// ����ǽ�ˣ�״̬ת��Ϊstand_by
		this->setCurrentRoleState( ROLE_STAND_BY );
		CCLOG("%s \n", "convert to stand by");
		return;
	}

	if( !this->isCollWithCollArea( COLL_BOTTOM ) ){
		// ����������Ӵ���״̬ת��Ϊ����״̬
		this->setCurrentRoleState( ROLE_JUMP_DOWN );
		CCLOG("%s \n", "convert to jump down");
		return;
	}

	CCPoint preRolePoint = this->getCurrentRolePoint();

	CCPoint targetRolePoint = ccp( preRolePoint.x + xSpeed, preRolePoint.y );

	this->setCurrentRolePoint( targetRolePoint );

}

/**
	����״̬
**/
void Role::jumpUpLogic(){
	if( ySpeed<=0 ){
		// תΪ����״̬
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
����״̬
**/
void Role::jumpDownLogic(){
	/**
		1.�ж�����棨Y���򣩵���ײ˳��
		2.�ж���X����

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
	�޸���ײ
***/
void Role::fixColl( COLL_STATE _collState, CCRect box ){
	/***
		1.����ཻ�ľ���
		2.���ݷ����������ص�Ӧ�õ�λ����
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
