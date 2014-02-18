#ifndef __HIGHSCORES
#define __HIGHSCORES

#include "passvars.h"

#ifndef HIGHSCORES_HIGHSCORES
#define HIGHSCORES_HIGHSCORES
struct HIGHSCORE{
	char Name[100];
   long Lines;
   long Effects;
   long Singles;
   long Doubles;
   long Triples;
   long Quads;
   long MoreThanQuads;
   long SinglesIAR;
   long DoublesIAR;
   long TriplesIAR;
   long QuadsIAR;
   long MoreThanQuadsIAR;
   long Earth;
   long Ice;
   long Fire;
   long Flammable;
   long Water;
   long Destruct;
   long Gravity;
   long Drops;
   long Score;
   long Paused;
   long Cheats;
   long Block1;
   long Block2;
   long Block3;
   long Block4;
   long Block5;
   long Block6;
   long Block7;
   char Month;
   char Day;
   char Year;
   char Hour;
   char Minute;
   char Second;
};


#define HIGHSCORESCOUNT    31//how many high scores are in the HIGHSCORES struct.
struct HIGHSCORES{
	HIGHSCORE HighScore[HIGHSCORESCOUNT];
};

#endif

#define SHS_NAME 0
#define SHS_DATE 1
#define SHS_SCORE 2
#define SHS_LINES 3
#define SHS_EFFECTS 4
#define SHS_CHEATS 5

int SortHighScores(int Field);
int ReadHighScores();
int WriteHighScores();


#endif