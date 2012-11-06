//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTWaveOut.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#include "MTWaveOut.h"
#include "MTAudio1.h"
//---------------------------------------------------------------------------
WODevice *wodev[MAX_AUDIODEVICES];
int nwodev;
//---------------------------------------------------------------------------
void showerror(int error,WODevice *device)
{
	char tmp[512];
	char *errt;
	int l;
	
	strcpy(tmp,device->name);
	strcat(tmp,"\n");
	errt = strchr(tmp,'\0');
	l = 512-((int)errt-(int)&tmp);
	if (waveOutGetErrorText(error,errt,l)==0){
		LOGD("%s - [Audio] WARNING (WO): ");
		LOG(errt);
		LOG(NL);
		si->dialog(tmp,"MadTracker - WaveOut Error",MTD_OK,MTD_EXCLAMATION,0);
	};
}
//---------------------------------------------------------------------------
MTWaveOutDevice::MTWaveOutDevice(WODevice *dev):
hwo(0),
device(dev),
delay(0)
{
	mtmemzero(&format,sizeof(format));
	mtmemzero(&header,sizeof(header));
}

MTWaveOutDevice::~MTWaveOutDevice()
{

}

bool MTWaveOutDevice::init(float frequency,int nchannels,int bits,double latency)
{
	int res;

	FENTER4("MTWaveOutDevice::init(%f,%d,%d,%f)",frequency,nchannels,bits,latency);
	format.wBitsPerSample = bits;
	format.nChannels = nchannels;
	format.nSamplesPerSec = (int)frequency;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nBlockAlign = (bits*nchannels)>>3;
	format.nAvgBytesPerSec = (int)frequency*format.nBlockAlign;
	format.cbSize = 0;
	delay = (int)(latency*frequency/1000.0);
	res = waveOutOpen(&hwo,device->id,&format,0,0,0);
	if (res==0){
		mtmemzero(&header,sizeof(WAVEHDR));
		datasamples = delay*8;
		datalength = datasamples*format.nBlockAlign;
		data = si->memalloc(datalength,MTM_ZERO);
		header.lpData = (char*)data;
		header.dwBufferLength = datalength;
		header.dwFlags = WHDR_BEGINLOOP|WHDR_ENDLOOP;
		header.dwLoops = -1;
		res = waveOutPrepareHeader(hwo,&header,sizeof(WAVEHDR));
		if (res==0){
			LEAVE();
			return true;
		};
		si->memfree(data);
		data = 0;
	};
	hwo = 0;
	showerror(res,device);
	LEAVE();
	return false;
}

void MTWaveOutDevice::uninit()
{
	if (!hwo) return;
	ENTER("MTWaveOutDevice::uninit");
	waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR));
	waveOutClose(hwo);
	hwo = 0;
	if (data){
		si->memfree(data);
		data = 0;
	};
	LEAVE();
}

bool MTWaveOutDevice::play()
{
	if (!hwo) return false;
	return (waveOutWrite(hwo,&header,sizeof(WAVEHDR))==0);
}

bool MTWaveOutDevice::stop()
{
	if (!hwo) return false;
	return (waveOutReset(hwo)==0);
}

int MTWaveOutDevice::getposition(bool playback)
{
	MMTIME cpos;

	if (!hwo) return -1;
	cpos.u.sample = 0;
	cpos.wType = TIME_SAMPLES;
	if (waveOutGetPosition(hwo,&cpos,sizeof(MMTIME))==0){
		return cpos.u.sample+((playback)?0:delay);
	};
	return -1;
}

bool MTWaveOutDevice::getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2)
{
	int x;

	*ptr1 = (char*)data+position*format.nBlockAlign;
	*ptr2 = data;
	*lng1 = length;
	*lng2 = 0;
	x = position+length-datasamples;
	if (x>0){
		*lng1 -= x;
		*lng2 = x;
	};
	return true;
}

bool MTWaveOutDevice::writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2)
{
	return true;
}
//---------------------------------------------------------------------------
MTWaveOutDeviceManager::MTWaveOutDeviceManager():
MTAudioDeviceManager()
{
	int x,n;

	nwodev = 0;
	n = waveOutGetNumDevs();
	for (x=0;x<n;x++){
		if (nwodev==MAX_AUDIODEVICES) break;
		wodev[nwodev] = mtnew(WODevice);
		WODevice &cdev = *wodev[nwodev++];
		if ((waveOutGetDevCaps(x,&cdev.caps,sizeof(WAVEOUTCAPS))==0) && (cdev.caps.dwFormats)){
			cdev.name = (char*)si->memalloc(strlen(cdev.caps.szPname)+1,0);
			strcpy(cdev.name,cdev.caps.szPname);
			cdev.id = x;
			FLOG1("[Audio] Found device (WO): %s"NL,cdev.name);
		}
		else{
			si->memfree(&cdev);
			wodev[--nwodev] = 0;
		};
	};
	for (x=0;x<nwodev;x++){
		devicename[x] = wodev[x]->name;
	};
}

MTWaveOutDeviceManager::~MTWaveOutDeviceManager()
{
	int x;

	for (x=0;x<nwodev;x++){
		WODevice &cdev = *wodev[x];
		si->memfree(cdev.name);
		si->memfree(&cdev);
		wodev[x] = 0;
	};
	mtmemzero(devicename,sizeof(devicename));
	nwodev = 0;
}

MTAudioDevice* MTWaveOutDeviceManager::newdevice(int id)
{
	if ((id<0) || (id>nwodev)) return 0;
	return new MTWaveOutDevice(wodev[id]);
}

void MTWaveOutDeviceManager::deldevice(MTAudioDevice *device)
{
	delete (MTWaveOutDevice*)device;
}
//---------------------------------------------------------------------------
#endif
