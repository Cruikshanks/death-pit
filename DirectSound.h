//////////////////////////////////////////////////////
// DirectSound.h
//////////////////////////////////////////////////////

#pragma once

#include <dsound.h>
#include <D3dx8tex.h>
#include "Wave.h" 
#include "Constants.h"

class CDirectSound
{
public:
	enum SOUNDTYPE {LOADING, START, PING, SCREAM, LOST, WIN};

	// Constructor
    CDirectSound(void);
	// Destructor
    ~CDirectSound(void);
	// Sets the class's copy of the appilication's window
    void SetWindowHandle(HWND hWnd);
	// Starts up DirectSound and loads the game's sound effects
    HRESULT InitDirectSound(void);
	// Returns the class's error-description string
    char* GetErrorString(void);
	// Plays the specified sound effect
    HRESULT PlaySound(IDirectSoundBuffer* pBuf);
	// Returns a sound buffer based on the sound buffer type ID
	IDirectSoundBuffer* GetSound(SOUNDTYPE soundType);

protected:
	HWND m_hWnd;							// The class's copy of the application's window handle
    char m_szErrorMsg[256];					// The class's error-description string
    IDirectSound8* m_pDirectSoundObj;		// A pointer to the class's IDirectSound object

	IDirectSoundBuffer* m_pLoadingSound;	// A pointer to a IDirectSoundBuffer that holds the 'Loading' sound
	IDirectSoundBuffer* m_pStartSound;		// A pointer to a IDirectSoundBuffer that holds the 'Start' sound
	IDirectSoundBuffer* m_pPingSound;		// A pointer to a IDirectSoundBuffer that holds the 'Ping' sound
	IDirectSoundBuffer* m_pScreamSound;		// A pointer to a IDirectSoundBuffer that holds the 'Scream' sound
	IDirectSoundBuffer* m_pLostSound;		// A pointer to a IDirectSoundBuffer that holds the 'Lost' sound
	IDirectSoundBuffer* m_pWinSound;		// A pointer to a IDirectSoundBuffer that holds the 'Win' sound

private:
	// Creates sound buffers and loads sound files into them
    HRESULT CreateSoundBuffers(void);
	// Releases all DirectSound objects created by the class
    void CleanUp(void);
	// Loads the specified sound effect from its file into a sound buffer
    HRESULT LoadSoundEffect(char* pstrFileName, IDirectSoundBuffer** ppSoundBuf);
};

