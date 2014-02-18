//catfile.h

#ifndef CATFILE_H
#define CATFILE_H

#include <windows.h>

#define CATFILE_ERROR 1
#define CATFILE_OK 0

/////////////////////////////////////////////////////////////
//this class is used simply as an easy interface to a file,
//so the program can look cleaner.
//it's just an interface to a file on disk
class CATFILE{
public:
	CATFILE();
	~CATFILE();

	int OpenForWrite(char* FileName);
	int OpenForRead(char* FileName);

	int Close();
	int Read(void*buf, int Size);
	int Write(void*buf, int Size);
	
private:

	char FileName[MAX_PATH];
	HFILE hfile;
	BOOL IsOpen;
	BOOL IsWrite;
	BOOL IsRead;

};
#endif