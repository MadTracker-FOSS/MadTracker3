//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAudio1.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTAUDIO1_INCLUDED
#define MTAUDIO1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXAudio.h"
//---------------------------------------------------------------------------
static const int audiotype = FOURCC('X','A','U','D');

#define MAX_AUDIODEVICES 256
#define PRIVATE_BUFFER   4096

#include "MTAudioDevice.h"
//---------------------------------------------------------------------------


/*
	int nbuffers;
	float bufferms;
	int buffersamples;
*/


struct MTDevice{
	char *devicename;
	MTAudioDeviceManager *manager;
	int id;
	bool selected;
};

class MTAudioInterface : public MTXInterface{
public:
	bool running,recording;

	MTAudioInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
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
	MTArray *_dp;
private:
	MTThread *thread;
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTObjectsInterface *oi;
extern WaveOutput output;
#ifdef _DEBUG
	extern MTFile *recf;
#endif
//---------------------------------------------------------------------------
#endif
