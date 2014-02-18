#include "textfilelib.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INIFILE::INIFILE()
{
	opened=false;
	hfile=0;
	position=0;
	ZeroMemory(&ofinfo, sizeof(ofinfo));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INIFILE::~INIFILE()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD INIFILE::GetFilePointer(){
	return SetFilePointer((HANDLE)hfile,0,0,FILE_CURRENT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD INIFILE::SearchString(LPCSTR String)
{
	//search for a string and set the file pointer to the first byte of the size ofthe value
	//if the string is not found, it returns 0...since all the valuenames are 10 characters long
	//the very first place SearchString() could return is 11.

	char Str[11];
   ZeroMemory(Str, 11);
   DWORD nobr;
   DWORD Size;
   DWORD Rvalue=0;//return value

	SetFilePointer((HANDLE)hfile, 0, 0, FILE_BEGIN);

   bool Loop=true;
	//LOOP
   do{
   	Loop=false;
	   ReadFile((HANDLE)hfile, Str, 10, &nobr, NULL);
	   if(nobr == 10){
      	Loop=true;

			//if we haven't reached the end of the file
			//read in the first 10 bytes.
	      //now compare that with the string
	      if(lstrcmp(String, Str)==0){
	      	//we have a match
	         Rvalue=GetFilePointer();
	         break;
	      }
	      ReadFile((HANDLE)hfile,&Size, sizeof(Size), &nobr, NULL);
	     	SetFilePointer((HANDLE)hfile, Size, 0, FILE_CURRENT);//move the cursor appropriately
	  	}
   }while(Loop==true);

return Rvalue;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int INIFILE::Open(LPCSTR FileName)
{
	hfile=OpenFile(FileName, &ofinfo, OF_READWRITE);
   if(hfile != HFILE_ERROR) opened=true;
   else{
	   unsigned long nBytes=0;
	   SECURITY_ATTRIBUTES sa;
	   sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	   sa.lpSecurityDescriptor=NULL;
	   sa.bInheritHandle=false;
	   hfile=
				(HFILE)CreateFile(FileName,	GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
   }
   return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int INIFILE::Close()
{
	_lclose(hfile);
   opened=false;
   return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int INIFILE::ReadValue(LPCSTR ValueName, void * data)
{
	//search for the value and stick it in data.
	DWORD pos=0;
	DWORD Size=0;
	DWORD nobr=0;

	pos=SearchString(ValueName);//pos is the position in the file
	if(pos==0){
		//doesn't exist
	   return 0;
	}
	ReadFile((HANDLE)hfile, &Size, sizeof(Size), &nobr, NULL);
	ReadFile((HANDLE)hfile, data, Size, &nobr, NULL);
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int INIFILE::WriteValue(LPCSTR ValueName, DWORD Length, void * Value)
{
//	DWORD FileSize;
   DWORD nobw;

	//search for the value....if it's there, then replace it.
	//if it's not, then we will have to insert it in.
	if(SearchString(ValueName)==0){
		//not found, so create a new one.
      SetFilePointer((HANDLE)hfile, 0,0,FILE_END);//set to the end of the file
      WriteFile((HANDLE)hfile, ValueName, 10, &nobw, NULL);
      WriteFile((HANDLE)hfile, &Length, sizeof(Length), &nobw,  NULL);
      WriteFile((HANDLE)hfile, Value, Length, &nobw,  NULL);
      return 10;
	}
   else{
   	//it's found, so we have to write the information inside the damn
      //file.  Insert it.  Damn it.  I had to write this all manaully; there
      //should be a function for this
      //to insert into the file, I'll start by reading in the rest of the file,
      //truncating the file, then writing the new data, and pasting the rest

      //1) Set file pointer to the end of this value chunk
//      char DummyName[10];
      DWORD DummySize;
      char DummyData[1000];
      DWORD nobr;
      bool NotLastValue=false;

      ReadFile((HANDLE)hfile, &DummySize, sizeof(DummySize),&nobr,  NULL);
      ReadFile((HANDLE)hfile, &DummyData, DummySize, &nobr, NULL);

      //2)  Read the rest of the file
      DWORD HowMuchIsLeft=GetFileSize((HANDLE)hfile, NULL) - GetFilePointer();
	   char RestOfFile[9000];//allocate 9k of memory
      if(HowMuchIsLeft==0){
      	NotLastValue=true;
	      ReadFile((HANDLE)hfile, RestOfFile, HowMuchIsLeft, &nobr, NULL);
      }

      //3 Get back to where we're supposed to write/truncate the file there
		SearchString(ValueName);
      SetFilePointer((HANDLE)hfile, -10, 0, FILE_CURRENT);
      SetEndOfFile((HANDLE)hfile);

      //4) Write the new data
      WriteFile((HANDLE)hfile, ValueName, 10,  &nobw, NULL);
      WriteFile((HANDLE)hfile, &Length, sizeof(Length), &nobw,  NULL);
      WriteFile((HANDLE)hfile, Value, Length, &nobw,  NULL);

      //5) Write the rest of the data
      if(NotLastValue==true){
	      WriteFile((HANDLE)hfile, RestOfFile, HowMuchIsLeft, &nobw,NULL);
      }
      return 20;

   }
	return 1;
}
