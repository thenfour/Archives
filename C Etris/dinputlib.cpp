//
#include "dinputlib.h"

LRESULT SetupDIObject(LPDIRECTINPUT& Object, HINSTANCE hInstance)
{
LRESULT Result;

Result=DirectInputCreate(hInstance, DIRECTINPUT_VERSION, &Object, NULL);

return Result;
}


LRESULT SetupDIDevice(LPDIRECTINPUT& Object, LPDIRECTINPUTDEVICE& Device, HWND hWnd)
{
LRESULT Result;

//create the device from thin air
Result=Object->CreateDevice(GUID_SysKeyboard, &Device, NULL);
//tell it how we'll read the data
if(Result==DI_OK){
	Result=Device->SetDataFormat(&c_dfDIKeyboard);
	if(Result==DI_OK){
	   //Tell DirectInput that we want foreground access only, and non-exclusive
		Result=Device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	}
}

return Result;
}

LRESULT Acquire(LPDIRECTINPUTDEVICE & Device)
{
LRESULT Result;
Result=Device->Acquire();
return Result;
}

//This doesn't work; I want to figure out why (it's in the passing of the
//parameters, but damn I don't understand what's wrong...of coruse I haven't thought
//about it much.
LRESULT ReadKeyboard(char far * Buffer, LPDIRECTINPUTDEVICE & Device)
{
LRESULT Result;
Result=Device->GetDeviceState(sizeof(Buffer),(LPVOID)*Buffer);
return Result;
}

