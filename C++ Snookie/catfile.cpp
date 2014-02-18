//catfile.cpp
#include "catfile.h"


///////////////////////////////////////////////////////////////////////////////////////
CATFILE::CATFILE(){
	this->FileName[0]=(char) 0;
	this->hfile=0;
	this->IsOpen=FALSE;
	this->IsRead=FALSE;
	this->IsWrite=FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
CATFILE::~CATFILE(){
	if(this->hfile != NULL){
		this->Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
int CATFILE::Close(){
	if(this->hfile != NULL){
		if(CloseHandle((HANDLE)this->hfile) != 0){
			this->hfile=0;
			return CATFILE_OK;
		}
	}
	return CATFILE_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////
int CATFILE::OpenForRead(char*FileName){
	if(this->hfile==NULL){
		this->hfile=(HFILE)CreateFile(
			FileName, 
			GENERIC_READ, 
			0, 
			0, 
			OPEN_EXISTING,
			0, 
			NULL);
	}
	return (this->hfile==NULL) ? CATFILE_ERROR : CATFILE_OK;
}

///////////////////////////////////////////////////////////////////////////////////////
int CATFILE::OpenForWrite(char *FileName){
	if(this->hfile==NULL){
		this->hfile=(HFILE)CreateFile(
			FileName, 
			GENERIC_WRITE, 
			0, 
			0, 
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
	}
	return (this->hfile==NULL) ? CATFILE_ERROR : CATFILE_OK;
}
///////////////////////////////////////////////////////////////////////////////////////
int CATFILE::Read(void *buf, int Size){
	DWORD rvalue;
	DWORD BytesRead;
	SetLastError(0);
	if(Size>0){
		if(this->hfile != NULL){
			if(ReadFile(
				(HANDLE)this->hfile,
				buf,
				Size,
				&BytesRead,
				NULL
				)==0){
				//error
				rvalue=GetLastError();
				return CATFILE_ERROR;
			}
		}else{
			//hfile == null
			rvalue=GetLastError();
			return CATFILE_ERROR;
		}
	}		
	return CATFILE_OK;
}

///////////////////////////////////////////////////////////////////////////////////////
int CATFILE::Write(void *buf, int Size){
	DWORD BytesWritten;
	DWORD rvalue;
	if(Size>0){
		if(this->hfile != NULL){
			if(WriteFile(
				(HANDLE)this->hfile,
				buf,
				Size,
				&BytesWritten,
				NULL
				)==0){
				//error
				rvalue=GetLastError();
				return CATFILE_ERROR;
			}
		}else{
			//hfile == null
			rvalue=GetLastError();
			return CATFILE_ERROR;
		}
	}
		
	rvalue=GetLastError();
	return CATFILE_OK;
}
