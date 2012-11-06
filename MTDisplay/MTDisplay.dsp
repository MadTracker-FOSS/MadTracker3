# Microsoft Developer Studio Project File - Name="MTDisplay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MTDisplay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MTDisplay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MTDisplay.mak" CFG="MTDisplay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MTDisplay - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MTDisplay - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MTDisplay"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MTDisplay - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTDISPLAY_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "D:\dev\DX7\include" /I "..\Headers\\" /I "..\opt\include\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTDISPLAY_EXPORTS" /D "MTXDISPLAY_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib gdi32.lib zlib.lib libpng.lib libjpeg.lib /nologo /base:"0x13000000" /dll /debug /machine:I386 /nodefaultlib:"libcmt.lib" /out:"../../release/Extensions/MTDisplay.mtx" /libpath:"..\opt\lib\\"
# SUBTRACT LINK32 /profile /pdb:none

!ELSEIF  "$(CFG)" == "MTDisplay - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTDISPLAY_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "D:\dev\DX7\include" /I "..\Headers\\" /I "..\opt\include\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTDISPLAY_EXPORTS" /D "MTXDISPLAY_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /GZ /c
# SUBTRACT CPP /X /u
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib user32.lib gdi32.lib zlib.lib libpng.lib libjpeg.lib /nologo /base:"0x13000000" /dll /map /debug /debugtype:cv /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"../../debug/Extensions/MTDisplay.mtx" /pdbtype:sept /libpath:"..\opt\lib\\"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "MTDisplay - Win32 Release"
# Name "MTDisplay - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "Devices"

# PROP Default_Filter "cpp;h"
# Begin Group "GDI"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTGDIBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGDIBitmap.h
# End Source File
# Begin Source File

SOURCE=.\MTGDIDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGDIDevice.h
# End Source File
# End Group
# Begin Group "DirectX"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE="D:\Microsoft Visual Studio\VC98\Include\D3D.H"
# End Source File
# Begin Source File

SOURCE="D:\Microsoft Visual Studio\VC98\Include\DDRAW.H"
# End Source File
# Begin Source File

SOURCE=.\MTDXBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDXBitmap.h
# End Source File
# Begin Source File

SOURCE=.\MTDXDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDXDevice.h
# End Source File
# End Group
# Begin Group "OpenGL"

# PROP Default_Filter "cpp;h"
# End Group
# Begin Source File

SOURCE=.\MTBitmap.h
# End Source File
# Begin Source File

SOURCE=.\MTDisplayDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDisplayDevice.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\opt\include\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\MTDecode.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDecode.h
# End Source File
# Begin Source File

SOURCE=.\MTDisplay.cpp

!IF  "$(CFG)" == "MTDisplay - Win32 Release"

# ADD CPP /Gd

!ELSEIF  "$(CFG)" == "MTDisplay - Win32 Debug"

# ADD CPP /Gd /ZI

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MTDisplay.def
# End Source File
# Begin Source File

SOURCE=.\MTDisplay1.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDisplay1.h
# End Source File
# Begin Source File

SOURCE=.\MTDisplayASM.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDisplayASM.h
# End Source File
# Begin Source File

SOURCE=.\MTDisplayMMX.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDisplayMMX.h
# End Source File
# Begin Source File

SOURCE=..\MTGUI\MTGUITools.cpp
# End Source File
# Begin Source File

SOURCE=..\MTGUI\MTGUITools.h
# End Source File
# Begin Source File

SOURCE=..\opt\include\png.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXDisplay.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
