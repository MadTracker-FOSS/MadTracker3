//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXTrack.h 98 2005-11-30 20:07:26Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXTRACK_INCLUDED
#define MTXTRACK_INCLUDED

//---------------------------------------------------------------------------
class Track;
//---------------------------------------------------------------------------
#include "MTXNode.h"

//---------------------------------------------------------------------------
// Track class
class Track: public Node
{
public:
    Track(MTObject* parent, mt_int32 i, int sub = 0);

    virtual ~Track();

    bool muted;
    bool solo;
    bool empty;
    double vol;
    float panx, pany, panz;
    sample* buffer[8];
    int nsamples;
    int offset;

    virtual void MTCT alloc() = 0;

    virtual void MTCT free() = 0;
};
//---------------------------------------------------------------------------
#endif
