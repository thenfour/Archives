#include "texteditbox.h"
#define BMP_TEXTEDITBOX "gfx\\24textedit.bmp"
#define TEB_TEXT_LEFT		234
#define TEB_TEXT_TOP		248

extern PASSVARS Pass;
#define TEBI_NOTHING 0
#define TEBI_DONE    1
TEXTEDITBOX * teb;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextEntryBox(char String[100]){
	TEXTEDITBOX TextEditBox;
	teb=&TextEditBox;
	teb->GetInput(String);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TextEditProc(PASSPROCVARS Params){
	teb->ProcFunction(Params);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TEXTEDITBOX::ProcFunction(PASSPROCVARS Params){
	TCHAR Char;
	//
	switch(Params.uMsg){
	case WM_PAINT:
		RestoreFonts();
		Pass.DDFront->Restore();
		Pass.DDBack->Restore();
		this->EditBoxSurface->Restore();
		DDReLoadBitmap(this->EditBoxSurface, BMP_TEXTEDITBOX);
		this->StorageSurface->Restore();
		break;
	case WM_CHAR:
		switch(Params.wParam){
			case 0x08:  /* backspace */
				DeleteChar();
				break;
			case 0x0A:  /* linefeed */
			case 0x0D:  /* carriage return */
				Info=TEBI_DONE;
				break;
			case 0x1B:  /* escape */
			case 0x09:  /* tab */
				break;
			default:
				//normal character:
				Char=(TCHAR) Params.wParam;
				AddChar(Char);
			break;
		}
		break;
		case WM_KEYDOWN:
		///put it in the box.
		switch(Params.wParam){
			case VK_RETURN:
				break;
			case VK_ESCAPE:
				break;
			case VK_LEFT:
				break;
			case VK_RIGHT:
				break;
		}
		break;
	}

	UpdateBox();
	return 0;
}


//text entry box class will start by just loading in the surface and everything; not doing anything on the screen
//though.
//the program must tell it to get input


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEXTEDITBOX::TEXTEDITBOX(){
	//copy the desktop surface into a buffer (storage) surface.
	MakeOffScreenSurface(Pass.DDObject, StorageSurface, TEXTENTRYBOX_WIDTH, TEXTENTRYBOX_HEIGHT);
	Blit(StorageSurface, Pass.DDFront, TEXTENTRYBOX_LEFT, TEXTENTRYBOX_TOP,
								 TEXTENTRYBOX_BOTTOM, TEXTENTRYBOX_RIGHT, 0,0);
	//load the image of the text box
	EditBoxSurface=DDLoadBitmap(Pass.DDObject, BMP_TEXTEDITBOX, 0,0);

	//set up the data for the input string.
	ZeroMemory(String, sizeof(String));
	CursorPosition=0;
	StringLength=0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEXTEDITBOX::~TEXTEDITBOX(){
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::GetInput(char Stringx[100]){
	MSG Msg;
	//save the original programflow variable to change it back later.
	int ProgramFlowTemp=Pass.ProgramFlow;
	Pass.ProgramFlow=PF_TEXTEDITBOX;
	Info=TEBI_NOTHING;
	//display the text box on the screen.

	//message loop
	while(Info!=TEBI_DONE){
		UpdateBox();
		if(!GetMessage(&Msg, NULL, 0, 0)){
   			//if they're trying to quit...
			Pass.SettingsInfo=SETTINGS_EXIT;
			Pass.ProgramFlow=PF_EXIT;
			Info=TEBI_DONE;
		}
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}	
	if(Pass.ProgramFlow!=PF_EXIT) Pass.ProgramFlow=ProgramFlowTemp;
	//now put the screen back together again.
	Blit(Pass.DDFront, StorageSurface, 0, 0, TEXTENTRYBOX_WIDTH, TEXTENTRYBOX_HEIGHT,
						TEXTENTRYBOX_LEFT, TEXTENTRYBOX_TOP);
	Blit(Pass.DDBack, StorageSurface, 0, 0, TEXTENTRYBOX_WIDTH, TEXTENTRYBOX_HEIGHT,
                         TEXTENTRYBOX_LEFT, TEXTENTRYBOX_TOP);
	lstrcpy(Stringx, String);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::AddChar(char ch){
	//we have String[100]
	//and int StringLength
	if(StringLength<99){
		String[StringLength]=ch;
		StringLength++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::DeleteChar(){
	if(this->StringLength>0){
		this->StringLength--;
		this->String[StringLength]=0;
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::Enter(){

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::Done(){

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TEXTEDITBOX::UpdateBox(){
	//updates the display of the box.
	//first re-copy the box and then draw the letters again, then flip it.
	LRESULT Result;
	Result=Blit(Pass.DDBack, EditBoxSurface, 0, 0,TEXTENTRYBOX_WIDTH, TEXTENTRYBOX_HEIGHT,TEXTENTRYBOX_LEFT,TEXTENTRYBOX_TOP);
	char str[100];
	lstrcpy(str, this->String);
	//now figure out what we need to put on the screen
	if(this->StringLength >15){
		//it's too big to fit the whole thing, so truncate it
		lstrcpy(str, &(this->String[this->StringLength-15]));
	}
	DrawString(str, TEB_TEXT_LEFT, TEB_TEXT_TOP, 12);
   Flip(Pass.DDFront);
}
