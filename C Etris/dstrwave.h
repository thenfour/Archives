#ifndef ER_MEM
#define ER_MEM 			0xe000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN 		0xe100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE 		0xe101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD 		0xe102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE	0xe103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE		0xe104
#endif
int WaveOpenFile(char *, HMMIO *, WAVEFORMATEX **, MMCKINFO *);
int WaveStartDataRead(HMMIO *, MMCKINFO *, MMCKINFO *);
int WaveReadFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *);
int WaveCloseReadFile(HMMIO *, WAVEFORMATEX **);

int WaveCreateFile(char *, HMMIO *, WAVEFORMATEX *, MMCKINFO *, MMCKINFO *);
int WaveStartDataWrite(HMMIO *, MMCKINFO *, MMIOINFO *);
int WaveWriteFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *, MMIOINFO *);
int WaveCloseWriteFile(HMMIO *, MMCKINFO *, MMCKINFO *, MMIOINFO *, DWORD);

int WaveLoadFile(char *, UINT *, DWORD *, WAVEFORMATEX **, BYTE **);
int WaveSaveFile(char *, UINT, DWORD, WAVEFORMATEX *, BYTE *);

int WaveCopyUselessChunks(HMMIO *, MMCKINFO *, MMCKINFO *, HMMIO *,
				MMCKINFO *, MMCKINFO *);
BOOL riffCopyChunk(HMMIO, HMMIO, const LPMMCKINFO);
