# Microsoft Developer Studio Project File - Name="MTAudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MTAudio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MTAudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MTAudio.mak" CFG="MTAudio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MTAudio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MTAudio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MTAudio"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MTAudio - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTAUDIO_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTAUDIO_EXPORTS" /D "MTXAUDIO_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 winmm.lib /nologo /base:"0x14000000" /dll /debug /machine:I386 /out:"../../release/Extensions/MTAudio.mtx"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MTAudio - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTAUDIO_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTAUDIO_EXPORTS" /D "MTXAUDIO_INCLUDED" /D "MTSYSTEM_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib version.lib /nologo /base:"0x14000000" /dll /debug /machine:I386 /out:"../../debug/Extensions/MTAudio.mtx" /pdbtype:sept
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "MTAudio - Win32 Release"
# Name "MTAudio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "Drivers Source"

# PROP Default_Filter "cpp;h"
# Begin Group "MME"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTWaveOut.cpp
# End Source File
# Begin Source File

SOURCE=.\MTWaveOut.h
# End Source File
# End Group
# Begin Group "DirectX"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=D:\dev\Dx7\include\dsound.h
# End Source File
# Begin Source File

SOURCE=.\MTDirectSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDirectSound.h
# End Source File
# End Group
# Begin Group "ASIO"

# PROP Default_Filter "cpp;h"
# End Group
# Begin Group "DevDSP"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTDevDSP.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDevDSP.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTAudioDevice.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\MTAudio.def
# End Source File
# Begin Source File

SOURCE=.\MTAudio1.cpp
# End Source File
# Begin Source File

SOURCE=.\MTAudio1.h
# End Source File
# Begin Source File

SOURCE=.\MTAudio2.cpp
# End Source File
# Begin Source File

SOURCE=.\MTAudio2.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXAudio.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
