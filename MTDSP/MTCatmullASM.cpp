//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTCatmullASM.cpp 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTCatmullASM.h"

//---------------------------------------------------------------------------
void MTCT a_splinereplace(sample *dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt)
{
    double t1 = (xf - x1) / (x2 - x1);
    double ti = (xt - x1) / (x2 - x1);
    double t2, t3;
    sample _p0, _p1, _p2, _p3;

    if ((t1 < 0.0) || (t1 > 1.0) || (ti < 0.0) || (ti > 1.0) || (ti < t1))
    { return; }
    ti = (ti - t1) / size;
    _p0 = 2 * p1;
    _p1 = -p0 + p2;
    _p2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
    _p3 = -p0 + 3 * p1 - 3 * p2 + p3;
    while(size-- > 0)
    {
        t2 = t1 * t1;
        t3 = t2 * t1;
        *dest++ = 0.5 * (_p0 + _p1 * t1 + _p2 * t2 + _p3 * t3);
        t1 += ti;
    };
}

//---------------------------------------------------------------------------
void MTCT a_splinemodulate(sample *dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt)
{
    double t1 = (xf - x1) / (x2 - x1);
    double ti = (xt - x1) / (x2 - x1);
    double t2, t3;
    sample _p0, _p1, _p2, _p3;

    if ((t1 < 0.0) || (t1 > 1.0) || (ti < 0.0) || (ti > 1.0) || (ti < t1))
    { return; }
    ti = (ti - t1) / size;
    _p0 = 2 * p1;
    _p1 = -p0 + p2;
    _p2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
    _p3 = -p0 + 3 * p1 - 3 * p2 + p3;
    while(size-- > 0)
    {
        t2 = t1 * t1;
        t3 = t2 * t1;
        *dest++ *= 0.5 * (_p0 + _p1 * t1 + _p2 * t2 + _p3 * t3);
        t1 += ti;
    };
}
//---------------------------------------------------------------------------
