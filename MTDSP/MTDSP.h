//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDSP1.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTDSP1_INCLUDED
#define MTDSP1_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>

//---------------------------------------------------------------------------
static const int dsptype = FOURCC('X', 'D', 'S', 'P');

#define RESAMPLE_8BIT   0x00
#define RESAMPLE_16BIT  0x01
#define RESAMPLE_32BIT  0x02

#define RESAMPLE_NONE   0x00
#define RESAMPLE_LINEAR 0x10
#define RESAMPLE_CUBIC1 0x20
#define RESAMPLE_CUBIC2 0x30
#define RESAMPLE_SINC1  0x40
#define RESAMPLE_SINC2  0x50
#define RESAMPLE_SINC3  0x60

#define FILTER_INIT 0x1
#define FILTER_RAMP 0x2
//---------------------------------------------------------------------------
struct ChannelStatus;
struct FilterStatus;
struct DSPFunctions;

class MTDSPInterface;
//---------------------------------------------------------------------------
#include <MTXAPI/MTXSystem.h>

//---------------------------------------------------------------------------
struct ChannelStatus
{
    int posi;
    unsigned int posd;
    double pitch;
    double pitchvar;
    double vol;
    double volvar;
    double multiplier;
    int pitchvarlng;
    int volvarlng;
    unsigned char tc;
    unsigned char sc;
    bool reverse;
    char reserved;
};

struct FilterStatus
{
    int flags;
    int fvarlng, rvarlng;
    double c, r;
    double cv, rv;
    double t[4];
    float frequency;
    float resonance;
    float frequency2;
    float resonance2;
};

typedef void (MTCT* MTFilterProc)(sample* dest, sample* source, FilterStatus& status, int count, int frequency);

typedef void (MTCT* MTResampleProc)(sample* dest, char* source, int count, ChannelStatus& status);

class MTDSPInterface: public MTXInterface
{
public:
    void (MTCT* emptybuffer)(sample* dest, int count);

    void (MTCT* replacebuffer)(sample* dest, sample* source, int count);

    void (MTCT* replacebuffermul)(sample* dest, sample* source, double mul, int count);

    void (MTCT* addbuffer)(sample* dest, sample* source, int count);

    void (MTCT* addbuffermul)(sample* dest, sample* source, double mul, int count);

    void (MTCT* addbuffermul2)(sample* dest1, sample* dest2, sample* source, double mul1, double mul2, int count);

    void (MTCT* addbufferslide)(sample* dest, sample* source, double mul, double inc, int count);

    void (MTCT* addbufferslide2)(sample* dest1, sample* dest2, sample* source, double mul1, double mul2, double inc1, double inc2, int count);

    void (MTCT* ampbuffer)(sample* dest, double mul, int count);

    void (MTCT* modulatebuffer)(sample* dest, sample* source, int count);

    MTFilterProc filter[16];
    MTResampleProc resample[32];

    void (MTCT* splinereplace)(sample* dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt);

    void (MTCT* splinemodulate)(sample* dest, int size, double x0, sample p0, double x1, sample p1, double x2, sample p2, double x3, sample p3, double xf, double xt);

    MTDSPInterface();

    bool MTCT init();

    void MTCT uninit();

    void MTCT start();

    void MTCT stop();

    void MTCT processcmdline(void* params);

    void MTCT showusage(void* out);

    int MTCT config(int command, int param);
};

//---------------------------------------------------------------------------
extern MTInterface* mtinterface;

extern MTDSPInterface* dspi;

extern MTSystemInterface* si;
//---------------------------------------------------------------------------
#endif
