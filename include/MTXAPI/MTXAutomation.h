//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXAutomation.h 98 2005-11-30 20:07:26Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXAUTOMATION_INCLUDED
#define MTXAUTOMATION_INCLUDED

//---------------------------------------------------------------------------
class Automation;
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXModule.h"

//---------------------------------------------------------------------------
// Automation
//---------------------------------------------------------------------------
struct AEnvelope
{
    mt_uint32 npoints;
    EnvPoint points[64];
};

struct TrackEnvelopes
{
    AEnvelope env[16];
};

struct TrackAuto
{
    int flags;
    int fx;
    TrackEnvelopes *trkenv;
};

class Automation: public MTObject
{
public:
    Automation(MTObject *parent, mt_int32 i);

    virtual ~Automation();

    TrackAuto trkauto[MAX_TRACKS + MAX_MTRACKS];
};
//---------------------------------------------------------------------------
#endif
