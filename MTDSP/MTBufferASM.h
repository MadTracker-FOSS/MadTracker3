//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTBufferASM.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTBUFFERASM_INCLUDED
#define MTBUFFERASM_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
//---------------------------------------------------------------------------
extern "C"
{
extern void MTCT a_emptybuffer(sample *dest,int count);
extern void MTCT a_replacebuffer(sample *dest,sample *source,int count);
extern void MTCT a_replacebuffermul(sample *dest,sample *source,double a,int count);
extern void MTCT a_addbuffer(sample *dest,sample *source,int count);
extern void MTCT a_addbuffermul(sample *dest,sample *source,double a,int count);
extern void MTCT a_addbuffermul2(sample *dest1,sample *dest2,sample *source,double a1,double a2,int count);
extern void MTCT a_addbufferslide(sample *dest,sample *source,double mul,double i,int count);
extern void MTCT a_addbufferslide2(sample *dest1,sample *dest2,sample *source,double mul1,double mul2,double i1,double i2,int count);
extern void MTCT a_ampbuffer(sample *dest,double a,int count);
extern void MTCT a_modulatebuffer(sample *dest,sample *source,int count);
}
//---------------------------------------------------------------------------
#endif
