# Microsoft Developer Studio Project File - Name="tetris" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tetris - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tetris.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tetris.mak" CFG="tetris - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tetris - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "tetris - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tetris - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /W3 /GR- /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Z<none> /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "tetris - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tetris - Win32 Release"
# Name "tetris - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\animation.cpp

!IF  "$(CFG)" == "tetris - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "tetris - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\commandbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\credits.cpp
# End Source File
# Begin Source File

SOURCE=.\ddrawlib.cpp
# End Source File
# Begin Source File

SOURCE=.\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dinputlib.cpp
# End Source File
# Begin Source File

SOURCE=.\displayhighscores.cpp
# End Source File
# Begin Source File

SOURCE=.\dsoundlib.cpp
# End Source File
# Begin Source File

SOURCE=.\dsstream.cpp
# End Source File
# Begin Source File

SOURCE=.\dswavlib.cpp
# End Source File
# Begin Source File

SOURCE=.\etetriz.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\gameover.cpp
# End Source File
# Begin Source File

SOURCE=.\highscores.cpp
# End Source File
# Begin Source File

SOURCE=.\onoff.cpp
# End Source File
# Begin Source File

SOURCE=.\options.cpp
# End Source File
# Begin Source File

SOURCE=.\pause.cpp
# End Source File
# Begin Source File

SOURCE=.\playfield.cpp
# End Source File
# Begin Source File

SOURCE=.\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\slider.cpp
# End Source File
# Begin Source File

SOURCE=.\texteditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\textfilelib.cpp
# End Source File
# Begin Source File

SOURCE=.\textlib.cpp
# End Source File
# Begin Source File

SOURCE=.\windowlibrary.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\commandbutton.h
# End Source File
# Begin Source File

SOURCE=.\credits.h
# End Source File
# Begin Source File

SOURCE=.\ddrawlib.h
# End Source File
# Begin Source File

SOURCE=.\ddutil.h
# End Source File
# Begin Source File

SOURCE=.\dinputlib.h
# End Source File
# Begin Source File

SOURCE=.\displayhighscores.h
# End Source File
# Begin Source File

SOURCE=.\dsoundlib.h
# End Source File
# Begin Source File

SOURCE=.\dsstream.h
# End Source File
# Begin Source File

SOURCE=.\dswavlib.h
# End Source File
# Begin Source File

SOURCE=.\etetriz.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\gameover.h
# End Source File
# Begin Source File

SOURCE=.\highscores.h
# End Source File
# Begin Source File

SOURCE=.\onoff.h
# End Source File
# Begin Source File

SOURCE=.\options.h
# End Source File
# Begin Source File

SOURCE=.\passvars.h
# End Source File
# Begin Source File

SOURCE=.\pause.h
# End Source File
# Begin Source File

SOURCE=.\playfield.h
# End Source File
# Begin Source File

SOURCE=.\settings.h
# End Source File
# Begin Source File

SOURCE=.\slider.h
# End Source File
# Begin Source File

SOURCE=.\texteditbox.h
# End Source File
# Begin Source File

SOURCE=.\textfilelib.h
# End Source File
# Begin Source File

SOURCE=.\textlib.h
# End Source File
# Begin Source File

SOURCE=.\windowlibrary.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\tetris.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Icon2.ico
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\ddraw.lib
# End Source File
# Begin Source File

SOURCE=.\dinput.lib
# End Source File
# Begin Source File

SOURCE=.\dsound.lib
# End Source File
# Begin Source File

SOURCE=.\dxguid.lib
# End Source File
# Begin Source File

SOURCE="D:\Program Files\Microsoft Visual Studio\Vc98\Lib\Winmm.lib"
# End Source File
# End Target
# End Project
