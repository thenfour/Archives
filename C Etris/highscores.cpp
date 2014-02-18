#include "highscores.h"
#include "stdio.h"

extern HIGHSCORES HighScores;
extern PASSVARS Pass;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ReadHighScores(){
BOOL RValue;
HFILE fHandle;
OFSTRUCT ofHighScores;
ZeroMemory(&HighScores, sizeof(HighScores));

fHandle=OpenFile("tetris.hh",&ofHighScores,OF_EXIST);
if(fHandle==HFILE_ERROR){
	//if there was an error, create it.
   //set security stuff for the file to create;
   unsigned long nBytes=0;
   SECURITY_ATTRIBUTES sa;
   sa.nLength=sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor=NULL;
   sa.bInheritHandle=false;

   HANDLE hNewFile=
			CreateFile("tetris.hh",	GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hNewFile==INVALID_HANDLE_VALUE) return 0;
   RValue=WriteFile(hNewFile, &HighScores, sizeof(HighScores),&nBytes,NULL);
   CloseHandle(hNewFile);
   if(RValue==FALSE) return 0;
}
//now no matter what, we're all good and we'll proceed to open the file and read it.
fHandle=OpenFile("tetris.hh", &ofHighScores, OF_READ);
unsigned long nBytesRead;
if(fHandle==HFILE_ERROR) return 0;
//read it
RValue=ReadFile((HANDLE)fHandle,&HighScores,sizeof(HighScores),&nBytesRead,NULL);
if(RValue==FALSE) return 0;
_lclose(fHandle);

return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WriteHighScores(){
HFILE fHandle;
BOOL RValue;
OFSTRUCT ofHighScores;

//test if it exists.
fHandle=OpenFile("tetris.hh", &ofHighScores, OF_EXIST);
if(fHandle==HFILE_ERROR){
	//if there was an error, create it.
   //set security stuff for the file to create;
   unsigned long nBytes=0;
   SECURITY_ATTRIBUTES sa;
   sa.nLength=sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor=NULL;
   sa.bInheritHandle=false;

   HANDLE hNewFile=
			CreateFile("tetris.hh",	GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hNewFile==INVALID_HANDLE_VALUE) Pass.ProgramFlow=PF_EXIT;
  	RValue=WriteFile(hNewFile, &HighScores, sizeof(HighScores),&nBytes,NULL);
	CloseHandle(hNewFile);
	if(RValue==FALSE) Pass.ProgramFlow=PF_EXIT;
}
else{
	fHandle=OpenFile("tetris.hh", &ofHighScores, OF_WRITE);
	unsigned long nBytesWritten;
	RValue=WriteFile((HANDLE)fHandle, &HighScores, sizeof(HighScores), &nBytesWritten, NULL);
	_lclose(fHandle);
	if(RValue==FALSE) Pass.ProgramFlow=PF_EXIT;
}


return 0;
}

//use these definitions to sort the high scores.
int SortHighScores(int Field){
	//sort the highscores and save them.
	HIGHSCORES NewHS;
	BOOL Used[HIGHSCORESCOUNT];//Used[x] will be true if HighScores.HighScore[x] was used.

	//these will be sorted and stuck in NewHS.
	int Index;//the index that references the largest value found so far
	int x;
	int i, j;
	HIGHSCORE *tHSi;
	HIGHSCORE *tHSj;
	int Changed;

	do{
	//clear out all of Used[];
	for(i=0;i<HIGHSCORESCOUNT;i++){
		Used[i]=FALSE;
	}

	Changed=0;
	for(i=0;i<HIGHSCORESCOUNT;i++){//each iteration of i will put a new one in NewHS.
		//set the index to an invalid value; that way when we start searching for the highest
		//field, if Index is -1, then we know to just use it because it's the first one.
		Index=-1;
		for(j=0;j<HIGHSCORESCOUNT; j++){
			//
			if(Used[j]==FALSE){
				//we can use it; see if it's the biggest yet.
				//we have to branch out now to handle the different types though;
				if(Index==-1){
					//if index is -1, that means this is the first time searching, so
					//automatically use this.
					Index=j;//this will set Index to the first available record.
				}
				else{
					//we need to make sure we're not prioritizing an invalid highscore, too
					//(One that's not used yet - where thesore is 0);
					if(HighScores.HighScore[j].Score !=0){

						switch(Field){
						case SHS_NAME:
							//compare the current name to the "largest" one;
							x=lstrcmp(HighScores.HighScore[j].Name, HighScores.HighScore[Index].Name);
							//if [j] is greater, the return value is positive.
							if(x<0){//if this one is greater
								Index=j;
								Changed++;
							}
							break;
						case SHS_DATE:
							tHSi=&(HighScores.HighScore[i]);
							tHSj=&(HighScores.HighScore[j]);
							if(tHSj->Year >= tHSi->Year){
								if(tHSj->Year > tHSi->Year){Index=j;Changed++;}//if it's greater than, we're golden
								if(tHSj->Month >= tHSi->Month){
									if(tHSj->Month > tHSi->Month){Index=j;Changed++;}
									if(tHSj->Day >= tHSi->Day){
										if(tHSj->Day > tHSi->Day){Index=j;Changed++;}
										if(tHSj->Hour >= tHSi->Hour){
											if(tHSj->Hour > tHSi->Hour){Index=j;Changed++;}
											if(tHSj->Minute >= tHSi->Minute){
												if(tHSj->Minute > tHSi->Minute){Index=j;Changed++;}
												if(tHSj->Second >= tHSi->Second){
													if(tHSj->Second >= tHSi->Second)
														Index=j;
														Changed++;
												}
											}
										}
									}
								}
							}
							
							
							break;
						case SHS_SCORE:
							if(HighScores.HighScore[j].Score > HighScores.HighScore[Index].Score){
								Index=j;
								Changed++;
							}
							break;
						case SHS_LINES:
							if(HighScores.HighScore[j].Lines > HighScores.HighScore[Index].Lines){
								Index=j;
								Changed++;
							}
							break;
						case SHS_EFFECTS:
							if(HighScores.HighScore[j].Effects > HighScores.HighScore[Index].Effects){
								Index=j;
								Changed++;
							}
							break;
						case SHS_CHEATS:
							if(HighScores.HighScore[j].Cheats > HighScores.HighScore[Index].Cheats){
								Index=j;
								Changed++;
							}
							break;
						}//switch
					}//if()
				}//else
			}//if we can use it.
		}//for j
		//we now have the largest one, and it's index is Index.
		Used[Index]=TRUE;
		NewHS.HighScore[i]=HighScores.HighScore[Index];
	}//for i

	//now we have it sorted in NewHS, copy it;
	HighScores=NewHS;
	}while(Changed != 0);
	return 0;

}