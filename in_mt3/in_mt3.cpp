//---------------------------------------------------------------------------
//
//	MadTracker 3 Winamp/XMMS Plugin
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: in_mt3.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include "in_mt3.h"

MTXInterfaces i;
MTInterface *mtinterface;
MTSystemInterface *si;
MTDSPInterface *dspi;
MTObjectsInterface *oi;
MTAudioInterface *ai;
MTDisplayInterface *di;
MTGUIInterface *gi;

#include "../MTSystem/MTKernel.cpp"
#include "../MTSystem/MTFile.cpp"
#include "../MTSystem/MTLocalFile.cpp"
#include "../MTSystem/MTMD5.cpp"
#include "../MTSystem/MTStructures.cpp"
#include "../MTSystem/MTMiniConfig.cpp"
#include "../MTSystem/MTSystem1.cpp"

#include "../MTDSP/MTDSP1.cpp"
#include "../MTDSP/MTShaper.cpp"
#include "../MTDSP/MTBufferASM.cpp"
#include "../MTDSP/MTFilterASM.cpp"
#include "../MTDSP/MTResamplingASM.cpp"
#include "../MTDSP/MTCatmullASM.cpp"

#include "../MTObjects/MTObjects1.cpp"
#include "../MTObjects/MTObjects2.cpp"
#include "../MTObjects/MTObjectsASM.cpp"
#include "../MTObjects/MTColumns.cpp"
#include "../MTObjects/MTGenerator.cpp"
#include "../MTObjects/MTInstrument.cpp"
#include "../MTObjects/MTOscillator.cpp"
#include "../MTObjects/MTPattern.cpp"
#include "../MTObjects/MTXMCommands.cpp"
#include "../MTObjects/MTDelay.cpp"
#include "../MTObjects/MTFilter.cpp"
#include "../MTObjects/MTAutomation.cpp"
#include "../MTObjects/MTEffect.cpp"
#include "../MTObjects/MTModule.cpp"
#include "../MTObjects/MTObject.cpp"
#include "../MTObjects/MTTrack.cpp"
#include "../MTObjects/MTIOModule_MT2.cpp"
#include "../MTObjects/MTIOInstrument.cpp"
#include "../MTObjects/MTIOOscillator.cpp"

#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#else
const char *argv0;
char *cmdline = "";
int main(int argc,const char* argv[])
{
	return 0;
}
#endif
//---------------------------------------------------------------------------
