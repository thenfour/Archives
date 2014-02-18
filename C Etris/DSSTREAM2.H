/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DSSTREAM library 1999 JAmes Corcoran

This object makes an incredibly easy interface for streaming with DirectSound.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#include "pass.h"
#ifndef __DSSTREAM
#define __DSSTREAM

struct STREAMINFO{
	DWORD pos;
   DWORD nTimes;
};

class DSSTREAM{
   public:
   	DSSTREAM();
      ~DSSTREAM();

      void Initialize(LPDIRECTSOUND* lplpDirectSound,  char* FileName);
      void Play();
      void Stop();
		static void CALLBACK TimerProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);

		MMRESULT TimerID;//ID of the itmer we'll use
		DSBUFFERDESC BufferDesc;//description of the streaming buffer
		WAVEFORMATEX WaveFormat;//the wave format to use
	   LPDIRECTSOUNDBUFFER dsBuffer;//a pointer to the actual buffer
	   LPDIRECTSOUND* DSObject;//pointer to the DS object to use.
		int StreamStatus;//this is here to make sure things run smoothly
		char FileName[100];//the name of the file to use
	   HMMIO hmm;//handle of the file we'll open
		STREAMINFO si;
   private:
};
#define DSSTREAM_IDLE						0x01
#define DSSTREAM_PLAYING               0x02
#define DSSTREAM_STOPPED               0x03
#define DSSTREAM_INITIALIZED				0x04



#endif