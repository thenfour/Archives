#include "dsoundlib.h"
extern PASSVARS Pass;
extern TETRISOPTIONS toOptions;

/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SetupDSound(){
//uses Pass.DSObject and Pass.DSPrimary to setup the object and primary buffer
LRESULT Result;

//Create the DirectSound Object
Result=DirectSoundCreate(NULL, &Pass.DSObject, NULL);
if(Result != DS_OK) return Result;
//Set the cooperative level to normal
Result=Pass.DSObject->SetCooperativeLevel(Pass.DDWindow, DSSCL_PRIORITY);
if(Result != DS_OK) return Result;

//now create the primary buffer.
WAVEFORMATEX WaveFormat;
ZeroMemory(&WaveFormat, sizeof(WaveFormat));
WaveFormat.wFormatTag=WAVE_FORMAT_PCM;
WaveFormat.nChannels=2;
WaveFormat.nSamplesPerSec=44100;
WaveFormat.wBitsPerSample=16;
WaveFormat.nBlockAlign=4;
WaveFormat.nAvgBytesPerSec=(44100*16*2)/8;

DSBUFFERDESC BufferDesc;
ZeroMemory(&BufferDesc, sizeof(BufferDesc));
BufferDesc.dwSize=sizeof(BufferDesc);
BufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;
BufferDesc.dwBufferBytes=0;
BufferDesc.lpwfxFormat=NULL;

//now that we have all that set up, create it.
Result=Pass.DSObject->CreateSoundBuffer(&BufferDesc, &Pass.Sounds.Primary, 0);
if(Result != DS_OK) return Result;
//and finally set the format of playback
Result=Pass.Sounds.Primary->SetFormat(&WaveFormat);
if(Result != DS_OK) return Result;
return DS_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CreateSecondaryBuffer(LPDIRECTSOUNDBUFFER *NewBuffer, int Seconds){
HRESULT Result;

WAVEFORMATEX WaveFormat;
ZeroMemory(&WaveFormat, sizeof(WaveFormat));
WaveFormat.wFormatTag=WAVE_FORMAT_PCM;
WaveFormat.nChannels=2;
WaveFormat.nSamplesPerSec=44100;
WaveFormat.wBitsPerSample=16;
WaveFormat.nBlockAlign=4;
WaveFormat.nAvgBytesPerSec=(44100*16*2)/8;

DSBUFFERDESC BufferDesc;
ZeroMemory(&BufferDesc, sizeof(BufferDesc));
BufferDesc.dwSize=sizeof(BufferDesc);
BufferDesc.dwFlags=DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_STATIC ;
BufferDesc.dwBufferBytes=(WaveFormat.nAvgBytesPerSec)*Seconds;
BufferDesc.lpwfxFormat=&WaveFormat;

//now create the buffer
Result=Pass.DSObject->CreateSoundBuffer(&BufferDesc, NewBuffer, NULL);
if(Result != DS_OK) return Result;
return DS_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Play(LPDIRECTSOUNDBUFFER * Buffer)
{
	if(toOptions.SFX){
		HRESULT Result;
		Result=(*Buffer)->Stop();
		if(Result != DS_OK) return Result;
		Result=(*Buffer)->SetCurrentPosition(0);
		if(Result != DS_OK) return Result;
		Result=(*Buffer)->Play(0,0,0);
		if(Result != DS_OK) return Result;
	}
	return DS_OK;
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT PlayWaveFile(LPCSTR File){
	if(toOptions.SFX){
	//uses on
	HRESULT Result;
	static bp=0;//bp==buffer pointer - will be 0, 1, or 2, pointing to DSBuffer[0], DSBuffer[1]...
	//play a wave file in DSBuffer[bp]
	Result=Pass.Sounds.DSBuffer[bp]->Stop();//if it's playing now, stop it.
	Result=LoadWaveFile(&Pass.Sounds.DSBuffer[bp], File);
		if(Result != DS_OK) return Result;
	Result=Pass.Sounds.DSBuffer[bp]->SetCurrentPosition(0);
		if(Result != DS_OK) return Result;
	Result=Pass.Sounds.DSBuffer[bp]->Play(0,0,0);
	//now adjust bp
	bp=(bp + 1) % NUMBER_OF_DSBUFFERS;
	return Result;
	}
	return DS_OK;
}