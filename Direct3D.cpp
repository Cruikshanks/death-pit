//////////////////////////////////////////////////////
// Direct3D.cpp
//////////////////////////////////////////////////////

#include "direct3d.h"

//////////////////////////////////////////////////////
// CDirect3D()
//////////////////////////////////////////////////////
CDirect3D::CDirect3D(void)
{
    m_pD3D = NULL;
    m_pD3DDevice = NULL;
    m_pBackBuffer = NULL;
	m_pSplashSurface = NULL;
	m_pTitleSurface = NULL;
	m_pBackgroundSurface = NULL;
	m_pSpritesSurface = NULL;
	m_pBannerSurface = NULL;
	m_pWinSurface = NULL;
	
    strcpy(m_szErrorMsg, "No Error");
}

//////////////////////////////////////////////////////
// ~CDirect3D()
//////////////////////////////////////////////////////
CDirect3D::~CDirect3D(void)
{
    CleanUp();
}

//////////////////////////////////////////////////////
// SetWindowHandle()
//////////////////////////////////////////////////////
void CDirect3D::SetWindowHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}

//////////////////////////////////////////////////////
// InitD3D()
//////////////////////////////////////////////////////
HRESULT CDirect3D::InitD3D()
{
    HRESULT hResult = CreateD3DObject();
    if (hResult != D3D_OK)
        return hResult;
    hResult = CheckDisplayMode();
    if (hResult != D3D_OK)
        return hResult;
    hResult = CreateD3DDevice();
    if (hResult != D3D_OK)
        return hResult;
	hResult = CreateTextures();
	if (hResult != D3D_OK)
		return hResult;
    hResult = CreateSurfaces();
    if (hResult != D3D_OK)
        return hResult;
    hResult = m_pD3DDevice->GetBackBuffer(
		0,
        D3DBACKBUFFER_TYPE_MONO,
		&m_pBackBuffer);

	ClearBackBuffer(D3DCOLOR_XRGB(0,0,0));
    m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

    return hResult;
}

//////////////////////////////////////////////////////
// CreateD3DObject()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateD3DObject()
{
    m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if (m_pD3D == NULL)
    {
        MessageBox(m_hWnd, "Couldn’t create DirectX object.",
            "DirectX Error", MB_OK);
        strcpy(m_szErrorMsg, "CreateD3DObject()");
        return E_FAIL;
    }
    return D3D_OK;
}

//////////////////////////////////////////////////////
// CheckDisplayMode()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CheckDisplayMode()
{
    HRESULT hResult = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, FALSE);
    if (hResult != D3D_OK)
    {
        MessageBox(m_hWnd, "The required display mode is \
            not available on this system.",
            "DirectX Error", MB_OK);
        strcpy(m_szErrorMsg, "CheckDisplayMode()");
        return hResult;
    }
    return D3D_OK;
}

//////////////////////////////////////////////////////
// CreateD3DDevice()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateD3DDevice()
{
    D3DPRESENT_PARAMETERS D3DPresentParams;
    ZeroMemory(&D3DPresentParams, sizeof(D3DPRESENT_PARAMETERS));
    D3DPresentParams.Windowed = FALSE;
    D3DPresentParams.BackBufferCount = 1;
    D3DPresentParams.BackBufferWidth = 640;
    D3DPresentParams.BackBufferHeight = 480;
    D3DPresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    D3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    D3DPresentParams.hDeviceWindow = m_hWnd;
    HRESULT hResult = m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&D3DPresentParams,
		&m_pD3DDevice);
    if (FAILED(hResult))
    {
        MessageBox(m_hWnd, "Failed to create Direct3D device.",
            "DirectX Error", MB_OK);
            strcpy(m_szErrorMsg, "CreateD3DDevice()");
        return hResult;
    }
    return hResult;
}

//////////////////////////////////////////////////////
// CreateTextures()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateTextures()
{
	return D3D_OK;	
}

//////////////////////////////////////////////////////
// CreateDefaultTexture() 0xFF000000
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateDefaultTexture(char* fileName, IDirect3DTexture8** pTexture, D3DCOLOR color)
{
	HRESULT hResult = D3DXCreateTextureFromFileEx(
		m_pD3DDevice,
		fileName,
        D3DX_DEFAULT,
		D3DX_DEFAULT,
		1,
		0,
		D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		color,
        NULL,
		NULL,
		pTexture);
    if (FAILED(hResult))
    {
        strcpy(m_szErrorMsg, "Error loading texture surface.");
        return hResult;
    }

	return D3D_OK;	
}

//////////////////////////////////////////////////////
// CreateSurfaces()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateSurfaces(void)
{
	HRESULT hResult = CreateDefaultSurface(
		"Graphics\\startup.bmp",
		&m_pSplashSurface,
		SPLASH_WIDTH, SPLASH_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Splash surface not created.");
		return hResult;
	}

	hResult = CreateDefaultSurface(
		"Graphics\\title.bmp",
		&m_pTitleSurface,
		TITLE_WIDTH, TITLE_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Title surface not created.");
		return hResult;
	}

	hResult = CreateDefaultSurface(
		"Graphics\\background.bmp",
		&m_pBackgroundSurface,
		BKGRD_IMG_WIDTH, BKGRD_IMG_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Background surface not created.");
		return hResult;
	}

	hResult = CreateDefaultSurface(
		"Graphics\\sprites.bmp",
		&m_pSpritesSurface,
		SPRITE_IMG_WIDTH, SPRITE_IMG_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Sprites surface not created.");
		return hResult;
	}

	hResult = CreateDefaultSurface(
		"Graphics\\banner.bmp",
		&m_pBannerSurface,
		BANNER_WIDTH, BANNER_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Banner surface not created.");
		return hResult;
	}

	hResult = CreateDefaultSurface(
		"Graphics\\win.bmp",
		&m_pWinSurface,
		WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Win surface not created.");
		return hResult;
	}

	return D3D_OK;
}

//////////////////////////////////////////////////////
// CreateDefaultSurface()
//////////////////////////////////////////////////////
HRESULT CDirect3D::CreateDefaultSurface(char* fileName, IDirect3DSurface8** pSurface, int iWidth, int iHeight)
{
	HRESULT hResult = m_pD3DDevice->CreateImageSurface(
		iWidth,
		iHeight,
		D3DFMT_X8R8G8B8,
		pSurface);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Surface not created.");
		return hResult;
	}

	hResult = D3DXLoadSurfaceFromFile(
		*pSurface,
		NULL,
		NULL,
		fileName,
		NULL,
		D3DX_DEFAULT,
		0,
		NULL);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Error loading surface.");
		return hResult;
	}

	return D3D_OK;
}

//////////////////////////////////////////////////////
// CleanUp()
//////////////////////////////////////////////////////
void CDirect3D::CleanUp()
{
	if (m_pWinSurface)
		m_pWinSurface->Release();
	if (m_pBannerSurface)
		m_pBannerSurface->Release();
	if (m_pSpritesSurface)
		m_pSpritesSurface->Release();
	if (m_pBackgroundSurface)
		m_pBackgroundSurface->Release();
	if (m_pTitleSurface)
		m_pTitleSurface->Release();
	if (m_pSplashSurface)
		m_pSplashSurface->Release();
	
    if (m_pBackBuffer)
        m_pBackBuffer->Release();
    if (m_pD3DDevice)
        m_pD3DDevice->Release();
    if (m_pD3D)
        m_pD3D->Release();
}

//////////////////////////////////////////////////////
// ClearBackBuffer()
//////////////////////////////////////////////////////
void CDirect3D::ClearBackBuffer(D3DCOLOR color)
{
    GetDevice()->Clear(
		0,
		NULL,
		D3DCLEAR_TARGET,
		color,
		1.0f,
		0);
}

//////////////////////////////////////////////////////
// PaintText()
//////////////////////////////////////////////////////
void CDirect3D::PaintText(char* pstrMsg, LONG lSize, LONG lWeight, char* fontType,
						  D3DCOLOR Color, LONG lLeft, LONG lTop,
						  BOOL bShadow, int iShdwOffset, D3DCOLOR ShdwColor)
{
	// Create a font.
	LPD3DXFONT pFont = NULL;
    LOGFONT LogFont = {lSize, 0, 0, 0, lWeight, false, false, false,
        DEFAULT_CHARSET,OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        PROOF_QUALITY, DEFAULT_PITCH, (CHAR)fontType};
    D3DXCreateFontIndirect(m_pD3DDevice, &LogFont, &pFont);
	
	RECT r;
	r.left = 0; r.top = 0; r.right = 0; r.bottom = 0;
	
	// Calculate the size of a rectangle that can hold the text.
    pFont->DrawText(pstrMsg, -1, &r, DT_CALCRECT, Color);

	int iWidth = r.right - r.left;
    int iHeight = r.bottom - r.top;
	if (lLeft == NULL)
		lLeft = (WINDOW_WIDTH - iWidth) / 2;
	if (lTop == NULL)
		lTop = (WINDOW_HEIGHT - iHeight) / 2;
	r.left = lLeft;
	r.right = r.left + iWidth;
	r.top = lTop;
	r.bottom = r.top + iHeight;
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
    {
		if (bShadow)
		{
			pFont->DrawText(pstrMsg, -1, &r, DT_CENTER, ShdwColor);

			r.left += iShdwOffset;
			r.right += iShdwOffset;
			r.top -= iShdwOffset;
			r.bottom -= iShdwOffset;
		}
		pFont->DrawText(pstrMsg, -1, &r, DT_CENTER, Color);
		m_pD3DDevice->EndScene();
	}
	pFont->Release();
}

//////////////////////////////////////////////////////
// ClearBackBuffer()
//////////////////////////////////////////////////////
HRESULT CDirect3D::PaintTile(RECT SrcRect, POINT DstPoint, IDirect3DSurface8* pSurface)
{
	HRESULT hResult = m_pD3DDevice->CopyRects(
		pSurface,
		&SrcRect,
		1,
		GetBackBuffer(),
		&DstPoint);
	if (FAILED(hResult))
	{
		strcpy(m_szErrorMsg, "Error painting tile.");
		PostQuitMessage(WM_QUIT);
	}

	return hResult;
}

//////////////////////////////////////////////////////
// GetBackBuffer()
//////////////////////////////////////////////////////
IDirect3DSurface8* CDirect3D::GetBackBuffer()
{
    return m_pBackBuffer;
}

//////////////////////////////////////////////////////
// GetDevice()
//////////////////////////////////////////////////////
LPDIRECT3DDEVICE8 CDirect3D::GetDevice()
{
    return m_pD3DDevice;
}

//////////////////////////////////////////////////////
// GetErrorString()
//////////////////////////////////////////////////////
char* CDirect3D::GetErrorString(void)
{
    return m_szErrorMsg;
}

//////////////////////////////////////////////////////
// GetTexture()
//////////////////////////////////////////////////////
IDirect3DTexture8* CDirect3D::GetTexture(TEXTURETYPE txtType)
{
    return NULL;
}

//////////////////////////////////////////////////////
// GetSurface()
//////////////////////////////////////////////////////
IDirect3DSurface8* CDirect3D::GetSurface(SURFACETYPE surType)
{
	switch (surType)
    {
	case CDirect3D::SPLASH:
        return m_pSplashSurface;
        break;
	case CDirect3D::TITLE:
        return m_pTitleSurface;
        break;
	case CDirect3D::BACKGROUND:
        return m_pBackgroundSurface;
        break;
	case CDirect3D::SPRITES:
        return m_pSpritesSurface;
        break;
	case CDirect3D::BANNER:
        return m_pBannerSurface;
        break;
	case CDirect3D::WIN:
        return m_pWinSurface;
        break;
	}

    return NULL;
}
