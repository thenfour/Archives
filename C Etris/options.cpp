#include "options.h"
#include "stdio.h"

extern PASSVARS Pass;
extern TETRISOPTIONS toOptions;
TETRISOPTIONS toOld;

#define BMP_OPTIONS "gfx\\24options.bmp"
#define BMP_OPTIONSH "gfx\\24optionsh.bmp"

SLIDER MusicSlider;
SLIDER SFXSlider;
ONOFF MusicOnOff;
ONOFF SFXOnOff;
COMMANDBUTTON cbMusic;
COMMANDBUTTON cbSFX;
COMMANDBUTTON cbRotateCW;
COMMANDBUTTON cbRotateCCW;
COMMANDBUTTON cbCancel;
COMMANDBUTTON cbOK;
COMMANDBUTTON cbDrop;
COMMANDBUTTON cbLeft;
COMMANDBUTTON cbRight;

LPDIRECTDRAWSURFACE OptionsScreen;//surface to use for the options screen
LPDIRECTDRAWSURFACE OptionsScreenH;//surface to use for the options screen

int OptionsInfo;
bool Changed;//settings have changed (if they haven't changed, we don't need to save them upon exit
#define OPTIONS_NORMAL		0
#define OPTIONS_EXIT		1

//////////////////////////////////////////////////////////////////////////////////////////////
int OptionsMain(){
	toOld=toOptions;//save the original options.
	int OldPF=Pass.ProgramFlow;
	OptionsInfo=OPTIONS_NORMAL;
	Pass.ProgramFlow=PF_OPTIONS;

	//set up the offscreen surfaces
	OptionsScreen=DDLoadBitmap(Pass.DDObject, BMP_OPTIONS, 0,0);//load the options screen into that surface
	OptionsScreenH=DDLoadBitmap(Pass.DDObject, BMP_OPTIONSH, 0, 0);

	//initialize the screen controls
	RECT rect;
	SetRect(&rect, 34, 32, 237, 130);
	cbMusic.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbMusicProc);
	SetRect(&rect, 326, 25, 528, 128);
	cbSFX.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbSFXProc);

	SetRect(&rect, 0, 131, 469, 200);
	cbRotateCW.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbRotateCWProc);
	SetRect(&rect, 0, 200, 469, 269);
	cbRotateCCW.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbRotateCCWProc);
	SetRect(&rect, 521, 321, 634, 394);
	cbOK.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbOKProc);
	SetRect(&rect, 431, 395, 626, 473);
	cbCancel.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbCancelProc);
	SetRect(&rect, 0, 275, 469, 344);
	cbLeft.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbLeftProc);
	SetRect(&rect, 0, 352, 431, 421);
	cbRight.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbRightProc);
	SetRect(&rect, 0, 427, 430, 480);
	cbDrop.Initialize(&OptionsScreenH, &OptionsScreen, &Pass.DDBack, rect, cbDropProc);
	SFXSlider.Initialize(&Pass.DDObject, &Pass.DDBack, 332, 97, 528,128);
	SFXSlider.MaxValue=10000;
	SFXSlider.MinValue=0;
	SFXSlider.Value=toOptions.SFXVol;
	MusicSlider.Initialize(&Pass.DDObject, &Pass.DDBack, 36, 96, 231,126);
	MusicSlider.MaxValue=10000;
	MusicSlider.MinValue=0;
	MusicSlider.Value=toOptions.MusicVol;
	SetRect(&rect, 195, 43, 235, 83);
	MusicOnOff.Initialize(rect, &Pass.DDBack, &Pass.DDObject);
	MusicOnOff.Value=toOptions.Music;
	SetRect(&rect, 498, 44, 538, 84);
	SFXOnOff.Initialize(rect, &Pass.DDBack, &Pass.DDObject);
	SFXOnOff.Value=toOptions.SFX;
	
	//display the screen now
	FadeIn(Pass.DDObject, &Pass.DDFront, &OptionsScreen, 0);

	optionsUpdateScreen();
	//now it's there; here's the code for this screen
	MSG msg;
	while(OptionsInfo==OPTIONS_NORMAL && Pass.ProgramFlow!=PF_EXIT){
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if(Pass.ProgramFlow==PF_OPTIONS) Pass.ProgramFlow=OldPF;
	FadeOut(Pass.DDObject, &Pass.DDFront, &OptionsScreen, 0);
	if(OptionsScreen != NULL){
		OptionsScreen->Release();
		OptionsScreen=NULL;
		OptionsScreenH->Release();
		OptionsScreenH=NULL;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int OptionsProc(PASSPROCVARS Params){
//	message loop handler for this screen
	int r=0;
	int r2=0;
	r+=cbRotateCW.ProcFunction(Params);
	r+=cbRotateCCW.ProcFunction(Params);
	r+=cbOK.ProcFunction(Params);
	r+=cbCancel.ProcFunction(Params);
	r+=cbLeft.ProcFunction(Params);
	r+=cbRight.ProcFunction(Params);
	r+=cbDrop.ProcFunction(Params);
	r+=MusicSlider.SliderProc(Params);
	r+=SFXSlider.SliderProc(Params);
	r+=MusicOnOff.OnOffProc(Params);
	r+=SFXOnOff.OnOffProc(Params);
	r2+=cbMusic.ProcFunction(Params);
	r2+=cbSFX.ProcFunction(Params);
	r+=r2;//if the music/sfx commandbuttons changed, make sure that we get that.
	if(r2){
		//if we updated the music/sfx commandbuttons, we have to
		//update the other music/sfx controls because they overlap.
		MusicOnOff.Update();
		MusicSlider.Update();
		SFXOnOff.Update();
		SFXSlider.Update();
	}

	//and draw teh keys
	if(r){
		if(toOptions.SFX){
		Pass.Sounds.MenuBuffer2->SetVolume(SFXSlider.Value-10000);
		Pass.Sounds.MenuBuffer1->SetVolume(SFXSlider.Value-10000);
		for(int i=0;i<NUMBER_OF_DSBUFFERS;i++){
			Pass.Sounds.DSBuffer[i]->SetVolume(SFXSlider.Value-10000);
		}
		}
		Pass.Sounds.MenuScore.SetVolume(MusicSlider.Value-10000);
		Pass.Sounds.GameScore.SetVolume(MusicSlider.Value-10000);
		PrintKey(toOptions.Key_rotatecw, 20, 156);	//print all the key commands
		PrintKey(toOptions.Key_rotateccw, 20, 228);	
		PrintKey(toOptions.Key_left, 20, 303);	
		PrintKey(toOptions.Key_right, 20, 380);	
		PrintKey(toOptions.Key_drop, 20, 455);	
		Flip(Pass.DDFront);//flip the screen now that it's all new
		Blit(Pass.DDBack, OptionsScreen, 0, 0, 640, 480, 0,0);//set up the back surface
		//the reason these next 4 lines are here is because when the cursor is not in
		//that region for cbmusic/cbsfx, this area will be covered by the unhighlighted
		//stuff- this will prevent that.
			MusicOnOff.Update();
			MusicSlider.Update();
			SFXOnOff.Update();
			SFXSlider.Update();
	}

	switch(Params.uMsg){
	case WM_PAINT:
		OptionsScreen->Restore();
		OptionsScreenH->Restore();
		DDReLoadBitmap(OptionsScreen, BMP_OPTIONS);
		DDReLoadBitmap(OptionsScreenH, BMP_OPTIONSH);
		Blit(Pass.DDFront, OptionsScreen, 0, 0, 640, 480, 0, 0);
		Blit(Pass.DDBack, OptionsScreen, 0, 0, 640, 480, 0, 0);
		break;
	case WM_CLOSE:
		OptionsInfo=OPTIONS_EXIT;
		Pass.ProgramFlow=PF_EXIT;
		break;
	case WM_KEYDOWN:
		int Key=(int) Params.wParam;
		if(Key==VK_ESCAPE){
			OptionsInfo=OPTIONS_EXIT;
		}
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int cbRotateCWProc(PASSPROCVARS Params){
	int t=GetKey();
	if(t && toAvailable(t)) toOptions.Key_rotatecw=t;
	optionsUpdateScreen();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int cbRotateCCWProc(PASSPROCVARS Params){
	int t=GetKey();
	if(t && toAvailable(t)) toOptions.Key_rotateccw=t;
	optionsUpdateScreen();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbCancelProc(PASSPROCVARS Params){
	toOptions=toOld;
	OptionsInfo=OPTIONS_EXIT;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbOKProc(PASSPROCVARS Params){
	//a macro to convert a percentage to volume in decibels.
	//toOptions.Music=MusicOnOff.Value;
	//toOptions.SFX=SFXOnOff.Value;

	toOptions.SFXVol=SFXSlider.Value;
	toOptions.MusicVol=MusicSlider.Value;

	INIFILE IniFile;

	IniFile.Open("tetris.ini");
	IniFile.WriteValue("KEYCCW    ", sizeof(toOptions.Key_rotateccw), &toOptions.Key_rotateccw);
	IniFile.WriteValue("KEYCW     ", sizeof(toOptions.Key_rotatecw), &toOptions.Key_rotatecw);
	IniFile.WriteValue("KEYLEFT   ", sizeof(toOptions.Key_left), &toOptions.Key_left);
	IniFile.WriteValue("KEYRIGHT  ", sizeof(toOptions.Key_right), &toOptions.Key_right);
	IniFile.WriteValue("KEYDROP   ", sizeof(toOptions.Key_drop), &toOptions.Key_drop);
	IniFile.WriteValue("MUSIC     ", sizeof(toOptions.Music), &MusicOnOff.Value);
	IniFile.WriteValue("SFX       ", sizeof(toOptions.SFX), &SFXOnOff.Value);
	IniFile.WriteValue("MUSICVOL  ", sizeof(toOptions.MusicVol), &toOptions.MusicVol);
	IniFile.WriteValue("SFXVOL    ", sizeof(toOptions.SFXVol), &toOptions.SFXVol);
	IniFile.Close();

	//now set the volume levels.

	OptionsInfo=OPTIONS_EXIT;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbDropProc(PASSPROCVARS Params){
	int t=GetKey();
	if(t && toAvailable(t)) toOptions.Key_drop=t;
	optionsUpdateScreen();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbLeftProc(PASSPROCVARS Params){
	int t=GetKey();
	if(t && toAvailable(t)) toOptions.Key_left=t;
	optionsUpdateScreen();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbRightProc(PASSPROCVARS Params){
	int t=GetKey();
	if(t && toAvailable(t)) toOptions.Key_right=t;
	optionsUpdateScreen();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbMusicProc(PASSPROCVARS Params){
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int cbSFXProc(PASSPROCVARS Params){
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
bool toAvailable(int KeyCode){
	//returns true if it's available (not taken by any other key)
	return	!((toOptions.Key_rotateccw	==KeyCode) ||
			(toOptions.Key_rotatecw		==KeyCode) ||
			(toOptions.Key_drop			==KeyCode) ||
			(toOptions.Key_left			==KeyCode) ||
			(toOptions.Key_right		==KeyCode));
}
//////////////////////////////////////////////////////////////////////////////////////////////
int GetKey(){
while(ShowCursor(FALSE) >=0){}
	//this function returns a DI key code for the key pressed.
	//return value is the key code, 0 if it was invalid.
	//wait for input.
	bool GoOn=false;
	char buf[256];
	int i;
	int Rvalue=0;
	while(!GoOn){
		//and now a message loop
		
		//get device state.
		Pass.DIKeyboard->GetDeviceState(sizeof(buf), (VOID*)buf);
		//find out if they pressed anything.
		//each member of buf[] corresponds to a key; 
		//if that button is pressed; then it's high bit will be set.
		for(i=0;i<255;i++){
			//check if buf[i]'s high bit is set.
			if(buf[i] & 0x80){
				//it was pressed.
				GoOn=true;
				if(i != DIK_ESCAPE) Rvalue=i;//i is the VK_code for that key
				//and don't set it equal unless we DIDN't hit escape
			}
		}

	}

while(ShowCursor(TRUE) <0){}
	return Rvalue;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int PrintKey(int DIKCode, int x, int y){
	//prints the name of the key (DIKCode) at position X,Y
	char str[30];//string to print on the screen

	switch(DIKCode){
	case DIK_ESCAPE:
		lstrcpy(str, "ESC");
		break;
	case DIK_1:
		lstrcpy(str, "1");
		break;
	case DIK_2:
		lstrcpy(str, "2");
		break;
	case DIK_3:
		lstrcpy(str, "3");
		break;
	case DIK_4:
		lstrcpy(str, "4");
		break;
	case DIK_5:
		lstrcpy(str, "5");
		break;
	case DIK_6:
		lstrcpy(str, "6");
		break;
	case DIK_7:
		lstrcpy(str, "7");
		break;
	case DIK_8:
		lstrcpy(str, "8");
		break;
	case DIK_9:
		lstrcpy(str, "9");
		break;
	case DIK_0:
		lstrcpy(str, "0");
		break;
	case DIK_MINUS:
		lstrcpy(str, "-");
		break;
	case DIK_EQUALS:
		lstrcpy(str, "=");
		break;
	case DIK_BACK:
		lstrcpy(str, "<-");
		break;
	case DIK_TAB:
		lstrcpy(str, "TAB");
		break;
	case DIK_Q:
		lstrcpy(str, "q");
		break;
	case DIK_W:
		lstrcpy(str, "w");
		break;
	case DIK_E:
		lstrcpy(str, "e");
		break;
	case DIK_R:
		lstrcpy(str, "r");
		break;
	case DIK_T:
		lstrcpy(str, "t");
		break;
	case DIK_Y:
		lstrcpy(str, "y");
		break;
	case DIK_U:
		lstrcpy(str, "u");
		break;
	case DIK_I:
		lstrcpy(str, "i");
		break;
	case DIK_O:
		lstrcpy(str, "o");
		break;
	case DIK_P:
		lstrcpy(str, "p");
		break;
	case DIK_LBRACKET:
		lstrcpy(str, "[");
		break;
	case DIK_RBRACKET:
		lstrcpy(str, "]");
		break;
	case DIK_RETURN:
		lstrcpy(str, "ENTER");
		break;
	case DIK_LCONTROL:
		lstrcpy(str, "L Control");
		break;
	case DIK_A:
		lstrcpy(str, "a");
		break;
	case DIK_S:
		lstrcpy(str, "s");
		break;
	case DIK_D:
		lstrcpy(str, "d");
		break;
	case DIK_F:
		lstrcpy(str, "f");
		break;
	case DIK_G:
		lstrcpy(str, "g");
		break;
	case DIK_H:
		lstrcpy(str, "h");
		break;
	case DIK_J:
		lstrcpy(str, "j");
		break;
	case DIK_K:
		lstrcpy(str, "k");
		break;
	case DIK_L:
		lstrcpy(str, "l");
		break;
	case DIK_SEMICOLON:
		lstrcpy(str, ";");
		break;
	case DIK_APOSTROPHE:
		lstrcpy(str, "'");
		break;
	case DIK_GRAVE:
		lstrcpy(str, "GRAVE");
		break;
	case DIK_LSHIFT:
		lstrcpy(str, "Left Shift");
		break;
	case DIK_BACKSLASH:
		lstrcpy(str, "Backslash");
		break;
	case DIK_Z:
		lstrcpy(str, "z");
		break;
	case DIK_X:
		lstrcpy(str, "x");
		break;
	case DIK_C:
		lstrcpy(str, "c");
		break;
	case DIK_V:
		lstrcpy(str, "v");
		break;
	case DIK_B:
		lstrcpy(str, "b");
		break;
	case DIK_N:
		lstrcpy(str, "n");
		break;
	case DIK_M:
		lstrcpy(str, "m");
		break;
	case DIK_COMMA:
		lstrcpy(str, "(comma)");
		break;
	case DIK_PERIOD:
		lstrcpy(str, "(period)");
		break;
	case DIK_SLASH:
		lstrcpy(str, "/");
		break;
	case DIK_RSHIFT:
		lstrcpy(str, "Right shift");
		break;
	case DIK_MULTIPLY:
		lstrcpy(str, "Num *");
		break;
	case DIK_LMENU:
		lstrcpy(str, "L Menu");
		break;
	case DIK_SPACE:
		lstrcpy(str, "Space");
		break;
	case DIK_CAPITAL:
		lstrcpy(str, "Caps");
		break;
	case DIK_F1:
		lstrcpy(str, "F1");
		break;
	case DIK_F2:
		lstrcpy(str, "F2");
		break;
	case DIK_F3:
		lstrcpy(str, "F3");
		break;
	case DIK_F4:
		lstrcpy(str, "F4");
		break;
	case DIK_F5:
		lstrcpy(str, "F5");
		break;
	case DIK_F6:
		lstrcpy(str, "F6");
		break;
	case DIK_F7:
		lstrcpy(str, "F7");
		break;
	case DIK_F8:
		lstrcpy(str, "F8");
		break;
	case DIK_F9:
		lstrcpy(str, "F9");
		break;
	case DIK_F10:
		lstrcpy(str, "F10");
		break;
	case DIK_NUMLOCK:
		lstrcpy(str, "Num Lock");
		break;
	case DIK_SCROLL:
		lstrcpy(str, "Scroll Lock");
		break;
	case DIK_NUMPAD7:
		lstrcpy(str, "Num 7");
		break;
	case DIK_NUMPAD8:
		lstrcpy(str, "Num 8");
		break;
	case DIK_NUMPAD9:
		lstrcpy(str, "Num 9");
		break;
	case DIK_SUBTRACT:
		lstrcpy(str, "Num -");
		break;
	case DIK_NUMPAD4:
		lstrcpy(str, "Num 4");
		break;
	case DIK_NUMPAD5:
		lstrcpy(str, "Num 5");
		break;
	case DIK_NUMPAD6:
		lstrcpy(str, "Num 6");
		break;
	case DIK_ADD:
		lstrcpy(str, "Num +");
		break;
	case DIK_NUMPAD1:
		lstrcpy(str, "Num 1");
		break;
	case DIK_NUMPAD2:
		lstrcpy(str, "Num 2");
		break;
	case DIK_NUMPAD3:
		lstrcpy(str, "Num 3");
		break;
	case DIK_NUMPAD0:
		lstrcpy(str, "Num 0");
		break;
	case DIK_DECIMAL:
		lstrcpy(str, "Num Decimal");
		break;
	case DIK_F11:
		lstrcpy(str, "F11");
		break;
	case DIK_F12:
		lstrcpy(str, "F12");
		break;
	case DIK_NUMPADEQUALS:
		lstrcpy(str, "Num =");
		break;
	case DIK_NUMPADENTER:
		lstrcpy(str, "Num Enter");
		break;
	case DIK_RCONTROL:
		lstrcpy(str, "Right Control");
		break;
	case DIK_NUMPADCOMMA:
		lstrcpy(str, "Num comma");
		break;
	case DIK_DIVIDE:
		lstrcpy(str, "Num Divide");
		break;
	case DIK_SYSRQ:
		lstrcpy(str, "SYSRQ");
		break;
	case DIK_RMENU:
		lstrcpy(str, "R Menu");
		break;
	case DIK_HOME:
		lstrcpy(str, "Home");
		break;
	case DIK_UP:
		lstrcpy(str, "Up");
		break;
	case DIK_PRIOR:
		lstrcpy(str, "Prior");
		break;
	case DIK_LEFT:
		lstrcpy(str, "Left");
		break;
	case DIK_RIGHT:
		lstrcpy(str, "Right");
		break;
	case DIK_END:
		lstrcpy(str, "End");
		break;
	case DIK_DOWN:
		lstrcpy(str, "Down");
		break;
	case DIK_NEXT:
		lstrcpy(str, "Next");
		break;
	case DIK_INSERT:
		lstrcpy(str, "Insert");
		break;
	case DIK_DELETE:
		lstrcpy(str, "Del");
		break;
	case DIK_LWIN:
		lstrcpy(str, "Left Windows");
		break;
	case DIK_RWIN:
		lstrcpy(str, "Right Windows");
		break;
	case DIK_APPS:
		lstrcpy(str, "App Key");
		break;
	}
	//print it now.
	DrawString(str, x, y, 12);
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//unused
int VKtoDIK(int VK){
	return 0;
}

int optionsUpdateScreen(){
	Blit(Pass.DDBack, OptionsScreen, 0, 0, 640, 480, 0,0);
		PrintKey(toOptions.Key_rotatecw, 20, 156);	//print all the key commands
		PrintKey(toOptions.Key_rotateccw, 20, 228);	
		PrintKey(toOptions.Key_left, 20, 303);	
		PrintKey(toOptions.Key_right, 20, 380);	
		PrintKey(toOptions.Key_drop, 20, 455);	
	MusicOnOff.Update();
	MusicSlider.Update();
	SFXOnOff.Update();
	SFXSlider.Update();
	Flip(Pass.DDFront);
	Blit(Pass.DDBack, OptionsScreen, 0, 0, 640, 480, 0,0);
	MusicOnOff.Update();
	MusicSlider.Update();
	SFXOnOff.Update();
	SFXSlider.Update();
	return 0;
}