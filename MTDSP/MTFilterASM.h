//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTFilterASM.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTFILTERASM_INCLUDED
#define MTFILTERASM_INCLUDED

#define FILTER_STEPS 100

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)
//---------------------------------------------------------------------------
#include "MTDSP1.h"
//---------------------------------------------------------------------------
void MTCT a_filter_(sample *dest,sample *source,FilterStatus &status,int count,int frequency);
//---------------------------------------------------------------------------
#endif
