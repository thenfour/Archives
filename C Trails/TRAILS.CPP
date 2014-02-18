#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <bios.h>





void MakeTables();
void SetMCGA();
void SetText();
void SetUpVirtual();
void ShutDown();
void Flip();
void Cls(unsigned char Col, unsigned char *Where);
void Putpixel (int x, int y, unsigned char Col, unsigned char *Where);
float Getpixel(unsigned int x,unsigned int y,unsigned char *Where);
void WaitRetrace();
void Pal    (unsigned char ColorNo,  unsigned char R,
				 unsigned char G,        unsigned char B);
float XRot(float x,float y,float OriginX,float OriginY,int a);
float YRot(float x,float y,float OriginX,float OriginY,int a);
void Rot(float x, float y, float OriginX, float OriginY,int a, float &XX, float &YY);
void IRot(float x, float y, float OriginX, float OriginY,int a, int &XX, int &YY);
unsigned char Menu();
void ShowMenu();
float NInput();
void PutBlock (int y1, int y2, unsigned char Col, unsigned char *Where);

int NOfTrails=360;//number of trails
//SunBurst effect: 10,0,1.01
//Normal effect:0,.4,1.01
int IVELOCITY =10;//initial velocity
float VFACT= 0;//frame-velocity OFFSET
float  VSPEED= 1.01;//frame-velocity DIVISOR
int NOFCOLORS= 64;//Number of colors for the gradient
float PSPEED =7; //speed of particle (10/PSPEED = # of pixels per frame)
int MAXX =320; // Screen bounaries
int MINX =0;
int MAXY= 150;
int MINY =50;
float PCR =0,PCR2=0;//RGB of trails (as percents) and second RGB (for gradient color change)
float PCG =1,PCG2=0;
float PCB =0,PCB2=1;
int WATERSPEED=20;//Speed of "water effect"
unsigned char WATER=1;//1 if water on, 0 if off.
unsigned char FIRE=0;
unsigned char DIFFCOLOR=1;
int GSPEED=200;
unsigned char GCOLOR=1;
unsigned char CYCLE=1;
unsigned char CLEAR=1;
unsigned char SKIP=1;
unsigned char RANDGRAD=1;
unsigned char FILLBLOCK=0;
float FFACT=.07;
unsigned int FPS;
unsigned long FrameCounter,LastSecond;

//Not implimented yet...
const int DROP=1;//Dropping of trails
float sine[360],cose[360];
unsigned char Toggles[2]={'N','Y'};
unsigned char *vaddr = NULL;
unsigned char *vga = (unsigned char *) MK_FP(0xA000, 0);

void main(int argc, char * argv[]){
//trails.exe NOfTrails IVELOCITY VFACT VSPEED PSPEED MAXX MINX MAXY MINY PCOLOR

randomize();
MakeTables();

//"Disposable" variables
SetUpVirtual();
SetMCGA();
unsigned int i,j;
unsigned char Frame,Key;
int GFrame=0;
float f=0,jz;
int dx,dy,xf,yf,cc;
float DR,DG,DB;
float GCincr,BCincr,RCincr;
float x,y,c; // current coords
unsigned int trailx[360],traily[360],
	 direction[360];   // x,y, direction(degrees), velocity of trails
float velocity[360]; //number of pixels to move it each frame.
float CFact=(float)NOFCOLORS/(float)NOfTrails; //for palette use
unsigned int ox=160; unsigned int oy=100;  //these are the center of the screen
unsigned int TrailCounter=0; //this is the position in the trail array
unsigned int Direc;  //Direction/Velocity factors for the point.
float Veloc;



MainLoop:
CFact=(float)NOFCOLORS/(float)NOfTrails;
Cls(0,vga);
Cls(0,vaddr);
Flip();
for(i=1;i<=NOFCOLORS;i++)
{
   if(float((float)NOFCOLORS/(float)63)==0){
   	Pal(i,63*PCR,63*PCG,63*PCB);
   }
   else{
      f=(float)i/float((float)NOFCOLORS/63);
		Pal(i,f*PCR,f*PCG,f*PCB);
   }

}

if(!DIFFCOLOR){
for(i=1;i<=NOFCOLORS;i++)
{
	Pal(i,PCR*64,PCG*64,PCB*64);
}
}

Pal(255,64-(PCR*64),64-(PCG*64),64-(PCB*64));
//clear all arrays for the trails before we begin
for(i=0;i<NOfTrails;i++){
	direction[i]=0;
	trailx[i]=0;
   traily[i]=0;
  velocity[i]=0;
}

RCincr=(float)(PCR2-PCR)*64/(float)GSPEED;
GCincr=(float)(PCG2-PCG)*64/(float)GSPEED;
BCincr=(float)(PCB2-PCB)*64/(float)GSPEED;
DR=PCR*64;
DG=PCG*64;
DB=PCB*64;
GFrame=0;
x=160;
y=100;
Direc=0;
FrameCounter=0;
LastSecond=biostime(0, 0L);
//This is the beginning of the loop: *******************************************
while(!kbhit()){
FrameCounter++;


if(CLEAR){
	if(!FIRE) Cls(0,vaddr);
}
if(FILLBLOCK) PutBlock(MINY,(MAXY-MINY),255,vaddr);
//MOVE THE POINT
//Direc is the angle at which we're moving, Veloc is the speed.
Veloc=(float)random(10)/PSPEED;
if(!WATER)
	Rot(x+Veloc,y,x,y,Direc,x,y);

if(GCOLOR){
   GFrame++;
   if(GFrame==GSPEED){
      GFrame=0;

		if(!RANDGRAD){
      	RCincr=-RCincr;
	   	GCincr=-GCincr;
	   	BCincr=-BCincr;
		}
      else{
	      PCR=PCR2;
	      PCG=PCG2;
	      PCB=PCB2;
	      PCR2=(float)random(100)/100;
	      PCG2=(float)random(100)/100;
	      PCB2=(float)random(100)/100;

	      RCincr=(float)(PCR2-PCR)*64/(float)GSPEED;
			GCincr=(float)(PCG2-PCG)*64/(float)GSPEED;
			BCincr=(float)(PCB2-PCB)*64/(float)GSPEED;
			DR=PCR*64;
			DG=PCG*64;
			DB=PCB*64;
      }
   }
	DR+=RCincr;
	DG+=GCincr;
	DB+=BCincr;
	for(i=1;i<=NOFCOLORS;i++)
	{
      if(DIFFCOLOR){
      jz=(float)i/(float)NOFCOLORS;
      }
      else jz=1;

	  	Pal(i,(int)(DR*jz),(int)(DG*jz),(int)(DB*jz));
	}
//   Pal(255,(255-DR)*FFACT,(255-DG)*FFACT,(255-DB)*FFACT);
   Pal(255,15,15,15);
   }

   //printf("%f, %f\n", GCincr,BCincr);

if(WATER){
	Frame++;
	Veloc=0;
	Frame=(Frame%WATERSPEED);
	if (Frame==0){
		x=int(random(MAXX+1))+MINX;
		y=int(random(MAXY+1))+MINY;
	}
}

if(x>MAXX){
   x=MAXX;
	Direc+=90;
}
if(x<MINX){
	x=MINX;
	Direc+=90;
}
if(y>MAXY){
   y=MAXY;
 	Direc+=90;
}
if(y<MINY){
	y=MINY;
	Direc+=90;
}
Direc=(Direc+random(9)-4)%360;

   for(j=TrailCounter;j<NOfTrails;j++)
   {
      if(CYCLE) IRot(trailx[j]+velocity[j]-IVELOCITY,  traily[j],
      	 trailx[j],                        traily[j],
          direction[j]+(j-TrailCounter),	 dx, dy);
      else IRot(trailx[j]+velocity[j]-IVELOCITY,  traily[j],
      	 trailx[j],                        traily[j],
          direction[j],	 dx, dy);
      if(!FIRE){cc=int(float(j-TrailCounter)*CFact);}
	   else{cc=NOFCOLORS;}

      dx=(int)(dx/SKIP)*SKIP;
      dy=(int)(dy/SKIP)*SKIP;

      Putpixel(dx,dy,cc,vaddr);
      velocity[j]/=VSPEED;
      velocity[j]-=VFACT;
   }
	for(j=0;j<TrailCounter;j++)
   {
      if(CYCLE) IRot(trailx[j]+velocity[j]-IVELOCITY,  traily[j],
      	  trailx[j],                        traily[j],
           direction[j]+(NOfTrails-(TrailCounter-j)),	 dx, dy);
		else IRot(trailx[j]+velocity[j]-IVELOCITY,  traily[j],
      	  trailx[j],                        traily[j],
           direction[j],	 dx, dy);
      if(!FIRE){cc= int(float(NOfTrails-(TrailCounter-j))*CFact) ;}
		else{cc=NOFCOLORS;}

      dx=(int)(dx/SKIP)*SKIP;
      dy=(int)(dy/SKIP)*SKIP;

      Putpixel(dx,dy,cc,vaddr);
      velocity[j]/=VSPEED;
      velocity[j]-=VFACT;
   }

   trailx[TrailCounter]=x;
   traily[TrailCounter]=y;
   velocity[TrailCounter]=IVELOCITY;
   direction[TrailCounter]=random(360);
   TrailCounter=(TrailCounter+1)%NOfTrails;

   //Fire effect
   if(FIRE){
   	for(yf=MINY;yf<MAXY;yf+=SKIP){
		 for(xf=MINX;xf<MAXX;xf+=SKIP){
	      c=Getpixel(xf,yf,vaddr);
	      c-=(.1*SKIP);
	      if(c<0) c=0;
	      //Putpixel(xf,yf-SKIP,c,vaddr);
	      Putpixel(xf,yf,c,vaddr);
	    }
		}
   }
   Flip();
}
   getch();
   Key=Menu();
	if(Key){
   	ShutDown();
		SetText();
   }
	else goto MainLoop;
}



void SetMCGA() {
  _AX = 0x0013;
  geninterrupt (0x10);
}
void SetText() {
  _AX = 0x0003;
  geninterrupt (0x10);
}
void Cls(unsigned char Col, unsigned char *Where) {
  _fmemset(Where, Col, 64000);
}
void WaitRetrace() {

  _DX = 0x03DA;

  l1: asm {
	in  al,dx;
	and al,0x08;
	jnz l1;
      }

  l2: asm {
	in  al,dx;
	and al,0x08;
	jz  l2;
      }
}
void SetUpVirtual() {

  vaddr = (unsigned char *) calloc(64000,1);
  if (vaddr == NULL) {
    SetText();
    printf("Not enough memory available, exiting program...");
    exit(1);
  }
}
void ShutDown() {
  free(vaddr);
}
void Putpixel (int x, int y, unsigned char Col, unsigned char *Where) {
char s,ss;
  if(x>MINX){
 if((x+SKIP)<MAXX){
  if(y>MINY){
  if((y+SKIP)<MAXY){

      //for(ss=1;ss<=SKIP;ss++){
	   	memset(Where+(x+((y+0)*320)),Col,SKIP);
//      }
  }}}}
}
void PutBlock (int y1, int y2, unsigned char Col, unsigned char *Where) {
	memset(Where+(y1*320),Col,y2*320);
}
float Getpixel(unsigned int x,unsigned int y,unsigned char *Where){
   unsigned int a;
	/*a= (float)(*((char far *) Where + x-SKIP+(y-SKIP)*320)+
   	*((char far *) Where + x+(y-SKIP)*320)+
	   *((char far *) Where + x+SKIP+(y+SKIP)*320))*.1/3;


	a+=(float)(*((char far *) Where + x-SKIP+y*320)+
   	*((char far *) Where + x+y*320)+
	   *((char far *) Where + x+SKIP+y*320))*.1/3;

   //*/
	a=(float)(*((char far *) Where + x-SKIP+(y+SKIP)*320)+
   	*((char far *) Where + x+(y+SKIP)*320)+
	   *((char far *) Where + x+SKIP+(y+SKIP)*320))*1/3;



//   return ((a)<NOFCOLORS)? a: 255;
   return a;
}
void Flip() {
  _fmemcpy(vga,vaddr,64000);
}
void Pal(unsigned char ColorNo, unsigned char R,
	 unsigned char G,       unsigned char B) {

  outp (0x03C8,ColorNo);
  outp (0x03C9,R);
  outp (0x03C9,G);
  outp (0x03C9,B);

}
void MakeTables(){
   float  r=(float)(3.141592654/180);
	for(int i=0;i<360;i++){
   	sine[i]=(float)(sin(i*r));
      cose[i]=(float)(cos(i*r));
      }
}

void Rot(float x, float y, float OriginX, float OriginY,int a, float &XX, float &YY){
a=a%360;
x-=OriginX;
y-=OriginY;
XX = (cose[a] * x) - (sine[a] * y)+OriginX;
YY = (sine[a] * x) + (cose[a] * y)+OriginY;
}
//This is the same thing, only integer returns
void IRot(float x, float y, float OriginX, float OriginY,int a, int &XX, int &YY){
a=a%360;
x-=OriginX;
y-=OriginY;
XX = (cose[a] * x) - (sine[a] * y)+OriginX;
YY = (sine[a] * x) + (cose[a] * y)+OriginY;
}

void ShowMenu(){
   FPS=FrameCounter/((biostime(0,0)-LastSecond)/_BIOS_CLOCKS_PER_SEC);

	SetText();
   clrscr();
   textbackground(BLACK);
   textcolor(LIGHTBLUE);
   cprintf("Frames per second:%d\n\r",FPS);
	textcolor(CYAN);
   cprintf("2- Number of trails: [%d]\n\r",NOfTrails);
	cprintf("3- Initial Velocity of trails: [%d]\n\r",IVELOCITY);
   cprintf("4- Frame-Velocity decreasing offset: [%f]\n\r",VFACT);
   cprintf("5- Frame-Velocity decreasing divisor: [%f]\n\r",VSPEED);
   cprintf("6- Origin Particle speed: [%f]\n\r",PSPEED);
   cprintf("X- Horiz Size: [%d]\n\r",MAXX-MINX);
   cprintf("Y- Vert Size : [%d]\n\r",MAXY-MINY);
   cprintf("r- Red 1  : [%3d] R- Red 2:   [%3d]\n\r",int(PCR*255), int(PCR2*255));
   cprintf("g- Green 1: [%3d] G- Green 2: [%3d]\n\r",int(PCG*255),int(PCG2*255));
   cprintf("b- Blue 1 : [%3d] B- Blue 2:  [%3d]\n\r",(int)(PCB*255),int(PCB2*255));
	cprintf("7- Water effect: [%c]\n\r",Toggles[WATER]);
   cprintf("8- Water relocation speed: [%d]\n\r",WATERSPEED);
   cprintf("9- Fire effect: [%c]\n\r",Toggles[FIRE]);
	cprintf("0- Multiple Colors: [%c]\r\n",Toggles[DIFFCOLOR]);
	cprintf("1- Gradient Color Change: [%c]\n\r",Toggles[GCOLOR]);
   cprintf("!- Gradient Color Change Speed: [%d]\n\r",GSPEED);
   cprintf("@- Rotating trails: [%c]\n\r",Toggles[CYCLE]);
   cprintf("#- Screen clearing per frame: [%c]\r\n",Toggles[CLEAR]);
	cprintf("$- Pixel skipping: [%d]\n\r",SKIP);
	cprintf("%%- Random gradient color picking: [%c]\n\r",Toggles[RANDGRAD]);
	cprintf("^- Fill domain with background color: [%c]\n\r",Toggles[FILLBLOCK]);
	textcolor(WHITE);
   cprintf("Q- Quit\n\r");
   }


unsigned char Menu(){
unsigned char Rv=0;//this sub returns 0 if we continue, a 1 if we end the program.
int itmp;
float ftmp;
ShowMenu();
unsigned char Key=getch();
textcolor(CYAN);
   switch(Key){
      case '2':
      	cprintf("Enter Number of Trails (1-360):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=360) NOfTrails=itmp;
      break;
      case '3'://ivelocity
			cprintf("Enter Initial Velocity (1-100):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=100) IVELOCITY=itmp;
      break;
      case '4'://vfact
			cprintf("Enter Frame-Velocity Offset (0-10):");
         textcolor(LIGHTBLUE);
         ftmp=NInput();
         if(ftmp>=0 && ftmp<=10) VFACT=ftmp;
      break;
      case '5'://vspeed
			cprintf("Enter Frame-Velocity Divisor (1-10):");
         textcolor(LIGHTBLUE);
         ftmp=NInput();
         if(ftmp>=1 && ftmp<=10) VSPEED=ftmp;
      break;
      case '6'://pspeed
			cprintf("Particle Speed (10/x pixels per frame):");
         textcolor(LIGHTBLUE);
         ftmp=NInput();
         if(ftmp>0 && ftmp<=10000) PSPEED=ftmp;
      break;
      case '7'://water
			cprintf("Water effect (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) WATER=itmp;
      break;
      case '8'://waterspeed
			cprintf("Enter Frames per Reposition (1-1000):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>=1 && itmp<1000) WATERSPEED=itmp;
      break;
      case '9'://fire
			cprintf("Fire effect? (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) FIRE=itmp;
      break;
      case '0'://diffcolor
			cprintf("Different Colors (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) DIFFCOLOR=itmp;
      break;
      case 'r':
      	cprintf("Red (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCR=(float)itmp/255;
      break;
      case 'R'://pcr
      	cprintf("Red (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCR2=(float)itmp/255;
      break;
      case 'g':
      	cprintf("Green (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCG=(float)itmp/255;
      break;
      case 'G'://pcg
      	cprintf("Green (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCG2=(float)itmp/255;
      break;
      case 'b':
      	cprintf("Blue (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCB=(float)itmp/255;
      break;
      case 'B'://pcb
      	cprintf("Blue (0-255):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>0 && itmp<=255) PCB2=(float)itmp/255;
      break;
      case 'x':
      case 'X':
      	cprintf("Horizontal Size of Window (10-320):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>=10 && itmp<=320){
         	MINX=160-(itmp/2);
            MAXX=160+(itmp/2);
         }
      break;
      case 'y':
      case 'Y':
      	cprintf("Vertical Size of Window (10-198):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>=10 && itmp<=198){
         	MINY=100-(itmp/2);
            MAXY=100+(itmp/2);
         }
      break;
      case '1'://GCOLOR 0 or 1
			cprintf("Gradient effect (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) GCOLOR=itmp;
      break;
		case '!':
      	cprintf("Gradient speed (Frames per cycle, 1-100000):");
			textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp>=0 && itmp<=100000) GSPEED=itmp;
      break;
      case '@'://water
			cprintf("Rotating Trails? (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) CYCLE=itmp;
      break;
      case '#'://water
			cprintf("Screen Clearing? (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) CLEAR=itmp;
      break;
		case '$':
      	cprintf("Pixel skip (for speed, 1-10):");
         textcolor(LIGHTBLUE);
         itmp=(int)(NInput());
         if(itmp>=1 && itmp<=10) SKIP=itmp;
      break;
		case '%':
      	cprintf("Choose gradient colors randomly? (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 || itmp==1) RANDGRAD=itmp;
      break;
      case '^':
      	cprintf("Fill the y-boundaries with a color? (0=No, 1=Yes):");
         textcolor(LIGHTBLUE);
         itmp=(int)NInput();
         if(itmp==0 ||itmp==1) FILLBLOCK=itmp;
      break;
      case 'q':
      case 'Q'://quit
      	Rv=1;
      break;
   }
	SetMCGA();
   return Rv;

   }
float NInput(){
char str[200];
float result;

gets(str);
if(strlen(str)){
if((str[0]>'0' && str[0]<='9') || (str[0]='.')){
	result=atof(str);
}
}
return result;
}

