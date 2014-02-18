//#include <sys/types.h>
//#include <sys/socket.h>
#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")
//#include <sys/time.h>
//#include <time.h>
//#include <netinet/in.h>
#include <io.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <strings.h>
#include <windows.h>
#include <windowsx.h>
#include "service.h"

BOOL LogEvent(LPCTSTR, WORD ,DWORD,PSID,LPCTSTR);

char *getUname();
char *getUser();
char *getUsers();
char *getLogin();

// fits in a byte
#define MAXBUFF 255


inline size_t xcslen(const char* sz)
{
  const char* sz2 = sz;
  while(*sz2) sz2 ++;
  return sz2 - sz;
}
//inline size_t xcsicmp(const char* sz)
//{
//}
//inline size_t memset(const char* sz)
//{
//}
//
//

void xprintf(const char* sz);
void xprintf_gle(const char* sz);
//
//template<typename T>
//void xprintf(const char* sz, const T& r)
//{
//  char buf[MAXBUFF];
//  wsprintf(buf, sz, r);
//  xprintf(buf);
//}
//
//template<typename T, typename T2>
//void xprintf(const char* sz, const T& a, const T2& b)
//{
//  char buf[MAXBUFF];
//  wsprintf(buf, sz, a, b);
//  xprintf(buf);
//}
//
//
//

