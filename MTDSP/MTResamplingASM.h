//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTResamplingASM.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTRESAMPLINGASM_INCLUDED
#define MTRESAMPLINGASM_INCLUDED

#include "MTDSP.h"
//---------------------------------------------------------------------------
extern "C" {
extern void MTCT a_resample_none_8(char* dest, char* source, int count, ChannelStatus& status);
extern void MTCT a_resample_none_16(char* dest, char* source, int count, ChannelStatus& status);
extern void MTCT a_resample_none_32(char* dest, char* source, int count, ChannelStatus& status);
extern void MTCT a_resample_linear_8(char* dest, char* source, int count, ChannelStatus& status);
extern void MTCT a_resample_linear_16(char* dest, char* source, int count, ChannelStatus& status);
extern void MTCT a_resample_linear_32(char* dest, char* source, int count, ChannelStatus& status);
}
//---------------------------------------------------------------------------
#endif
