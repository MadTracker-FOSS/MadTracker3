//---------------------------------------------------------------------------
//
//	MadTracker 3 Winamp/XMMS Plugin
//
//		Platforms:	Win32
//		Processors:	All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: in_mt3.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef IN_MT3_INCLUDED
#define IN_MT3_INCLUDED
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#	include "in2.h"
#else
#	include "plugin.h"
#endif
//---------------------------------------------------------------------------
#define MTBUILTIN

#define MTXSYSTEM_INCLUDED
#define MTXDSP_INCLUDED
#define MTXOBJECTS_INCLUDED
//#define MTXAUDIO_INCLUDED

#ifndef MTSYSTEM_EXPORTS
#	define MTSYSTEM_EXPORTS
#endif
#define MTDSP_EXPORTS
#define MTOBJECTS_EXPORTS
#define MTAUDIO_EXPORTS

#define MT3CONFIG_INCLUDED
#define MTSYSTEM_MINICONFIG
//---------------------------------------------------------------------------
#include "../MTSystem/MTSystem1.h"
#include "../MTDSP/MTDSP1.h"
#include "../MTObjects/MTObjects1.h"
#include "MTXAudio.h"
//---------------------------------------------------------------------------
extern WaveDevice wadevice;
extern WaveOutput waoutput;
extern MTModule *module;
#ifdef _WIN32
	extern In_Module mod;
#else
	extern InputPlugin mod;
#endif
extern MTXInterfaces i;
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTDSPInterface *dspi;
extern MTObjectsInterface *oi;
extern MTAudioInterface *ai;
extern MTDisplayInterface *di;
extern MTGUIInterface *gi;
//---------------------------------------------------------------------------
#endif
