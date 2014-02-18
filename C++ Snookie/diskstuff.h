//diskstuff.h

//These routines are just some disk routines, mainly directory stuff now.
#ifndef DISKSTUFF_H
#define DISKSTUFF_H

#include <windows.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
//these routines return -1 if there was an error; or the number of files/directories if it works
#define DISKSTUFF_ERROR -1

int GetFolderStatistics(int * Folders, int * Files);

#endif