//////////////////////////////////////////////////////
// Engine.h
//////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <time.h>
#include <fstream>
#include <d3d8.h>

#include "Constants.h"
#include "Direct3D.h"
#include "DirectSound.h"
#include "Sprite.h"
#include "PlayerSprite.h"

const MAX_HSCORES = 10;

struct HSCORE
{
	int iScore;
	char pstrName[6];
};

class CEngine
{
public:
	
	enum SPRITETILEID {TROLL, COIN, HERO};
	enum GAMELEVEL {LEVEL1, LEVEL2, LEVEL3, ENDGAME};
	enum GAMESTATE {GS_LOADING, GS_TITLE, GS_NEWGAME, GS_RUNNING, GS_STOPPED, GS_LOST, GS_WON, GS_HSCORE, GS_EXIT};

	// Constructor
	CEngine(void);
	// Destructor
	~CEngine(void);

	// Sets the class's copy of the window handle
	void SetWindowHandle(HWND hWnd);
	// Processes game data during the appilication's idle time
	void ProcessGame(void);
	// Handles keystrokes for the application
	void HandleKeys(WPARAM wParam);
	// Returns a pointer to the class's CDirect3D object
	CDirect3D* GetCDirect3DObj();
	// Returns a pointer to the class's CDirectSound object
	CDirectSound* GetCDirectSoundObj();
	// Renders the back buffer ready for display
	void PaintBackBuffer();

protected:
	HWND m_hWnd;				// Holds the application's window handle
	CDirect3D m_Direct3D;		// Holds the class's CDirect3D object
	CDirectSound m_DirectSound;	// Holds the class's CDirectSound object
	BOOL m_bGamePaused;			// Indicates whether the game is paused due to having lost its Direct3D device
	BOOL m_bHighScoreEntered;	// Indicates whether the player has completed high score entry or not

	CPlayerSprite* m_PlayerSprite;
	CSprite* m_TrollSprite[NUM_SPRITES];
	CSprite* m_CoinSprite[NUM_SPRITES];
	HSCORE m_HScores[MAX_HSCORES];
	char m_pstrMsg[MAX_PATH];
	int m_iPlayerScore;
	int m_iPlayerLives;
	DWORD m_dwGameTime;
	DWORD m_dwCurrentTime;
	int m_iDisplayTime;
	int m_iCounter;
	GAMESTATE m_GameState;
	GAMELEVEL m_GameLevel;

	// 
	void HandleLoading(void);
	//
	void HandleTitle(void);
	// 
	void HandleNewGame(void);
	//
	void HandleRunning(void);
	//
	void HandleStopped(void);
	//
	void HandleLost(void);
	//
	void HandleWon(void);
	//
	void HandleHScore(void);
	//
	void InitTrolls(void);
	//
	void InitCoins(void);
	//
	void InitGame(void);
	// Resets game for next level
	void NextLevel(void);
	// Handle down arrow key
	void HandleDownArrow(void);
	// Handle up arrow key
	void HandleUpArrow(void);
	// Handle left arrow key
	void HandleLeftArrow(void);
	// Handle right arrow key
	void HandleRightArrow(void);
	// Handle space key
	void HandleSpace(void);
	// Handle 's' key
	void HandleS(void);
	// Handle escape key
	void HandleEscape(void);
	// Updates the game time
	void UpdateDisplayTime(void);
	// Checks for collision between two given sprites
	BOOL SpriteCollision(float sprite1X, float sprite1Y, int width1, int height1,
		float sprite2X, float sprite2Y, int width2, int height2);
	// Checks for collision with a given border size
	BOOL BorderCollision(CSprite* sprite, int minWidth, int maxWidth, int minHeight, int maxHeight);

private:	
	//
	void CleanUp(void);
	// Restores the Direct3D device and its surfaces/textures
	void ResetDirect3D(void);
	//
	void OpenHScoreFile(void);
	//
	void SaveHScoreFile(void);
	//
	void PaintLoading(void);
	//
	void PaintTitle(void);
	//
	void PaintRunning(void);
	//
	void PaintStopped(void);
	//
	void PaintLost(void);
	//
	void PaintWon(void);
	//
	void PaintHScore(void);
	//
	void PaintBackground(void);
	// 
	void PaintSprites(void);
	//
	void PaintHUD(void);
	//
	void PaintTitleAnimation(void);
};

