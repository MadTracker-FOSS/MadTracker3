//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDirectSound.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#include <stdio.h>
#include "MTDirectSound.h"
#include "MTAudio1.h"
#include <MTXAPI/MTXGUI.h>
//---------------------------------------------------------------------------
MTGUIInterface *gi;
HMODULE hdsound;
HRESULT (WINAPI *dscreate)(LPGUID,LPDIRECTSOUND*,IUnknown*);
HRESULT (WINAPI *dsenum)(LPDSENUMCALLBACK,LPVOID);
DSDevice *dsdev[MAX_AUDIODEVICES];
int ndsdev;
//---------------------------------------------------------------------------
int WINAPI dsenumproc(GUID *guid,const char *desc,const char *module,void*)
{
    IDirectSound *ds = 0;

    if (ndsdev>=MAX_AUDIODEVICES) return 0;
    dsdev[ndsdev] = mtnew(DSDevice);
    DSDevice &cdev = *dsdev[ndsdev++];
    cdev.caps.dwSize = sizeof(DSCAPS);
    cdev.bcaps.dwSize = sizeof(DSBCAPS);
    cdev.name = (char*)si->memalloc(strlen(desc)+1,0);
    strcpy(cdev.name,desc);
    if (guid){
        cdev.guid = mtnew(GUID);
        memcpy(cdev.guid,guid,sizeof(GUID));
    };
    if (dscreate(guid,&ds,0)==0){
        FLOG1("[Audio] Found device (DS): %s"NL,cdev.name);
        ds->Release();
        return 1;
    };
    if (ds) ds->Release();
    if (cdev.name) si->memfree(cdev.name);
    if (cdev.guid) si->memfree(cdev.guid);
    si->memfree(&cdev);
    dsdev[--ndsdev] = 0;
    return 1;
}

void showerror(int error,DSDevice *device)
{
    char tmp[512];
    char *e;

    strcpy(tmp,device->name);
    strcat(tmp,"\n");
    if (error>>16==0x8878){
        e = strchr(tmp,0);
        sprintf(e,"DirectSound error %d",error & 0xFFFF);
        LOGD("%s - [Audio] WARNING (DS): ");
        LOG(e);
        LOG(NL);
        si->dialog(tmp,"MadTracker - DirectSound Error",MTD_OK,MTD_EXCLAMATION,0);
    }
    else si->showoserror(error);
}
//---------------------------------------------------------------------------
MTDirectSoundDevice::MTDirectSoundDevice(DSDevice *dev):
ids(0),
idsb(0),
device(dev),
delay(0)
{
    mtmemzero(&format,sizeof(format));
    mtmemzero(&bufferdesc,sizeof(bufferdesc));
}

MTDirectSoundDevice::~MTDirectSoundDevice()
{

}

bool MTDirectSoundDevice::init(float frequency,int nchannels,int bits,double latency)
{
    int res;
    void *data;
    unsigned long length;
    bool primary = true;

    FENTER4("MTDirectSoundDevice::init(%f,%d,%d,%f)",frequency,nchannels,bits,latency);
    format.wBitsPerSample = bits;
    format.nChannels = nchannels;
    format.nSamplesPerSec = (int)frequency;
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nBlockAlign = (bits*nchannels)>>3;
    format.nAvgBytesPerSec = (int)frequency*format.nBlockAlign;
    format.cbSize = 0;
    delay = (int)(latency*frequency/1000.0);
    res = dscreate(device->guid,&ids,0);
    if (res==0){
        HWND wnd = (gi)?(HWND)gi->getappwindow():0;
        res = ids->SetCooperativeLevel(wnd,DSSCL_WRITEPRIMARY);
        if (res){
            primary = false;
            res = ids->SetCooperativeLevel(wnd,DSSCL_EXCLUSIVE);
        };
        if (res==0){
            mtmemzero(&bufferdesc,sizeof(DSBUFFERDESC));
            bufferdesc.dwSize = sizeof(DSBUFFERDESC);
            if (primary){
                bufferdesc.dwFlags = DSBCAPS_GLOBALFOCUS|DSBCAPS_PRIMARYBUFFER;
            }
            else{
                bufferdesc.dwFlags = DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_GETCURRENTPOSITION2;
                bufferdesc.dwBufferBytes = 32768;
                bufferdesc.lpwfxFormat = &format;
            };
            res = ids->CreateSoundBuffer(&bufferdesc,&idsb,0);
            if (res){
                bufferdesc.dwFlags &= ~DSBCAPS_GLOBALFOCUS;
                bufferdesc.dwFlags |= DSBCAPS_STICKYFOCUS;
                res = ids->CreateSoundBuffer(&bufferdesc,&idsb,0);
            };
            if (res==0){
                if (primary) res = idsb->SetFormat(&format);
                idsb->GetCaps(&device->bcaps);
                if (res==0){
                    res = idsb->Lock(0,device->bcaps.dwBufferBytes,&data,&length,0,0,0);
                    if (res==DSERR_BUFFERLOST){
                        idsb->Restore();
                        res = idsb->Lock(0,device->bcaps.dwBufferBytes,&data,&length,0,0,0);
                    };
                    if (res==0){
                        mtmemzero(data,length);
                        idsb->Unlock(data,length,0,0);
                        datasamples = length/format.nBlockAlign;
                        LEAVE();
                        return true;
                    };
                };
                idsb->Release();
                idsb = 0;
            };
        };
        ids->Release();
        ids = 0;
    };
    showerror(res,device);
    LEAVE();
    return false;
}

void MTDirectSoundDevice::uninit()
{
    ENTER("MTDirectSoundDevice::uninit");
    if (idsb){
        idsb->Release();
        idsb = 0;
    };
    if (ids){
        ids->Release();
        ids = 0;
    };
    LEAVE();
}

bool MTDirectSoundDevice::play()
{
    if (!idsb) return false;
    return (idsb->Play(0,0,DSBPLAY_LOOPING)==0);
}

bool MTDirectSoundDevice::stop()
{
    if (!idsb) return false;
    return (idsb->Stop()==0);
}

int MTDirectSoundDevice::getposition(bool playback)
{
    int status;
    int sl = format.nBlockAlign;
    int pos;

    if (!idsb) return -1;
    if (idsb->GetStatus((DWORD*)&status)==0){
        if (status & DSBSTATUS_BUFFERLOST) idsb->Restore();
        if (idsb->GetCurrentPosition((DWORD*)&pos,0)==0){
            return pos/sl+((playback)?0:delay);
        };
    };
    return -1;
}

bool MTDirectSoundDevice::getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2)
{
    int status;
    int sl = format.nBlockAlign;

    if (!idsb) return false;
    if (idsb->GetStatus((DWORD*)&status)==0){
        if (status & DSBSTATUS_BUFFERLOST) idsb->Restore();
        if (idsb->Lock(position*sl,length*sl,ptr1,(LPDWORD)lng1,ptr2,(LPDWORD)lng2,0)==0){
            *lng1 /= sl;
            *lng2 /= sl;
            return true;
        }
    };
    return false;
}

bool MTDirectSoundDevice::writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2)
{
    int sl = format.nBlockAlign;

    if (!idsb) return false;
    return (idsb->Unlock(ptr1,lng1*sl,ptr2,lng2*sl)==0);
}
//---------------------------------------------------------------------------
MTDirectSoundDeviceManager::MTDirectSoundDeviceManager():
MTAudioDeviceManager()
{
    int x;

    gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
    hdsound = LoadLibrary("DSOUND.DLL");
    if (hdsound!=0){
#		ifdef _DEBUG
            int infosize,fixedsize;
            void *info;
            VS_FIXEDFILEINFO *fixed;

            infosize = GetFileVersionInfoSize("DSOUND.DLL",(LPDWORD)&x);
            if (infosize>0){
                info = si->memalloc(infosize,MTM_ZERO);
                *(short*)info = infosize;
                if ((GetFileVersionInfo("DSOUND.DLL",0,infosize,info)) && (VerQueryValue(info,"\\",(void**)&fixed,(unsigned int*)&fixedsize))){
                    FLOG4("[Audio] DirectSound version %d.%d.%d.%d"NL,fixed->dwFileVersionMS>>16,fixed->dwFileVersionMS & 0xFFFF,fixed->dwFileVersionLS>>16,fixed->dwFileVersionMS & 0xFFFF);
                };
                si->memfree(info);
            };
#		endif
        *(int*)&dscreate = (int)GetProcAddress(hdsound,"DirectSoundCreate");
        *(int*)&dsenum = (int)GetProcAddress(hdsound,"DirectSoundEnumerateA");
        if ((dscreate) && (dsenum)) dsenum(dsenumproc,0);
    };
    for (x=0;x<ndsdev;x++){
        devicename[x] = dsdev[x]->name;
    };
}

MTDirectSoundDeviceManager::~MTDirectSoundDeviceManager()
{
    int x;

    FreeLibrary(hdsound);
    hdsound = 0;
    for (x=0;x<ndsdev;x++){
        DSDevice &cdev = *dsdev[x];
        if (cdev.name) si->memfree(cdev.name);
        if (cdev.guid) si->memfree(cdev.guid);
        si->memfree(&cdev);
        dsdev[x] = 0;
    };
    mtmemzero(devicename,sizeof(devicename));
    ndsdev = 0;
}

MTAudioDevice* MTDirectSoundDeviceManager::newdevice(int id)
{
    if ((id<0) || (id>=ndsdev)) return 0;
    return new MTDirectSoundDevice(dsdev[id]);
}

void MTDirectSoundDeviceManager::deldevice(MTAudioDevice *device)
{
    delete (MTDirectSoundDevice*)device;
}
//---------------------------------------------------------------------------
#endif
