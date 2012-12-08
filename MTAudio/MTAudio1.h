//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAudio1.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTAUDIO1_INCLUDED
#define MTAUDIO1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXAudio.h"
#include "MTXObjects.h"
#include "MTAudioDevice.h"
//---------------------------------------------------------------------------

struct MTDevice{
	char *devicename;
	MTAudioDeviceManager *manager;
	int id;
	bool selected;
};

//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTObjectsInterface *oi;
extern WaveOutput output;
#ifdef _DEBUG
	extern MTFile *recf;
#endif
//---------------------------------------------------------------------------
#endif
