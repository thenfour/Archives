#include <math.h>
#include <mem.h>
#include <dos.h>

struct Coords3d{
	int x[3];
   int y[3];
   int z[3];
};
struct Coords2d{
	int x[3];
   int y[3];
};
struct CPoint2d{
	int *x[3];
   int *y[3];
};
struct CPoint3d{
	int *x[3];
   int *y[3];
   int *z[3];
};
struct {
	int x;
   int y;
   int z;
} Angles;//Angle to rotate

void PutHLine (int x, int y, int length, unsigned char Col, unsigned char *Where);
void MakeTables();
void SetMCGA();
void SetText();
void SetUpVirtual();
void ShutDown();
void SetUpVirtual2();
void ShutDown2();
void Flip();
void Cls(unsigned char Col, unsigned char *Where);
void Putpixel (int x, int y, unsigned char Col, unsigned char *Where);
void DPutpixel (int x, int y, unsigned char Col, unsigned char *Where);
unsigned char Getpixel(unsigned int x,unsigned int y,unsigned char *Where);
unsigned char DGetpixel(unsigned int x,unsigned int y,unsigned char *Where);
void WaitRetrace();
void Pal    (unsigned char ColorNo,  unsigned char R,
				 unsigned char G,        unsigned char B);
void IRot(int x, int y, int OriginX, int OriginY,int a, int &XX, int &YY);
void DrawPoly(CPoint2d Coords,unsigned char polycol,unsigned char *where);

float sine[360],cose[360];
unsigned char *vga = (unsigned char *) MK_FP(0xA000, 0);
unsigned char *vaddr = NULL;
unsigned char *v2 = NULL;

void CopyToVaddr();
void CopyToV2();



void SetMCGA() {
  _AX = 0x0013;
  geninterrupt (0x10);
}
void SetText() {
  _AX = 0x0003;
  geninterrupt (0x10);
}
void Cls(unsigned char Col, unsigned char *Where) {
  _fmemset(Where, Col,(long)64000);
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
  vaddr = (unsigned char *) calloc((long)64000,1);
  if (vaddr == NULL) {
    SetText();
    printf("Not enough memory available, exiting program...");
    exit(1);
  }
}
void SetUpVirtual2() {
  v2 = (unsigned char *) calloc((long)64000,1);
  if (v2 == NULL) {
    SetText();
    printf("Not enough memory available, exiting program...");
    exit(1);
  }
}

void ShutDown() {
  free(vaddr);
}
void ShutDown2() {
  free(v2);
}

void Putpixel (int x, int y, unsigned char Col, unsigned char *Where) {
if(x<=319){
if(x>=0){
if(y<=199){
if(y>=0){
 memset(Where+(x+(y*320)),Col,1);
}}}}}

void DPutpixel (int x, int y, unsigned char Col, unsigned char *Where) {
float s;
if(x<=317){
if(x>=0){
if(y<=197){
if(y>=0){
	s=DGetpixel(x,y,Where);
	s+=Col;
   s/=2;
 memset(Where+(x+(y*320)),s,1);
}}}}}




unsigned char DGetpixel(unsigned int x,unsigned int y,unsigned char *Where){
	int v=*((char far *) Where + (x+(y*320)));
	    v +=*((char far *) Where + (x+1+(y*320)));
       v+=*((char far *) Where + (x+1+((y-1)*320)));
       v+=*((char far *) Where + (x+1+((y+1)*320)));
       v+=*((char far *) Where + (x-1+(y*320)));
       v+=*((char far *) Where + (x-1+((y-1)*320)));
       v+=*((char far *) Where + (x-1+((y+1)*320)));
       v+=*((char far *) Where + (x+((y-1)*320)));
       v+=*((char far *) Where + (x+((y+1)*320)));
       v/=9;

   return v;



}
unsigned char Getpixel(unsigned int x,unsigned int y,unsigned char *Where){
	int v=*((char far *) Where + (x+(y*320)));
   return v;
}
void Flip() {
  _fmemcpy(vga,vaddr,(long)64000);
}
void CopyToV2(){
	_fmemcpy(v2,vga,(long)64000);
}
void CopyToVaddr(){
	_fmemcpy(vaddr,v2,(long)64000);
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

void IRot(int x, int y, int OriginX, int OriginY,int a, int &XX, int &YY){
//a=angle
//x, y are coords to rotate
//OriginX, OriginY are coords of the origin around which to rotate
a%=360;
x-=OriginX;
y-=OriginY;
XX = (int)(cose[a] * x) - (int)(sine[a] * y)+OriginX;
YY = (int)(sine[a] * x) + (int)(cose[a] * y)+OriginY;
}

void DrawPoly(CPoint2d Coords,unsigned char polycol,unsigned char *where){
//sort by y from lowest to highest...
int i,CurrentMin=0,CurrentMax=0,CurrentMid=0;
int x,y;
int x2,y2;
float Slope1=0, Slope2=0,vy=0;
Coords2d Coords2;
for(i=0;i<=2;i++){
	if(*Coords.y[i]<*Coords.y[CurrentMin]) CurrentMin=i;
	if(*Coords.y[i]>*Coords.y[CurrentMax]) CurrentMax=i;
}
for(i=0;i<=2;i++)	if(i!=CurrentMin && i!=CurrentMax) CurrentMid=i;
Coords2.x[0]=*Coords.x[CurrentMin];
Coords2.y[0]=*Coords.y[CurrentMin];
Coords2.x[2]=*Coords.x[CurrentMax];
Coords2.y[2]=*Coords.y[CurrentMax];
Coords2.x[1]=*Coords.x[CurrentMid];
Coords2.y[1]=*Coords.y[CurrentMid];
//Now draw the top portion (from coords2.y[0] to coords2.y[1])
//calculate the slope
if(Coords2.y[0]!=Coords2.y[2]){
y2=(Coords2.y[1]-Coords2.y[0]);
Slope2=(float)(Coords2.x[2]-Coords2.x[0])/(float)(Coords2.y[2]-Coords2.y[0]);
if(y2){
Slope1=(float)(Coords2.x[1]-Coords2.x[0])/(float)(Coords2.y[1]-Coords2.y[0]);
for(y=0;y<y2;y++){
	if(Coords2.x[1]<Coords2.x[2]){//is the second point from teh top left of the last?
   	x=(int)(y*Slope1);//if it is, use THAT slope to start the line.
   	x2=(int)(y*Slope2);
   }else{
   	x=(int)(y*Slope2);//If it's not, use the other slope for the start.
   	x2=(int)(y*Slope1);
   }
   x+=Coords2.x[0];
   x2+=Coords2.x[0];
   PutHLine(x,y+Coords2.y[0],x2,polycol,where);
}
}
//Now draw the bottom of the triangle...
if(Coords2.y[2]!=Coords2.y[1]){
Slope1=(float)(Coords2.x[2]-Coords2.x[1])/(float)(Coords2.y[2]-Coords2.y[1]);
for(y=0;y<(Coords2.y[2]-Coords2.y[1]);y++){
	if(Coords2.x[1]<Coords2.x[2]){
   	x=(int)(y*Slope1);
   	x2=(int)((y2+y)*Slope2);
   	x+=Coords2.x[1];
      x2+=Coords2.x[0];
   }else{
   	x=(int)((y2+y)*Slope2);
   	x2=(int)(y*Slope1);
   	x+=Coords2.x[0];
      x2+=Coords2.x[1];

   }
   PutHLine(x,y+Coords2.y[1],x2,polycol,where);
   }
}
}
}
void PutHLine (int x, int y, int length, unsigned char Col, unsigned char *Where) {
//if(y<320){
	if(length<x) memset(Where+(length+(y*320)),Col,x-length);
	else memset(Where+(x+(y*320)),Col,length-x);
//}
}
