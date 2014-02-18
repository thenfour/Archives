#include "commandbutton.h"
#include "dsoundlib.h"
extern PASSVARS Pass;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int COMMANDBUTTON::Initialize(LPDIRECTDRAWSURFACE * highlighted, LPDIRECTDRAWSURFACE * unhighlighted,
							  LPDIRECTDRAWSURFACE * ddSurface, 
							  RECT Rect, int (&Proc) (PASSPROCVARS Params)){
	Left=Rect.left;
	Right=Rect.right;
	Top=Rect.top;
	Bottom=Rect.bottom;

	IsHighlighted=false;

	CommandButtonProc=Proc;//
	Surface=ddSurface;
	Highlighted=highlighted;
	UnHighlighted=unhighlighted;
	this->Valid=true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COMMANDBUTTON::~COMMANDBUTTON()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COMMANDBUTTON::COMMANDBUTTON()
{
	this->Valid=false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int COMMANDBUTTON::ProcFunction(PASSPROCVARS Params){
	//this function returns 0 if nothing happened; 1 ifsomething was drawn to the screen.
	int Rvalue=0;
	if(this->Valid==true){
		//get mouse coords
		//then use them to figure out if it's inside the bounaries.
		//if we clicked it, then do it's action.
		LRESULT Result;
		POINT MouseCoords;

		switch(Params.uMsg){
			case WM_MOUSEMOVE:
   			//moved the mouse
				MouseCoords.x=LOWORD(Params.lParam);
				MouseCoords.y=HIWORD(Params.lParam);
				if(IsInside(GetRect(Left, Top, Right, Bottom), MouseCoords)){
					//we're inside.
					if(IsHighlighted==false) Play(&Pass.Sounds.MenuBuffer1);
      				IsHighlighted=true;
					Result=Blit(*Surface, *Highlighted, Left,Top,Right,Bottom,Left,Top);
					Rvalue=1;
   				}
				else{
      			//if we're not in the rectangle, and we just were, then draw the unhighlighted button;
				if(IsHighlighted){
						Result=Blit(*Surface, *UnHighlighted, Left,Top,Right,Bottom, Left,Top);
						Rvalue=1;
					IsHighlighted=false;
				}
			}
		break;
		case WM_LBUTTONDOWN:
   			//clicked
				MouseCoords.x=LOWORD(Params.lParam);
				MouseCoords.y=HIWORD(Params.lParam);
			if(IsInside(GetRect(Left, Top, Right, Bottom), MouseCoords)){
      			//we're golden, like the shower..so call the function and deal with it there.
				Play(&Pass.Sounds.MenuBuffer2);
				CommandButtonProc(Params);
			}
		break;
		}
	}
	return Rvalue;
}


