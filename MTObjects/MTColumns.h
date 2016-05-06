//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTColumns.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTCOLUMNS_INCLUDED
#define MTCOLUMNS_INCLUDED

#define EDF_VOLMODIFIED  0x0001
#define EDF_TONEMODIFIED 0x0002

#define MAX_POLYPHONY 8
//---------------------------------------------------------------------------
struct NoteData;
struct EffectData;

class NoteColumn;

class DrumsColumn;

class VolumeColumn;

class PatternColumn;

class EffectColumn;
//---------------------------------------------------------------------------
#include "MTPattern.h"

//---------------------------------------------------------------------------
struct NoteData
{
    unsigned char lastnote, lastins, ninstances, flags;
    InstrumentInstance *lastinstance[MAX_POLYPHONY];
    double onote, cnote, dnote;
};

struct EffectData
{
    int flags;
    unsigned short old[8];
    char rc, mrc, mri, mrvc;
    char vibtype, vibspeed;
    short vibdepth, vibphase;
    short ivibdepth, ivibphase;
    char tretype, trespeed;
    short tredepth, trephase;
    char tremorcount, tremorstatus, tremorontime, tremorofftime;
    char tonespeed;
    bool needbreak;
    mt_uint8 needjump;
};

class NoteColumn: public Column
{
public:
    NoteColumn();

    void MTCT init(MTPatternInstance *, ColumnStatus &status);

    void MTCT firstpass(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    void MTCT columnhandle(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    int MTCT getwidth(int charwidth);

    void MTCT drawcolumn(MTBitmap *, MTRect &, unsigned char *celldata, ColumnDrawState &state);

    void MTCT onmessage(MTPattManager *pm, MTCMessage &, int cursor, unsigned char *celldata);
};

class DrumsColumn: public Column
{
public:
    DrumsColumn();

    void MTCT init(MTPatternInstance *, ColumnStatus &status);

    void MTCT firstpass(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    void MTCT columnhandle(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    int MTCT getwidth(int charwidth);

    void MTCT drawcolumn(MTBitmap *, MTRect &, unsigned char *celldata, ColumnDrawState &state);

    void MTCT onmessage(MTPattManager *pm, MTCMessage &, int cursor, unsigned char *celldata);
};

class VolumeColumn: public Column
{
public:
    VolumeColumn();

    void MTCT init(MTPatternInstance *, ColumnStatus &status);

    void MTCT firstpass(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    void MTCT columnhandle(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    int MTCT getwidth(int charwidth);

    void MTCT drawcolumn(MTBitmap *, MTRect &, unsigned char *celldata, ColumnDrawState &state);

    void MTCT onmessage(MTPattManager *pm, MTCMessage &, int cursor, unsigned char *celldata);
};

class PanningColumn: public Column
{
public:
    PanningColumn();

    void MTCT init(MTPatternInstance *, ColumnStatus &status);

    void MTCT firstpass(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    void MTCT columnhandle(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    int MTCT getwidth(int charwidth);

    void MTCT drawcolumn(MTBitmap *, MTRect &, unsigned char *celldata, ColumnDrawState &state);

    void MTCT onmessage(MTPattManager *pm, MTCMessage &, int cursor, unsigned char *celldata);
};

class EffectColumn: public Column
{
public:
    EffectColumn();

    void MTCT init(MTPatternInstance *, ColumnStatus &status);

    void MTCT firstpass(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    void MTCT columnhandle(MTPatternInstance *, unsigned char *celldata, FirstPass &pass, ColumnStatus &status, int tick, int nticks);

    int MTCT getwidth(int charwidth);

    void MTCT drawcolumn(MTBitmap *, MTRect &, unsigned char *celldata, ColumnDrawState &state);

    void MTCT onmessage(MTPattManager *pm, MTCMessage &, int cursor, unsigned char *celldata);
};

//---------------------------------------------------------------------------
void initColumns();

void uninitColumns();
//---------------------------------------------------------------------------
#endif
