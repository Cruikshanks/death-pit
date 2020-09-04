//////////////////////////////////////////////////////
// Direct3D.h
//////////////////////////////////////////////////////

#pragma once

#include <d3d8.h>
#include <D3dx8tex.h>

#include "Constants.h"

class CDirect3D
{
public:
	enum TEXTURETYPE {};
	enum SURFACETYPE {SPLASH, TITLE, BACKGROUND, SPRITES, BANNER, WIN};

	// Constructor
    CDirect3D(void);
	// Destructor
    ~CDirect3D(void);

	// Sets the class's copy of the application's window handle
    void SetWindowHandle(HWND hWnd);
	// Initializes Direct3D for the application, including creating
	// the Direct3D object, checking the display mode, creating
	// the Direct3D device object, and creating the various
	// required surfaces
    HRESULT InitD3D(void);
	// Creates and loads a default image file
	HRESULT CreateDefaultSurface(char* fileName, IDirect3DSurface8** pSurface, int iWidth, int iHeight);
	// Creates and loads a default texture file
	HRESULT CreateDefaultTexture(char* fileName, IDirect3DTexture8** pTexture, D3DCOLOR color);
	// Clears the backbuffer to color given
	void ClearBackBuffer(D3DCOLOR color);
	// Paints a given message to the screen - can also paint a text shadow
	void PaintText(char* pstrMsg, LONG lSize, LONG lWeight, char* fontType,
		D3DCOLOR color, LONG lLeft = NULL, LONG lTop = NULL,
		BOOL bShadow=FALSE, int iShdwOffset = NULL, D3DCOLOR ShadowColor = D3DCOLOR_ARGB(0xff,53,53,44));
	// Paints a tile to the backbuffer
	HRESULT PaintTile(RECT SrcRect, POINT DstPoint, IDirect3DSurface8* pSurface);
	// Returns a pointer to the application's Direct3D device
    LPDIRECT3DDEVICE8 GetDevice(void);
	// Returns a pointer to the application's back buffer
    IDirect3DSurface8* GetBackBuffer(void);
	// Returns the current error message
    char* GetErrorString(void);
	// Releases all Direct3D objects created by the class
    void CleanUp(void);

	// Returns a texture based on the texture type ID
	IDirect3DTexture8* GetTexture(TEXTURETYPE txtType);
	// Returns a surface based on the surface type ID
	IDirect3DSurface8* GetSurface(SURFACETYPE surType);

protected:
    LPDIRECT3D8 m_pD3D;							// Holds a pointer to the class's Direct3D object
    LPDIRECT3DDEVICE8 m_pD3DDevice;				// Holds a pointer to the application's Direct3D device
    IDirect3DSurface8* m_pBackBuffer;			// Holds a pointer to the application's back buffer
	HWND m_hWnd;								// Holds the application's window handle
    char m_szErrorMsg[256];						// Holds the current error message

private:
	IDirect3DSurface8* m_pSplashSurface;		// Holds a pointer to the splash screen surface
	IDirect3DSurface8* m_pTitleSurface;			// Holds a pointer to the title surface
	IDirect3DSurface8* m_pBackgroundSurface;	// Holds a pointer to the background tile surface
	IDirect3DSurface8* m_pSpritesSurface;		// Holds a pointer to the sprites tile surface
	IDirect3DSurface8* m_pBannerSurface;		// Holds a pointer to the banner surface
	IDirect3DSurface8* m_pWinSurface;			// Holds a pointer to the win screen surface

	// Creates the application's Direct3D object
    HRESULT CreateD3DObject(void);
	// Verifies that the user's system supports the required display
    HRESULT CheckDisplayMode(void);
	// Creates the application's Direct3D device
    HRESULT CreateD3DDevice(void);
	// Creates the application's texture surfaces
	HRESULT CreateTextures(void);
	// Creates the application's image surfaces
    HRESULT CreateSurfaces(void);
};
