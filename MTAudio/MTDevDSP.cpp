//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDevDSP.cpp 107 2005-11-30 21:30:55Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef __linux__

#include <MTXAPI/MTXSystem.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <linux/soundcard.h>
#include <errno.h>
#include "MTDevDSP.h"

//---------------------------------------------------------------------------
MTDevDSPDevice::MTDevDSPDevice(const char* dev):
    buffer(0),
    bsize(0)
{
/*
	int l;
	char *e;
	char rdev[256];
*/
    dbits = 8;
    dchannels = 1;
    drate = 8000;
/*	
	rdev[255] = 0;
	l = readlink(dev,rdev,1);
	if (l!=-1){
		if (rdev[0]=='/'){
			l = readlink(dev,rdev,sizeof(rdev)-1);
			if (l>=0) rdev[l] = 0;
		}
		else{
			strcpy(rdev,dev);
			e = strrchr(rdev,'/');
			if (!e) l = -1;
			else{
				e++;
				l = readlink(dev,e,sizeof(rdev)-1-(e-rdev));
			};
		};
	};
	if (l==-1) strcpy(rdev,dev);
*/
    f = open(dev, O_WRONLY);
    if (f < 0)
        FLOGD2("%s - [Audio] ERROR: Cannot open %s! %s"
                   NL, dev, strerror(errno));
}

MTDevDSPDevice::~MTDevDSPDevice()
{
    if (f >= 0)
    {
        close(f);
    }
    if (buffer)
    {
        si->memfree(buffer);
    }
}

bool MTDevDSPDevice::init(float frequency, int nchannels, int bits, double latency)
{
    int status, arg;

    if (f < 0)
    {
        return false;
    }
    FENTER4("MTDevDSPDevice::init(%f,%d,%d,%f)", frequency, nchannels, bits, latency);
    dbits = bits;
    dchannels = nchannels;
    drate = (int) frequency;
    arg = dbits;
    status = ioctl(f, SOUND_PCM_WRITE_BITS, &arg);
    if ((status != 0) || (arg != dbits))
    {
        FLOGD2("%s - [Audio] ERROR: Cannot set sample size! Status: %d Error: %d"
                   NL, status, errno);
        LEAVE();
        return false;
    };
    arg = dchannels;
    status = ioctl(f, SOUND_PCM_WRITE_CHANNELS, &arg);
    if ((status != 0) || (arg != dchannels))
    {
        FLOGD2("%s - [Audio] ERROR: Cannot set number of channels! Status: %d Error: %d"
                   NL, status, errno);
        LEAVE();
        return false;
    };
    arg = drate;
    status = ioctl(f, SOUND_PCM_WRITE_RATE, &arg);
    if ((status != 0) || (arg != drate))
    {
        FLOGD2("%s - [Audio] ERROR: Cannot set sample rate! Status: %d Error: %d"
                   NL, status, errno);
        LEAVE();
        return false;
    };
/*
	bsize = 1024;
	if (ioctl(f,SNDCTL_DSP_SETFRAGMENT,&bsize)==-1){
		FLOGD2("%s - [Audio] ERROR: Cannot set block size! Status: %d Error: %d"NL,status,errno);
		LEAVE();
		return false;
	};
*/
    if (ioctl(f, SNDCTL_DSP_GETBLKSIZE, &bsize) == -1)
    {
        FLOGD2("%s - [Audio] ERROR: Cannot determine block size! Status: %d Error: %d"
                   NL, status, errno);
        LEAVE();
        return false;
    };
    datasamples = bsize / (dbits * dchannels / 8);
    buffer = si->memalloc(bsize, MTM_ZERO);
    LEAVE();
    return true;
}

void MTDevDSPDevice::uninit()
{
    ENTER("MTWaveOutDevice::uninit");
    if (buffer)
    {
        si->memfree(buffer);
        buffer = 0;
    };
    LEAVE();
}

bool MTDevDSPDevice::play()
{
    if (f < 0)
    {
        return false;
    }

    return false;
}

bool MTDevDSPDevice::stop()
{
    if (f < 0)
    {
        return false;
    }

    return false;
}

int MTDevDSPDevice::getposition(bool playback)
{
    if (f < 0)
    {
        return -1;
    }

    return 0;
}

bool MTDevDSPDevice::getdata(int position, int length, void** ptr1, void** ptr2, unsigned long* lng1, unsigned long* lng2)
{
    if (f < 0)
    {
        return false;
    }
    *ptr1 = buffer;
    *ptr2 = 0;
    *lng1 = bsize / (dbits * dchannels / 8);
    *lng2 = 0;
    return true;
}

bool MTDevDSPDevice::writedata(void* ptr1, void* ptr2, unsigned long lng1, unsigned long lng2)
{
    if (f < 0)
    {
        return false;
    }
    write(f, ptr1, lng1);
    write(f, ptr2, lng2);
    printf("%d", ioctl(f, SOUND_PCM_SYNC, 0));
    return true;
}

//---------------------------------------------------------------------------
MTDevDSPDeviceManager::MTDevDSPDeviceManager():
    MTAudioDeviceManager()
{
    int x, n;
    char buf[256];
    struct stat es;

    n = 0;
    for(x = 0; x < 16; x++)
    {
        if (x == 0)
        {
            strcpy(buf, "/dev/dsp");
        }
        else
        {
            sprintf(buf, "/dev/dsp%d", x);
        }
        if (stat(buf, &es) == 0)
        {
            FLOG1("[Audio] Found device: %s"
                      NL, buf);
            devicename[n] = (char*) si->memalloc(strlen(buf) + 1, 0);
            strcpy(devicename[n], buf);
            n++;
        };
    };
}

MTDevDSPDeviceManager::~MTDevDSPDeviceManager()
{
    int x;

    for(x = 0; x < 16; x++)
    {
        if (devicename[x])
        {
            si->memfree(devicename[x]);
        }
    };
}

MTAudioDevice* MTDevDSPDeviceManager::newdevice(int id)
{
    if (devicename[id])
    {
        return new MTDevDSPDevice(devicename[id]);
    }
    else
    {
        return 0;
    }
}

void MTDevDSPDeviceManager::deldevice(MTAudioDevice* device)
{
    delete (MTDevDSPDevice*) device;
}
//---------------------------------------------------------------------------
#endif
