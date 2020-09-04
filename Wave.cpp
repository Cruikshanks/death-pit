///////////////////////////////////////////////////////////
// Wave.cpp
///////////////////////////////////////////////////////////

#include "windowsx.h"
#include "wave.h"

///////////////////////////////////////////////////////////
// CWave::CWave()
///////////////////////////////////////////////////////////
CWave::CWave(char* fileName)
{
    m_dwWaveSize = 0;
    m_bWaveOK = FALSE;
    m_pWave = NULL;
    m_bWaveOK = LoadWaveFile(fileName);
}

///////////////////////////////////////////////////////////
// CWave::~CWave()
///////////////////////////////////////////////////////////
CWave::~CWave()
{
    GlobalFreePtr(m_pWave);
}

///////////////////////////////////////////////////////////
// CWave::LoadWaveFile()
///////////////////////////////////////////////////////////
BOOL CWave::LoadWaveFile(char* fileName)
{
    MMCKINFO mmCkInfoRIFF;
    MMCKINFO mmCkInfoChunk;
    MMRESULT result;
    HMMIO hMMIO;
    long bytesRead;

    // Open the wave file.
    hMMIO = mmioOpen(fileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (hMMIO == NULL)
        return FALSE;

    // Descend into the RIFF chunk.
    mmCkInfoRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    result = mmioDescend(hMMIO, &mmCkInfoRIFF, NULL, MMIO_FINDRIFF);
    if (result != MMSYSERR_NOERROR)
        return FALSE;

    // Descend into the format chunk.
    mmCkInfoChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    result = mmioDescend(hMMIO, &mmCkInfoChunk,
        &mmCkInfoRIFF, MMIO_FINDCHUNK);
    if (result != MMSYSERR_NOERROR)
        return FALSE;

    // Read the format information into the WAVEFORMATEX structure.
    bytesRead = mmioRead(hMMIO, (char*)&m_waveFormatEx,
        sizeof(WAVEFORMATEX));
    if (bytesRead == -1)
        return FALSE;

    // Ascend out of the format chunk.
    result = mmioAscend(hMMIO, &mmCkInfoChunk, 0);
    if (result != MMSYSERR_NOERROR)
        return FALSE;

    // Descend into the data chunk.
    mmCkInfoChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    result = mmioDescend(hMMIO, &mmCkInfoChunk,
        &mmCkInfoRIFF, MMIO_FINDCHUNK);
    if (result != MMSYSERR_NOERROR)
        return FALSE;

    // Save the size of the wave data.
    m_dwWaveSize = mmCkInfoChunk.cksize;

    // Allocate a buffer for the wave data.
    m_pWave = (char*)GlobalAllocPtr(GMEM_MOVEABLE, m_dwWaveSize);
    if (m_pWave == NULL)
        return FALSE;

    // Read the wave data into the buffer.
    bytesRead = mmioRead(hMMIO, (char*)m_pWave, m_dwWaveSize);
    if (bytesRead == -1)
        return FALSE;
    mmioClose(hMMIO, 0);

    return TRUE;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveSize()
///////////////////////////////////////////////////////////
DWORD CWave::GetWaveSize()
{
    return m_dwWaveSize;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveFormatPtr()
///////////////////////////////////////////////////////////
LPWAVEFORMATEX CWave::GetWaveFormatPtr()
{
    return &m_waveFormatEx;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveDataPtr()
///////////////////////////////////////////////////////////
char* CWave::GetWaveDataPtr()
{
    return m_pWave;
}

///////////////////////////////////////////////////////////
// CWave::WaveOK()
///////////////////////////////////////////////////////////
BOOL CWave::WaveOK()
{
    return m_bWaveOK;
}
