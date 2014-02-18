//options.h

#ifndef __OPTIONS
#define __OPTIONS

#include "passvars.h"
#include "slider.h"
#include "onoff.h"
#include "textlib.h"
#include "textfilelib.h"
#include "windows.h"
int optionsUpdateScreen();

int OptionsMain();
int OptionsProc(PASSPROCVARS Params);
int cbRotateCWProc(PASSPROCVARS Params);
int cbRotateCCWProc(PASSPROCVARS Params);
int cbCancelProc(PASSPROCVARS Params);
int cbOKProc(PASSPROCVARS Params);
int cbDropProc(PASSPROCVARS Params);
int cbLeftProc(PASSPROCVARS Params);
int cbRightProc(PASSPROCVARS Params);
int cbMusicProc(PASSPROCVARS Params);
int cbSFXProc(PASSPROCVARS Params);

bool toAvailable(int KeyCode);

int PrintKey(int DIKCode, int x, int y);
int GetKey();
int VKtoDIK(int VK);


#endif