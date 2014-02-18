#include "gameover.h"
#define WAV_GAMEOVER "sfx\\gameover.wav"

extern PASSVARS Pass;
extern HIGHSCORES HighScores;
extern PLAYFIELD pfPlayField;

//GameOverMain is called upon a game ending...
//990201
//this saves high scores and displays the game over screen.

void GameOverMain(){
	Pass.Sounds.GameScore.Stop();
	PlayWaveFile(WAV_GAMEOVER);
	//first, figure out if we've gotten a higher score than any of the other games in the HighScores struct.
	long CurrentHighScore=-1;
	int CurrentHighScoreNumber=HIGHSCORESCOUNT+1;//I set this equal to the highest high score plus one so that
												//when we get done scanning all the scores and this still equals
												//this, we know they didn't get a high score.
												//in other words, this is one number that it can NEVER be if
												//it's changed at all.
	for(int i=0;i<HIGHSCORESCOUNT;i++){
		if(HighScores.HighScore[i].Score<pfPlayField.Stats.Score){
			if(HighScores.HighScore[i].Score>CurrentHighScore){
				//if it's the highest score, but below the player's score, then record it.
				CurrentHighScoreNumber=i;
				CurrentHighScore=HighScores.HighScore[i].Score;
			}
		}
	}

	//now we know if they got a high score; record it
	if(CurrentHighScoreNumber!=(HIGHSCORESCOUNT+1)){
		//we got a high score.
		//we have to move all the high scores down.
		if(CurrentHighScoreNumber!=(HIGHSCORESCOUNT-1)){//only if it's not the
			for(int i=HIGHSCORESCOUNT-1; i>CurrentHighScoreNumber; i--){
				HighScores.HighScore[i]=HighScores.HighScore[i-1];
				//CopyMemory(&(HighScores.HighScore[i]), &(HighScores.HighScore[i-1]), sizeof(HighScores.HighScore[i]));
			}
		}
		//we have to fill the HighScore[] data. (ugh, thank god for cut/paste)
		HighScores.HighScore[CurrentHighScoreNumber].Score=pfPlayField.Stats.Score;
		HighScores.HighScore[CurrentHighScoreNumber].Block1=pfPlayField.Stats.Block1;
		HighScores.HighScore[CurrentHighScoreNumber].Block2=pfPlayField.Stats.Block2;
		HighScores.HighScore[CurrentHighScoreNumber].Block3=pfPlayField.Stats.Block3;
		HighScores.HighScore[CurrentHighScoreNumber].Block4=pfPlayField.Stats.Block4;
		HighScores.HighScore[CurrentHighScoreNumber].Block5 =pfPlayField.Stats.Block5;
		HighScores.HighScore[CurrentHighScoreNumber].Block6 =pfPlayField.Stats.Block6;
		HighScores.HighScore[CurrentHighScoreNumber].Block7 =pfPlayField.Stats.Block7;

		HighScores.HighScore[CurrentHighScoreNumber].Drops =pfPlayField.Stats.Drops;
		HighScores.HighScore[CurrentHighScoreNumber].Cheats =pfPlayField.Stats.Cheats;

		//get the date and store it, too.
		time_t timet;//create a time_t struct that holds elapsed seconds.
		tm *Time;//this struct will hold the minutes/hours/etc....
		time(&timet);//gets the seconds in time_t.
		Time=localtime(&timet);//converts it to minute/etc....

		HighScores.HighScore[CurrentHighScoreNumber].Day =Time->tm_mday;
		HighScores.HighScore[CurrentHighScoreNumber].Month =(Time->tm_mon)+1;
		HighScores.HighScore[CurrentHighScoreNumber].Year =Time->tm_year;
		HighScores.HighScore[CurrentHighScoreNumber].Second =Time->tm_sec;
		HighScores.HighScore[CurrentHighScoreNumber].Hour =Time->tm_hour;

		HighScores.HighScore[CurrentHighScoreNumber].Lines=pfPlayField.Stats.Lines;
		HighScores.HighScore[CurrentHighScoreNumber].Doubles =pfPlayField.Stats.Doubles;
		HighScores.HighScore[CurrentHighScoreNumber].DoublesIAR =pfPlayField.Stats.DoublesIAR;
		HighScores.HighScore[CurrentHighScoreNumber].Singles =pfPlayField.Stats.Singles;
		HighScores.HighScore[CurrentHighScoreNumber].SinglesIAR =pfPlayField.Stats.SinglesIAR;
		HighScores.HighScore[CurrentHighScoreNumber].Triples =pfPlayField.Stats.Triples;
		HighScores.HighScore[CurrentHighScoreNumber].TriplesIAR =pfPlayField.Stats.TriplesIAR;
		HighScores.HighScore[CurrentHighScoreNumber].Quads =pfPlayField.Stats.Quads;
		HighScores.HighScore[CurrentHighScoreNumber].QuadsIAR =pfPlayField.Stats.QuadsIAR;
		HighScores.HighScore[CurrentHighScoreNumber].MoreThanQuads =pfPlayField.Stats.MoreThanQuads;
		HighScores.HighScore[CurrentHighScoreNumber].MoreThanQuadsIAR =pfPlayField.Stats.MoreThanQuadsIAR;

		HighScores.HighScore[CurrentHighScoreNumber].Earth =pfPlayField.Stats.Earth;
		HighScores.HighScore[CurrentHighScoreNumber].Fire =pfPlayField.Stats.Fire;
		HighScores.HighScore[CurrentHighScoreNumber].Water =pfPlayField.Stats.Water;
		HighScores.HighScore[CurrentHighScoreNumber].Ice =pfPlayField.Stats.Ice;
		HighScores.HighScore[CurrentHighScoreNumber].Destruct =pfPlayField.Stats.Destruct;
		HighScores.HighScore[CurrentHighScoreNumber].Gravity =pfPlayField.Stats.Gravity;
		HighScores.HighScore[CurrentHighScoreNumber].Flammable =pfPlayField.Stats.Flammable;

		HighScores.HighScore[CurrentHighScoreNumber].Effects  =pfPlayField.Stats.Effects;
		HighScores.HighScore[CurrentHighScoreNumber].Paused  =pfPlayField.Stats.Paused;

		//get their name
		TextEntryBox(HighScores.HighScore[CurrentHighScoreNumber].Name);
	}
	WriteHighScores();
}
