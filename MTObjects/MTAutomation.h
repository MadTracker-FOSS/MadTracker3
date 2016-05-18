//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAutomation.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTAUTOMATION_INCLUDED
#define MTAUTOMATION_INCLUDED

class Automation;

#include "MTModule.h"

//---------------------------------------------------------------------------
// Automation
//---------------------------------------------------------------------------
struct AEnvelope
{
    mt_uint32 npoints;
    EnvPoint points[64];
};

struct TrackAuto
{
    MTObject* target;
    mt_uint32 param;
    mt_uint32 flags;
    AEnvelope env;
};

class Automation: public MTObject
{
public:
    Automation(MTObject* parent, mt_int32 i);

    virtual ~Automation();

    MTArray* envelopes;
};
//---------------------------------------------------------------------------
#endif
