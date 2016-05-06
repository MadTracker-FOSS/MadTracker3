//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTRiff.h 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTRIFF_INCLUDED
#define MTRIFF_INCLUDED
//---------------------------------------------------------------------------
struct RIFFfmt
{
    int fmtid;
    int fmtsize;
    short tag;
    short nchannels;
    int nsamplepersec;
    int nbytepersec;
    short nalign;
    short nbits;
};

struct RIFFdata
{
    int dataid;
    int datasize;
};

struct RIFFWAVE
{
    int waveid;
    RIFFfmt fmt;
};

struct RIFF
{
    int riffid;
    int riffsize;
    RIFFWAVE wave;
    RIFFdata data;
};
//---------------------------------------------------------------------------
#endif
