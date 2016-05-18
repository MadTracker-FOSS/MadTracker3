//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXInstrument.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXINSTRUMENT_INCLUDED
#define MTXINSTRUMENT_INCLUDED

#define IF_NEEDPREPROCESS  0x01
#define IF_NEEDPOSTPROCESS 0x02
#define IF_NEEDPREBUFFER   0x04
#define IF_NEEDPOSTBUFFER  0x08
#define IF_SAMETHREAD      0x10
#define IF_SAMECOMPUTER    0x20

#define IIF_BACKGROUND 0x001
#define IIF_SLEEPING   0x002

enum
{
    MTIE_NOTE = 0, MTIE_KILL, MTIE_RESET, MTIE_PARAM
};

#define MTIEF_ISNOTE       0x01
#define MTIEF_ISINS        0x02
#define MTIEF_VOLUME       0x01
#define MTIEF_PITCH        0x02
#define MTIEF_ADD          0x04
#define MTIEF_MUL          0x08
#define MTIEF_DONTSATURATE 0x10

enum
{
    MTIP_VOLUME = 0,
    MTIP_PANNING,
    MTIP_PITCH,
    MTIP_TREMOLO,
    MTIP_VIBRATO,
    MTIP_OFFSET,
    MTIP_PRESSURE,
    MTIP_MIDI,
    MTIP_CUSTOM
};

enum
{
    MTIS_BEGIN = 0, MTIS_CURRENT, MTIS_END
};

enum
{
    MTIS_BEATS = 0, MTIS_MS, MTIS_SAMPLES, MTIS_INNER = 16
};

//---------------------------------------------------------------------------
class Instrument;

class InstrumentInstance;
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXTrack.h"
#include "MTXPattern.h"

//---------------------------------------------------------------------------
// Instrument classes
//---------------------------------------------------------------------------
class Instrument: public MTObject
{
public:
    Instrument(MTObject* parent, mt_uint32 type, mt_int32 i):
        MTObject(parent, type, i)
    {
    };

    virtual ~Instrument()
    {
    };

    virtual InstrumentInstance* MTCT createinstance(Track* track, PatternInstance* caller, InstrumentInstance* previous) = 0;

    virtual void MTCT preprocess(int count)
    {
    };

    virtual void MTCT postprocess(int count)
    {
    };

    virtual void MTCT prebuffer(int count)
    {
    };

    virtual void MTCT postbuffer(int count)
    {
    };

    virtual bool MTCT acceptoscillator()
    {
        return false;
    };

    virtual int MTCT addoscillator(Oscillator* o)
    {
        return -1;
    };

    virtual int MTCT deloscillator(Oscillator* o)
    {
        return -1;
    };
};

struct MTIEvent
{
    MTObject* source;
    int type;
    double offset;
    int flags;
    int size;
    char data[8];
};

struct MTINoteEvent
{
    MTObject* source;
    int type;
    double offset;
    int flags;
    int size;
    double notelength;
    double noteoffset;
    double note;
    double noteoffvelocity;
    double gvolume;
    double volume;
    float gpanx, gpany, gpanz;
    float panx, pany, panz;
};

struct MTIParamEvent
{
    MTObject* source;
    int type;
    double offset;
    int flags;
    int size;
    int param;
    union
    {
        double dvalue1;
        struct
        {
            int ivalue1;
            int ivalue2;
        };
    };
    double dvalue2;
};

class InstrumentInstance
{
public:
    int id;
    int flags;
    int cpu;
    MTModule* module;
    Instrument* parent;
    Track* track;
    PatternInstance* caller;
    int layer;
    double note;
    double cpos;
    double nextevent, sleepingtime;
    double gvol;
    double mvol;
    float gpanx, gpany, gpanz;
    float mpanx, mpany, mpanz;

    InstrumentInstance(Instrument* p, Track* t, PatternInstance* c, int l, InstrumentInstance* previous)
    {
        flags = 0;
        cpu = 0;
        parent = p;
        module = p->module;
        track = t;
        caller = c;
        layer = l;
        note = cpos = nextevent = 0.0;
        gvol = mvol = 1.0;
        mpanx = mpany = mpanz = 0.0;
    };

    virtual ~InstrumentInstance()
    {
    };

    virtual void MTCT processevents() = 0;

    virtual bool MTCT seek(double offset, int origin = MTIS_BEGIN, int units = MTIS_BEATS)
    {
        return false;
    };

    virtual void MTCT process(int count) = 0;

    virtual void MTCT sendevents(int nevents, MTIEvent** events) = 0;

    virtual float MTCT getimportance() = 0;

    virtual void MTCT changecaller(PatternInstance* newcaller)
    {
        caller = newcaller;
    };
};
//---------------------------------------------------------------------------
#endif
