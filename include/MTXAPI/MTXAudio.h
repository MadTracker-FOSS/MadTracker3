//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXAudio.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXAUDIO_INCLUDED
#define MTXAUDIO_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int audiotype = FOURCC('X','A','U','D');

#define MAX_AUDIODEVICES 256
#define PRIVATE_BUFFER   1024 //was 4096 in MTAudio1.h...
//---------------------------------------------------------------------------
struct WaveDevice;
struct WaveOutput;
class MTAudioDevice;
class MTAudioDeviceManager;
class MTAudioInterface;
//---------------------------------------------------------------------------
#include "MTXSystem.h"
#include "MTXSystem2.h"
#include "MTXObject.h"
#include "MTXTrack.h"
//---------------------------------------------------------------------------
struct WaveDevice{
	char *name;
	MTAudioDevice *device;
	MTAudioDeviceManager *manager;
	Track *master;
	double timeopened;
	int nchannels;
	int bits;
	int datasamples;
};

struct WaveOutput{
	int ndevices;
	int sync;
	WaveDevice *device[MAX_AUDIODEVICES];
	int frequency;
	float mincpu,maxcpu;
	float latency,interval;
	int playlng;
	int buffersamples;
	MTTimer *timer;
	MTLock *lock;
	MTEvent *event;
};

class MTAudioDevice{
public:
	int datasamples;

	// FIXME: These were pure, should this class be extended? -flibit
	// ...but making them non-pure without imlpementing them breaks the build, so I made them pure again for now. -ih3
	virtual bool MTCT init(float frequency,int nchannels,int bits,double latency)=0;
	virtual void MTCT uninit()=0;
	virtual bool MTCT play()=0;
	virtual bool MTCT stop()=0;
	virtual int MTCT getposition(bool playback = false) = 0;
	virtual bool MTCT getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2)=0;
	virtual bool MTCT writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2)=0;
};

class MTAudioDeviceManager{
public:
	char *devicename[16];

        MTAudioDeviceManager(){ mtmemzero(devicename,sizeof(devicename)); };
	virtual MTAudioDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTAudioDevice *device) = 0;
};

class MTAudioInterface : public MTXInterface{
public:
	bool running,recording;

	// FIXME: DEBUG? -flibit
	MTArray *_dp;

	// FIXME: Should this be explicitly here? -flibit
	MTAudioInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	int MTCT processinput(const char *input);

	// FIXME: These were pure, should this class be extended? -flibit
	virtual int MTCT getnumdevices();
	virtual const char* MTCT getdevicename(int id);
	virtual void MTCT selectdevice(int id);
	virtual void MTCT unselectdevices();
	virtual void MTCT activatedevices();
	virtual void MTCT deactivatedevices();
	virtual bool MTCT adddevicemanager(MTAudioDeviceManager *manager);
	virtual void MTCT deldevicemanager(MTAudioDeviceManager *manager);
	virtual WaveOutput* MTCT getoutput();
	virtual void MTCT debugpoint(int offset,const char *text);
private:
	MTThread *thread;
};
//---------------------------------------------------------------------------
#endif
