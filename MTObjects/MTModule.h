//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTModule.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTMODULE_INCLUDED
#define MTMODULE_INCLUDED

#define MAX_INSTRS   256
#define MAX_SAMPLES  256
#define MAX_PATT     256
#define MAX_TRACKS   64
#define MAX_MTRACKS  1

#define MAX_LAYERS    32
#define MAX_SEQUENCES 256

enum
{
    PLAY_STOP = 0, PLAY_SONG, PLAY_PATTERN, PLAY_LOOP, PLAY_EXPORT
};

#define MIN_BPM 32
#define MAX_BPM 500

enum
{
    SF_SELECTED = 0x01, SF_DISABLED = 0x02
};

enum
{
    TF_BPM = 0x01, TF_SLIDE = 0x10
};

struct Sequence;

class MTObject;

class MTModule;

class Pattern;

class PatternInstance;

class Instrument;

class InstrumentInstance;

class Track;

#include "MTObject.h"
#include <MTXAPI/MTXAudio.h>

//---------------------------------------------------------------------------
// Sequence
struct Sequence
{
    double pos;
    double length;
    mt_uint16 patt;
    mt_uint16 automation;
    mt_int8 transpose;
    mt_uint8 flags;
    mt_uint16 reserved;
};

// Tempo
struct Tempo
{
    mt_uint32 flags;
    double pos;
    double bpm;
    mt_uint32 reserved1;
    mt_uint32 reserved2;
};

// Player status
struct PlayStatus
{
    mt_uint32 flags;
    mt_uint32 cseq[MAX_LAYERS];
    double pos;
    double bpm;
    double loopfrom;
    double loopto;
    double nextevent;
    double length;
    double spb;
    PatternInstance* patti[MAX_LAYERS];
    mt_uint32 nchannels, nachannels;
    InstrumentInstance** chan;
    WaveOutput* coutput;
};

// Module class
// Describes a tracker module. Unknown yet if it differentiates mt2, mod, xm etc.
class MTModule: public MTObject
{
public:
    mt_uint16 nmtracks;
    mt_uint16 ntracks;
    double loops;
    double loope;
    char* message;
    bool showmessage;
    mt_uint8 summarymask;
    char* summary[8];
    char* filename;

    MTArray* patt, * apatt, * instr, * spl;
    MTArray* master, * trk;
    MTArray* fx;
    MTArray* tempo;

    PlayStatus playstatus;
    MTCPUMonitor* cpu;

    int nsequ[MAX_LAYERS];
    Sequence sequ[MAX_LAYERS][MAX_SEQUENCES];

    MTModule(mt_int32 i);

    virtual ~MTModule();

    void MTCT setmodified(int value, int flags);

    void MTCT notify(MTObject* source, int message, int param1, void* param2);

    void MTCT enumchildren(MTObjectEnum enumproc, void* data);

    virtual void MTCT getdisplayname(char* buffer, int cb);

    virtual void MTCT setstatus();

    virtual void MTCT updatelength();

    virtual bool MTCT settracks(int tracks, int mtracks);

    virtual void MTCT enabletracks();

    virtual void MTCT disabletracks();

    virtual int MTCT getpattpos(int layer, double pos, double* offset, int* cseq, int from = 0);

    virtual int MTCT getsequence(int layer, double pos, int last);

    virtual void MTCT play(int mode, bool fromengine = false);

    virtual void MTCT setpos(double pos, bool fromengine = false);

    virtual void MTCT settempo(int ctempo, int param, void* value, bool fromengine = false);

    virtual bool MTCT process(WaveOutput* output);

    virtual bool MTCT addchannel(InstrumentInstance* c);

    virtual void MTCT delchannel(InstrumentInstance* c);

    virtual void MTCT resetchannels();

    virtual void MTCT resetpatterns();

    virtual InstrumentInstance* MTCT getlessimportantchannel(int* importance);

    virtual double MTCT beatstosamples(double nbeats);

    virtual double MTCT samplestobeats(double nsamples);

    virtual void MTCT updaterouting();

    virtual void MTCT needupdaterouting();

    virtual void MTCT remove(MTObject* o);

private:
    friend class MTObjectsInterface;

    friend int MTCT engineproc(MTThread* thread, void* param);

    MTLock* mlock;
    int lastbeat;
    int crashcount;
    bool needupdate;
    MTArray* buffers;
    MTArray* ris;
    void* mts;

    bool MTCT subprocess(WaveOutput* output, int = 0, double = 0.0, int = 0, bool silence = false);
};
//---------------------------------------------------------------------------
#include "MTPattern.h"
#include "MTInstrument.h"
//---------------------------------------------------------------------------
#endif
