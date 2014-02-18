//this is the library that handles displaying text on screen, etc...
#include "textlib.h"
#include "ddutil.h"

extern PASSVARS Pass;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawLetter(char Character, int X, int Y, int Size){
//Draws letter Character of size Size at (x,y)
	int SizeX=0;
	int SizeY=0;
if(Character!=0){
	LPDIRECTDRAWSURFACE* FontSurface=NULL;
	int FontX,FontY;//the upper-left coords of the DD font surface
	//get the pointer to which surface we'll use for the font.
	if(Size==8){
		FontSurface=&Pass.DDFont8;
		SizeX=8;
		SizeY=8;
	}
	if(Size==21){
		FontSurface=&Pass.DDFont22; Size=21;
		SizeX=21;
		SizeY=22;
	}
	if(Size==12){ 
		FontSurface=&Pass.DDFont12;
		SizeX=12;
		SizeY=12;
	}
	if(Size==13){ 
		FontSurface=&Pass.DDFont13; Size=12;
		SizeX=12;
		SizeY=12;
	}

	if(FontSurface==NULL) return;

	//find out from where we will blit on the font's surface
	//we have to set FontX/FontY to the coords on the surface
	if(Character>='A' && Character <='Z'){
		//ABCDEFGHIJKLMNOPQRTUVWXYZ
		FontX=(Character-'A')*SizeX;
		FontY=0;
	}
	else{
		if(Character>='0' && Character<='@'){
			//0123456789:;<=>?@
			FontY=SizeY;
			FontX=(Character-'0')*SizeX;
		}
		else{
			if(Character>='a' && Character<='z'){
				//abcdefghijklmnopqrstuvwxyz
				FontY=SizeY*2;
				FontX=(Character-'a')*SizeX;
			}
			else{
				if(Character>'Z' && Character <'a'){
					//[\]^_`
					FontY=SizeY*3;
					FontX=(Character-'[')*SizeX;
				}
				else{
					if(Character>'z' && Character <='~'){
						//{|}~
						FontY=SizeY*3;
						FontX=((Character-'{')+6)*SizeX;
					}
					else{
						if(Character>' ' && Character<'0'){
							//!"#$%&'()*+,-./
							FontY=SizeY*3;
							FontX=((Character-'!')+10)*SizeX;
						}
					}
				}
			}
		}
	}

	//Now blit it;
	CKBlit(Pass.DDBack, *FontSurface, FontX, FontY, FontX+SizeX, FontY+SizeY, X, Y);
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawString(LPCSTR String, int X, int Y, int Size){
//draws a string on the screen
for(int count=0;count<lstrlen(String);count++){
	DrawLetter(String[count],X+(Size*(count)), Y, Size);
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawNumber(long Num, int X, int Y, int Size){
//converts number to string and then draws it.  I love how straight-
//forward this library is.

//ALSO this funcion corrects X to align to the Right.
char Str[100];

int AdjX;

itoa(Num, Str, 10);
AdjX=X-(strlen(Str)*Size);
DrawString(Str,AdjX, Y, Size);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RestoreFonts(){
	Pass.DDFont12->Restore();
	Pass.DDFont13->Restore();
	Pass.DDFont22->Restore();
	Pass.DDFont8->Restore();
	DDReLoadBitmap(Pass.DDFont12, BMP_FONT12);
	DDReLoadBitmap(Pass.DDFont13, BMP_FONT13);
	DDReLoadBitmap(Pass.DDFont22, BMP_FONT22);
	DDReLoadBitmap(Pass.DDFont8, BMP_FONT8);
}