//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAudio2.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTAUDIO2_INCLUDED
#define MTAUDIO2_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
//---------------------------------------------------------------------------
typedef void (MTCT *RenderProc)(sample *source,void *dest,int count,int nchannels,int channel);
//---------------------------------------------------------------------------
void generateoutput();
//---------------------------------------------------------------------------
#endif
