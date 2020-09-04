//////////////////////////////////////////////////////
// Engine.cpp
//////////////////////////////////////////////////////

#include "engine.h"

//////////////////////////////////////////////////////
// CEngine()
//////////////////////////////////////////////////////
CEngine::CEngine(void)
{
	m_PlayerSprite = new CPlayerSprite(SPRITE_TILE_WIDTH, SPRITE_TILE_HEIGHT, CSprite::SPRITE_TYPE::HERO);
	for (int i=0; i<NUM_SPRITES; i++)
	{
		m_TrollSprite[i] = new CSprite(SPRITE_TILE_WIDTH, SPRITE_TILE_HEIGHT, CSprite::SPRITE_TYPE::TROLL);
		m_CoinSprite[i] = new CSprite(SPRITE_TILE_WIDTH, SPRITE_TILE_HEIGHT, CSprite::SPRITE_TYPE::COIN);
	}
	m_iPlayerScore = 0;
	m_iPlayerLives = PLAYER_LIVES;
	m_bHighScoreEntered = FALSE;
	m_dwGameTime = GetTickCount();
	m_dwCurrentTime = GetTickCount();
	m_iDisplayTime = 0;
	m_iCounter = 0;
	m_GameState = CEngine::GS_LOADING;
	m_GameLevel = CEngine::LEVEL1;
	OpenHScoreFile();
	srand((unsigned)time(NULL));
}

//////////////////////////////////////////////////////
// ~CEngine()
//////////////////////////////////////////////////////
CEngine::~CEngine(void)
{
	SaveHScoreFile();
	CleanUp();
}

//////////////////////////////////////////////////////
// CleanUp()
//////////////////////////////////////////////////////
void CEngine::CleanUp(void)
{
	delete m_PlayerSprite;
	for (int i=0; i<NUM_SPRITES; i++)
	{
		delete m_TrollSprite[i];
		delete m_CoinSprite[i];
	}
}

//////////////////////////////////////////////////////
// SetWindowHandle()
//////////////////////////////////////////////////////
void CEngine::SetWindowHandle(HWND hWnd)
{
    m_hWnd = hWnd;
	m_Direct3D.SetWindowHandle(hWnd);
	m_DirectSound.SetWindowHandle(hWnd);
}

//////////////////////////////////////////////////////
// ProcessGame()
//////////////////////////////////////////////////////
void CEngine::ProcessGame(void)
{
	HRESULT hResult = m_Direct3D.GetDevice()->TestCooperativeLevel();
    if (hResult == D3DERR_DEVICELOST)
        m_bGamePaused = TRUE;
    else if (hResult == D3DERR_DEVICENOTRESET)
    {
        ResetDirect3D();
        m_bGamePaused = FALSE;
    }
    else
    {
        // Perform normal game operation
		if ((GetTickCount() - m_dwGameTime) < 33)
		{
			PaintBackBuffer();
		}
		else
		{
			switch (m_GameState)
			{
			case CEngine::GS_LOADING:
				HandleLoading();
				break;
			case CEngine::GS_TITLE:
				HandleTitle();
				break;
			case CEngine::GS_NEWGAME:
				HandleNewGame();
				break;
			case CEngine::GS_RUNNING:
				HandleRunning();
				break;
			case CEngine::GS_STOPPED:
				HandleStopped();
				break;
			case CEngine::GS_LOST:
				HandleLost();
				break;
			case CEngine::GS_WON:
				HandleWon();
				break;
			case CEngine::GS_HSCORE:
				HandleHScore();
				break;
			case CEngine::GS_EXIT:
				PostQuitMessage(WM_QUIT);
				break;
			}
			m_dwGameTime = GetTickCount();
		}
    }
}

//////////////////////////////////////////////////////
// HandleLoading()
//////////////////////////////////////////////////////
void CEngine::HandleLoading(void)
{
	static BOOL bBeenHere = 0;
	
	if (!bBeenHere)
	{
		if ((GetTickCount() - m_dwCurrentTime) > LOADING_DELAY)
		{
			bBeenHere = TRUE;
			m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.LOADING));
		}
	}
	else if ((GetTickCount() - m_dwCurrentTime) > SPLASH_DELAY)
	{
		m_GameState = CEngine::GS_TITLE;
		bBeenHere = FALSE;
	}
}

//////////////////////////////////////////////////////
// HandleTitle()
//////////////////////////////////////////////////////
void CEngine::HandleTitle(void)
{
	float xPos = 150.0f;
	float yPos = 300.0f;

	switch (m_iCounter)
	{
	case 0:
		m_PlayerSprite->SetPosition(xPos, yPos);
		m_PlayerSprite->SetSpeed(3.0);
		m_PlayerSprite->SetRECTValues(HERO * SPRITE_TILE_WIDTH,(HERO * SPRITE_TILE_WIDTH) + SPRITE_TILE_WIDTH,
			0, SPRITE_TILE_HEIGHT);
		m_iCounter++;
		break;

	case 1:
		m_PlayerSprite->SetXPosition(m_PlayerSprite->GetXPosition() + m_PlayerSprite->GetSpeed());
		if ((m_PlayerSprite->GetXPosition() + m_PlayerSprite->GetWidth()) > WINDOW_WIDTH / 2)
		{
			m_iCounter++;
			m_PlayerSprite->SetXPosition(xPos);
		}
		break;

	case 2:
		m_PlayerSprite->SetXPosition(m_PlayerSprite->GetXPosition() + m_PlayerSprite->GetSpeed());
		if ((m_PlayerSprite->GetXPosition() + m_PlayerSprite->GetWidth()) > WINDOW_WIDTH / 2)
		{
			m_iCounter++;
			m_PlayerSprite->SetXPosition(xPos);
		}
		break;

	default:
		m_iCounter = 0;
	}
}

//////////////////////////////////////////////////////
// HandleRunning()
//////////////////////////////////////////////////////
void CEngine::HandleRunning(void)
{
	UpdateDisplayTime();
	m_PlayerSprite->UpdatePlayer();
	if (BorderCollision(m_PlayerSprite, 0, WINDOW_WIDTH, BKGRD_TILE_HEIGHT, WINDOW_HEIGHT))
		m_GameState = CEngine::GS_STOPPED;
	int coinCnt = 0;
	try{
		for (int i=0; i<NUM_SPRITES; i++)
		{
			if (!m_TrollSprite[i]->GetHit())
			{
				if(SpriteCollision(
					m_TrollSprite[i]->GetXPosition(), m_TrollSprite[i]->GetYPosition(),
					m_TrollSprite[i]->GetWidth(), m_TrollSprite[i]->GetHeight(),
					m_PlayerSprite->GetXPosition(), m_PlayerSprite->GetYPosition(),
					m_PlayerSprite->GetWidth(), m_PlayerSprite->GetHeight()))
				{
					m_TrollSprite[i]->SetHit(TRUE);
					m_GameState = CEngine::GS_STOPPED;
					break;
				}
			}
			if (!m_CoinSprite[i]->GetHit())
			{	
				if(SpriteCollision(
					m_CoinSprite[i]->GetXPosition(), m_CoinSprite[i]->GetYPosition(),
					m_CoinSprite[i]->GetWidth(), m_CoinSprite[i]->GetHeight(),
					m_PlayerSprite->GetXPosition(), m_PlayerSprite->GetYPosition(),
					m_PlayerSprite->GetWidth(), m_PlayerSprite->GetHeight()))
				{
					m_CoinSprite[i]->SetHit(TRUE);
					m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.PING));
					m_iPlayerScore += COIN_SCORE;
					if (m_GameLevel == CEngine::LEVEL1)
						m_PlayerSprite->SetSpeed(m_PlayerSprite->GetSpeed() + LEVEL1_INC);
					else if (m_GameLevel == CEngine::LEVEL2)
						m_PlayerSprite->SetSpeed(m_PlayerSprite->GetSpeed() + LEVEL2_INC);
					else if (m_GameLevel == CEngine::LEVEL3)
						m_PlayerSprite->SetSpeed(m_PlayerSprite->GetSpeed() + LEVEL3_INC);
				}
			}
			if (m_CoinSprite[i]->GetHit())
				coinCnt++;
		}
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Possible Array index out of bounds.\nHandleRunning()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}
	if (coinCnt == NUM_SPRITES)
		NextLevel();
}

//////////////////////////////////////////////////////
// HandleNewGame()
//////////////////////////////////////////////////////
void CEngine::HandleNewGame(void)
{
	m_iPlayerScore = 0;
	m_iPlayerLives = PLAYER_LIVES;
	m_iDisplayTime = GAME_TIME;
	m_bHighScoreEntered = FALSE;
	m_GameLevel = CEngine::LEVEL1;

	m_PlayerSprite->SetRECTValues(HERO * SPRITE_TILE_WIDTH,
		(HERO * SPRITE_TILE_WIDTH) + SPRITE_TILE_WIDTH,
		0, SPRITE_TILE_HEIGHT);
	
	m_PlayerSprite->InitPlayer((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 3.0, 0, (float)(BKGRD_TILE_HEIGHT/2));
	InitTrolls();
	InitCoins();
	InitGame();
}

//////////////////////////////////////////////////////
// HandleHScore()
//////////////////////////////////////////////////////
void CEngine::HandleHScore(void)
{
	if (!m_bHighScoreEntered)
	{
		int pos;

		for (pos=0; pos<MAX_HSCORES; pos++)
		{
			if (m_iPlayerScore > m_HScores[pos].iScore)
				break;
		}

		if (pos >= MAX_HSCORES)
		{
			m_bHighScoreEntered = TRUE;
			return;
		}

		for (int i=MAX_HSCORES-1; i>pos; i--)
		{
			m_HScores[i].iScore = m_HScores[i-1].iScore;
			strcpy(m_HScores[i].pstrName, m_HScores[i-1].pstrName);
		}
		m_HScores[pos].iScore = m_iPlayerScore;

		strcpy(m_pstrMsg, "PLAYER 1"); strcpy(m_HScores[pos].pstrName, m_pstrMsg);
	}
	m_bHighScoreEntered = TRUE;
}

//////////////////////////////////////////////////////
// InitGame()
//////////////////////////////////////////////////////
void CEngine::InitGame(void)
{
	m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.START));
	m_PlayerSprite->InitPlayer((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT,
		m_PlayerSprite->GetSpeed(), 0, (float)(BKGRD_TILE_HEIGHT/2));
	m_GameState = CEngine::GS_RUNNING;
	m_dwCurrentTime = GetTickCount();
}

//////////////////////////////////////////////////////
// NextLevel()
//////////////////////////////////////////////////////
void CEngine::NextLevel(void)
{
	if (m_GameLevel == LEVEL1 || m_GameLevel == LEVEL2 || m_GameLevel == LEVEL3)
	{
		m_iPlayerScore += m_iDisplayTime * SEC_SCORE;
		m_iDisplayTime = GAME_TIME;
		m_PlayerSprite->InitPlayer((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 3.0, 0, (float)(BKGRD_TILE_HEIGHT/2));
		InitTrolls();
		InitCoins();
		InitGame();
		
		switch (m_GameLevel)
		{
		case LEVEL1:
			m_GameLevel = LEVEL2;
			break;
		case LEVEL2:
			m_GameLevel = LEVEL3;
			break;
		case LEVEL3:
			m_iCounter = 0;
			m_GameState = GS_WON;
			m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.WIN));
			break;
		}
	}
}

//////////////////////////////////////////////////////
// HandleStopped()
//////////////////////////////////////////////////////
void CEngine::HandleStopped(void)
{
	if (m_iDisplayTime <= 0)
	{
		m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.LOST));
		m_GameState = CEngine::GS_LOST;
		return;
	}

	m_iPlayerLives--;

	if (m_iPlayerLives == 0)
	{
		m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.LOST));
		m_GameState = CEngine::GS_LOST;
		return;
	}
	else
	{
		m_DirectSound.PlaySound(m_DirectSound.GetSound(m_DirectSound.SCREAM));
		InitGame();
		return;
	}
}

//////////////////////////////////////////////////////
// HandleLost()
//////////////////////////////////////////////////////
void CEngine::HandleLost(void)
{
	m_iCounter = 0;
}

//////////////////////////////////////////////////////
// HandleWon()
//////////////////////////////////////////////////////
void CEngine::HandleWon(void)
{
	m_iPlayerScore += m_iPlayerLives * LIFE_SCORE;
	m_iPlayerLives = 0;
	m_iCounter = 0;
}

//////////////////////////////////////////////////////
// HandleKeys()
//////////////////////////////////////////////////////
void CEngine::HandleKeys(WPARAM wParam)
{
    switch(wParam)
    {
	case VK_ESCAPE:
		HandleEscape();		
		break;
	case VK_UP:
		HandleUpArrow();
		break;
	case VK_DOWN:
		HandleDownArrow();
		break;
	case VK_LEFT:
        HandleLeftArrow();
        break;
    case VK_RIGHT:
        HandleRightArrow();
        break;
	case VK_SPACE:
		HandleSpace();
		break;
	case 83:
		HandleS();
		break;
    }
}

//////////////////////////////////////////////////////
// HandleS()
//////////////////////////////////////////////////////
void CEngine::HandleS(void)
{
	if (m_GameState != CEngine::GS_TITLE) return;
    m_GameState = CEngine::GS_HSCORE;
	m_bHighScoreEntered = TRUE;
}

//////////////////////////////////////////////////////
// HandleEscape()
//////////////////////////////////////////////////////
void CEngine::HandleEscape(void)
{
	if (m_GameState == CEngine::GS_TITLE)
		m_GameState = CEngine::GS_EXIT;
	if (m_GameState == CEngine::GS_RUNNING || m_GameState == CEngine::GS_STOPPED)
	{
		m_GameState = CEngine::GS_TITLE;
		m_iCounter = 0;
	}
}

//////////////////////////////////////////////////////
// HandleSpace()
//////////////////////////////////////////////////////
void CEngine::HandleSpace(void)
{
	if (m_GameState == CEngine::GS_TITLE)
		m_GameState = CEngine::GS_NEWGAME;
	if (m_GameState == CEngine::GS_HSCORE)
		m_GameState = CEngine::GS_TITLE;
	if (m_GameState == CEngine::GS_LOST || m_GameState == CEngine::GS_WON)
		m_GameState = CEngine::GS_HSCORE;
}

//////////////////////////////////////////////////////
// HandleUpArrow()
//////////////////////////////////////////////////////
void CEngine::HandleUpArrow(void)
{
	if (m_GameState != CEngine::GS_RUNNING) return;
	m_PlayerSprite->SetDirection(m_PlayerSprite->NORTH);
}

//////////////////////////////////////////////////////
// HandleDownArrow()
//////////////////////////////////////////////////////
void CEngine::HandleDownArrow(void)
{
	if (m_GameState != CEngine::GS_RUNNING) return;
	m_PlayerSprite->SetDirection(m_PlayerSprite->SOUTH);
}

//////////////////////////////////////////////////////
// HandleLeftArrow()
//////////////////////////////////////////////////////
void CEngine::HandleLeftArrow(void)
{
	if (m_GameState != CEngine::GS_RUNNING) return;
	m_PlayerSprite->SetDirection(m_PlayerSprite->WEST);
}

//////////////////////////////////////////////////////
// HandleRightArrow()
//////////////////////////////////////////////////////
void CEngine::HandleRightArrow(void)
{
	if (m_GameState != CEngine::GS_RUNNING) return;
	m_PlayerSprite->SetDirection(m_PlayerSprite->EAST);
}

//////////////////////////////////////////////////////
// PaintBackBuffer()
//////////////////////////////////////////////////////
void CEngine::PaintBackBuffer(void)
{
	m_Direct3D.ClearBackBuffer(D3DCOLOR_XRGB(0,0,0));

	switch (m_GameState)
	{
	case CEngine::GS_LOADING:
		PaintLoading();
		break;
	case CEngine::GS_TITLE:
		PaintTitle();
		break;
	case CEngine::GS_RUNNING:
		PaintRunning();
		break;
	case CEngine::GS_LOST:
		PaintLost();
		break;
	case CEngine::GS_WON:
		PaintWon();
		break;
	case CEngine::GS_HSCORE:
		PaintHScore();
		break;
	}
		
	m_Direct3D.GetDevice()->Present(NULL, NULL, NULL, NULL);
}

//////////////////////////////////////////////////////
// PaintBackground()
//////////////////////////////////////////////////////
void CEngine::PaintBackground(void)
{
	RECT SrcRect;
	switch (m_GameLevel)
	{
	case CEngine::LEVEL1:
		SrcRect.left = LEVEL1 * BKGRD_TILE_WIDTH;
		SrcRect.right = SrcRect.left + BKGRD_TILE_WIDTH;
		SrcRect.top = 0;
		SrcRect.bottom = BKGRD_TILE_HEIGHT;
		break;
	case CEngine::LEVEL2:
		SrcRect.left = LEVEL2 * BKGRD_TILE_WIDTH;
		SrcRect.right = SrcRect.left + BKGRD_TILE_WIDTH;
		SrcRect.top = 0;
		SrcRect.bottom = BKGRD_TILE_HEIGHT;
		break;
	case CEngine::LEVEL3:
		SrcRect.left = LEVEL3 * BKGRD_TILE_WIDTH;
		SrcRect.right = SrcRect.left + BKGRD_TILE_WIDTH;
		SrcRect.top = 0;
		SrcRect.bottom = BKGRD_TILE_HEIGHT;
		break;
	}
	
	POINT DstPoint;
	for (int x=0; x<(WINDOW_WIDTH); x+=BKGRD_TILE_WIDTH)
	{
		for (int y=BKGRD_TILE_HEIGHT; y<(WINDOW_HEIGHT); y+=BKGRD_TILE_HEIGHT)
		{
			DstPoint.x = x;
			DstPoint.y = y;
			if (y > BKGRD_TILE_HEIGHT * 6)
				SrcRect.bottom = BKGRD_TILE_HEIGHT / 2;

			m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.BACKGROUND));
		}
		SrcRect.bottom = BKGRD_TILE_HEIGHT;
	}
}

//////////////////////////////////////////////////////
// PaintHUD()
//////////////////////////////////////////////////////
void CEngine::PaintHUD(void)
{
	wsprintf(m_pstrMsg, "LEVEL = %d", (m_GameLevel + 1));
	m_Direct3D.PaintText(m_pstrMsg, 20, FW_BOLD, "Arial", D3DCOLOR_ARGB(0xff,225,0,0), 10, 10, TRUE, 3);

	wsprintf(m_pstrMsg, "LIVES =");
	m_Direct3D.PaintText(m_pstrMsg, 20, FW_BOLD, "Arial", D3DCOLOR_ARGB(0xff,225,0,0), 10, 40, TRUE, 3);

	POINT DstPoint;
	DstPoint.y = 40; DstPoint.x = 85;
	for (int i=0; i< m_iPlayerLives; i++)
	{
		m_Direct3D.PaintTile(m_PlayerSprite->GetRECT(), DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));
		DstPoint.x +=(SPRITE_TILE_WIDTH+5);
	}

	RECT SrcRect;
	SrcRect.left = 0; SrcRect.right = SrcRect.left + BANNER_WIDTH;
	SrcRect.top = 0; SrcRect.bottom = BANNER_HEIGHT;
	DstPoint.x = (WINDOW_WIDTH / 2) - (BANNER_WIDTH / 2); DstPoint.y = 0;
	m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.BANNER));

	wsprintf(m_pstrMsg, "TIME = %d", m_iDisplayTime);
	D3DCOLOR color;
	if (m_iDisplayTime < 11 && ((m_iDisplayTime % 2) == 0))
		color = D3DCOLOR_ARGB(0xff,225,255,0);
	else
		color = D3DCOLOR_ARGB(0xff,225,0,0);
	m_Direct3D.PaintText(m_pstrMsg, 20, FW_BOLD, "Arial",color, 440, 10, TRUE, 3);

	wsprintf(m_pstrMsg, "SCORE = %d", m_iPlayerScore);
	m_Direct3D.PaintText(m_pstrMsg, 20, FW_BOLD, "Arial", D3DCOLOR_ARGB(0xff,225,0,0), 440, 40, TRUE, 3);
}

//////////////////////////////////////////////////////
// PaintLoading()
//////////////////////////////////////////////////////
void CEngine::PaintLoading(void)
{
	RECT SrcRect;
	SrcRect.left = 0;
	SrcRect.right = SPLASH_WIDTH;
	SrcRect.top = 0;
	SrcRect.bottom = SPLASH_HEIGHT;

	POINT DstPoint;
	DstPoint.x = (WINDOW_WIDTH / 2) - (SPLASH_WIDTH / 2) + 20;
	DstPoint.y = (WINDOW_HEIGHT / 2) - (SPLASH_HEIGHT / 2);

	m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPLASH));
}

//////////////////////////////////////////////////////
// PaintTitle()
//////////////////////////////////////////////////////
void CEngine::PaintTitle(void)
{
	RECT SrcRect;
	SrcRect.left = 0; SrcRect.right = TITLE_WIDTH;
	SrcRect.top = 0; SrcRect.bottom = TITLE_HEIGHT;

	POINT DstPoint;
	DstPoint.x = (WINDOW_WIDTH / 2) - (TITLE_WIDTH / 2); DstPoint.y = 10;

	m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.TITLE));

	PaintTitleAnimation();

	m_Direct3D.PaintText("Press 'SPACE' to play - Press 'ESCAPE' to quit - Press 'S' for high scores",
		20, FW_BOLD, "Arial",D3DCOLOR_ARGB(0xff,225,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.90), TRUE, 3);
}

//////////////////////////////////////////////////////
// PaintTitleAnimation()
//////////////////////////////////////////////////////
void CEngine::PaintTitleAnimation(void)
{
	RECT SrcRect;
	POINT DstPoint;
	LONG yPos = 300;

	switch (m_iCounter)
	{
	case 1:
		SrcRect.left = COIN * SPRITE_TILE_WIDTH; SrcRect.right = SrcRect.left + SPRITE_TILE_WIDTH;
		SrcRect.top = 0; SrcRect.bottom = SPRITE_TILE_HEIGHT;

		DstPoint.x = WINDOW_WIDTH / 2; DstPoint.y = yPos;
		m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));

		DstPoint.x = (LONG)m_PlayerSprite->GetXPosition(); DstPoint.y = (LONG)m_PlayerSprite->GetYPosition();
		m_Direct3D.PaintTile(m_PlayerSprite->GetRECT(), DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));

		m_Direct3D.PaintText("Collect all 20 coins to move up a level", 16, FW_BOLD, "Arial",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (yPos + SPRITE_TILE_HEIGHT + 15));
		break;

	case 2:
		SrcRect.left = TROLL * SPRITE_TILE_WIDTH; SrcRect.right = SrcRect.left + SPRITE_TILE_WIDTH;
		SrcRect.top = 0; SrcRect.bottom = SPRITE_TILE_HEIGHT;
		
		DstPoint.x = WINDOW_WIDTH / 2; DstPoint.y = yPos;
		m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));

		DstPoint.x = (LONG)m_PlayerSprite->GetXPosition(); DstPoint.y = (LONG)m_PlayerSprite->GetYPosition();
		m_Direct3D.PaintTile(m_PlayerSprite->GetRECT(), DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));

		m_Direct3D.PaintText("Avoid the trolls or lose a life", 16, FW_BOLD, "Arial",
			D3DCOLOR_ARGB(0xff,225,0,0), NULL, (yPos + SPRITE_TILE_HEIGHT + 15));
		break;
	}
}

//////////////////////////////////////////////////////
// PaintRunning()
//////////////////////////////////////////////////////
void CEngine::PaintRunning(void)
{
	PaintHUD();
	PaintBackground();
	PaintSprites();
	if (m_PlayerSprite->GetDirection() == m_PlayerSprite->STATIONARY)
		m_Direct3D.PaintText("MOVE IT!", 20, FW_BOLD, "Arial",
		D3DCOLOR_ARGB(0xff,221,221,0), NULL, NULL, TRUE, 3, D3DCOLOR_ARGB(0xff,0,0,0));
}

//////////////////////////////////////////////////////
// PaintSprites()
//////////////////////////////////////////////////////
void CEngine::PaintSprites(void)
{
	POINT DstPoint;

	try 
	{
		for (int i=0; i<NUM_SPRITES; i++)
		{
			if (!m_TrollSprite[i]->GetHit())
			{
				DstPoint.x = (LONG)m_TrollSprite[i]->GetXPosition();
				DstPoint.y = (LONG)m_TrollSprite[i]->GetYPosition();
				m_Direct3D.PaintTile(m_TrollSprite[i]->GetRECT(), DstPoint,
					m_Direct3D.GetSurface(m_Direct3D.SPRITES));
			}
			if (!m_CoinSprite[i]->GetHit())
			{
				DstPoint.x = (LONG)m_CoinSprite[i]->GetXPosition();
				DstPoint.y = (LONG)m_CoinSprite[i]->GetYPosition();
				m_Direct3D.PaintTile(m_CoinSprite[i]->GetRECT(), DstPoint,
					m_Direct3D.GetSurface(m_Direct3D.SPRITES));
			}
		}
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Possible Array index out of bounds.\nPaintSprites()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}

	DstPoint.x = (LONG)m_PlayerSprite->GetXPosition(); DstPoint.y = (LONG)m_PlayerSprite->GetYPosition();
	m_Direct3D.PaintTile(m_PlayerSprite->GetRECT(), DstPoint, m_Direct3D.GetSurface(m_Direct3D.SPRITES));
}

//////////////////////////////////////////////////////
// PaintLost()
//////////////////////////////////////////////////////
void CEngine::PaintLost(void)
{
	if (m_iDisplayTime <= 0)
	{
		m_Direct3D.PaintText("You have failed to escape from\nDeath Pit,"
			"and are doomed to spend\nthe rest of your days in darkness,"
			"\nwith only the screams and torments of those\nlike you "
			"for company.", 24, FW_NORMAL, "Times New Roman",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.20));

		m_Direct3D.PaintText("Press 'SPACE'", 20, FW_BOLD, "Arial",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.90), TRUE, 3);
	}
	else
	{
		m_Direct3D.PaintText("You lie, bleeding on the floor of Death Pit\n"
			"while a smile gently flourishes on your face.\n"
			"You may be dying, but that just means your one\n"
			"of the lucky ones!", 24, FW_NORMAL, "Times New Roman",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.20));
		
		m_Direct3D.PaintText("Press 'SPACE'", 20, FW_BOLD, "Arial",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.90), TRUE, 3);
	}
}

//////////////////////////////////////////////////////
// PaintHScore()
//////////////////////////////////////////////////////
void CEngine::PaintHScore(void)
{
	if (m_bHighScoreEntered)
	{
		wsprintf(m_pstrMsg, "PAST HEROES", m_iPlayerScore);
		m_Direct3D.PaintText(m_pstrMsg, 24, FW_NORMAL, "Times New Roman",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.10));

		LONG x = (LONG)(WINDOW_HEIGHT * 0.20);
		for (int i=0; i<MAX_HSCORES; i++)
		{
			wsprintf(m_pstrMsg, "%9d         %10s", m_HScores[i].iScore, m_HScores[i].pstrName);
			m_Direct3D.PaintText(m_pstrMsg, 16, FW_BOLD, "Arial",
				D3DCOLOR_ARGB(0xff,221,0,0), NULL, x);
			x += (LONG)(WINDOW_HEIGHT * 0.05);
		}
	}
	if (m_iPlayerScore > 0)
	{
		wsprintf(m_pstrMsg, "Your score was %d", m_iPlayerScore);
		m_Direct3D.PaintText(m_pstrMsg, 24, FW_BOLD, "Times New Roman",
			D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.75));
	}

	m_Direct3D.PaintText("Press 'SPACE'", 20, FW_BOLD, "Arial",
		D3DCOLOR_ARGB(0xff,221,0,0), NULL, (LONG)(WINDOW_HEIGHT * 0.90), TRUE, 3);
}

//////////////////////////////////////////////////////
// PaintWon()
//////////////////////////////////////////////////////
void CEngine::PaintWon(void)
{
	m_Direct3D.ClearBackBuffer(D3DCOLOR_XRGB(255,255,255));

	RECT SrcRect;
	SrcRect.left = 0; SrcRect.right = WIN_WIDTH;
	SrcRect.top = 0; SrcRect.bottom = WIN_HEIGHT;

	POINT DstPoint;
	DstPoint.x = 0; DstPoint.y = (WINDOW_HEIGHT - WIN_HEIGHT);

	m_Direct3D.PaintTile(SrcRect, DstPoint, m_Direct3D.GetSurface(m_Direct3D.WIN));

	m_Direct3D.PaintText("Press 'SPACE'", 20, FW_BOLD, "Arial",
		D3DCOLOR_ARGB(0xff,255,255,255), NULL, (LONG)(WINDOW_HEIGHT * 0.90));
}

//////////////////////////////////////////////////////
// SpriteCollision()
//////////////////////////////////////////////////////
BOOL CEngine::SpriteCollision(float sprite1X, float sprite1Y, int width1, int height1,
							  float sprite2X, float sprite2Y, int width2, int height2)
{
	// Test each corner of sprite 2 to see if it is within the bounds of sprite 1
	if (sprite2X >= sprite1X && sprite2X <= (sprite1X + width1))
		if (sprite2Y >= sprite1Y && sprite2Y <= (sprite1Y + height1))
			return TRUE;

	if ((sprite2X + width2) >= sprite1X && (sprite2X + width2) <= (sprite1X + width1))
		if (sprite2Y >= sprite1Y && sprite2Y <= (sprite1Y + height1))
			return TRUE;

	if (sprite2X >= sprite1X && sprite2X <= (sprite1X + width1))
		if ((sprite2Y + height2) >= sprite1Y && (sprite2Y + height2) <= (sprite1Y + height1))
			return TRUE;

	if ((sprite2X + width2) >= sprite1X && (sprite2X + width2) <= (sprite1X + width1))
		if ((sprite2Y + height2) >= sprite1Y && (sprite2Y + height2) <= (sprite1Y + height1))
			return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////
// BorderCollsion()
//////////////////////////////////////////////////////
BOOL CEngine::BorderCollision(CSprite* sprite, int minWidth, int maxWidth, int minHeight, int maxHeight)
{
	if (sprite->GetXPosition() < minWidth)
	{
		sprite->SetXPosition((float)minWidth);
		return TRUE;
	}
    else if ((sprite->GetXPosition() + sprite->GetWidth()) > maxWidth)
	{
		sprite->SetXPosition((float)(maxWidth - sprite->GetWidth()));
		return TRUE;
	}
	else if (sprite->GetYPosition() < minHeight)
	{
		sprite->SetYPosition((float)minHeight);
		return TRUE;
	}
	else if ((sprite->GetYPosition() + sprite->GetHeight()) > maxHeight)
	{
		sprite->SetYPosition((float)(maxHeight - sprite->GetHeight()));
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////
// UpdateDisplayTime()
//////////////////////////////////////////////////////
void CEngine::UpdateDisplayTime(void)
{
	if ((GetTickCount() - m_dwCurrentTime) > 1000)
	{
		m_iDisplayTime--;
		m_dwCurrentTime = GetTickCount();
	}

	if (m_iDisplayTime <= 0)
		m_GameState = CEngine::GS_STOPPED;
}

//////////////////////////////////////////////////////
// InitTrolls()
//////////////////////////////////////////////////////
void CEngine::InitTrolls(void)
{
	try{
		for (int i=0; i<NUM_SPRITES; i++)
		{
			m_TrollSprite[i]->SetRECTValues(TROLL * SPRITE_TILE_WIDTH,
				(TROLL * SPRITE_TILE_WIDTH) + SPRITE_TILE_WIDTH, 0, SPRITE_TILE_HEIGHT);

			BOOL overLapChk = TRUE;
			float x = 0;
			float y = 0;
			while (overLapChk)
			{
				overLapChk = FALSE;
				x = (float)(rand() % (WINDOW_WIDTH - SPRITE_TILE_WIDTH));
				y = (float)((rand()%((WINDOW_HEIGHT-BKGRD_TILE_HEIGHT)-SPRITE_TILE_HEIGHT))+BKGRD_TILE_HEIGHT);

				for (int j=0; j<NUM_SPRITES; j++)
				{
					if (SpriteCollision(
						m_TrollSprite[j]->GetXPosition(), m_TrollSprite[j]->GetYPosition(),
						m_TrollSprite[j]->GetWidth(), m_TrollSprite[j]->GetHeight(),
						x, y, m_TrollSprite[i]->GetWidth(), m_TrollSprite[i]->GetHeight()))
					{
						overLapChk = TRUE; break;
					}
				}
				if (SpriteCollision(
						m_PlayerSprite->GetXPosition(), m_PlayerSprite->GetYPosition(),
						m_PlayerSprite->GetWidth(), m_PlayerSprite->GetHeight(),
						x, y, m_TrollSprite[i]->GetWidth(), m_TrollSprite[i]->GetHeight()))
						overLapChk = TRUE;
			}
			m_TrollSprite[i]->SetPosition(x, y);
			m_TrollSprite[i]->SetHit(FALSE);
		}
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Possible Array index out of bounds.\nInitTrolls()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}
}

//////////////////////////////////////////////////////
// InitCoins()
//////////////////////////////////////////////////////
void CEngine::InitCoins(void)
{
	try{
		for (int i=0; i<NUM_SPRITES; i++)
		{
			m_CoinSprite[i]->SetRECTValues(COIN * SPRITE_TILE_WIDTH,
				(COIN * SPRITE_TILE_WIDTH) + SPRITE_TILE_WIDTH, 0, SPRITE_TILE_HEIGHT);

			BOOL overLapChk = TRUE;
			float x = 0;
			float y = 0;
			while (overLapChk)
			{
				overLapChk = FALSE;
				x = (float)(rand() % (WINDOW_WIDTH - SPRITE_TILE_WIDTH));
				y = (float)((rand()%((WINDOW_HEIGHT-BKGRD_TILE_HEIGHT)-SPRITE_TILE_HEIGHT))+BKGRD_TILE_HEIGHT);

				for (int j=0; j<NUM_SPRITES; j++)
				{
					if (SpriteCollision(
						m_CoinSprite[j]->GetXPosition(), m_CoinSprite[j]->GetYPosition(),
						m_CoinSprite[j]->GetWidth(), m_CoinSprite[j]->GetHeight(),
						x, y, m_CoinSprite[i]->GetWidth(), m_CoinSprite[i]->GetHeight()) ||
						SpriteCollision(
						m_TrollSprite[j]->GetXPosition(), m_TrollSprite[j]->GetYPosition(),
						m_TrollSprite[j]->GetWidth(), m_TrollSprite[j]->GetHeight(),
						x, y, m_TrollSprite[i]->GetWidth(), m_TrollSprite[i]->GetHeight()))
					{
						overLapChk = TRUE; break;
					}
				}
				if (SpriteCollision(
						m_PlayerSprite->GetXPosition(), m_PlayerSprite->GetYPosition(),
						m_PlayerSprite->GetWidth(), m_PlayerSprite->GetHeight(),
						x, y, m_CoinSprite[i]->GetWidth(), m_CoinSprite[i]->GetHeight()))
						overLapChk = TRUE;
			}
			m_CoinSprite[i]->SetPosition(x, y);
			m_CoinSprite[i]->SetHit(FALSE);
		}
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Possible Array index out of bounds.\nInitCoins()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}
}

//////////////////////////////////////////////////////
// GetCDirect3DObj()
//////////////////////////////////////////////////////
CDirect3D* CEngine::GetCDirect3DObj(void)
{
	return &m_Direct3D;
}

//////////////////////////////////////////////////////
// GetCDirectSoundObj()
//////////////////////////////////////////////////////
CDirectSound* CEngine::GetCDirectSoundObj(void)
{
	return &m_DirectSound;
}

//////////////////////////////////////////////////////
// ResetDirect3D()
//////////////////////////////////////////////////////
void CEngine::ResetDirect3D(void)
{
    m_Direct3D.CleanUp();
    m_Direct3D.InitD3D();
}

//////////////////////////////////////////////////////
// OpenHScoreFile()
//////////////////////////////////////////////////////
void CEngine::OpenHScoreFile(void)
{
	try
	{
		strcpy(m_pstrMsg, "deathpit.dat");
		std::ifstream hscoreFile(m_pstrMsg, std::ios::binary);

		if (hscoreFile.fail())
		{
			for (int i=0; i<MAX_HSCORES; i++)
			{
				m_HScores[i].iScore = 5000;
				strcpy(m_HScores[i].pstrName, "FE");
			}
			return;
		}

		for (int i=0; i<MAX_HSCORES; i++)
		{
			hscoreFile.read((char*)&m_HScores[i].iScore, sizeof m_HScores[i].iScore);
			hscoreFile.read(m_HScores[i].pstrName, sizeof m_HScores[i].pstrName);
		}
		hscoreFile.close();
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Unable to open deathpit.dat\nOpenHScoreFile()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}
}

//////////////////////////////////////////////////////
// SaveHScoreFile()
//////////////////////////////////////////////////////
void CEngine::SaveHScoreFile(void)
{
	try
	{
		strcpy(m_pstrMsg, "deathpit.dat");
		std::ofstream hscoreFile(m_pstrMsg, std::ios::binary);
		if (hscoreFile.fail())
		{
			MessageBox(m_hWnd, "Unable to save high scores", "Sorry", MB_OK);
			return;
		}

		for (int i=0; i<MAX_HSCORES; i++)
		{
			hscoreFile.write((char*)&m_HScores[i].iScore, sizeof m_HScores[i].iScore);
			hscoreFile.write(m_HScores[i].pstrName, sizeof m_HScores[i].pstrName);
		}
		hscoreFile.close();
	}
	catch(...)
	{
		MessageBox(m_hWnd, "Unable to write to deathpit.dat\nSaveHScoreFile()", "Error", MB_OK);
		PostQuitMessage(WM_QUIT);
	}
}