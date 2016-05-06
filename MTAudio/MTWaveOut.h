//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTWaveOut.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTWAVEOUT_INCLUDED
#define MTWAVEOUT_INCLUDED
//---------------------------------------------------------------------------
#include "MTAudioDevice.h"
#include <windows.h>
#include <mmsystem.h>

//---------------------------------------------------------------------------
struct WODevice
{
    int id;
    char *name;
    WAVEOUTCAPS caps;
};

class MTWaveOutDevice: public MTAudioDevice
{
public:
    MTWaveOutDevice(WODevice *dev);

    ~MTWaveOutDevice();

    bool MTCT init(float frequency, int nchannels, int bits, double latency);

    void MTCT uninit();

    bool MTCT play();

    bool MTCT stop();

    int MTCT getposition(bool playback = false);

    bool MTCT getdata(int position, int length, void **ptr1, void **ptr2, unsigned long *lng1, unsigned long *lng2);

    bool MTCT writedata(void *ptr1, void *ptr2, unsigned long lng1, unsigned long lng2);

private:
    WODevice *device;
    int datalength, delay;
    void *data;
    WAVEFORMATEX format;
    HWAVEOUT hwo;
    WAVEHDR header;
};

class MTWaveOutDeviceManager: public MTAudioDeviceManager
{
public:
    MTWaveOutDeviceManager();

    ~MTWaveOutDeviceManager();

    MTAudioDevice *MTCT newdevice(int id);

    void MTCT deldevice(MTAudioDevice *device);
};
//---------------------------------------------------------------------------
#endif
