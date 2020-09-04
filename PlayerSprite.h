//////////////////////////////////////////////////////
// PlayerSprite.h
//////////////////////////////////////////////////////

#pragma once

#include "sprite.h"

class CPlayerSprite : public CSprite
{
public:
	enum DIRECTION {NORTH, EAST, SOUTH, WEST, STATIONARY};

	// Constructor
	CPlayerSprite(int iWidth, int iHeight, SPRITE_TYPE SpriteType);
	// Destructor
	~CPlayerSprite(void);

	//
	void SetDirection(DIRECTION dir);
	//
	DIRECTION GetDirection(void);
	//
	void UpdatePlayer(void);
	//
	void InitPlayer(float winWidth, float winHeight, float speed, float xOffset = 0, float yOffset = 0);

private:
	DIRECTION m_Direction;
};
