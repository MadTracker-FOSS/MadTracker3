# Microsoft Developer Studio Project File - Name="MTGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MTGUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MTGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MTGUI.mak" CFG="MTGUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MTGUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MTGUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MTGUI"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MTGUI - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTGUI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTGUI_EXPORTS" /D "MTXGUI_INCLUDED" /D "MTXCONTROLS_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib gdi32.lib winmm.lib /nologo /base:"0x12000000" /dll /debug /machine:I386 /out:"../../release/Extensions/MTGUI.mtx"
# SUBTRACT LINK32 /verbose /pdb:none /force

!ELSEIF  "$(CFG)" == "MTGUI - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTGUI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /Op /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTGUI_EXPORTS" /D "MTXGUI_INCLUDED" /D "MTXCONTROLS_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib gdi32.lib winmm.lib /nologo /base:"0x12000000" /dll /debug /debugtype:cv /machine:I386 /out:"../../debug/Extensions/MTGUI.mtx" /pdbtype:sept
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "MTGUI - Win32 Release"
# Name "MTGUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Controls"

# PROP Default_Filter "cpp;h"
# Begin Group "Windowed"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTDesktop.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDesktop.h
# End Source File
# Begin Source File

SOURCE=.\MTPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\MTPanel.h
# End Source File
# Begin Source File

SOURCE=.\MTTabControl.cpp
# End Source File
# Begin Source File

SOURCE=.\MTTabControl.h
# End Source File
# Begin Source File

SOURCE=.\MTToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MTToolBar.h
# End Source File
# Begin Source File

SOURCE=.\MTVisual.cpp
# End Source File
# Begin Source File

SOURCE=.\MTVisual.h
# End Source File
# Begin Source File

SOURCE=.\MTWinControl.cpp
# End Source File
# Begin Source File

SOURCE=.\MTWinControl.h
# End Source File
# Begin Source File

SOURCE=.\MTWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MTWindow.h
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTButton.cpp
# End Source File
# Begin Source File

SOURCE=.\MTButton.h
# End Source File
# Begin Source File

SOURCE=.\MTCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MTCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\MTEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MTEdit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTControl.cpp
# End Source File
# Begin Source File

SOURCE=.\MTControl.h
# End Source File
# Begin Source File

SOURCE=.\MTCustControls.cpp
# End Source File
# Begin Source File

SOURCE=.\MTCustControls.h
# End Source File
# Begin Source File

SOURCE=.\MTItems.cpp
# End Source File
# Begin Source File

SOURCE=.\MTItems.h
# End Source File
# Begin Source File

SOURCE=.\MTLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\MTLabel.h
# End Source File
# Begin Source File

SOURCE=.\MTOSWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MTOSWindow.h
# End Source File
# Begin Source File

SOURCE=.\MTProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\MTProgress.h
# End Source File
# Begin Source File

SOURCE=.\MTSign.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSign.h
# End Source File
# Begin Source File

SOURCE=.\MTSysControls.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSysControls.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGUI.def
# End Source File
# Begin Source File

SOURCE=.\MTGUI1.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGUI1.h
# End Source File
# Begin Source File

SOURCE=.\MTGUITools.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGUITools.h
# End Source File
# Begin Source File

SOURCE=.\MTOSSkin.cpp
# End Source File
# Begin Source File

SOURCE=.\MTOSSkin.h
# End Source File
# Begin Source File

SOURCE=.\MTSkin.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSkin.h
# End Source File
# Begin Source File

SOURCE=..\MTSystem\strnatcmp.cpp
# End Source File
# Begin Source File

SOURCE=..\MTSystem\strnatcmp.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXControls.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXGUI.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXGUITypes.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXInput.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXSkin.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXWrapper.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Font.txt
# End Source File
# Begin Source File

SOURCE=.\Messages.txt
# End Source File
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
