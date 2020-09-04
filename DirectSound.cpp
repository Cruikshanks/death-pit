//////////////////////////////////////////////////////
// DirectSound.cpp
//////////////////////////////////////////////////////

#include "directsound.h"

//////////////////////////////////////////////////////
// CDirectSound()
//////////////////////////////////////////////////////
CDirectSound::CDirectSound(void)
{
    m_pDirectSoundObj = NULL;

    m_pLoadingSound = NULL;
	m_pStartSound = NULL;
	m_pPingSound = NULL;
	m_pScreamSound = NULL;
	m_pLostSound = NULL;
	m_pWinSound = NULL;
}

//////////////////////////////////////////////////////
// ~CDirectSound()
//////////////////////////////////////////////////////
CDirectSound::~CDirectSound(void)
{
    CleanUp();
}

//////////////////////////////////////////////////////
// CleanUp()
//////////////////////////////////////////////////////
void CDirectSound::CleanUp()
{
	if (m_pWinSound)
        m_pWinSound->Release();
	if (m_pLostSound)
        m_pLostSound->Release();
	if (m_pScreamSound)
        m_pScreamSound->Release();
	if (m_pPingSound)
        m_pPingSound->Release();
	if (m_pStartSound)
        m_pStartSound->Release();
    if (m_pLoadingSound)
        m_pLoadingSound->Release();

    if (m_pDirectSoundObj)
        m_pDirectSoundObj->Release();
}

//////////////////////////////////////////////////////
// SetWindowHandle()
//////////////////////////////////////////////////////
void CDirectSound::SetWindowHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}

//////////////////////////////////////////////////////
// InitDirectSound()
//////////////////////////////////////////////////////
HRESULT CDirectSound::InitDirectSound()
{
    HRESULT hResult = DirectSoundCreate8(NULL, &m_pDirectSoundObj, NULL);
    if (hResult != DS_OK)
        return hResult;

    hResult = m_pDirectSoundObj->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL);
    if (hResult != DS_OK)
        return hResult;

	hResult = CreateSoundBuffers();
    if (hResult != DS_OK)
        return hResult;

    return DS_OK;
}

//////////////////////////////////////////////////////
// CreateSoundBuffers()
//////////////////////////////////////////////////////
HRESULT CDirectSound::CreateSoundBuffers(void)
{
	HRESULT hResult = LoadSoundEffect("Sounds\\Loading.wav", &m_pLoadingSound);
	if (hResult != DS_OK)
        return hResult;

	hResult = LoadSoundEffect("Sounds\\Start.wav", &m_pStartSound);
	if (hResult != DS_OK)
        return hResult;

	hResult = LoadSoundEffect("Sounds\\Ping.wav", &m_pPingSound);
	if (hResult != DS_OK)
        return hResult;

	hResult = LoadSoundEffect("Sounds\\Scream.wav", &m_pScreamSound);
	if (hResult != DS_OK)
        return hResult;

	hResult = LoadSoundEffect("Sounds\\Lost.wav", &m_pLostSound);
	if (hResult != DS_OK)
        return hResult;

	hResult = LoadSoundEffect("Sounds\\Win.wav", &m_pWinSound);
	if (hResult != DS_OK)
        return hResult;

	return DS_OK;
}

//////////////////////////////////////////////////////
// PlaySound()
//////////////////////////////////////////////////////
HRESULT CDirectSound::PlaySound(IDirectSoundBuffer* pBuf)
{
    HRESULT hResult = pBuf->SetCurrentPosition(0);
    if (hResult != DS_OK)
        return hResult;

	hResult = pBuf->Play(0, 0, 0);
    if (hResult != DS_OK)
        return hResult;

    return hResult;
}

//////////////////////////////////////////////////////
// LoadSoundEffect()
//////////////////////////////////////////////////////
HRESULT CDirectSound::LoadSoundEffect(char* pstrFileName, 
    IDirectSoundBuffer** ppSoundBuf)
{
    CWave* pWave = new CWave(pstrFileName);
	if (!pWave->WaveOK())
	{
		MessageBox(m_hWnd, "Could not load wave file.", "Error", MB_OK);
        PostQuitMessage(WM_QUIT);
		return 0;
	}

    LPWAVEFORMATEX pWaveFormatEx = pWave->GetWaveFormatPtr();
    char* pWaveData = pWave->GetWaveDataPtr();
    DWORD dwWaveSize = pWave->GetWaveSize();

    DSBUFFERDESC dsBufferDesc;
    memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
    dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    dsBufferDesc.dwBufferBytes = dwWaveSize;
    dsBufferDesc.lpwfxFormat = (LPWAVEFORMATEX) pWaveFormatEx;

    IDirectSoundBuffer* pDSBuf;
	HRESULT hResult = m_pDirectSoundObj->
		CreateSoundBuffer(&dsBufferDesc, &pDSBuf, NULL);
    if (hResult != DS_OK)
        return hResult;

    LPVOID pSoundBlock1;
    LPVOID pSoundBlock2;
    DWORD dwBytesSoundBlock1;
    DWORD dwBytesSoundBlock2;
    hResult = pDSBuf->Lock(0, dwWaveSize,
        &pSoundBlock1, &dwBytesSoundBlock1,
        &pSoundBlock2, &dwBytesSoundBlock2, 0);
    if (hResult != DS_OK)
        return hResult;

    memcpy((void*)pSoundBlock1, pWaveData, dwWaveSize);

	pDSBuf->Unlock(pSoundBlock1, dwBytesSoundBlock1,
            pSoundBlock2, dwBytesSoundBlock2);

	delete pWave;
    *ppSoundBuf = pDSBuf;
    return hResult;
}

//////////////////////////////////////////////////////
// GetErrorString()
//////////////////////////////////////////////////////
char* CDirectSound::GetErrorString(void)
{
    return m_szErrorMsg;
}

//////////////////////////////////////////////////////
// GetSound()
//////////////////////////////////////////////////////
IDirectSoundBuffer* CDirectSound::GetSound(CDirectSound::SOUNDTYPE soundType)
{
	switch (soundType)
    {
	case CDirectSound::LOADING:
        return m_pLoadingSound;
        break;
	case CDirectSound::START:
        return m_pStartSound;
        break;
	case CDirectSound::PING:
        return m_pPingSound;
        break;
	case CDirectSound::SCREAM:
        return m_pScreamSound;
        break;
	case CDirectSound::LOST:
        return m_pLostSound;
        break;
	case CDirectSound::WIN:
        return m_pWinSound;
        break;
    }

    return NULL;
}