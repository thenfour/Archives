#include "dswavlib.h"

//LOAD A WAV file into a buffer:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT LoadWaveFile(LPDIRECTSOUNDBUFFER * Buffer, LPCSTR FileName)
{
	if(*Buffer != NULL){
HMMIO hmm; //handle of multimedia file to open (FileName)
MMRESULT Result;
MMCKINFO ParentCK;
MMCKINFO CKInfo;
//PCMWAVEFORMAT WaveFormat;
////////////////
hmm=mmioOpen((char *)FileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if(hmm==NULL) return 0;//error
//fill ParentCK with data about the main RIFF chunk of the file.
ParentCK.fccType=mmioFOURCC('W','A','V','E');
Result=mmioDescend(hmm, &ParentCK, NULL, MMIO_FINDRIFF);
	if(Result!=MMSYSERR_NOERROR) return 0;
//now find the data subchunk.
CKInfo.ckid=mmioFOURCC('d','a','t','a');
Result=mmioDescend(hmm, &CKInfo, &ParentCK, MMIO_FINDCHUNK);
	if(Result!=MMSYSERR_NOERROR) return 0;
////////////////////////////////////////
//now put data into the buffer
char * lpBuf1;
char * lpBuf2;
unsigned long Bytes1, Bytes2;
(*Buffer)->Lock(0, NULL, (void**)&lpBuf1, &Bytes1, (void **)&lpBuf2, &Bytes2, DSBLOCK_ENTIREBUFFER);
//read in the wave file
DWORD Size;
if(Bytes1 > CKInfo.cksize) Size=CKInfo.cksize;
if(Bytes1 <= CKInfo.cksize) Size=Bytes1;

for(unsigned int i=0; i<Bytes1; i++){
	*(lpBuf1+i)=rand();
}
ZeroMemory(lpBuf1, Bytes1);
mmioRead(hmm, lpBuf1, Size);

(*Buffer)->Unlock(lpBuf1, Bytes1, lpBuf2, Bytes2);
Result=mmioClose(hmm, NULL);
	}
return DD_OK;
}