#include "dsstream.h"
#include <mmsystem.h>
#include "stdio.h"

#include "etetriz.h"

#define DSS_BUFFERLENGTH			3000
#define DSS_TIMERWAIT				1000

extern TETRISOPTIONS toOptions;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DSSTREAM::DSSTREAM()
{
	StreamStatus=DSSTREAM_IDLE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DSSTREAM::~DSSTREAM()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DSSTREAM::SetVolume(LONG vol){
	if(toOptions.Music && StreamStatus==DSSTREAM_INITIALIZED){
		if(this->dsBuffer !=NULL){
		this->dsBuffer->SetVolume(vol);
	}}
	return DS_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DSSTREAM::Initialize(LPDIRECTSOUND* lplpDirectSound, LPCSTR File)
{
	if(toOptions.Music){
	LRESULT Result;
	DSObject=lplpDirectSound;//copy this address for the use of DSSTREAM
	lstrcpy(FileName, File);//and copy the filename for the use of DSSTREAM

	//Create the sound buffer.
	//it will be 1.5 seconds long, and 44.1khz/16-bit stereo.
	//the DSBCAPS_STATIC flag is not present here, as it is in CreateSecondaryBuffer()
	//in "dsoundlib.cpp".  This will not be a static buffer; we will be writing to
	//it continuously.

	ZeroMemory(&WaveFormat, sizeof(WaveFormat));
	WaveFormat.wFormatTag=WAVE_FORMAT_PCM;
	WaveFormat.nChannels=2;
	WaveFormat.nSamplesPerSec=44100;
	WaveFormat.wBitsPerSample=16;
	WaveFormat.nBlockAlign=4;
	WaveFormat.nAvgBytesPerSec=(44100*16*2)/8;

	ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.dwSize=sizeof(BufferDesc);//initialize this structure
	BufferDesc.dwFlags=DSBCAPS_CTRLVOLUME;//we'll have volume control of this buffer
	BufferDesc.dwBufferBytes=(WaveFormat.nAvgBytesPerSec)*(DSS_BUFFERLENGTH/1000);//1.5 second buffer
	BufferDesc.lpwfxFormat=&WaveFormat;

	//now create the buffer
	Result=(*lplpDirectSound)->CreateSoundBuffer(&BufferDesc, &dsBuffer, NULL);
	if(Result==DS_OK){
	   StreamStatus=DSSTREAM_INITIALIZED;
	}
	}
	return DS_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DSSTREAM::Play()
{
if(toOptions.Music){
if(StreamStatus==DSSTREAM_INITIALIZED){
	//OPEN that damn wave file for reading using MM functions (ugh)
	MMRESULT Result;
	MMCKINFO ParentCK;
	MMCKINFO CKInfo;

	char *lpBuf1, *lpBuf2;//pointers to buffer in memory
	unsigned long Bytes1, Bytes2;//length of the pointers

	///////////////////////////////////////
	hmm=mmioOpen(FileName, NULL, MMIO_ALLOCBUF | MMIO_READ);//open for reading
  		if(hmm==NULL) return DS_OK;//error
	ParentCK.fccType=mmioFOURCC('W','A','V','E');
	Result=mmioDescend(hmm, &ParentCK, NULL, MMIO_FINDRIFF);//get into the "WAVE" chunk
		if(Result!=MMSYSERR_NOERROR) return DS_OK;
	CKInfo.ckid=mmioFOURCC('d','a','t','a');
	Result=mmioDescend(hmm, &CKInfo, &ParentCK, MMIO_FINDCHUNK);//get into the "data" subchunk
		if(Result!=MMSYSERR_NOERROR) return DS_OK;
	////////////////////////////////////////

	//lock the whole buffer and fill it with data, then unlock it.
	Result=dsBuffer->Lock(0, NULL, (void**)(&lpBuf1), &Bytes1, (void**)&lpBuf2, &Bytes2, DSBLOCK_ENTIREBUFFER);
   if(Result==DS_OK){
			mmioRead(hmm, lpBuf1, Bytes1);
		dsBuffer->Unlock(lpBuf1, Bytes1, lpBuf2, Bytes2);
   }
   else{
   	StreamStatus=100;
   }
	////////////////////////////////////////
	//now that we are all set up to start playing, we have to do some final things.
	//namely, setup variables
	si.pos=WaveFormat.nAvgBytesPerSec*(DSS_TIMERWAIT/1000);//set buffer position to the first time we'll be in there.
	si.pos=0;
	si.nTimes=0;//number of times timer proc has been called

	//set a variable.
	BytesPerRead=WaveFormat.nAvgBytesPerSec/(1000/DSS_TIMERWAIT);
	////////////////////////////////////////
	//now create a timer.  This will be a 500 ms timer and will call the TimerProc function.
	//with a resolution of 4 milliseconds accuracy, and will be periodic as opposed to one-time-only.

	TimerID=timeSetEvent(DSS_TIMERWAIT, DSS_TIMERWAIT,&(DSSTREAM::TimerProc),(DWORD) this,TIME_PERIODIC);

	//and finally, play the buffer.
	Result=(dsBuffer)->SetCurrentPosition(0);
	Result=(dsBuffer)->Play(0,0,DSBPLAY_LOOPING);
	if(Result!=DS_OK){
		StreamStatus=200;
	}

	return DS_OK;
	}}
	return DS_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DSSTREAM::Stop()
{
	if(toOptions.Music && StreamStatus==DSSTREAM_INITIALIZED){
	MMRESULT Result;
	Result=dsBuffer->Stop();
	Result=timeKillEvent(TimerID);//kill this timer event.
	Result=mmioClose(hmm, NULL);
	}
	return DS_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK DSSTREAM::TimerProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	DSSTREAM * lpThis;
	lpThis=(DSSTREAM *) dwUser;

	//char str[100];
	//sprintf(str, "pos: %d\n", lpThis->si.pos);
	//OutputDebugString(str);

	//get current position - if the new position is < the old position,
	//that's as good as a notify event that it went to 0 so we can use that
	//info to set the position to the last position.
	static DWORD oldplay=0;
//	DWORD play, write;
	//lpThis->dsBuffer->GetCurrentPosition(&play, &write);
	//if(play<oldplay){
	//	lpThis->si.pos=(DSS_BUFFERLENGTH-DSS_TIMERWAIT) * (176.4);
	//}
	//oldplay=play;


	MMRESULT Result;
	lpThis->si.nTimes++;
	if(lpThis->si.nTimes>1){//let it play the first chunk, then start to read in after that.

		char *lpBuf1, *lpBuf2;//pointers to buffer in memory
		unsigned long Bytes1, Bytes2;//length of the pointers
		//now we have to read in a bunch of data.
		//si.pos is the position we are in the buffer
		Result=(lpThis->dsBuffer)->Lock(lpThis->si.pos, lpThis->BytesPerRead,
			(void**)&lpBuf1, &Bytes1, (void **)&lpBuf2, &Bytes2, 0);
		if(Result==DS_OK){
			if(mmioRead(lpThis->hmm, lpBuf1, Bytes1)==0){
				//endof file...
				//now stop playback and then start it again ( loop it)
				//sprintf(str, "EOF\n");
				//OutputDebugString(str);
				lpThis->Stop();
				lpThis->StreamStatus=DSSTREAM_INITIALIZED;
				oldplay=0;
				lpThis->Play();
				return;
            }
			if(Bytes2>0){
				Result=mmioRead(lpThis->hmm, lpBuf2, Bytes2);
			}
			(lpThis->dsBuffer)->Unlock(lpBuf1, Bytes1, lpBuf2, Bytes2);
		}
		else{
      		//not OK
			lpThis->Stop();
		}

		lpThis->si.pos+=lpThis->BytesPerRead;//add on to our position
		lpThis->si.pos %=lpThis->BufferDesc.dwBufferBytes;//and if it should wrap around, calculate accordingly
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DSSTREAM::Release(){
	if(dsBuffer != NULL && StreamStatus==DSSTREAM_INITIALIZED){
		dsBuffer->Release();
		dsBuffer=NULL;
	}
		return DS_OK;
}