# Microsoft Developer Studio Project File - Name="MT3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MT3 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MT3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MT3.mak" CFG="MT3 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MT3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MT3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MT3"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MT3 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTSYSTEM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../release/MT3.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MT3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MT3___Win32_Debug"
# PROP BASE Intermediate_Dir "MT3___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTSYSTEM_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../debug/MT3.exe" /pdbtype:sept
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "MT3 - Win32 Release"
# Name "MT3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "Interface"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\..\debug\Interface\MT3RES.h
# End Source File
# Begin Source File

SOURCE=.\MTWC_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MTWC_Main.h
# End Source File
# Begin Source File

SOURCE=.\MTWC_Main2.cpp
# End Source File
# Begin Source File

SOURCE=.\MTWC_Main2.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MT3.cpp
# End Source File
# Begin Source File

SOURCE=.\MTConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\MTConsole.h
# End Source File
# Begin Source File

SOURCE=.\MTData.cpp
# End Source File
# Begin Source File

SOURCE=.\MTData.h
# End Source File
# Begin Source File

SOURCE=.\MTExtensions.cpp
# End Source File
# Begin Source File

SOURCE=.\MTExtensions.h
# End Source File
# Begin Source File

SOURCE=.\MTInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\MTInterface.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon_main.ico
# End Source File
# Begin Source File

SOURCE=.\MT3.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Group "Objects"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXAutomation.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXInstrument.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXModule.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXObject.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXOscillator.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXPattern.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXTrack.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Headers\MTXASM.inc

!IF  "$(CFG)" == "MT3 - Win32 Release"

!ELSEIF  "$(CFG)" == "MT3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Headers\MTXAudio.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXControls.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXDisplay.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXDSP.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXExtension.h
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

SOURCE=..\Headers\MTXObjects.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXSkin.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXSystem.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXWrapper.h
# End Source File
# End Group
# Begin Group "Configuration"

# PROP Default_Filter ".conf"
# Begin Group "System"

# PROP Default_Filter "conf"
# Begin Source File

SOURCE=..\..\debug\Conf\System\Global.conf
# End Source File
# Begin Source File

SOURCE=..\..\debug\Conf\System\VST.conf
# End Source File
# End Group
# Begin Group "User"

# PROP Default_Filter "conf"
# Begin Source File

SOURCE="..\..\debug\Conf\Unregistered User\Global.conf"
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\Planning.txt
# End Source File
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
