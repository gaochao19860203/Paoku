#include "GameFatherLayer.h"
#include "Role.h"

GameFatherLayer::GameFatherLayer(){

}

GameFatherLayer::~GameFatherLayer(){

	CC_SAFE_RELEASE_NULL(m_GameMap);

}

bool GameFatherLayer::init(){
	if(!CCLayer::init()){
		return false;
	}

	initUI();

	initMap();

	initRole();

	this->setTouchEnabled(true);

	return true;
}

void GameFatherLayer::initUI(){
	
}

void GameFatherLayer::initMap(){
	m_GameMap = CCTMXTiledMap::create("map_4_game.tmx");

	CC_SAFE_RETAIN(m_GameMap);

	this->addChild(m_GameMap, 1);
}

void GameFatherLayer::initRole(){
	mRole = Role::createWithTMX( m_GameMap );
	this->addChild(mRole);
}

CCScene* GameFatherLayer::scene(){
	CCScene* sc = CCScene::create();
	sc->addChild(GameFatherLayer::create());

	return sc;
}

bool GameFatherLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent ){
	dispatchTouchRoleMove();
	return true;
}

void GameFatherLayer::onEnter(){
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate( this, 0, true );
}

void GameFatherLayer::onExit(){
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate( this );
	CCLayer::onExit();
}

void GameFatherLayer::dispatchTouchRoleMove(){
	if( mRole->getCurrentRoleState() == ROLE_RUN ){
		mRole->setCurrentRoleState( ROLE_JUMP_UP );
		return;
	}

}
