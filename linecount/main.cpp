// ANSI ONLY!

#include <windows.h>
#include "aggressiveoptimize.h"
#include "pathmatchspec.h"
#include <iostream>


//#define TEST_P4


DWORD GetLineCount(const std::string& s);

// lcount.exe *.*
int main(int argc, char* argv[])
{
    argc--;
    argv++;

#ifndef TEST_P4

    if(argc != 1)
    {
			std::cout << "Usage: xxxx dir" << std::endl << "Dir can be a pattern like \"*.cpp;*.hpp\" or \"*.?pp\", or even a Perforce-style pattern, like \\source\\...\\*.?pp" << std::endl;
    }
    else
    {
        CP4PathMatchSpec pms;
        CFileIteratorBase* pi = 0;
        CFileIterator fi;
        CRecursiveFileIterator rfi;
        long lFileCount = 0;
        DWORD dwLineCountTotal = 0;
        DWORD dwLineCount = 0;

        //std::cout << "Searching for '" << argv[0] << "'" << std::endl;

        std::string s;
        std::string sRoot;

        pms.SetCriteria(argv[0]);
        pms.GetRootDir(sRoot);

        //std::cout << "Scanning directory: '" << sRoot << "'" << std::endl;
        //pms.DumpCriteria();

        if(pms.SpansSubDirectories())
        {
            pi = &rfi;
        }
        else
        {
            pi = &fi;
        }

        DWORD dwAttr;
        pi->Reset(sRoot);
        while(pi->Next(s, &dwAttr))
        {
            if(pms.Match(s))
            {
                lFileCount++;
                dwLineCount = GetLineCount(s);
                std::cout << s << " (" << dwLineCount << ")" << std::endl;
                dwLineCountTotal += dwLineCount;
            }
        }

        std::cout << std::endl << lFileCount << " file(s) found." << std::endl;
        std::cout << dwLineCountTotal << " line(s) found." << std::endl;
    }

#else

    if(argc != 1)
    {
        std::cout << "Usage: lcount.exe [path-spec]" << std::endl;
        std::cout << "    The path-spec can have p4-style specs (supporting '...' to include " << std::endl;
        std::cout << "    files in one or more levels of directories." << std::endl;
    }
    else
    {
    }

    CP4PathMatchSpec pms;

    std::cout << "Enter x to quit." << std::endl;
    std::cout << "Enter c to be prompted for new criteria." << std::endl;

    while(1)
    {
        std::string sCmdLine;

        std::getline(std::cin, sCmdLine);
        if(sCmdLine == "x") break;
        if(sCmdLine == "q") break;
        if(sCmdLine == "Q") break;
        if(sCmdLine == "X") break;

        if(sCmdLine == "c")
        {
            std::cout << "Enter the search criteria, or 'x' to cancel." << std::endl;
            std::getline(std::cin, sCmdLine);
            if(sCmdLine != "x")
            {
                pms.SetCriteria(sCmdLine);
                pms.DumpCriteria();
                std::cout << (pms.SpansSubDirectories() ? "Spans sub-directories" : "Restricted to one directory") << std::endl;
            }
        }
        else
        {
            std::cout << (pms.Match(sCmdLine) ? "True" : "False") << std::endl;
        }
    }
#endif

    return 0;
}


DWORD GetLineCount(const std::string& s)
{
    // open the file and just count \r\n occurences.
    DWORD r = 0;
    DWORD dwFileSize = 0;
    HANDLE hFile = CreateFile(s.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(P4_IS_VALID_HANDLE(hFile))
    {
        dwFileSize = GetFileSize(hFile, 0);
        DWORD dwBufSize = 0;
        // allocate a buffer for it.
        char* pBuf = new char[dwFileSize+2];
        if(pBuf)
        {
            ReadFile(hFile, pBuf, dwFileSize, &dwBufSize, 0);
            CloseHandle(hFile);

            // scan...
            DWORD pos = 0;
            for(pos=0;pos<dwBufSize;pos++)
            {
                if(pBuf[pos] == '\n') r ++;
            }

            delete [] pBuf;
        }
    }

    return r;
}


