//---------------------------------------------------------------------------
#ifndef MTAUDIODEVICE_INCLUDED
#define MTAUDIODEVICE_INCLUDED
//---------------------------------------------------------------------------
class MTAudioDevice;
class MTAudioDeviceManager;
//---------------------------------------------------------------------------
#include "MTXSystem.h"
#include "MTXSystem2.h"
#include "MTAudio1.h"
//---------------------------------------------------------------------------
class MTAudioDevice{
public:
	int datasamples;

	virtual bool MTCT init(float frequency,int nchannels,int bits,double latency) = 0;
	virtual void MTCT uninit() = 0;
	virtual bool MTCT play() = 0;
	virtual bool MTCT stop() = 0;
	virtual int MTCT getposition(bool playback = false) = 0;
	virtual bool MTCT getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2) = 0;
	virtual bool MTCT writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2) = 0;
};

class MTAudioDeviceManager{
public:
	char *devicename[MAX_AUDIODEVICES];

	MTAudioDeviceManager(){ mtmemzero(devicename,sizeof(devicename)); };
	virtual MTAudioDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTAudioDevice *device) = 0;
};
//---------------------------------------------------------------------------
#endif
