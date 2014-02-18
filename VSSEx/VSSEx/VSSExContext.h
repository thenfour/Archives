/*
    VSS Sync
    VSS Revert
    VSS CheckOut
    VSS CheckIn
    VSS Diff
*/

#pragma once


#include <Windows.h>
#include <shlobj.h>
#include "Guids.h"
#include "CCString2.h"


void AddExclusion(PCWSTR wsz);


class CVSSExContext : public IShellExtInit, IContextMenu
{
public:

    CVSSExContext();
    ~CVSSExContext();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID, PVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    // IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST pidlFolder,
                          IDataObject *lpdobj, HKEY hkeyProgID);

    // IContextMenu methods
    STDMETHODIMP QueryContextMenu(HMENU,UINT,UINT,UINT,UINT);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    STDMETHODIMP GetCommandString(UINT_PTR,UINT,UINT*,LPSTR,UINT);

private:

    static INT_PTR CALLBACK AddProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnAdd();

    static INT_PTR CALLBACK RemoveProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnRemove();

    void OnDiff();

    static INT_PTR CALLBACK CheckInProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnCheckIn();
    
    static INT_PTR CALLBACK CheckOutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnCheckOut();
    
    static INT_PTR CALLBACK SyncProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSync();

    static INT_PTR CALLBACK RevertProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnRevert();

    static INT_PTR CALLBACK RenameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnRename();

    void StartProgress();
    void SignalProgressFinished();
    static void CDECL ProgressThreadProc(PVOID pParam);
    HANDLE hEventCompleted;
    static INT_PTR CALLBACK ProgressProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    CCString Folder;
    CCString* pFiles;
    int nCount;
    IDataObject* pDataObj;
    HWND hWnd;

    BOOL bRename;
    BOOL bAdd;
    BOOL bDiff;
    BOOL bCheckIn;
    BOOL bCheckOut;
    BOOL bSync;
    BOOL bRevert;
    BOOL bRemove;

    HBITMAP hContextBmp;

    DWORD dwRefCount;

    IAutoComplete* pAC;
};


