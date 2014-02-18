#include "identd.h"

int stop=0;

void xprintf(const char* sz)
{
  DWORD dw;
  WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), sz, xcslen(sz), &dw, 0);
}
void xprintf_gle(const char* sz)
{
  xprintf(sz);
  xprintf(SZSERVICEDISPLAYNAME);

  DWORD dwRet;
  LPTSTR lpszTemp = NULL;
  dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
    NULL,
    GetLastError(),
    LANG_NEUTRAL,
    (LPTSTR)&lpszTemp,
    0,
    NULL);

  xprintf(lpszTemp);
  xprintf("\r\n");
  LocalFree((HLOCAL) lpszTemp );
}

VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv)
{
  SOCKET sock, connfd, ready;
  int len;
  sockaddr_in servaddr = {0};
  sockaddr_in cliaddr;
  timeval timeout;
  fd_set mask;
  char buff[MAXBUFF];
  int local_port, remote_port;
  
  WSADATA __wsadata;
  WSAStartup(MAKEWORD(2, 0), &__wsadata);

  /* report the status to the service control manager.*/
  AddToMessageLog(TEXT("starting service"),EVENTLOG_INFORMATION_TYPE);
  ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000);
  
  Sleep(10);
		 
  /* Report running status to SCM */
  ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);
  
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    AddToMessageLog(TEXT("Error open socket"));
    stop=1;
  }
  else
  {
    int one = 1;
    if(SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof (int)))
    {
      AddToMessageLog(TEXT("setsockopt failed."));
    }
    
    //bzero((void*)&servaddr, sizeof(servaddr));
    //ZeroMemory((void*)&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(113);
  
    if(SOCKET_ERROR == bind(sock,(struct sockaddr *) &servaddr,sizeof(servaddr)))
    {
      AddToMessageLog(TEXT("Error bind"));
      stop=1;
    }
    else
    {
      if(SOCKET_ERROR == listen(sock, 32))
      {
	      AddToMessageLog(TEXT("Error listen"));
	      stop=1;
      }
      else
      {
	      len = sizeof(cliaddr);
	      timeout.tv_sec = 1;
	      timeout.tv_usec = 0;
      }
    }
  }

  while(stop==0)
  {
    FD_ZERO(&mask);
    FD_SET(sock, &mask);
    ready=select(0, &mask, NULL, NULL, &timeout);
    if(ready == SOCKET_ERROR)
    {
      AddToMessageLog(TEXT("Error select"));
    }
    else if(ready != 0)
    {
      if((connfd=accept(sock, (struct sockaddr *) &cliaddr, &len)) < 0)
      {
	      AddToMessageLog(TEXT("Error accept"));
      }
      else
      {
	      HKEY keyhandle;
	      LONG result;
	      static char uname[MAXBUFF];
        lstrcpy(uname, "unk");

	      if(ERROR_SUCCESS != (result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Ident", 0, KEY_READ, &keyhandle)))
        {
		      AddToMessageLog("reg");
	      }
        else
        {
          result = MAXBUFF;
          RegQueryValue(keyhandle, 0, uname, &result);
          RegCloseKey(keyhandle);
        }

        wsprintf(buff, "%d , %d : USERID : WIN32 : %s\r\n", local_port, remote_port, uname);

        send(connfd, buff, xcslen(buff), 0);

        closesocket(connfd);
      }
    }
  }

  closesocket(sock);
  
  Sleep(1000);

  AddToMessageLog(TEXT("stopping."),EVENTLOG_INFORMATION_TYPE);
  ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);

  WSACleanup();
}


void ServiceStop()
{
  if(stop==0)
    stop=1;
}


