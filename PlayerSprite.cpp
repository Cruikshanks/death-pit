//////////////////////////////////////////////////////
// PlayerSprite.cpp
//////////////////////////////////////////////////////

#include "playersprite.h"

//////////////////////////////////////////////////////
// PlayerSprite()
//////////////////////////////////////////////////////
CPlayerSprite::CPlayerSprite(int iWidth, int iHeight, SPRITE_TYPE SpriteType) :
    CSprite(iWidth, iHeight, SpriteType)
{
	m_Direction = CPlayerSprite::STATIONARY;
}

//////////////////////////////////////////////////////
// ~PlayerSprite()
//////////////////////////////////////////////////////
CPlayerSprite::~CPlayerSprite(void)
{
}

//////////////////////////////////////////////////////
// SetDirection()
//////////////////////////////////////////////////////
void CPlayerSprite::SetDirection(CPlayerSprite::DIRECTION dir)
{
	m_Direction = dir;
}

//////////////////////////////////////////////////////
// GetDirection()
//////////////////////////////////////////////////////
CPlayerSprite::DIRECTION CPlayerSprite::GetDirection(void)
{
	return m_Direction;
}

//////////////////////////////////////////////////////
// UpdatePlayer()
//////////////////////////////////////////////////////
void CPlayerSprite::UpdatePlayer(void)
{
	switch (m_Direction)
	{
	case CPlayerSprite::NORTH:
		SetYPosition(GetYPosition() - GetSpeed());
		break;
	case CPlayerSprite::EAST:
		SetXPosition(GetXPosition() + GetSpeed());
		break;
	case CPlayerSprite::SOUTH:
		SetYPosition(GetYPosition() + GetSpeed());
		break;
	case CPlayerSprite::WEST:
		SetXPosition(GetXPosition() - GetSpeed());
		break;
	}
}

//////////////////////////////////////////////////////
// InitPlayer()
//////////////////////////////////////////////////////
void CPlayerSprite::InitPlayer(float winWidth, float winHeight, float speed, float xOffset, float yOffset)
{
	SetPosition(((winWidth/2) - (GetWidth()/2)) + xOffset,
		((winHeight/2) - (GetHeight()/2)) + yOffset);
	SetSpeed(speed);
	SetDirection(CPlayerSprite::STATIONARY);
}