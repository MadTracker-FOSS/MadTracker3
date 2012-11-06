# Microsoft Developer Studio Project File - Name="MTObjects" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MTObjects - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MTObjects.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MTObjects.mak" CFG="MTObjects - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MTObjects - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MTObjects - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "MTObjects"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MTObjects - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTOBJECTS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_WINDOWS" /D "MTOBJECTS_EXPORTS" /D "MTXOBJECTS_INCLUDED" /D "MTSYSTEM_EXPORTS" /D "MTVERSION_COMMERCIAL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib gdi32.lib user32.lib /nologo /base:"0x11000000" /dll /debug /machine:I386 /out:"../../release/Extensions/MTObjects.mtx"

!ELSEIF  "$(CFG)" == "MTObjects - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MTOBJECTS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "..\Headers\\" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_WINDOWS" /D "MTOBJECTS_EXPORTS" /D "MTXOBJECTS_INCLUDED" /D "MTSYSTEM_EXPORTS" /D "MTVERSION_COMMERCIAL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib gdi32.lib user32.lib /nologo /base:"0x11000000" /dll /debug /machine:I386 /out:"../../debug/Extensions/MTObjects.mtx" /pdbtype:sept
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "MTObjects - Win32 Release"
# Name "MTObjects - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "Objects"

# PROP Default_Filter "cpp;h"
# Begin Group "Built-in"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTColumns.cpp
# End Source File
# Begin Source File

SOURCE=.\MTColumns.h
# End Source File
# Begin Source File

SOURCE=.\MTGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGenerator.h
# End Source File
# Begin Source File

SOURCE=.\MTInstrument.cpp
# End Source File
# Begin Source File

SOURCE=.\MTInstrument.h
# End Source File
# Begin Source File

SOURCE=.\MTOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\MTOscillator.h
# End Source File
# Begin Source File

SOURCE=.\MTPattern.cpp
# End Source File
# Begin Source File

SOURCE=.\MTPattern.h
# End Source File
# Begin Source File

SOURCE=.\MTXMCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\MTXMCommands.h
# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTDelay.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDelay.h
# End Source File
# Begin Source File

SOURCE=.\MTFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\MTFilter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTAutomation.cpp
# End Source File
# Begin Source File

SOURCE=.\MTAutomation.h
# End Source File
# Begin Source File

SOURCE=.\MTEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\MTEffect.h
# End Source File
# Begin Source File

SOURCE=.\MTModule.cpp
# End Source File
# Begin Source File

SOURCE=.\MTModule.h
# End Source File
# Begin Source File

SOURCE=.\MTNode.h
# End Source File
# Begin Source File

SOURCE=.\MTObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MTObject.h
# End Source File
# Begin Source File

SOURCE=.\MTTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\MTTrack.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\..\debug\Interface\MT3RES.h
# End Source File
# Begin Source File

SOURCE=.\MTGraphs.cpp
# End Source File
# Begin Source File

SOURCE=.\MTGraphs.h
# End Source File
# Begin Source File

SOURCE=..\MTGUI\MTGUITools.cpp
# End Source File
# Begin Source File

SOURCE=..\MTGUI\MTGUITools.h
# End Source File
# Begin Source File

SOURCE=.\MTInstrEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\MTInstrEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\debug\Interface\MTObjects.mtr
# End Source File
# Begin Source File

SOURCE=..\..\debug\Interface\MTObjectsRES.h
# End Source File
# Begin Source File

SOURCE=.\MTPattEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\MTPattEditor.h
# End Source File
# Begin Source File

SOURCE=.\MTPattManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MTPattManager.h
# End Source File
# Begin Source File

SOURCE=.\MTSeqManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MTSeqManager.h
# End Source File
# End Group
# Begin Group "I/O"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\MTIOInstrument.cpp
# End Source File
# Begin Source File

SOURCE=.\MTIOInstrument.h
# End Source File
# Begin Source File

SOURCE=.\MTIOModule_MT2.cpp
# End Source File
# Begin Source File

SOURCE=.\MTIOModule_MT2.h
# End Source File
# Begin Source File

SOURCE=.\MTIOOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\MTIOOscillator.h
# End Source File
# Begin Source File

SOURCE=.\MTRiff.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MTObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\MTObjects.def
# End Source File
# Begin Source File

SOURCE=.\MTObjects1.cpp
# End Source File
# Begin Source File

SOURCE=.\MTObjects1.h
# End Source File
# Begin Source File

SOURCE=.\MTObjects2.cpp
# End Source File
# Begin Source File

SOURCE=.\MTObjects2.h
# End Source File
# Begin Source File

SOURCE=.\MTObjectsASM.cpp
# End Source File
# Begin Source File

SOURCE=.\MTObjectsASM.h
# End Source File
# Begin Source File

SOURCE=.\MTTest.cpp

!IF  "$(CFG)" == "MTObjects - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MTObjects - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MTTest.h
# End Source File
# End Group
# Begin Group "Extension Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\Headers\MTXAutomation.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXEffect.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXEffect.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXInstrument.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXModule.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXNode.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXObject.h
# End Source File
# Begin Source File

SOURCE=..\Headers\MTXObjects.h
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

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project
