//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAudio1.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTAudio1.h"
#include "MTAudio2.h"
#include "MTXGUI.h"
#include "MTXAudio.h"
#include "MTXControls.h"
#include "MTXModule.h"
#include "MTXSystem2.h"
#ifdef _WIN32
#	include "MTWaveOut.h"
#	include "MTDirectSound.h"
#else
#	include "MTDevDSP.h"
#endif
//---------------------------------------------------------------------------
static const char *audioname = {"MadTracker Audio Core"};
static const int audioversion = 0x30000;
static const MTXKey audiokey = {0,0,0,0};
MTXInterfaces i;
MTAudioInterface *ai;
MTInterface *mtinterface;
MTSystemInterface *si;
MTObjectsInterface *oi;
WaveOutput output;
#ifdef _WIN32
	MTWaveOutDeviceManager *womanager;
	MTDirectSoundDeviceManager *dsmanager;
#else
	MTDevDSPDeviceManager *devdspmanager;
#endif
MTDevice *devices[32];
int ndevices;
#ifdef _DEBUG
	MTFile *recf;
#endif
//---------------------------------------------------------------------------
struct _DP{
	mt_uint32 offset;
	char label[256];
};
void menurecord(MTShortcut *s,MTControl *c,MTUndo*);
//---------------------------------------------------------------------------
int AudioThread(MTThread*,void* pool)
{
	double oldc,curc,intc,usec;
	
	ENTER("AudioThread");
	si->setprivatedata(0,si->memalloc(sizeof(sample)*PRIVATE_BUFFER,MTM_ZERO));
	oldc = 0;
	while (ai->running){
		if (output.event->wait(1000)){
			if (!output.ndevices) continue;
			si->syscounterex(&curc);
			if (oldc)
				intc = curc-oldc;
			else
				intc = -1;
			oldc = curc;
			output.lock->lock();
			MTTRY
				generateoutput();
			MTCATCH
			MTEND
			output.lock->unlock();
			si->syscounterex(&usec);
			usec -= curc;
		};
	};
	si->memfree(si->getprivatedata(0));
	LEAVE();
	return 0;
}
//---------------------------------------------------------------------------
MTAudioInterface::MTAudioInterface():
thread(0),
running(true),
recording(false),
_dp(0)
{
	type = audiotype;
	key = &audiokey;
	name = audioname;
	version = audioversion;
	status = 0;
}

bool MTAudioInterface::init()
{
	int x;
	MTConfigFile *conf;
	char device[64];
	
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	oi = (MTObjectsInterface*)mtinterface->getinterface(objectstype);
	if ((!si) || (!oi)) return false;
	ENTER("MTAudioInterface::init");
	LOGD("%s - [Audio] Initializing..."NL);
	mtmemzero(&output,sizeof(output));
	output.lock = si->lockcreate();
	output.event = si->eventcreate(true,0,0,false,false);
	output.frequency = 44100;
	output.latency = 50.0;
	output.interval = 50.0;
	output.mincpu = 0.0;
	output.maxcpu = 0.75;
#	ifdef _WIN32
		womanager = new MTWaveOutDeviceManager();
		dsmanager = new MTDirectSoundDeviceManager();
		adddevicemanager(womanager);
		adddevicemanager(dsmanager);
#	else
		devdspmanager = new MTDevDSPDeviceManager();
		adddevicemanager(devdspmanager);
#	endif
	if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
		if (conf->setsection("MTAudio")){
			conf->getparameter("Frequency",&output.frequency,MTCT_SINTEGER,sizeof(output.frequency));
			conf->getparameter("Latency",&output.latency,MTCT_FLOAT,sizeof(output.latency));
			conf->getparameter("Interval",&output.interval,MTCT_FLOAT,sizeof(output.interval));
			conf->getparameter("MinCPU",&output.mincpu,MTCT_FLOAT,sizeof(output.mincpu));
			conf->getparameter("MaxCPU",&output.maxcpu,MTCT_FLOAT,sizeof(output.maxcpu));
			if (output.maxcpu>0.9f) output.maxcpu = 0.9f;
			if (output.maxcpu<0.1f) output.maxcpu = 0.1f;
			if (output.mincpu>output.maxcpu) output.mincpu = output.maxcpu -0.1f;
			if (output.mincpu<0.0f) output.mincpu = 0.0f;
			if (conf->getparameter("Device",device,MTCT_STRING,sizeof(device))){
				unselectdevices();
				for (x=0;x<ndevices;x++){
					if (stricmp(devices[x]->devicename,device)==0){
						selectdevice(x);
						break;
					};
				};
			};
		};
		mtinterface->releaseconf(conf);
	};
#	ifdef _DEBUG
		_dp = si->arraycreate(4,sizeof(_DP));
#	endif
	LOGD("%s - [Audio] Starting audio thread..."NL);
	running = true;
	thread = si->threadcreate(AudioThread,false,true,this,MTT_REALTIME,"Audio");
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTAudioInterface::uninit()
{
	ENTER("MTAudioInterface::uninit");
#	ifdef _DEBUG
		if (recording) menurecord(0,0,0);
		_dp->clear(true);
		si->arraydelete(_dp);
#	endif
	LOGD("%s - [Audio] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	LOGD("%s - [Audio] Stopping audio thread..."NL);
	running = false;
	if (thread){
		if (output.event) output.event->set();
		thread->terminate();
		thread = 0;
	};
	LOGD("%s - [Audio] Freeing all devices..."NL);
	deactivatedevices();
#	ifdef _WIN32
		deldevicemanager(womanager);
		deldevicemanager(dsmanager);
		delete womanager;
		delete dsmanager;
#	else
		deldevicemanager(devdspmanager);
		delete devdspmanager;
#	endif
	si->lockdelete(output.lock);
	si->eventdelete(output.event);
	LEAVE();
}

void MTAudioInterface::start()
{
#	ifdef _DEBUG
		MTDesktop *dsk;
		MTMenuItem *item;
		MTGUIInterface *gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
		if (gi){
			dsk = gi->getdesktop(0);
			if (dsk){
				dsk->popup->additem("|MTAudio",0,0,false,0);
				item = (MTMenuItem*)dsk->popup->additem("Start Recording",23,0,false,0);
				item->tag = 0;
				item->command = menurecord;
			};
		};
#	endif
}

void MTAudioInterface::stop()
{
	deactivatedevices();
}

void MTAudioInterface::processcmdline(void *params)
{

}

void MTAudioInterface::showusage(void *out)
{

}

int MTAudioInterface::config(int command,int param)
{
	return 0;
}


int MTAudioInterface::getnumdevices()
{
	return ndevices;
}

const char* MTAudioInterface::getdevicename(int id)
{
	if ((id<0) || (id>=ndevices)) return 0;
	return devices[id]->devicename;
}

void MTAudioInterface::selectdevice(int id)
{
	if ((id<0) || (id>=ndevices)) return;
	devices[id]->selected = true;
}

void MTAudioInterface::unselectdevices()
{
	int x;

	for (x=0;x<ndevices;x++) devices[x]->selected = false;
}

void MTAudioInterface::activatedevices()
{
	int x,y;
	bool primary = true;
	bool ok = true;
	
	ENTER("MTAudioInterface::activatedevices");
	MTTRY
		output.lock->lock();
		output.ndevices = 0;
		output.sync = 0;
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->disabletracks();
		};
	MTCATCH
	MTEND
	output.lock->unlock();
	mtmemzero(&output.device,sizeof(output.device));
	output.timer = 0;
	output.buffersamples = (int)(output.interval*output.frequency/1000+3) & (~3);
	y = 0;
	for (x=0;x<ndevices;x++){
		if (devices[x]->selected){
			MTAudioDevice *cdev = devices[x]->manager->newdevice(devices[x]->id);
			if (!cdev) continue;
			if (cdev->init((float)output.frequency,2,16,output.latency)){
				WaveDevice &cwdev = *mtnew(WaveDevice);
				output.device[y++] = &cwdev;
				cwdev.name = (char*)si->memalloc(strlen(devices[x]->devicename)+1,0);
				strcpy(cwdev.name,devices[x]->devicename);
				cwdev.device = cdev;
				cwdev.manager = devices[x]->manager;
				cwdev.bits = 16;
				cwdev.nchannels = 2;
				cwdev.datasamples = cdev->datasamples;
				cdev->getposition();
			};
		};
	};
	MTTRY
		output.lock->lock();
		output.ndevices = y;
		if (y){
			output.timer = si->timerevent((int)output.interval,0,true,output.event,true);
			if (!output.timer){
				si->showlastoserror();
				return;
			};
			output.buffersamples = (int)(output.interval*output.frequency/1000+3) & (~3);
			if (y>1) output.sync = 1;
			for (x=0;x<y;x++) output.device[x]->master = (Track*)oi->newobject(MTO_TRACK,0,x);
			for (x=0;x<y;x++){
				WaveDevice &cdev = *output.device[x];
				cdev.device->play();
				si->syscounterex(&cdev.timeopened);
			};
		};
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->enabletracks();
		};
	MTCATCH
	MTEND
	output.lock->unlock();
	LEAVE();
}

void MTAudioInterface::deactivatedevices()
{
	int x,y;
	bool ok = true;
	
	ENTER("MTAudioInterface::deactivatedevices");
	if (output.timer){
		si->timerdelete(output.timer);
		output.timer = 0;
	};
	MTTRY
		output.lock->lock();
		y = output.ndevices;
		output.ndevices = 0;
		for (x=0;x<y;x++) oi->deleteobject(output.device[x]->master);
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->disabletracks();
		};
		for (x=0;x<y;x++){
			WaveDevice &cdev = *output.device[x];
			cdev.device->stop();
			cdev.device->uninit();
			cdev.manager->deldevice(cdev.device);
			si->memfree(cdev.name);
			si->memfree(&cdev);
			output.device[x] = 0;
		};
	MTCATCH
	MTEND
	output.lock->unlock();
	LEAVE();
}

bool MTAudioInterface::adddevicemanager(MTAudioDeviceManager *manager)
{
	int x,y;

	if (ndevices==MAX_AUDIODEVICES) return false;
	FENTER1("MTAudioInterface::adddevicemanager(%.8X)",manager);
	y = ndevices;
	for (x=0;x<MAX_AUDIODEVICES,y<MAX_AUDIODEVICES;x++){
		if (!manager->devicename[x]) break;
		MTDevice *cdevice = mtnew(MTDevice);
		cdevice->devicename = manager->devicename[x];
		cdevice->id = x;
		cdevice->manager = manager;
		if (y==0) cdevice->selected = true;
		devices[y++] = cdevice;
	};
	ndevices = y;
	LEAVE();
	return true;
}

void MTAudioInterface::deldevicemanager(MTAudioDeviceManager *manager)
{
	int x,y;

	FENTER1("MTAudioInterface::deldevicemanager(%.8X)",manager);
	for (x=0;x<ndevices;x++){
		if (devices[x]->manager==manager){
			si->memfree(devices[x]);
			ndevices--;
			for (y=x;y<ndevices;y++) devices[y] = devices[y+1];
			devices[ndevices] = 0;
			x--;
		};
	};
	LEAVE();
}

WaveOutput* MTAudioInterface::getoutput()
{
	return &output;
}

void MTAudioInterface::debugpoint(int offset,const char *text)
{
	_DP ndp;

	if (!recording) return;
	ndp.offset = offset;
	strcpy(ndp.label,text);
	ai->_dp->push(&ndp);
}

#ifdef _DEBUG
void menurecord(MTShortcut *s,MTControl *c,MTUndo*)
{
	MTMenuItem *item = (MTMenuItem*)c;
	char file[512];
	static struct RIFF{
		mt_uint32 riff;
		mt_uint32 filesize;
		mt_uint32 wave;
		mt_uint32 fmt;
		mt_uint32 fmtsize;
		mt_uint16 comp;
		mt_uint16 channels;
		mt_uint32 samplerate;
		mt_uint32 byterate;
		mt_uint16 blockalign;
		mt_uint16 bitspersample;
		mt_uint32 data;
		mt_uint32 datasize;
	} head;
	mt_uint32 x,csize,tmp;
	_DP *cdp;

	MTTRY
		output.lock->lock();
		if ((item) && (item->tag==0)){
			strcpy(file,mtinterface->getprefs()->syspath[SP_ROOT]);
			strcat(file,"Record.wav");
			recf = si->fileopen(file,MTF_WRITE|MTF_CREATE);
			if (recf){
				ai->recording = true;
				item->setcaption("Stop Recording");
				item->tag = 1;
				recf->seteof();
				head.riff = FOURCC('R','I','F','F');
				head.filesize = 0;
				head.wave = FOURCC('W','A','V','E');
				head.fmt = FOURCC('f','m','t',' ');
				head.fmtsize = 16;
				head.comp = 1;
				head.channels = 2;
				head.samplerate = 44100;
				head.byterate = 44100*4;
				head.blockalign = 4;
				head.bitspersample = 16;
				head.data = FOURCC('d','a','t','a');
				head.datasize = 0;
				recf->write(&head,sizeof(head));
			};
		}
		else{
			ai->recording = false;
			head.datasize = recf->pos()-44;
			if (ai->_dp->nitems>0){
				tmp = FOURCC('c','u','e',' ');
				recf->write(&tmp,4);
				csize = recf->pos();
				tmp = 0;
				recf->write(&tmp,4);
				tmp = ai->_dp->nitems;
				recf->write(&tmp,4);
				ai->_dp->reset();
				x = 1;
				while (cdp = (_DP*)ai->_dp->next()){
					recf->write(&x,4);
					recf->write(&cdp->offset,4);
					tmp = FOURCC('d','a','t','a');
					recf->write(&tmp,4);
					tmp = 0;
					recf->write(&tmp,4);
					recf->write(&tmp,4);
					recf->write(&cdp->offset,4);
					x++;
				};
				x = recf->pos();
				recf->seek(csize,MTF_BEGIN);
				tmp = x-csize-4;
				recf->write(&tmp,4);
				recf->seek(x,MTF_BEGIN);
				tmp = FOURCC('L','I','S','T');
				recf->write(&tmp,4);
				csize = recf->pos();
				tmp = 0;
				recf->write(&tmp,4);
				tmp = FOURCC('a','d','t','l');
				recf->write(&tmp,4);
				ai->_dp->reset();
				x = 1;
				while (cdp = (_DP*)ai->_dp->next()){
					tmp = FOURCC('l','a','b','l');
					recf->write(&tmp,4);
					tmp = strlen(cdp->label)+1+4;
					if (tmp & 1) tmp++;
					recf->write(&tmp,4);
					recf->write(&x,4);
					tmp = strlen(cdp->label)+1;
					recf->write(cdp->label,tmp);
					if (tmp & 1){
						tmp = 0;
						recf->write(&tmp,1);
					};
					x++;
				};
				x = recf->pos();
				recf->seek(csize,MTF_BEGIN);
				tmp = x-csize-4;
				recf->write(&tmp,4);
				recf->seek(x,MTF_BEGIN);
				ai->_dp->clear(true);
			};
			head.filesize = recf->pos()-8;
			recf->seek(0,MTF_BEGIN);
			recf->write(&head,sizeof(head));
			si->fileclose(recf);
			recf = 0;
			if (item){
				item->setcaption("Start Recording");
				item->tag = 0;
			};
		};
	MTCATCH
	MTEND
	output.lock->unlock();
}
#endif
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!ai) ai = new MTAudioInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)ai;
	return &i;
}

}
//---------------------------------------------------------------------------
