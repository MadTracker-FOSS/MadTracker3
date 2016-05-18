//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTCatmullASM.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTCATMULLASM_INCLUDED
#define MTCATMULLASM_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>

//---------------------------------------------------------------------------
void MTCT a_splinereplace(sample* dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt);

void MTCT a_splinemodulate(sample* dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt);
//---------------------------------------------------------------------------
#endif
