//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXModule.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXMODULE_INCLUDED
#define MTXMODULE_INCLUDED

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
//---------------------------------------------------------------------------
struct Sequence;

class MTObject;

class MTModule;

class Pattern;

class Automation;

class Oscillator;

class Track;
//---------------------------------------------------------------------------
#include "MTXAudio.h"
#include "MTXObject.h"
#include "MTXAutomation.h"
#include "MTXPattern.h"
#include "MTXInstrument.h"
#include "MTXOscillator.h"
#include "MTXTrack.h"

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
class MTModule: public MTObject
{
    /*
     * This represents a tracker module with all properties: Amount of tracks, loop points,
     * creator's message, filename, patterns, instruments, samples, automation patterns,
     * equalizer settings, BPM settings, and for some squeaking reason the CPU monitor. ?!?!?.
     * As one should expect, this is not a POD type. It's loaded to the brim with member functions
     * modyfing the module.
     * A reasonable refactor would be to clearly differentiate between the module itself, and the editor that
     * modifies a module.
     */
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

    mt_uint32 nsequ[MAX_LAYERS];
    Sequence sequ[MAX_LAYERS][MAX_SEQUENCES];

    MTModule(mt_int32 i);

    virtual ~MTModule();

    virtual void MTCT getdisplayname(char* buffer, int cb) = 0;

    virtual void MTCT setstatus() = 0;

    virtual void MTCT updatelength() = 0;

    virtual bool MTCT settracks(int tracks, int mtracks) = 0;

    virtual void MTCT enabletracks() = 0;

    virtual void MTCT disabletracks() = 0;

    virtual int MTCT getpattpos(int layer, double pos, double* offset, int* cseq, int from = 0) = 0;

    virtual int MTCT getsequence(int layer, double pos, int last) = 0;

    virtual void MTCT play(int mode, bool fromengine = false) = 0;

    virtual void MTCT setpos(double pos, bool fromengine = false) = 0;

    virtual void MTCT settempo(int ctempo, int param, void* value, bool fromengine = false) = 0;

    virtual bool MTCT process(WaveOutput* output) = 0;

    virtual bool MTCT addchannel(InstrumentInstance* c) = 0;

    virtual void MTCT delchannel(InstrumentInstance* c) = 0;

    virtual void MTCT resetchannels() = 0;

    virtual void MTCT resetpatterns() = 0;

    virtual InstrumentInstance* MTCT getlessimportantchannel(int* importance) = 0;

    virtual double MTCT beatstosamples(double nbeats) = 0;

    virtual double MTCT samplestobeats(double nsamples) = 0;

    virtual void MTCT updaterouting() = 0;

    virtual void MTCT needupdaterouting() = 0;

    virtual void MTCT remove(MTObject* o) = 0;
};
//---------------------------------------------------------------------------
#endif
