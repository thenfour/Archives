# Microsoft Developer Studio Project File - Name="Snookie" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Snookie - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Snookie.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Snookie.mak" CFG="Snookie - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Snookie - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Snookie - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Snookie - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"pch" /FD /GZ /c
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

!ELSEIF  "$(CFG)" == "Snookie - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Snookie___Win32_Release"
# PROP BASE Intermediate_Dir "Snookie___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Snookie___Win32_Release"
# PROP Intermediate_Dir "Snookie___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"pch" /FD /GZ /c
# ADD CPP /nologo /Gr /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /YX"pch" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "Snookie - Win32 Debug"
# Name "Snookie - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "cControls"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\editbox.cpp
# End Source File
# Begin Source File

SOURCE=.\imagelist.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\slider.cpp
# End Source File
# Begin Source File

SOURCE=.\static.cpp
# End Source File
# Begin Source File

SOURCE=.\statusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\treeview.cpp
# End Source File
# End Group
# Begin Group "cMain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\findfileproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Snookie.cpp
# End Source File
# Begin Source File

SOURCE=.\SnookieWindowProc.cpp
# End Source File
# Begin Source File

SOURCE=.\snookiewindows.cpp
# End Source File
# End Group
# Begin Group "cCatalog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\catfile.cpp
# End Source File
# Begin Source File

SOURCE=.\snookiecatalogv3.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "hControls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\controls.h
# End Source File
# Begin Source File

SOURCE=.\editbox.h
# End Source File
# Begin Source File

SOURCE=.\imagelist.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\slider.h
# End Source File
# Begin Source File

SOURCE=.\static.h
# End Source File
# Begin Source File

SOURCE=.\statusbar.h
# End Source File
# Begin Source File

SOURCE=.\treeview.h
# End Source File
# End Group
# Begin Group "hMain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debugwindow.h
# End Source File
# Begin Source File

SOURCE=.\Findfileproc.h
# End Source File
# Begin Source File

SOURCE=.\snookie.h
# End Source File
# Begin Source File

SOURCE=.\snookievars.h
# End Source File
# Begin Source File

SOURCE=.\SnookieWindowProc.h
# End Source File
# Begin Source File

SOURCE=.\snookiewindows.h
# End Source File
# End Group
# Begin Group "hCatalog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\catfile.h
# End Source File
# Begin Source File

SOURCE=.\snookiecatalogv3.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource2.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Catalog.bmp
# End Source File
# Begin Source File

SOURCE=.\filenorm.bmp
# End Source File
# Begin Source File

SOURCE=.\foldernorm.bmp
# End Source File
# Begin Source File

SOURCE=.\foldersel.bmp
# End Source File
# Begin Source File

SOURCE=.\snookie.ico
# End Source File
# Begin Source File

SOURCE=.\snookie.rc
# End Source File
# End Group
# Begin Group "V2 Files (Not used)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\currentcat.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\currentcat.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\snookiecatalog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\snookiecatalog.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\Comctl32.lib
# End Source File
# End Target
# End Project
