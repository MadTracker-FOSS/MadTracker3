//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDevDSP.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTDEVDSP_INCLUDED
#define MTDEVDSP_INCLUDED
//---------------------------------------------------------------------------
#include "MTAudioDevice.h"

//---------------------------------------------------------------------------
class MTDevDSPDevice: public MTAudioDevice
{
public:
    MTDevDSPDevice(const char *dev);

    ~MTDevDSPDevice();

    bool MTCT init(float frequency, int nchannels, int bits, double latency);

    void MTCT uninit();

    bool MTCT play();

    bool MTCT stop();

    int MTCT getposition(bool playback = false);

    bool MTCT getdata(int position, int length, void **ptr1, void **ptr2, unsigned long *lng1, unsigned long *lng2);

    bool MTCT writedata(void *ptr1, void *ptr2, unsigned long lng1, unsigned long lng2);

private:
    int f;
    void *buffer;
    int bsize, dbits, dchannels, drate;
};

class MTDevDSPDeviceManager: public MTAudioDeviceManager
{
public:
    MTDevDSPDeviceManager();

    ~MTDevDSPDeviceManager();

    MTAudioDevice *MTCT newdevice(int id);

    void MTCT deldevice(MTAudioDevice *device);
};
//---------------------------------------------------------------------------
#endif
