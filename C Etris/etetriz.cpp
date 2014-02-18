#include "etetriz.h"
#include "stdio.h"

unsigned int SystemCheck();
LPDIRECTDRAWSURFACE WaitScreen;

#define BMP_WAIT "gfx\\24Wait.bmp"
#define WAV_MENUSCORE "sfx\\Menuscore.wav"
#define WAV_GAMESCORE "sfx\\Gamescore.wav"
#define WAV_MOUSEOVER "sfx\\beep1.wav"
#define WAV_CLICK "sfx\\beep2.wav"

HIGHSCORES HighScores;
PASSVARS Pass;
TETRISOPTIONS toOptions;
INIFILE  IniFile;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance, LPSTR  lpszCmdLine, int  nCmdShow){
time_t tm;
time_t oldtime;
time(&oldtime);
unsigned int count1=0;
unsigned int count2=0;
unsigned int countaverage=0;
Pass.ProgramFlow=PF_STARTUP;//State that we are in the setup of the game
Pass.hInstance=hInstance;

int ReturnValue;
LRESULT Result;

//make a window, make it DirectDraw...full-screen 640x480x8
Pass.DDWindow=MakeAWindow(MainWindowProc,hInstance, "MainWindow");

//set up the object and its parameters
Result=SetupDDObject(Pass.DDObject, Pass.DDWindow);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "SetupDDObject");
//set up the primary buffer
Result=SetupPriSurface(Pass.DDObject, Pass.DDFront);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "SetupPriSurface");
//Setup the back buffer
Result=SetupBackSurface(Pass.DDObject, Pass.DDFront, Pass.DDBack);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "SetupBackSurface");


WaitScreen=DDLoadBitmap(Pass.DDObject, BMP_WAIT, 0, 0);
Blit(Pass.DDFront, WaitScreen, 0, 0, 640, 480, 0, 0);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "Display Main Window");

//Setup the fonts surfaces
Pass.DDFont8=DDLoadBitmap(Pass.DDObject, BMP_FONT8, 0, 0);
if(!Pass.DDFont8) return DD_OK;
DDSetColorKey(Pass.DDFont8, CLR_INVALID);

Pass.DDFont22=DDLoadBitmap(Pass.DDObject, BMP_FONT22, 0, 0);
if(!Pass.DDFont22) return DD_OK;
DDSetColorKey(Pass.DDFont22, CLR_INVALID);

Pass.DDFont12=DDLoadBitmap(Pass.DDObject, BMP_FONT12, 0, 0);
if(!Pass.DDFont12) return DD_OK;
DDSetColorKey(Pass.DDFont12, CLR_INVALID);

Pass.DDFont13=DDLoadBitmap(Pass.DDObject, BMP_FONT13, 0, 0);
if(!Pass.DDFont13) return DD_OK;
DDSetColorKey(Pass.DDFont13, CLR_INVALID);

//first system check
count1=SystemCheck();

//now begin printing on the screen.
Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Setting up alpha table", 0, 0, 8);
Flip(Pass.DDFront);

//Now set up the Pass.Rgb16 struct
IniRGB16(&Pass.DDFront);
InitAlphaTable();

//setup the random number generator
//this is really clumsy looking, but I don't know the msvc time functions well
time(&tm);
srand(tm);

//get the high scores from disk
Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Reading High Scores", 0, 0, 8);
Flip(Pass.DDFront);
ReadHighScores();

//get the options in toOptions
Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Reading Options", 0, 0, 8);
Flip(Pass.DDFront);
IniFile.Open("tetris.ini");
toOptions.Key_drop=DIK_DOWN;
toOptions.Key_right=DIK_RIGHT;
toOptions.Key_left=DIK_LEFT;
toOptions.Key_rotateccw=DIK_0;
toOptions.Key_rotatecw=DIK_UP;
toOptions.Music=true;
toOptions.MusicVol=100;
toOptions.SFX=true;
toOptions.SFXVol=100;

IniFile.ReadValue("KEYCCW    ", &toOptions.Key_rotateccw);
IniFile.ReadValue("KEYCW     ", &toOptions.Key_rotatecw);
IniFile.ReadValue("KEYLEFT   ", &toOptions.Key_left);
IniFile.ReadValue("KEYRIGHT  ", &toOptions.Key_right);
IniFile.ReadValue("KEYDROP   ", &toOptions.Key_drop);
IniFile.ReadValue("MUSIC     ", &toOptions.Music);
IniFile.ReadValue("SFX       ", &toOptions.SFX);
IniFile.ReadValue("MUSICVOL  ", &toOptions.MusicVol);
IniFile.ReadValue("SFXVOL    ", &toOptions.SFXVol);

IniFile.Close();
int i;
Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Setting up DirectSound", 0, 0, 8);
Flip(Pass.DDFront);
//DirectSound stuff;
Result=SetupDSound();
	if(Result==DSERR_BADFORMAT) return Error2(Pass.DDWindow, "BADFORMAT");
	if(Result==DSERR_INVALIDCALL) return Error2(Pass.DDWindow, "invalid call");
	if(Result==DSERR_UNSUPPORTED) return Error2(Pass.DDWindow, "us");
	if(Result==DSERR_PRIOLEVELNEEDED) return Error2(Pass.DDWindow, "pln");
	if(Result==DSERR_OUTOFMEMORY) return Error2(Pass.DDWindow, "oom$$$$$$$");
	if(Result==DSERR_INVALIDPARAM) return Error2(Pass.DDWindow, "%^^^^");
	if(Result!=DS_OK) return Error2(Pass.DDWindow, "DirectSound Initialization Failed");

	Result=CreateSecondaryBuffer(&Pass.Sounds.MenuBuffer1, 2);
	if(Result!=DS_OK) return Error2(Pass.DDWindow, "blue in the face?");
	Result=CreateSecondaryBuffer(&Pass.Sounds.MenuBuffer2, 2);
	if(Result!=DS_OK) return Error2(Pass.DDWindow, "blue in the face?");
	for(i=0;i<NUMBER_OF_DSBUFFERS; i++){
		Result=CreateSecondaryBuffer(&(Pass.Sounds.DSBuffer[i]), 3);
		if(Result!=DS_OK) return Error2(Pass.DDWindow, "Create Buffer Failed");
	}
	//load the menu sounds into their buffers
	LoadWaveFile(&Pass.Sounds.MenuBuffer1, WAV_MOUSEOVER);
	LoadWaveFile(&Pass.Sounds.MenuBuffer2, WAV_CLICK);

	Pass.Sounds.MenuScore.Initialize(&Pass.DSObject, WAV_MENUSCORE);
		if(Result!=DS_OK) return Error2(Pass.DDWindow, "Levi's Trousers on sale now!");
	Pass.Sounds.GameScore.Initialize(&Pass.DSObject, WAV_GAMESCORE);
		if(Result!=DS_OK) return Error2(Pass.DDWindow, "Levi's Trousers on sale now!");
OutputDebugString("Setting up game sounds");
Pass.Sounds.MenuScore.SetVolume(toOptions.MusicVol-10000);
Pass.Sounds.GameScore.SetVolume(toOptions.MusicVol-10000);
if(toOptions.SFX){
	Pass.Sounds.MenuBuffer2->SetVolume(toOptions.SFXVol-10000);
	Pass.Sounds.MenuBuffer1->SetVolume(toOptions.SFXVol-10000);
for(i=0;i<NUMBER_OF_DSBUFFERS;i++){
	Pass.Sounds.DSBuffer[i]->SetVolume(toOptions.SFXVol-10000);
}
}
Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Setting up DirectInput", 0, 0, 8);
Flip(Pass.DDFront);
//DIRECT INPUT STUFF NOW
Result=SetupDIObject(Pass.DIObject,hInstance);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "DI Object");
Result=SetupDIDevice(Pass.DIObject,Pass.DIKeyboard, Pass.DDWindow);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "DI DEVICE");
Result=Acquire(Pass.DIKeyboard);
	if(Result!=DD_OK) return Error2(Pass.DDWindow, "DI Keyboard");

//second system check
count2=SystemCheck();
//now find the average.
countaverage=(int)(((float)(count1+count2))/2);
//7763 is the average I get on my machine; and I should wait 10 ms each frame.
//using that info, we calculate how much they should wait on their machine
//it's proportional, so that if they get 15527, they should wait 20 ms.
Pass.FrameWait=(countaverage*10)/7763;

Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
DrawString("Finishing up...", 0, 0, 8);
char str[100];
sprintf(str, "Count1: %lu Count2: %lu\n", count1, count2);
OutputDebugString(str);
Flip(Pass.DDFront);
//do other setup stuff
PauseInit();
//Now we're all set up and ready for the game!
Result=FadeOut(Pass.DDObject, &Pass.DDFront, &WaitScreen, 0);
switch(Result){
case DDERR_SURFACELOST:
	OutputDebugString("surface monkey");
	break;
case DDERR_INVALIDOBJECT:
	OutputDebugString("fallopian");
	break;
case DDERR_INVALIDPARAMS:
	OutputDebugString("seminal");
	break;
case DDERR_OUTOFMEMORY:
	OutputDebugString("touch");
	break;
case DDERR_SURFACEBUSY:
	OutputDebugString("porcuphone");
	break;
case DDERR_WASSTILLDRAWING:
	OutputDebugString("armen");
	break;

case DDERR_NOFLIPHW:
	OutputDebugString("shucks");
	break;
case DDERR_UNSUPPORTED:
	OutputDebugString("shucks");
	break;
case DDERR_GENERIC:
	OutputDebugString("shucks");
	break;
case DDERR_NOTFLIPPABLE:
	OutputDebugString("shucks");
	break;
case DD_OK:
	OutputDebugString("CAPM");
	break;
default:
	OutputDebugString("jesus");
	break;
}






if(WaitScreen != NULL){
	WaitScreen->Release();
	WaitScreen=NULL;
}

////////////////////////////
do{
	Pass.Sounds.MenuScore.Stop();
	Pass.Sounds.MenuScore.Play();
	ReturnValue=SettingsMain();
	//now based on what we get from Settings, direct the program
	switch(Pass.SettingsInfo){
		case SETTINGS_PLAY:
			Pass.Sounds.MenuScore.Stop();
			GameMain();
		break;
	}
}while(Pass.ProgramFlow!=PF_EXIT);
////////////////////////////
Pass.Sounds.MenuScore.Stop();
StrikeTheSet();

return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainWindowProc(HWND  hwnd, UINT  uMsg, WPARAM  wParam, LPARAM  lParam)
{
PASSPROCVARS Params;
Params.hwnd=hwnd;
Params.uMsg=uMsg;
Params.wParam=wParam;
Params.lParam=lParam;

if(Pass.DIKeyboard != NULL){
	Pass.DIKeyboard->Acquire();
}

switch(uMsg)
{
case WM_PAINT:
	Pass.DDFront->Restore();
	Pass.DDBack->Restore();
	RestoreFonts();
	pauseRestore();
	break;
case WM_CLOSE:
	Pass.ProgramFlow=PF_EXIT;
	break;
}
switch(Pass.ProgramFlow){
	case PF_DISPLAYHIGHSCORES:
   		DisplayHighScoresProc(Params);
	break;
	case PF_CREDITS:
		CreditsProc(Params);
	break;
	case PF_PAUSE:
		PauseProc(Params);
	break;
	case PF_GAME:
   		GameWindowProc(Params);
	break;
	case PF_SETTINGS:
   		SettingsWindowProc(Params);
	break;
	case PF_OPTIONS:
		OptionsProc(Params);
		break;
	case PF_TEXTEDITBOX:
		TextEditProc(Params);
	break;
}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Error(HWND hwnd){
	StrikeTheSet();
	MessageBox(hwnd, "Error", "Error", MB_OK);
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Error2(HWND hwnd, LPCTSTR string){
	StrikeTheSet();
	MessageBox(hwnd, string, "Error", MB_OK);
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StrikeTheSet(){
	//destroy the DirectX objects

	//DirectDraw:

	if(Pass.DDObject != NULL){
		if(Pass.DDFront != NULL){
			Pass.DDFront->Release();
			Pass.DDFront=0;
		}
		Pass.DDObject->Release();
		Pass.DDObject=0;
	}

	//DirectInput:
	if(Pass.DIObject != NULL){
		if(Pass.DIKeyboard){
			Pass.DIKeyboard->Unacquire();
			Pass.DIKeyboard->Release();
			Pass.DIKeyboard=0;
		}
		Pass.DIObject->Release();
		Pass.DIObject=0;
	}

	//DirectSound:
	if(Pass.DSObject != NULL){
		if(Pass.Sounds.Primary != NULL){
			Pass.Sounds.Primary->Release();
			Pass.Sounds.Primary=NULL;
			Pass.Sounds.MenuBuffer1->Release();
			Pass.Sounds.MenuBuffer1=NULL;
			Pass.Sounds.MenuBuffer2->Release();
			Pass.Sounds.MenuBuffer2=NULL;
		}
		for(int i=0; i<NUMBER_OF_DSBUFFERS; i++){
			if(Pass.Sounds.DSBuffer[i] != NULL){
				Pass.Sounds.DSBuffer[i]->Release();
				Pass.Sounds.DSBuffer[i]=NULL;
			}
		}
		Pass.DSObject->Release();
		Pass.DSObject=0;
	}
return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int SystemCheck(){
//performs a check on the system speed that lasts no longer than 4 seconds
	unsigned int count=0;
	time_t tm;
	time_t oldtime;

	Blit(Pass.DDBack, WaitScreen, 0, 0, 640, 480, 0, 0);
	DrawString("Performing 3-second system speed test...", 0, 0, 8);
	Flip(Pass.DDFront);
	//now figure out the speed of this puppy so we can set Pass.FrameWait to an appropriate value
	//for two seconds, we'll count...we'll see what number we end up with.

	time(&oldtime);
	//first wait for a new second to come
	do{
		time(&tm);
	}while(oldtime==tm);
	//now start...
	time(&oldtime);
	do{
	count++;
	time(&tm);
	} while(tm < oldtime+3);
	/*ok; on my computer, I get between here
	*/
	return count;
}
