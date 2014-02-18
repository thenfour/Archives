// GuidGen.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GuidGen.h"
#include "guid.h"
#include "resource.h"
#include <libcc\blob.h>

HWND g_hDlg;
bool entered;

extern DWORD chksum_crc32(unsigned char *block, unsigned int length);


std::string GetWindowTextX(HWND h, int idc = -1)
{
  HWND hwnd = h;
  if(idc != -1)
  {
    hwnd = GetDlgItem(h, idc);
  }
  if(!IsWindow(hwnd)) return "";
  int size = GetWindowTextLength(hwnd);
  LibCC::Blob<char> buffer;
  buffer.Alloc(size+2);
  GetWindowText(hwnd, buffer.GetBuffer(), size + 1);
  return buffer.GetBuffer();
}

void RefreshOutput()
{
  std::string guid = GetWindowTextX(g_hDlg, IDC_GUID);
  std::string iname = GetWindowTextX(g_hDlg, IDC_INTERFACE);
  std::string format = GetWindowTextX(g_hDlg, IDC_FORMAT);
  std::string output;

  Guid guidObj;
  guidObj.Assign(guid);

  for(std::string::iterator it = format.begin(); it != format.end(); ++ it)
  {
    char c = *it;
    if(c == '%')
    {
      // advance
      ++ it;
      if(it == format.end())
      {
        // crap, end of format string?  ok, just append the % and exit.
        output.push_back('%');
        break;
      }
      char c = *it;
      switch(c)
      {
      case 'a':
        output.append(LibCC::Format().ul<16,8>(guidObj.val.Data1).Str());
        break;
      case 'b':
        output.append(LibCC::Format().ul<16,4>(guidObj.val.Data2).Str());
        break;
      case 'c':
        output.append(LibCC::Format().ul<16,4>(guidObj.val.Data3).Str());
        break;
      case 'd':
        output.append(LibCC::Format()
          .ul<16,2>(guidObj.val.Data4[0])
          .ul<16,2>(guidObj.val.Data4[1])
          .Str());
        break;
      case 'e':
        output.append(LibCC::Format()
          .ul<16,2>(guidObj.val.Data4[0])
          .ul<16,2>(guidObj.val.Data4[1])
          .ul<16,2>(guidObj.val.Data4[2])
          .ul<16,2>(guidObj.val.Data4[3])
          .ul<16,2>(guidObj.val.Data4[4])
          .ul<16,2>(guidObj.val.Data4[5])
          .Str());
      case 'f':
        // 0x58, 0x85, ....
        output.append(LibCC::Format("0x%, 0x%, 0x%, 0x%, 0x%, 0x%, 0x%, 0x%")
          .ul<16,2>(guidObj.val.Data4[0])
          .ul<16,2>(guidObj.val.Data4[1])
          .ul<16,2>(guidObj.val.Data4[2])
          .ul<16,2>(guidObj.val.Data4[3])
          .ul<16,2>(guidObj.val.Data4[4])
          .ul<16,2>(guidObj.val.Data4[5])
          .ul<16,2>(guidObj.val.Data4[6])
          .ul<16,2>(guidObj.val.Data4[7])
          .Str());
        break;
      case 'g':
        output.append(guidObj.ToString());
        break;
      case 'i':
        output.append(iname);
        break;
      default:
        output.push_back('%');
        output.push_back(c);
      }
    }
    else
    {
      output.push_back(c);
    }
  }

  SetDlgItemText(g_hDlg, IDC_OUTPUT, output.c_str());
}

void OnGenerate()
{
  Guid g;
  g.CreateNew();
	std::string sg = g.ToString();
  SetDlgItemText(g_hDlg, IDC_GUID, sg.c_str());

	DWORD dw = chksum_crc32((BYTE*)sg.c_str(), (unsigned int)sg.length());
	SetDlgItemText(g_hDlg, IDC_CRC32, LibCC::Format("0x%").ul<16>(dw).CStr());

  RefreshOutput();
}

void OnCopy(const std::string& s)
{
  HANDLE hMem;
  size_t nSize;
  PVOID hMemLoc;

  if(OpenClipboard(0) == FALSE) return;
  EmptyClipboard();

  nSize = (s.length() + 1) * sizeof(WCHAR);
  hMem = GlobalAlloc(GMEM_MOVEABLE, nSize);
  hMemLoc = GlobalLock(hMem);
  LibCC::StringCopyN((WCHAR*)hMemLoc, s.c_str(), s.length() + 1);
  GlobalUnlock(hMem);
  SetClipboardData(CF_UNICODETEXT, hMem);

  nSize = s.length() + 1;
  hMem = GlobalAlloc(GMEM_MOVEABLE, nSize);
  hMemLoc = GlobalLock(hMem);
  strcpy((char*)hMemLoc, s.c_str());
  GlobalUnlock(hMem);
  SetClipboardData(CF_TEXT, hMem);

  CloseClipboard();
}


INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
  case WM_CLOSE:
    {
      EndDialog(hWnd, 0);
      return TRUE;
    }
  case WM_INITDIALOG:
    g_hDlg = hWnd;
    return TRUE;
  case WM_COMMAND:
    {
      USHORT code = HIWORD(wParam);
      USHORT id = LOWORD(wParam);
      if(code == EN_UPDATE)
      {
        int start, end;
        SendMessage((HWND)lParam, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        OutputDebugString(LibCC::Format("EN_UPDATE(%)(%,%)|").ul<16,8>(lParam).i(start).i(end).CStr());
        return TRUE;
      }
      if(code == EN_SETFOCUS)
      {
        // select all
        SendMessage((HWND)lParam, EM_SETSEL, 0, -1);
        OutputDebugString(LibCC::Format("EN_SETFOCUS(%)|").ul<16,8>(lParam).CStr());
        return TRUE;
      }
      if(code == EN_CHANGE)
      {
        if(!entered)
        {
          entered = true;
          RefreshOutput();
          entered = false;
        }
        return TRUE;
      }
      if(code == BN_CLICKED)
      {
        if(id == IDC_COPY)
        {
          std::string s = GetWindowTextX(hWnd, IDC_OUTPUT);
          OnCopy(s);
          return TRUE;
        }
        
        if(id == IDC_FORMAT_BARE)
        {
          SetDlgItemText(hWnd, IDC_FORMAT, "%g");
          RefreshOutput();
          return TRUE;
        }
        if(id == IDC_FORMAT_REGISTRY)
        {
          SetDlgItemText(hWnd, IDC_FORMAT, "{%g}");
          RefreshOutput();
          return TRUE;
        }
        if(id == IDC_FORMAT_DEFINE_GUID)
        {
          SetDlgItemText(hWnd, IDC_FORMAT, "// {%g}\r\nDEFINE_GUID(%i, \r\n0x%a, 0x%b, 0x%c, %f);");
          RefreshOutput();
          return TRUE;
        }
        if(id == IDC_FORMAT_GUID)
        {
          SetDlgItemText(hWnd, IDC_FORMAT, "// {%g}\r\nstatic const IID %i =\r\n{ 0x%a, 0x%b, 0x%c, { %f } };");
          RefreshOutput();
          return TRUE;
        }
        if(id == IDC_FORMAT_OLECREATE)
        {
          SetDlgItemText(hWnd, IDC_FORMAT, "// {%g}\r\nIMPLEMENT_OLECREATE(%i, \"%i\", \r\n0x%a, 0x%b, 0x%c, %f);");
          RefreshOutput();
          return TRUE;
        }
        if(id == IDCANCEL)
        {
          EndDialog(hWnd, 0);
          return TRUE;
        }
        if(id == IDC_GENERATE)
        {
          OnGenerate();
          return TRUE;
        }
      }
      return FALSE;
    }
  }
	return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  entered = false;
  return (int)DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);
}
