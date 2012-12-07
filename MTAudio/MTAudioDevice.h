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
#include "MTXAudio.h"
//---------------------------------------------------------------------------

class MTAudioDeviceManager{
public:
	char *devicename[MAX_AUDIODEVICES];

	MTAudioDeviceManager(){ mtmemzero(devicename,sizeof(devicename)); };
	virtual MTAudioDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTAudioDevice *device) = 0;
};
//---------------------------------------------------------------------------
#endif
