//////////////////////////////////////////////////////
// Sprite.h
//////////////////////////////////////////////////////

#pragma once

#include <windows.h>

class CSprite
{
public:
	enum SPRITE_TYPE {TROLL, COIN, HERO};

	// Constructor - initializes the sprites width and height
	CSprite(int iWidth, int iHeight, SPRITE_TYPE SpriteType);
	// Destructor
	~CSprite(void);

	// Sets the sprites X and Y position
	void SetPosition(float fXPos, float fYPos);
	// Sets the sprites X position
	void SetXPosition(float fXPos);
	// Returns the sprites X position
	float GetXPosition(void);
	// Sets the sprites Y position
	void SetYPosition(float fYPos);
	// Returns the sprites Y position
	float GetYPosition(void);
	// Sets the sprites speed
	void SetSpeed(float fSpeed);
	// Gets the sprites speed
	float GetSpeed(void);
	// Set whether the sprite has collided with something
	void SetHit(BOOL fHit);
	// Get whether the sprite has collided with something
	BOOL GetHit(void);
	// Sets the RECT for this sprite
	void SetRECT(RECT rect);
	// Returns the RECT for this sprite
	RECT GetRECT(void);
	// Sets the RECTs value for this sprite
	void SetRECTValues(LONG lLeft, LONG lRight, LONG lTop, LONG lBottom);
	// Returns the sprites width
	int GetWidth(void);
	// Returns the sprites height
	int GetHeight(void);
	// Returns the sprites type
	SPRITE_TYPE GetSpriteType(void);

protected:

private:
	int m_iWidth;
	int m_iHeight;
	float m_fXPos;
	float m_fYPos;
	float m_fSpeed;
	BOOL m_fHit;
	RECT m_Rect;
	SPRITE_TYPE m_SpriteType;
};
