/*
  Funny Text displayer - (C)Oct. 6, 1998, Carl J Corcoran
  --------------------

*/
int TDither=0;
unsigned char KeyHit=0;
int Angle[640];//640 is the max number of dots that I allocate.  No particular reason.
int PEnd[640][2];//10 letters on screen at same time,
       					   	 //64 max dots for each letter,
                            //2 coords (x and y)
//PEnd is where the dots will end up
int NumberOfWords;
unsigned char Words[20][20];//a is the message we will use for the display
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include "datadefs.cpp"
#include "cjgfx.cpp"  //my own graphics implementations...
void GoGraphic();//sets VGA mode 13

void PutDot(int x, int y, int Intensity, unsigned char *Where);//draws an additive-blended dot on the screen
void Menu();//displays the menu and handles it
void Fade(char Factor, unsigned char *Where);//fades the screen at _Where _Factor amount. (darkens it)
int Spray(unsigned char *a);//This is the whole routine that gets the word on the screen.
int CalculateDots(unsigned char *s);//this figures out how many dots there are the for the word and where they are (interprets the letter data)

//*************Settings
int AngleToRotate=160;
int StartingDistance=150;
int Frames=100;
int SmallestRadius=3;
int LargestRadius=8;
int Intensity=30;
int FadeFrameSkip=1;//how many frames to skip each fading.
int FadeFactor=2;//how much to fade each time.
int randomvalues=0;
int Trails=1;
float DotRadius=5;
int DotHeight=7;
int DotWidth=7;
int CharacterWidth=56;
int FramesToDisplay=30;
int Dithering=1;
int DitherJustOnDisplay=1;       //
int PixelSkip=1;
//**********************


void main(){
randomize();
MakeTables();
SetUpVirtual();
SetUpVirtual2();
GoGraphic();
//unsigned char i,j,k;//various variables.
//int l=0; //how many dots we've drawn

LargestRadius-=SmallestRadius;

NumberOfWords=5;
strcpy((char *)Words[0],"what");
strcpy((char *)Words[1],"the");
strcpy((char *)Words[2],"fuck");
strcpy((char *)Words[3],"do you");
strcpy((char *)Words[4],"want");

int c=0;
for(;;){
	if(Spray(Words[c])==1){
   		Menu();

         c=-1;
   }
  	c=(c+1)%NumberOfWords;
}
ShutDown();
ShutDown2();

}

void PutDot(int x, int y, int Intensity, unsigned char *Where){
if(!kbhit()){
//Putpixel(x,y,Intensity,Where);

int tx=0,tc2=0;
float tc=0;
for(int sy=(-DotRadius);sy<=0;sy+=PixelSkip){
	tx=sqrt(abs((DotRadius*DotRadius)-(sy*sy)));
	//this is the loop
   for(int sx=(-tx);sx<=0;sx+=PixelSkip){

   	tc=sqrt(abs((sx*sx)+(sy*sy)));//the distance from the center
      if(sx==0 && sy==0){tc=1;}
      tc=Intensity-(((float)tc/(float)DotRadius)*Intensity);
      if(sx!=0){
      if(sy!=0){
		tc2=Getpixel(x+sx,y+sy,Where)+tc; if(tc2>63){tc2=63;}
   	if(TDither==1){DPutpixel(    x+sx,y+sy,tc2,Where);}
      else{Putpixel(    x+sx,y+sy,tc2,Where);}
		}
		tc2=Getpixel(x+sx,y-sy,Where)+tc; if(tc2>63){tc2=63;}
      if(TDither==1){DPutpixel(    x+sx,y-sy,tc2,Where);}
      else{Putpixel(    x+sx,y-sy,tc2,Where);}
      }
		tc2=Getpixel(x-sx,y-sy,Where)+tc; if(tc2>63){tc2=63;}
		if(TDither==1){DPutpixel(    x-sx,y-sy,tc2,Where);}
      else{Putpixel(    x-sx,y-sy,tc2,Where);}

      if(sy!=0){
		tc2=Getpixel(x-sx,y+sy,Where)+tc; if(tc2>63){tc2=63;}
   	if(TDither==1){DPutpixel(    x-sx,y+sy,tc2,Where);}
      else{Putpixel(    x-sx,y+sy,tc2,Where);}
      }
     }
	}

   }//kbhit
   else{
   	KeyHit=1;

   }

   }

int CalculateDots(unsigned char *s){
int j,k;
//this takes string s and calculates all the dots for it...s can be no longer
//than 8 characters (or they will simply be omitted).

//return value is the numvber of dots
int l=0;
for(int c=0;c<strlen(s);c++){
if(*(s+c)==32){//it's a space; there are no dots in a space
c++;
}


//this is if it's a letter
for(int x=0;x<8;x++){
	for(int y=0;y<8;y++){
   	//see if the letter has this bit set and then decide whether to draw or not.
      j=pow(2,x);
		k=Letters[(*(s+c))-'a'][y];
      if((j & k)==j){
			PEnd[l][0]=(8-x)*DotWidth+(CharacterWidth*c);
         PEnd[l][1]=y*DotHeight;
         l++;
      }
   }
}}
return l;
}

int Spray(unsigned char *a)
{
KeyHit=0;
CopyToV2();
int tpx, tpy, tpx2, tpy2, rad;
int s,ss;
float percent;
int l=CalculateDots(a);


//These are the variables that make this routine so flexible
if(randomvalues==1){//calculate random values if necessary
	AngleToRotate=rand()%720;
	StartingDistance=rand()%200;
   Intensity=(rand()%30)+3;
	SmallestRadius=(rand()%3)+2;
	LargestRadius=(rand()%13)+SmallestRadius;

	FadeFrameSkip=1;//how many frames to skip each fading.
	FadeFactor=4;//how many frames to skip each fading.
   LargestRadius-=SmallestRadius;
   }//if randomvalues

int cy=100-(4*DotHeight);           //these are the left/top most x,y coords of the final word
int cx=160-((float)((float)strlen(a)/2)*CharacterWidth);

//create random angles for all the points
for(int s=0;s<l;s++){
	Angle[s]=rand()%360;
}
TDither=0;
if(DitherJustOnDisplay==0 && Dithering==1){
	TDither=1;
}

//MAIN LOOP:

for(ss=Frames-1;ss>=(-FramesToDisplay);ss--){
	if(ss<0){//this is where we are just displaying the still image
   	if(DitherJustOnDisplay==1 && Dithering==1){
      	TDither=1;
      }
   	s=0;
   }
   else{
   	s=ss;
   }

	if(s%FadeFrameSkip==0){Fade(FadeFactor,v2);}//darkens v2
	percent=(float)s/(float)Frames;
	DotRadius=(percent*(float)LargestRadius)+SmallestRadius;
	CopyToVaddr();
	for(int x=0;x<l ;x++){
	   tpx=PEnd[x][0];
	   tpy=PEnd[x][1];
	   rad=1+(percent*StartingDistance);
	   tpx2=0;
	   tpy2=0;
	   IRot(tpx+rad, tpy, tpx, tpy,Angle[x]+(percent*AngleToRotate), tpx2, tpy2);
		PutDot(tpx2+cx,tpy2+cy,Intensity,vaddr);
	if(KeyHit==1){break;}
	}//x (number of dots in one frame)
Flip();
if(KeyHit==1){break;}
}//s (number of frames)
if(KeyHit==1){return 1;}
else{return 0;}
}

void Fade(char Factor, unsigned char *Where){
long cyy=100-(4*DotHeight)-DotRadius;
cyy*=320;
long cyy2=100+(4*DotHeight)+DotRadius;
cyy2*=320;
int y;
if(Trails==1){
	CopyToV2();
   cyy=0;
   cyy2=63999;
}
	for(long x=cyy;x<=cyy2;x++){
		y=*(((char far *)Where)+x)-Factor;
      if(y<0){y=0;}
      memset(Where+x,y,1);
   }
}

void Menu(){
unsigned char ms[200];
unsigned char k;
int tline=34;
int h;
	SetText();
   clrscr();
   gotoxy(1,1);
   printf("Funny text Displayer (c) 1998 Carl James Corcoran\n");
   printf("-------------------------------------------------\n");
   printf("1 - Change text\n");
	printf("2 - Display text\n\n");
   printf("3 - Total Rotation of Dots: %d\n",AngleToRotate);
   printf("4 - Starting Distance of Dots from Origin: %d\n",StartingDistance);
   printf("5 - Frames per word: %d\n",Frames);
   printf("6 - Ending Radius of Dots: %d\n",SmallestRadius);
   printf("7 - Starting Radius of Dots: %d\n",LargestRadius+SmallestRadius);
	printf("8 - Intensity: %d\n",Intensity);
	printf("9 - Character Width: %d\n",CharacterWidth);
	printf("( - Dot Space Height: %d\n",DotHeight);
	printf(") - Dot Space Width: %d\n",DotWidth);
	printf("! - Trail effect: %d\n",Trails);
	printf("@ - Frames To Display: %d\n",FramesToDisplay);
	printf("# - Dithering: %d\n",Dithering);
   printf("$ - Dither Only On Still Display: %d\n",DitherJustOnDisplay);
   printf("%% - Pseudo line skip: %d\n",PixelSkip);
   printf("^ - Fade Factor Per Frame: %d\n",FadeFactor);
   printf("& - Fade Frame Skip (frames per fade): %d\n",FadeFrameSkip);
   printf("* - Palette options");

	printf("r - Random settings: %d\n",randomvalues);
   printf("q - Quit\n");
   k=getch();
   k=getch();
	switch(k){
   	case '1':
      		clrscr();
            h=0;
				do{
            	printf("\nEnter Word #%d (type nothing to end): ",h+1);
               gets((char *)Words[h]);
               h++;
            }while(strlen(Words[h-1])!=0);
            NumberOfWords=(h-1);;

            break;
		case '2':
      		clrscr();
            for(h=0;h<NumberOfWords;h++){
            	printf("\n%s",Words[h]);
            }
            printf("\n\nPress Any Key To Continue Demo...");
            getch();
      		break;
		case '3':
      		gotoxy(1,tline);
            printf("Enter an angle to Rotate the dots: ");
            gets((char *)ms);
            AngleToRotate=(int)atof((char *)ms);
      		break;
		case '4':
      		gotoxy(1,tline);
            printf("Enter a starting distance for dots: ");
            gets((char *)ms);
            StartingDistance=(int)atof((char *)ms);
      		break;
		case '5':
      		gotoxy(1,tline);
            printf("Enter number of frames per word: ");
            gets((char *)ms);
            Frames=(int)atof((char *)ms);
      		break;
		case '6':
            LargestRadius+=SmallestRadius;
      		gotoxy(1,tline);
            printf("Enter smallest dot radius: ");
            gets((char *)ms);
            SmallestRadius=(int)atof((char *)ms);
            LargestRadius-=SmallestRadius;
      		break;
		case '7':
      		gotoxy(1,tline);
            printf("Enter largest dot radius: ");
            gets((char *)ms);
            LargestRadius=(int)atof((char *)ms);
            LargestRadius-=SmallestRadius;
      		break;
		case '8':
      		gotoxy(1,tline);
            printf("Enter Intensity (0-63): ");
            gets((char *)ms);
            Intensity=(int)atof((char *)ms);
      		break;
		case '9':
      		gotoxy(1,tline);
            printf("Character Width: ");
            gets((char *)ms);
            CharacterWidth=(int)atof((char *)ms);
      		break;
		case '(':
      		gotoxy(1,tline);
            printf("Enter Dot Space Height: ");
            gets((char *)ms);
            DotHeight=(int)atof((char *)ms);
      		break;
		case ')':
      		gotoxy(1,tline);
            printf("Enter Dot Space Width: ");
            gets((char *)ms);
            DotWidth=(int)atof((char *)ms);
      		break;
		case '!':
      		gotoxy(1,tline);
            printf("Trail Effect (0=No, 1=Yes): ");
            gets((char *)ms);
            Trails=(int)atof((char *)ms);
      		break;
		case '@':
      		gotoxy(1,tline);
            printf("Enter Frames to Display Still: ");
            gets((char *)ms);
            FramesToDisplay=(int)atof((char *)ms);
      		break;
		case '#':
      		gotoxy(1,tline);
            printf("Dithering? (0=No, 1=Yes): ");
            gets((char *)ms);
            Dithering=(int)atof((char *)ms);
      		break;
		case '$':
      		gotoxy(1,tline);
            printf("Dither only on Still Display? (0=No, 1=Yes): ");
            gets((char *)ms);
            DitherJustOnDisplay=(int)atof((char *)ms);
      		break;
		case '%':
      		gotoxy(1,tline);
            printf("Enter Pseudo Line Skip: ");
            gets((char *)ms);
            PixelSkip=(int)atof((char *)ms);
      		break;
		case '^':
      		gotoxy(1,tline);
            printf("Enter Fade Factor Per Frame: ");
            gets((char *)ms);
            FadeFactor=(int)atof((char *)ms);
      		break;
		case '&':
      		gotoxy(1,tline);
            printf("Enter Frames per Fade: ");
            gets((char *)ms);
            FadeFrameSkip=(int)atof((char *)ms);
      		break;
		case 'r':
      		gotoxy(1,tline);
            printf("Enter 0 for set values, 1 for random values: ");
            gets((char *)ms);
            randomvalues=(int)atof((char *)ms);
      		break;
      case 'q':
//				ms[]="sdf";
      		exit(0);
            break;
      default:
      		break;
   };
GoGraphic();
   }

void GoGraphic(){
SetMCGA();
   for(int i=0;i<=64;i++){
	Pal(i,i,0,0);
};
Cls(0,vga);
Cls(0,vaddr);
Cls(0,v2);
}

