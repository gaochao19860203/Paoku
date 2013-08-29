#include "MHeaders.h"

class Role;

class GameFatherLayer : public CCLayer
{
public:
	GameFatherLayer();
	~GameFatherLayer();

	CREATE_FUNC( GameFatherLayer );
	
	virtual bool init();

	void initUI();

	static CCScene* scene();

	void initMap();

	void initRole();

	virtual bool ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent );

	virtual void onEnter();

	virtual void onExit();

	void dispatchTouchRoleMove();

private:
	CCTMXTiledMap* m_GameMap;

	Role* mRole;
};

