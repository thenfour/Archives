#ifndef __TEXTFILELIB
#define __TEXTFILELIB

#include <windows.h>

class INIFILE{

	private:

   public:
   	INIFILE();
      ~INIFILE();

		DWORD SearchString(LPCSTR String);
		DWORD GetFilePointer();

      int Open(LPCSTR FileName);
      int Close();
      int ReadValue(LPCSTR ValueName, void * data);
      int WriteValue(LPCSTR ValueName, DWORD Length, void * Value);

      OFSTRUCT ofinfo;
      bool opened;
      DWORD position;
      HFILE hfile;
};
/*my ini file will be like this:
not a text file, but there will be "values"
each value will have a code (like a 4-character RIFF code)
and a header, and data.

so the form will be like:

char[10] = "SOUNDVOLUM"         'the name of the value
int = 5                         'the length of the data (bytes)
char[5]=0xFFFF00FF4C            'the data.

and consequently those three values are the three parameters of INIFILE::WriteValue()

/////////////////
so how this will work is as follows:
This is a high level interface for .ini files.  EAch member function will be responsible
for many things.  When I was coming up with the functions, I had in mind how VB works with
strings.  So I will probably have a lot of private functions that will be like "Instr"
and other VB functions.

A list of the functions:
--------------------------------------
void SearchString(LPCSTR String);
----------------------------------
	This will be like VB's "instr" - kinda.  It will find the occurence of the value name
   String.  This function will set the cursor to 10 (right after the first value name).
   Then it will read in the next INT, skip that many, and keep reading.  Every time it
   scans in that 10 byte name, though, it will compare it with String.  When it's found,
   the cursor will be set to the beginning of the valuename.

void Open(LPCSTR FileName);
	As one would expect, this is required before any operations are available to the user...
   just opens the file for read/write.

void Close();
	Hmmm, I wonder....

void * ReadValue(LPCSTR ValueName);
	This function will call SearchString() and then proceed to return a pointer to the
   data.

void WriteValue(LPCSTR ValueName, DWORD Length, void * Value);
	This function first calls SearchString() and if the value name doesn't exist, then
   it sticks on the info. at the end of the file.  If it's found, then that's a different
   story.  It has to insert it in the middle of the file.


*/
#endif