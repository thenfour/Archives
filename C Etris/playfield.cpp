#define WAV_FIRE "sfx\\fire.wav"
#define WAV_EARTH "sfx\\earth.wav"
#define WAV_WATER "sfx\\water.wav"
#define WAV_ICE "sfx\\ice.wav"
#define WAV_DESTRUCT "sfx\\destruct.wav"
#define WAV_GRAVITY "sfx\\gravity.wav"
#define WAV_FLAMMABLE "sfx\\flammable.wav"
#define WAV_CHEAT "sfx\\cheat.wav"
#include "playfield.h"

extern PASSVARS Pass;

//Make the animations global
extern PASSANIMS Anims;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*This procedure returns a pointer to the correct animation, given the parameter Blocktype.
Blocktype is anywhere from 1 to TET_BLOCK_MAX-1.
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ANIMATION* GetAnimation(int Blocktype){
switch(Blocktype){
	case TET_BLOCK_FIRE:
   	return Anims.aFire;
	case TET_BLOCK_EARTH:
   	return Anims.aEarth;
	case TET_BLOCK_WATER:
   	return Anims.aWater;
	case TET_BLOCK_ICE:
   	return Anims.aIce;
	case TET_BLOCK_GRAVITY:
   	return Anims.aGravity;
	case TET_BLOCK_DESTRUCT:
   	return Anims.aDestruct;
	case TET_BLOCK_FLAMMABLE:
   	return Anims.aFlammable;
}
//this is the only one left.
return Anims.aWater;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DrawShapeAnywhere(int Xcoord, int Ycoord, SHAPE Shape){
int x,y;
for(int i=0;i<4;i++){
	x=Xcoord;
   y=Ycoord;
   x+=(Shapes[Shape.Type][Shape.Rotation][i][0])*GRIDSIZE;
   y+=(Shapes[Shape.Type][Shape.Rotation][i][1])*GRIDSIZE;
	DrawBlock(x,y,Shape.BlockType.Type);
}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DrawShape(int GridX, int GridY, SHAPE Shape){
DrawShapeAnywhere((GridX * GRIDSIZE)+PLAYFIELD_MAIN_LEFT, (GridY * GRIDSIZE)+PLAYFIELD_MAIN_TOP, Shape);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DrawNext(){
DrawShapeAnywhere(PLAYFIELD_NEXT_LEFT,PLAYFIELD_NEXT_TOP, Next);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLAYFIELD::~PLAYFIELD(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLAYFIELD::PLAYFIELD(LPDIRECTDRAWSURFACE & FieldSurface, LPDIRECTDRAWSURFACE & PauseSurface){
srand(time(NULL));

DDField=&FieldSurface;
DDPause=&PauseSurface;

//This is the data for the shapes.
//Shapes[Shape#][Rotation][Block#][x/y coord].
//It's just in X/Y coordinate format.
char ShapesTemp[7][4][4][2]={
   {{{ 0, 0},{ 0, 1},{ 1, 0},{ 1, 1}}, //      **
    {{ 0, 0},{ 0, 1},{ 1, 0},{ 1, 1}}, //      **
    {{ 0, 0},{ 0, 1},{ 1, 0},{ 1, 1}}, //
    {{ 0, 0},{ 0, 1},{ 1, 0},{ 1, 1}}},//

   {{{ 1, 0},{ 1, 1},{ 1, 2},{ 0, 2}}, //       *
    {{ 0, 0},{ 0, 1},{ 1, 1},{ 2, 1}}, //       *
    {{ 0, 0},{ 0, 1},{ 0, 2},{ 1, 0}}, //      **
    {{-1, 1},{ 0, 1},{ 1, 1},{ 1, 2}}},//

   {{{ 0, 0},{ 0, 1},{ 0, 2},{ 1, 2}}, //       *
    {{-1, 1},{ 0, 1},{ 1, 1},{-1, 2}}, //       *
    {{-1, 0},{ 0, 0},{ 0, 1},{ 0, 2}}, //       **
    {{-1, 1},{ 0, 1},{ 1, 1},{ 1, 0}}},//

   {{{ 0, 0},{ 1, 1},{ 0, 1},{-1, 1}}, //       *
    {{ 0, 0},{ 1, 1},{ 0, 1},{ 0, 2}}, //      ***
    {{-1, 1},{ 1, 1},{ 0, 1},{ 0, 2}}, //
    {{-1, 1},{ 0, 0},{ 0, 1},{ 0, 2}}},//

   {{{ 0, 0},{ 0, 1},{ 0, 2},{ 0, 3}}, //     *
    {{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1}}, //     *
    {{ 0, 0},{ 0, 1},{ 0, 2},{ 0, 3}}, //     *
    {{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1}}},//     *

   {{{ 0, 0},{ 0, 1},{ 1, 1},{ 1, 2}}, //
    {{ 0, 0},{-1, 1},{ 0, 1},{ 1, 0}}, //     *
    {{ 0, 0},{ 0, 1},{ 1, 1},{ 1, 2}}, //     **
    {{ 0, 0},{-1, 1},{ 0, 1},{ 1, 0}}},//      *

   {{{ 0, 0},{ 0, 1},{-1, 1},{-1, 2}}, //       *
    {{ 0, 0},{ 0, 1},{-1, 0},{ 1, 1}}, //      **
    {{ 0, 0},{ 0, 1},{-1, 1},{-1, 2}}, //      *
    {{ 0, 0},{ 0, 1},{-1, 0},{ 1, 1}}},//
    };

//copy information from ShapesTemp to Shapes (I wanted to do the definitions
//of all the data in the constructor, and not in the declaration of the class
//memeber.)
CopyMemory (Shapes,ShapesTemp,sizeof(ShapesTemp));

Reset();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//990124 Pause:
void PLAYFIELD::Pause(){
LRESULT Result;

if(PauseFrameCount==0){
	PauseFrameCount=40;
	//Toggles the paused state
	if(GamePaused==false){
		while(ShowCursor(TRUE) <0){}
		Stats.Paused++;
	   GamePaused=true;
      Pass.ProgramFlow=PF_PAUSE;
		Result=Blit(Pass.DDBack, *DDPause, 0,0,640,480,0,0);
		Result=Blit(Pass.DDFront, *DDPause, 0,0,640,480,0,0);
      Flip(Pass.DDFront);
   }
	else{
		while(ShowCursor(FALSE) >=0){}

	   GamePaused=false;
      Pass.ProgramFlow=PF_GAME;
   }

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::MoveRight()
{
	//990121
	//moves the Current Block to the right, provided it won't run into the walls or other
   //blocks.
POINT TempPoint;

TempPoint.x=Pos.x+1;
TempPoint.y=Pos.y;

if(!CheckHit(Current, TempPoint)){
   Pos.x+=1;
}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::MoveLeft()
{
	//990121
	//moves the Current Block to the left, provided it won't run into the walls or other
   //blocks.
POINT TempPoint;
TempPoint.x=Pos.x-1;
TempPoint.y=Pos.y;
if(!CheckHit(Current,TempPoint)){
   Pos.x-=1;
}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::AdvanceFrame()
{
//this is called every frame.

//first, if we were paused and they hit the "continue" button, then
//the pause screen will change Pass.ProgramFlow, but Paused=true;
//so we have to handle that change
if((Pass.ProgramFlow==PF_GAME) && (GamePaused==true)){
	Pause();
}

//and another thing...if we were paused and they hit the "Quit game"
//button, then programflow will ==PF_GAMEEXIING;  to tell this thing
//to quit the game and return.
if(Pass.ProgramFlow==PF_GAMEEXITING) return 1;

int Rvalue;

PauseFrameCount--;
if(PauseFrameCount<0) PauseFrameCount=0;

//THIS IS NOW OBSOLETE; IT'S A ROUTINE THAT FIGURED OUT HOW MUCH TO WAIT TO SLOW DOWN
//YOUR COMPUTER...NOW I USE A GLOBAL VARIABLE TO STAY CONSISTANT.
//ok, no it's not; I just deleted it and replaced it with this following routine...
//was that confusing???  hehe
////////////////////////////////////////////
static time_t Seconds=0;//msvc only
static time_t OldSeconds=0;
static int OldFrame=0;
static int FrameRate=0;
time(&Seconds);
if(Seconds != OldSeconds){
	//a second has elapsed.
	FrameRate=(this->Frame)-OldFrame;
	time(&OldSeconds);
	OldFrame=this->Frame;
}

Sleep(Pass.FrameWait);
////////////////////////////////////////////
if(GamePaused==false) Rvalue=UnpausedFrame();
if(GamePaused==true) Rvalue=PauseFrame(this);

//draw the frame rate in the corner
if(this->Cheat_FrameRate){
	DrawNumber(FrameRate,50,0,21);
}
CalculateScore();

return Rvalue;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::PausedFrame()
{
//obsolete; not used
return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::UnpausedFrame()
{
//990121
//here we need to draw the screen, draw the shapes, and the NEXT shape.
//this used to be AdvanceFrame() but now it's UnpausedFrame;
POINT TempPoint;
LRESULT Result;
Frame++;
FramesLeft--;
//now if we're dropping, make sure that we don't hit...
if(FramesLeft<0){
	//reset FramesLeft first
	FramesLeft=20-(Stats.Lines/2);
	TempPoint.x=Pos.x;
	TempPoint.y=Pos.y+1;
	if(!CheckHit(Current, TempPoint)){
		Pos.y+=1;
	}
else{
	//we hit...
   //first if we hit without moving anywhere, then game over man
   if(Pos.x==5 && Pos.y==0) return 1;

	//now we have to glue the piece to the board, do effects,
   //check for lines, update statistics, and reset the pieces and position.

	int x,y;
	int i;
	//glue the shape on to the GRID[][];
   //But only if it's not a DESTRUCT or GRAVITY block; they don't stay and
   //their effects are handled separately.
   if(Current.BlockType.Type!=TET_BLOCK_DESTRUCT){
   	if(Current.BlockType.Type!=TET_BLOCK_GRAVITY){
			for(i=0;i<4;i++){
				x=Pos.x+Shapes[Current.Type][Current.Rotation][i][0];
			   y=Pos.y+Shapes[Current.Type][Current.Rotation][i][1];
			   Grid[x][y].Type=Current.BlockType.Type;
			}
      }
   }

   //Do the Effects;
	DoEffects();

	//Reset the pieces.
	CopyMemory(&Current, &Next, sizeof(Current));
	Next.BlockType.Type=(rand()%TET_BLOCK_MAX)+1;
	Next.Type=rand()%7;
	Next.Rotation=0;
	if(Cheat_GiveAllLines) Next.Type=4;
	if(Cheat_GiveAllEarth) Next.BlockType.Type=TET_BLOCK_EARTH;

   //update statistics for the shape
	CurrentStatUpdate();
	Pos.x=5;
	Pos.y=0;
}

}

//copy the blank playfield to the back surface
Result=Blit(Pass.DDBack, *DDField, 0,0,640,480,0,0);
//Draw the next shape

DrawNext();
UpdateField();

//Draw all the other stuff like lines, etc...
DrawNumber(Stats.Lines,623,100,21);
DrawNumber(Stats.Score,623,200,21);

DrawShape(Pos.x,Pos.y, Current);
return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PLAYFIELD::CheckHit(SHAPE HitShape, POINT Coord){
int x;
int y;
bool Hit=false;

for(int i=0;i<4;i++){
	//figure adjusted x and y
   x=(Coord.x+Shapes[HitShape.Type][HitShape.Rotation][i][0]);
   y=Coord.y+Shapes[HitShape.Type][HitShape.Rotation][i][1];
	if(Grid[x][y].Type!=TET_BLOCK_NOTHING) Hit=true;
   if(x>9 || x<0) Hit=true;
   if(y>19) Hit=true;
}

return Hit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::UpdateField()
{
	//this routine just draws the field with information in Grid[][]
   int x,y;

for(x=0;x<10;x++){
	for(y=0;y<20;y++){
   	if(Grid[x][y].Type!=TET_BLOCK_NOTHING){
      	//ther'es something to draw.
			DrawBlock(PLAYFIELD_MAIN_LEFT+(x*GRIDSIZE),PLAYFIELD_MAIN_TOP+(y*GRIDSIZE),Grid[x][y].Type);
      }
   }
}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Clockwise(){
//rotates the Current piece if it can handle it.
SHAPE TempShape;
TempShape.Type=Current.Type;
TempShape.Rotation=(Current.Rotation+1)%4;
if(!CheckHit(TempShape, Pos)){
	//Rotate it
   Current.Rotation=TempShape.Rotation;
}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::CounterClockwise(){
	//rotates the Current piece if it can handle it.
	SHAPE TempShape;
	TempShape.Type=Current.Type;
	TempShape.Rotation=(Current.Rotation-1);
	if(TempShape.Rotation==255) TempShape.Rotation=3;

	if(!CheckHit(TempShape, Pos)){
		//Rotate it
		Current.Rotation=TempShape.Rotation;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DrawBlock(int Xcoord, int Ycoord, char BlockType){
//draws a block on the screen
int AnimFrame;
ANIMATION* Animation;//this will point to teh correct animation

Animation=GetAnimation(BlockType);
      //Determine the frame of the animation to use.
      AnimFrame=(Frame / Animation->Speed)%Animation->FrameCount;

      AnimFrame*=GRIDSIZE;//To convert frames to pixels X.
      CKBlit(Pass.DDBack, *Animation->DDSurface, AnimFrame, 0, AnimFrame+GRIDSIZE, GRIDSIZE, Xcoord, Ycoord);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DoEffects()
{
static int LastNumberOfLines=0;
static int HowManyInARow=1;
//play a sound
switch(Current.BlockType.Type){
case TET_BLOCK_FIRE:
	PlayWaveFile(WAV_FIRE);
	break;
case TET_BLOCK_FLAMMABLE:
	PlayWaveFile(WAV_FLAMMABLE);
	break;
case TET_BLOCK_EARTH:
	PlayWaveFile(WAV_EARTH);
	break;
case TET_BLOCK_WATER:
	PlayWaveFile(WAV_WATER);
	break;
case TET_BLOCK_ICE:
	PlayWaveFile(WAV_ICE);
	break;
case TET_BLOCK_GRAVITY:
	PlayWaveFile(WAV_GRAVITY);
	break;
default:
case TET_BLOCK_DESTRUCT:
	PlayWaveFile(WAV_DESTRUCT);
	break;
}


//this routine scans through all the Grid[][] and for each block, do its effects.
int x,y;
int EffectCount;
int TLineCount;//used to count how many lines we get in this frame (for statistics)

//First, handle the destruct and gravity (of course, only if the current piece was
//one of those types.
if(Current.BlockType.Type==TET_BLOCK_DESTRUCT){
	DoDestruct();
}
else{
	if(Current.BlockType.Type==TET_BLOCK_GRAVITY){
   	DoGravity();
   }
}
TLineCount=Stats.Lines;
do{//every time there is a line, we have to redo the effects
	do{
		EffectCount=0;
		for(x=0;x<10;x++){
			for(y=19;y>=0;y--){
		   	//we have x and y, now figure out where to send it.
				if(Grid[x][y].Type != TET_BLOCK_NOTHING){
			      switch(Grid[x][y].Type){
						case TET_BLOCK_FIRE:
		            	EffectCount+=DoFire(x,y);
						break;
			         case TET_BLOCK_EARTH:
		            	EffectCount+=DoEarth(x,y);
						break;
			         case TET_BLOCK_FLAMMABLE:
		            	EffectCount+=DoFlammable(x,y);
						break;
			         case TET_BLOCK_WATER:
		            	EffectCount+=DoWater(x,y);
						break;
			         case TET_BLOCK_ICE:
		            	EffectCount+=DoIce(x,y);
						break;
			      };
		      }
		   }
		}
	Stats.Effects+=EffectCount;
	Stats.Score+=EffectCount*STATS_EFFECTS*Stats.Level;
	}while(EffectCount>0);
}while(CheckForLines());
Stats.Level=(Stats.Lines/10)+1;
//Now update statistics.
switch(Stats.Lines-TLineCount){
	//update the statistics
   case 0:
   break;
   case 1:
   	Stats.Singles++;
		Stats.Score+=STATS_SINGLES*Stats.Level;

		//if we had a single last time, let's count it as another in a row.
		if(LastNumberOfLines==1){
      	HowManyInARow++;
  			UpdateIARs(LastNumberOfLines, HowManyInARow);
      }
      else{
			//if the last WASN'T a single, then we have to
         //reset everything to show that we're doing singles now.
         LastNumberOfLines=1;
         HowManyInARow=1;
      }
   break;
   case 2:
   	Stats.Doubles++;
		Stats.Score+=STATS_DOUBLES*Stats.Level;
		if(LastNumberOfLines==2) HowManyInARow++;
      else{
         LastNumberOfLines=2;
         HowManyInARow=1;
      }
   break;
   case 3:
   	Stats.Triples++;
		Stats.Score+=STATS_TRIPLES*Stats.Level;
		if(LastNumberOfLines==3) HowManyInARow++;
      else{
         LastNumberOfLines=3;
         HowManyInARow=1;
      }
   break;
   case 4:
   	Stats.Quads++;
		Stats.Score+=STATS_QUADS*Stats.Level;
		if(LastNumberOfLines==4) HowManyInARow++;
      else{
         LastNumberOfLines=4;
         HowManyInARow=1;
      }
   break;
	default://more than 4 comes here
   	Stats.MoreThanQuads++;
      Stats.Score+=STATS_MORETHANQUADS*Stats.Level;
		if(LastNumberOfLines==5) HowManyInARow++;
      else{
         LastNumberOfLines=5;
         HowManyInARow=1;
      }
   break;
}


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//These are the members of PLAYFIELD that actually alter the Grid[][] based on
//The effects.  These functions are passed X and Y of the current block...the affected
//block will be the current block only.
//
//despite order in which we scan the blocks, (down, left to up, right, moving first along X)
//we will still check all those blocks surrounding the current one.
//
//
//ALSONOTE:
//Any effect only needs to be handled once.  In other words, turning Flammable to Fire may be handled
//in DoFire() OR DoFlammable() but not necessary in BOTH.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PLAYFIELD::DoEarth(int x, int y){
return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoIce(int x, int y){
int x2, y2;
for(x2=x-1;x2<=x+1;x2++){
	for(y2=y-1;y2<=y+1;y2++){
   	if((x2>=0 && x2<10) && (y2>=0 && y2<20)){
      	//we can go.
         switch(Grid[x2][y2].Type){
         	case TET_BLOCK_FIRE:
            	Grid[x][y].Type=TET_BLOCK_WATER;
		         EffectFrame();
            return 1;
         }
      }
   }
}

return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoFire(int x, int y){
int x2, y2;
for(x2=x-1;x2<=x+1;x2++){
	for(y2=y-1;y2<=y+1;y2++){
   	if((x2>=0 && x2<10) && (y2>=0 && y2<20)){
      	//we can go.
         switch(Grid[x2][y2].Type){
		 case TET_BLOCK_ICE:
			 Grid[x][y].Type=TET_BLOCK_WATER;
		         EffectFrame();
			 return 1;

         case TET_BLOCK_WATER:
            	Grid[x][y].Type=TET_BLOCK_NOTHING;
		         EffectFrame();
            return 1;
         }
      }
   }
}

return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoFlammable(int x, int y){
int x2, y2;//x and y is the current block, x2 and y2 are the compare blocks.
//scan all 9 blocks, and make sure we don't change anything if it's
//the current block, or out of the playfield range.
for(x2=x-1;x2<=x+1;x2++){
	for(y2=y-1;y2<=y+1;y2++){
   	if((x2>=0 && x2<10) && (y2>=0 && y2<20)){
         //we can go.
      	switch(Grid[x2][y2].Type){
          	case TET_BLOCK_FIRE:
         	   Grid[x][y].Type=TET_BLOCK_FIRE;
		         EffectFrame();
            return 1;
         }
      }
   }
}
return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoWater(int x, int y){
int x2, y2;
for(x2=x-1;x2<=x+1;x2++){
	for(y2=y-1;y2<=y+1;y2++){
   	if((x2>=0 && x2<10) && (y2>=0 && y2<20)){
      	//we can go.
         switch(Grid[x2][y2].Type){
         	case TET_BLOCK_ICE:
            	Grid[x][y].Type=TET_BLOCK_ICE;
		         EffectFrame();
            return 1;
         }
      }
   }
}

return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//these two are completely different; they make thir own pass of Grid[][] and they handle
//things themselves, first.
//
//remember that the only way these are called is if the current piece is Gravity or Destruct,
//because that is the ONLY way those are on the field - and then they are immediately removed
//after these routines take care of it.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoGravity(){
//990122
//we know where the damn gravity blocks are; just use them.
//they aren't even a part of the Grid[][], we know where they are because
//it's the current piece.
//now here's the deal:  only blocks UNDER where you set it will fall...
//so we have to scan through all 4 blocks of the shape and then drop all the
//blocks under it.  This might get tricky.
int x,y;

for(int i=0;i<4;i++){
	x=Pos.x+Shapes[Current.Type][Current.Rotation][i][0];
	y=Pos.y+Shapes[Current.Type][Current.Rotation][i][1];
   //we have x and y; now just scan from Y to 19, and drop all the blocks
	if(y<18){
	   for(int scan=y+1;scan<20;scan++){
	   	if(Grid[x][scan].Type==TET_BLOCK_NOTHING && Grid[x][scan-1].Type!=TET_BLOCK_NOTHING){
	      	//if there's a hole, then here we drop all the blocks from y to scan
	         for(int scan2=scan;scan2>y;scan2--){
	         	Grid[x][scan2].Type=Grid[x][scan2-1].Type;
               Stats.Effects++;
               Stats.Score+=STATS_EFFECTS*Stats.Level;
	         }
		     EffectFrame();
	      }
	   }
   }
}

return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PLAYFIELD::DoDestruct(){
//990122
//same thing here, but less complicated.
int x,y;
for(int i=0;i<4;i++){
	x=Pos.x+Shapes[Current.Type][Current.Rotation][i][0];
	y=Pos.y+Shapes[Current.Type][Current.Rotation][i][1];
   if(y<19){
   	for(int scan=y;scan<20;scan++){
      	Grid[x][scan].Type=TET_BLOCK_NOTHING;
         Stats.Effects++;
			Stats.Score+=STATS_EFFECTS*Stats.Level;
      }

   }
EffectFrame();
}
return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PLAYFIELD::CheckForLines(){
//return value is true if they had lines
int x,y;
int x2,y2;
int BlockCount;//when this equals 10 after scanning through Xs, then remove that line.
int LineCount=0;

for(y=19;y>0;y--){
	BlockCount=0;
	for(x=0;x<10;x++){
		if(Grid[x][y].Type!=TET_BLOCK_NOTHING){
      	BlockCount++;
      }
   }
   if(BlockCount==10){
   	//we have to get rid of that damn line
      for(y2=y;y2>1;y2--){//scan from the current y to the first Y
      	for(x2=0;x2<10;x2++){
         	Grid[x2][y2].Type=Grid[x2][y2-1].Type;

         }
      }
      LineCount++;
   }
}
Stats.Lines+=LineCount;
//Stats.Score+=LineCount*STATS_LINES;

if(LineCount>0) return true;
return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::CalculateScore(){
//this is the home of the score-calculating algorithm.
long ScoreSoFar;
ScoreSoFar=0;

//Stats.Score=ScoreSoFar;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Drop(){
FramesLeft=0;
Stats.Drops++;
Stats.Score+=STATS_DROPS*Stats.Level;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::EffectFrame(){
//this is a routine that updates the screen after doing an effect, so
//the player can see the effects happening
Frame++;
LRESULT Result=Blit(Pass.DDBack, *DDField, 0,0,640,480,0,0);
UpdateField();
Flip(Pass.DDFront);
Sleep(70);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::CurrentStatUpdate(){
	//update the statistic for that block
	switch(Current.BlockType.Type){
		case TET_BLOCK_FIRE:
	   	Stats.Fire++;
			Stats.Score+=STATS_FIRE*Stats.Level;
  	   break;
	   case TET_BLOCK_ICE:
	   	Stats.Ice++;
			Stats.Score+=STATS_ICE*Stats.Level;
	   break;
	   case TET_BLOCK_EARTH:
	   	Stats.Earth++;
			Stats.Score+=STATS_EARTH*Stats.Level;
	   break;
	   case TET_BLOCK_DESTRUCT:
	   	Stats.Destruct++;
			Stats.Score+=STATS_DESTRUCT*Stats.Level;
	   break;
	   case TET_BLOCK_GRAVITY:
	   	Stats.Gravity++;
			Stats.Score+=STATS_GRAVITY*Stats.Level;
	   break;
	   case TET_BLOCK_WATER:
	   	Stats.Water++;
			Stats.Score+=STATS_WATER*Stats.Level;
	   break;
	   case TET_BLOCK_FLAMMABLE:
	   	Stats.Flammable++;
			Stats.Score+=STATS_FLAMMABLE*Stats.Level;
	   break;
	}
switch(Current.Type){
	case 0:
   	Stats.Block1++;
		Stats.Score+=STATS_BLOCK1*Stats.Level;
   break;
	case 1:
   	Stats.Block2++;
		Stats.Score+=STATS_BLOCK2*Stats.Level;
   break;
	case 2:
   	Stats.Block3++;
		Stats.Score+=STATS_BLOCK3*Stats.Level;
   break;
	case 3:
   	Stats.Block4++;
		Stats.Score+=STATS_BLOCK4*Stats.Level;
   break;
	case 4:
   	Stats.Block5++;
		Stats.Score+=STATS_BLOCK5*Stats.Level;
   break;
	case 5:
   	Stats.Block6++;
		Stats.Score+=STATS_BLOCK6*Stats.Level;
   break;
	case 6:
   	Stats.Block7++;
		Stats.Score+=STATS_BLOCK7*Stats.Level;
   break;
}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::UpdateIARs(int NumberOfLines, int HowManyInARow){
//given the lines,
//Stats.QuadsIAR++;
switch(NumberOfLines){
	case 1:
		if(Stats.SinglesIAR<HowManyInARow){
      	Stats.SinglesIAR=HowManyInARow;
         Stats.Score+=STATS_SINGLESIAR*Stats.Level;
      }
   break;
   case 2:
		if(Stats.DoublesIAR<HowManyInARow){
      	Stats.DoublesIAR=HowManyInARow;
         Stats.Score+=STATS_DOUBLESIAR*Stats.Level;
      }
   break;
   case 3:
		if(Stats.TriplesIAR<HowManyInARow){
      	Stats.TriplesIAR=HowManyInARow;
         Stats.Score+=STATS_TRIPLESIAR*Stats.Level;
      }
   break;
   case 4:
		if(Stats.QuadsIAR<HowManyInARow){
      	Stats.QuadsIAR=HowManyInARow;
         Stats.Score+=STATS_QUADSIAR*Stats.Level;
      }
   break;
   default:
		if(Stats.MoreThanQuadsIAR<HowManyInARow){
      	Stats.MoreThanQuadsIAR=HowManyInARow;
         Stats.Score+=STATS_MORETHANQUADSIAR*Stats.Level;
      }
   break;
}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Reset(){
//Now give Next and Current some data.
Next.BlockType.Type=(rand()%TET_BLOCK_MAX)+1;
Next.Type=rand()%7;
Next.BlockType.Frame=0;
Next.Rotation=0;
Current.Type=rand()%7;
Current.BlockType.Type=(rand()%TET_BLOCK_MAX)+1;

Cheat_GiveAllLines=false;
Cheat_FrameRate=false;
Cheat_GiveAllEarth=false;

Current.BlockType.Frame=0;
Current.Rotation=0;
//Clear all data in Grid[10][20]
ClearField();
//set the frame to zero
Frame=0;
FramesLeft=50;
Level=0;
Pos.x=5;
Pos.y=0;

Stats.Lines=0;
Stats.Effects=0;
Stats.Doubles=0;
Stats.DoublesIAR=0;
Stats.Singles=0;
Stats.SinglesIAR=0;
Stats.Triples=0;
Stats.TriplesIAR=0;
Stats.Quads=0;
Stats.QuadsIAR=0;
Stats.Earth=0;
Stats.Ice=0;
Stats.Fire=0;
Stats.Water=0;
Stats.Destruct=0;
Stats.Gravity=0;
Stats.Flammable=0;
Stats.Drops=0;

Stats.Block1=0;
Stats.Block2=0;
Stats.Block3=0;
Stats.Block4=0;
Stats.Block5=0;
Stats.Block6=0;
Stats.Block7=0;

Stats.Level=1;
Stats.Paused=0;
Stats.Cheats=0;
Stats.Score=0;
CurrentStatUpdate();

GamePaused=false;
PauseFrameCount=0;

}
////////////////////////////////////////////////////////////////////////////////////////
//CHEATS SECTION.....
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::ClearField()
{
	//990121
	//just clears Grid[][] essentially.
	//this is half a cheat becaue it's used in the playfield object too, legitimately
	for(int x=0;x<10;x++){
	for(int y=0;y<20;y++){
		Grid[x][y].Type=TET_BLOCK_NOTHING;
		Grid[x][y].Frame=0;
	}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::RandomizeField(){
	PlayWaveFile(WAV_CHEAT);
	//I don't know about this one yet.
	this->Stats.Cheats++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::DoubleScore(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Score*=2;
	this->Stats.Cheats++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::TurnToEarth(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	//turns the whole playfield to earth
	int x, y;
	for(x=0;x<10;x++){
		for(y=0;y<20;y++){
			if(Grid[x][y].Type!=TET_BLOCK_NOTHING) Grid[x][y].Type=TET_BLOCK_EARTH;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::GravitizeAll(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	//drops all the pieces down
	//so we must scan from bottom to top and each time we find a space, drop everything.
	int x,y, y2;
	int Changes=0;
	do{
		Changes=0;
		for(y=19;y>1;y--){
			for(x=0;x<10;x++){
				//now if this block is a space, drop them all.
				if(this->Grid[x][y].Type==TET_BLOCK_NOTHING && this->Grid[x][y-1].Type!=TET_BLOCK_NOTHING){
					OutputDebugString("here we are");
					for(y2=y;y2>1;y2--){
						Grid[x][y2].Type=Grid[x][y2-1].Type;
					}
					Grid[x][1].Type=TET_BLOCK_NOTHING;
					Changes++;
				}
			}
		}
	}while(Changes);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::InstantQuad(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	//makes half the screen earth with one size open,
	//with 6 blocks above there
	//and gives you a gravity block
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::GiveAllLines(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	if(Cheat_GiveAllLines==false){
		Cheat_GiveAllLines=true;
		Next.Type=4;
	}
	else Cheat_GiveAllLines=false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::GiveAllEarth(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	if(Cheat_GiveAllEarth==false){
		Cheat_GiveAllEarth=true;
		Next.BlockType.Type=TET_BLOCK_EARTH;
	}
	else Cheat_GiveAllEarth=false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::NextGravity(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	Next.BlockType.Type=TET_BLOCK_GRAVITY;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::NextDestruct(){
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
	Next.BlockType.Type=TET_BLOCK_DESTRUCT;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Cheat1(){//framerate
	PlayWaveFile(WAV_CHEAT);
	if(this->Cheat_FrameRate==false) Cheat_FrameRate=true;
	else Cheat_FrameRate=false;
	this->Stats.Cheats++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Cheat2(){//currently not used
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PLAYFIELD::Cheat3(){//currently not used
	PlayWaveFile(WAV_CHEAT);
	this->Stats.Cheats++;
}