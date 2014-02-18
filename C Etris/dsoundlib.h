#ifndef __DSOUNDLIB
#define __DSOUNDLIB

#include "dsound.h"
#include "etetriz.h"

extern HRESULT SetupDSound();
extern HRESULT CreateSecondaryBuffer(LPDIRECTSOUNDBUFFER * NewBuffer, int Seconds);
extern HRESULT Play(LPDIRECTSOUNDBUFFER * Buffer);
extern HRESULT PlayWaveFile(LPCSTR File);

#endif