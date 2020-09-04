///////////////////////////////////////////////////////////
// Wave.h
///////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <mmsystem.h>

class CWave
{
protected:
    DWORD m_dwWaveSize;
    BOOL m_bWaveOK;
    char* m_pWave;
    WAVEFORMATEX m_waveFormatEx;

public:
    CWave(char* fileName);
    ~CWave();

    DWORD GetWaveSize();
    LPWAVEFORMATEX GetWaveFormatPtr();
    char* GetWaveDataPtr();
    BOOL WaveOK();

protected:
    BOOL LoadWaveFile(char* fileName);
};