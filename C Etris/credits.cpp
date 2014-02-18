//credits.cpp
#include "credits.h"
extern PASSVARS Pass;

#define BMP_CREDITS "gfx\\24Credits.bmp"
#define BMP_CREDITSH "gfx\\24Creditsh.bmp"

LPDIRECTDRAWSURFACE ddsCredits;
LPDIRECTDRAWSURFACE ddsCreditsh;

COMMANDBUTTON ccbOK;

#define CREDITS_NORMAL 0
#define CREDITS_EXIT 1

int CreditsInfo;

/////////////////////////////////////////////////////////////////////////////////////
void CreditsMain(){
	CreditsInfo=CREDITS_NORMAL;
	ddsCredits=DDLoadBitmap(Pass.DDObject, BMP_CREDITS, 0, 0);
	ddsCreditsh=DDLoadBitmap(Pass.DDObject, BMP_CREDITSH, 0, 0);
	RECT rect;
	SetRect(&rect, 237, 336, 385, 417);
	ccbOK.Initialize(&ddsCreditsh, &ddsCredits, &Pass.DDBack, rect, ccbOKProc);
	int OldPF=Pass.ProgramFlow;
	Pass.ProgramFlow=PF_CREDITS;
	FadeIn(Pass.DDObject, &Pass.DDFront, &ddsCredits, 0);
	Blit(Pass.DDBack, ddsCredits, 0, 0, 640, 480, 0, 0);
MSG Msg;
	//messageloop
	while(CreditsInfo != CREDITS_EXIT){
		if(!GetMessage(&Msg, NULL, 0, 0)){
			//trying to quit
   			Pass.ProgramFlow=PF_EXIT;
	   		CreditsInfo=CREDITS_EXIT;
		}
		else{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	FadeOut(Pass.DDObject, &Pass.DDFront, &ddsCredits, 0);
	if(Pass.ProgramFlow != PF_EXIT) Pass.ProgramFlow=OldPF;

	//destroy the surfaces
	if(ddsCredits != NULL){
		ddsCredits->Release();
		ddsCredits=NULL;
		ddsCreditsh->Release();
		ddsCreditsh=NULL;
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////////////
int CreditsProc(PASSPROCVARS Params){
	int r=0;
	r+=ccbOK.ProcFunction(Params);
	if(r){
		//we have to flip the screen
		Flip(Pass.DDFront);
		Blit(Pass.DDBack, ddsCredits, 0, 0, 640, 480, 0, 0);
	}
//now handle some messages
	switch(Params.uMsg){
	case WM_CLOSE:
		Pass.ProgramFlow=PF_EXIT;
		CreditsInfo=CREDITS_EXIT;
		break;
	case WM_PAINT:
		ddsCredits->Restore();
		ddsCreditsh->Restore();
		DDReLoadBitmap(ddsCredits,BMP_CREDITS);
		DDReLoadBitmap(ddsCreditsh,BMP_CREDITSH);
		Blit(Pass.DDFront, ddsCredits, 0, 0, 640, 480, 0, 0);
		Blit(Pass.DDBack, ddsCredits, 0, 0, 640, 480, 0, 0);
		break;
	case WM_KEYDOWN:
   	switch(Params.wParam){
      	case VK_ESCAPE:
         	CreditsInfo=CREDITS_EXIT;
         break;
      }
	break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
int ccbOKProc(PASSPROCVARS Params){
	CreditsInfo=CREDITS_EXIT;
	return 0;
}