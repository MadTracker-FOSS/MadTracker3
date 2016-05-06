//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTFilterASM.cpp 110 2006-01-08 20:13:42Z Yannick $
//
//---------------------------------------------------------------------------
#include <math.h>
#include "MTFilterASM.h"

//---------------------------------------------------------------------------
const double f2pi = 6.283185307179586476925286766559;

const double fresostart = 1.44927536231884057971014492753e-2;

const double fresorange = 0.92753623188405797101449275362319;

//---------------------------------------------------------------------------
void MTCT a_subfilter_(sample *dest, sample *source, FilterStatus &status, int count)
{
    if ((status.fvarlng) && (status.rvarlng))
    {
        while(count-- > 0)
        {
            status.t[0] += (*source++ - status.t[1]) * status.c;
            status.t[1] += status.t[0];
            status.t[0] *= status.r;
            *dest++ += status.t[1];
            if (status.fvarlng-- > 0)
            { status.c += status.cv; }
            if (status.rvarlng-- > 0)
            { status.r += status.rv; }
        };
    }
    else if (status.fvarlng)
    {
        while(count-- > 0)
        {
            status.t[0] += (*source++ - status.t[1]) * status.c;
            status.t[1] += status.t[0];
            status.t[0] *= status.r;
            *dest++ += status.t[1];
            if (status.fvarlng-- > 0)
            { status.c += status.cv; }
        };
    }
    else if (status.rvarlng)
    {
        while(count-- > 0)
        {
            status.t[0] += (*source++ - status.t[1]) * status.c;
            status.t[1] += status.t[0];
            status.t[0] *= status.r;
            *dest++ += status.t[1];
            if (status.rvarlng-- > 0)
            { status.r += status.rv; }
        };
    }
    else
    {
        while(count-- > 0)
        {
            status.t[0] += (*source++ - status.t[1]) * status.c;
            status.t[1] += status.t[0];
            status.t[0] *= status.r;
            *dest++ += status.t[1];
        };
    };
    if (IS_DENORMAL(status.t[0]))
    { status.t[0] = 0.0; }
    if (IS_DENORMAL(status.t[1]))
    { status.t[1] = 0.0; }
}

void MTCT a_filter_(sample *dest, sample *source, FilterStatus &status, int count, int frequency)
{
    double fc, r;
    int i = count;
    bool init = false;

    i = frequency / FILTER_STEPS;
    if ((status.flags & FILTER_INIT) == 0)
    {
        fc = status.frequency / frequency;
        if (fc > 0.25)
        {
            fc = 0.25;
        }
        else if (fc < 0.0001)
        { fc = 0.0001; }
        status.c = 2 - 2 * cos(f2pi * fc);
        status.r = sqrt(sqrt(fresostart + status.resonance * fresorange));
        status.flags |= FILTER_INIT;
    };
    while(count > 0)
    {
        if (count < i)
        { i = count; }
        if (status.fvarlng > 0)
        {
            r = (double) i / (double) status.fvarlng;
            if (r > 1.0)
            { r = 1.0; }
            status.frequency = status.frequency * (1.0 - r) + status.frequency2 * r;
            fc = status.frequency / frequency;
            if (fc > 0.25)
            {
                fc = 0.25;
            }
            else if (fc < 0.0001)
            { fc = 0.0001; }
            status.cv = (2 - 2 * cos(f2pi * fc) - status.c) / i;
        };
        if (status.rvarlng > 0)
        {
            r = (double) i / (double) status.rvarlng;
            if (r > 1.0)
            { r = 1.0; }
            status.resonance = status.resonance2 * (1.0 - r) + status.resonance * r;
            status.rv = (sqrt(sqrt(fresostart + status.resonance * fresorange)) - status.r) / i;
        };
        a_subfilter_(dest, source, status, i);
        source += i;
        dest += i;
        count -= i;
    };
    status.flags &= (~FILTER_RAMP);
}
//---------------------------------------------------------------------------
