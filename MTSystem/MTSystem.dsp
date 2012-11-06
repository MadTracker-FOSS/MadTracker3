# Microsoft Developer Studio Project File - Name="MTSystem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MTSystem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MTSystem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MTSystem.mak" CFG="MTSystem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MTSystem - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MTSystem - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MTSystem"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MTSystem - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTSYSTEM_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Headers\\" /I "..\opt\include\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTXSYSTEM_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib shell32.lib winmm.lib libxml2.lib /nologo /base:"0x10000000" /dll /debug /machine:I386 /out:"../../release/Extensions/MTSystem.mtx" /libpath:"..\opt\lib\\"

!ELSEIF  "$(CFG)" == "MTSystem - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MTSystem___Win32_Debug"
# PROP BASE Intermediate_Dir "MTSystem___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTSYSTEM_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /Oy /I "..\Headers\\" /I "..\opt\include\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTXSYSTEM_INCLUDED" /D "MTSYSTEM_EXPORTS" /D "MTSO_ALL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib shell32.lib winmm.lib libxml2.lib /nologo /base:"0x10000000" /dll /debug /machine:I386 /out:"../../debug/Extensions/MTSystem.mtx" /pdbtype:sept /libpath:"..\opt\lib\\"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "MTSystem - Win32 Release"
# Name "MTSystem - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "Standards"

# PROP Default_Filter "c;cpp;h"
# Begin Source File

SOURCE=.\MTBase64.cpp
# End Source File
# Begin Source File

SOURCE=.\MTBase64.h
# End Source File
# Begin Source File

SOURCE=.\MTInternet.cpp
# End Source File
# Begin Source File

SOURCE=.\MTInternet.h
# End Source File
# Begin Source File

SOURCE=.\MTMD5.cpp
# End Source File
# Begin Source File

SOURCE=.\MTMD5.h
# End Source File
# Begin Source File

SOURCE=.\MTSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSocket.h
# End Source File
# End Group
# Begin Group "Stream"

# PROP Default_Filter "c;cpp;h"
# Begin Source File

SOURCE=.\MTFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MTFile.h
# End Source File
# Begin Source File

SOURCE=.\MTHTTPFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MTHTTPFile.h
# End Source File
# Begin Source File

SOURCE=.\MTLocalFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MTLocalFile.h
# End Source File
# Begin Source File

SOURCE=.\MTMemoryFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MTMemoryFile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\MTConfig.h
# End Source File
# Begin Source File

SOURCE=.\MTKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\MTKernel.h
# End Source File
# Begin Source File

SOURCE=.\MTMiniConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\MTMiniConfig.h
# End Source File
# Begin Source File

SOURCE=.\MTResources.cpp
# End Source File
# Begin Source File

SOURCE=.\MTResources.h
# End Source File
# Begin Source File

SOURCE=.\MTStructures.cpp
# End Source File
# Begin Source File

SOURCE=.\MTStructures.h
# End Source File
# Begin Source File

SOURCE=.\MTSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSystem.def
# End Source File
# Begin Source File

SOURCE=.\MTSystem1.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSystem1.h
# End Source File
# Begin Source File

SOURCE=..\..\debug\Interface\MTSystemRES.h
# End Source File
# Begin Source File

SOURCE=.\MTXML.cpp
# End Source File
# Begin Source File

SOURCE=.\MTXML.h
# End Source File
# Begin Source File

SOURCE=.\strnatcmp.cpp
# End Source File
# Begin Source File

SOURCE=.\strnatcmp.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXSystem.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXSystem2.cpp

!IF  "$(CFG)" == "MTSystem - Win32 Release"

!ELSEIF  "$(CFG)" == "MTSystem - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Headers\MTXSystem2.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
