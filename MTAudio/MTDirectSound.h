//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDirectSound.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTDIRECTSOUND_INCLUDED
#define MTDIRECTSOUND_INCLUDED
//---------------------------------------------------------------------------
#include "MTAudioDevice.h"
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
//---------------------------------------------------------------------------
struct DSDevice{
	GUID *guid;
	char *name;
	DSCAPS caps;
	DSBCAPS bcaps;
};

class MTDirectSoundDevice : public MTAudioDevice{
public:
	MTDirectSoundDevice(DSDevice *dev);
	~MTDirectSoundDevice();
	bool MTCT init(float frequency,int nchannels,int bits,double latency);
	void MTCT uninit();
	bool MTCT play();
	bool MTCT stop();
	int MTCT getposition(bool playback = false);
	bool MTCT getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2);
	bool MTCT writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2);
private:
	int delay;
	DSDevice *device;
	WAVEFORMATEX format;
	DSBUFFERDESC bufferdesc;
	IDirectSound* ids;
	IDirectSoundBuffer *idsb;
};

class MTDirectSoundDeviceManager : public MTAudioDeviceManager{
public:
	MTDirectSoundDeviceManager();
	~MTDirectSoundDeviceManager();
	MTAudioDevice* MTCT newdevice(int id);
	void MTCT deldevice(MTAudioDevice *device);
};
//---------------------------------------------------------------------------
#endif
