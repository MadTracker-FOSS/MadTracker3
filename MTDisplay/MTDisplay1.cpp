//---------------------------------------------------------------------------
//
//	MadTracker Display Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <windows.h>
#ifdef _DEBUG
#include <stdio.h>
#endif
#include "MTDisplay1.h"
#include "MTGDIDevice.h"
#ifndef NODIRECTX
#include "MTDXDevice.h"
#endif
#include "../Headers/MTXSystem.h"
#include "../Headers/MTXSkin.h"
#include "../Headers/MTXControls.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
static const char *disname = {"MadTracker Display Core"};
static const int displayversion = 0x30000;
static const MTXKey displaykey = {0,0,0,0};
MTXInterfaces i;
MTDisplayInterface *di;
MTInterface *mtinterface;
MTSystemInterface *si;
MTGUIInterface *gi;
MTDisplayPreferences displayprefs = {0,false,-0.5f,-0.5f,0.0f,0.0f};
MTBitmap *skinbmp[16];
MTBitmap *screen;
int cdisplaytype = -1;
MTDesktop *desktops[32];
int ndesktops;
MTGDIDeviceManager *gdimanager;
#ifndef NODIRECTX
MTDXDeviceManager *dxmanager;
#endif
MTDisplayDevice *cdevice;
MTDisplayDeviceManager *cmanager;
MTDevice *devices[MAX_DISPLAYDEVICES];
int ndevices;
//---------------------------------------------------------------------------
MTDisplayInterface::MTDisplayInterface():
fullscreen(false)
{
	type = displaytype;
	key = &displaykey;
	name = disname;
	version = displayversion;
	status = 0;
}

bool MTDisplayInterface::init()
{
	int x;
	MTConfigFile *conf;
	char device[64];

	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	if (!si) return false;
	ENTER("MTDisplayInterface::init");
	LOGD("%s - [Display] Initializing..."NL);
	gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
	gdimanager = new MTGDIDeviceManager();
	adddevicemanager(gdimanager);
#ifndef NODIRECTX
	dxmanager = new MTDXDeviceManager();
	adddevicemanager(dxmanager);
#endif
	if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
		if (conf->setsection("MTDisplay")){
			if (conf->getparameter("Device",device,MTCT_STRING,sizeof(device))){
				displayprefs.device = 0;
				if (stricmp(device,"DXPrimary")==0) displayprefs.device = 1;
				else{
					for (x=0;x<ndevices;x++){
						if (stricmp(devices[x]->devicename,device)==0){
							displayprefs.device = x;
							break;
						};
					};
				};
			};
			conf->getparameter("Fullscreen",&displayprefs.fullscreen,MTCT_BOOLEAN,sizeof(displayprefs.fullscreen));
			conf->getparameter("TexelAdjustX",&displayprefs.texeladjustx,MTCT_FLOAT,sizeof(displayprefs.texeladjustx));
			conf->getparameter("TexelAdjustY",&displayprefs.texeladjusty,MTCT_FLOAT,sizeof(displayprefs.texeladjusty));
			conf->getparameter("SkinTexelAdjustX",&displayprefs.skintexeladjustx,MTCT_FLOAT,sizeof(displayprefs.skintexeladjustx));
			conf->getparameter("SkinTexelAdjustY",&displayprefs.skintexeladjusty,MTCT_FLOAT,sizeof(displayprefs.skintexeladjusty));
		};
		mtinterface->releaseconf(conf);
	};
	setdevice(-1,true);
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTDisplayInterface::uninit()
{
	ENTER("MTDisplayInterface::uninit");
	LOGD("%s - [Display] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	if (gi) gi->freedisplay();
	if (screen){
		delbitmap(screen);
		screen = 0;
	};
	if (cdevice){
		cdevice->uninit();
		cmanager->deldevice(cdevice);
		cdevice = 0;
		cmanager = 0;
	};
	deldevicemanager(gdimanager);
	delete gdimanager;
#ifndef NODIRECTX
	deldevicemanager(dxmanager);
	delete dxmanager;
#endif
	di->fullscreen = false;
	LEAVE();
}

#ifdef _DEBUG
void MTCT menusetdevice(MTShortcut *s,MTControl *c,MTUndo*)
{
	if (!c) return;
	if (gi) gi->setmouseshape(DCUR_WORKING);
	di->setdevice(c->tag);
	if (gi) gi->restoremouseshape();
}

void MTCT menusetfullscreen(MTShortcut *s,MTControl *c,MTUndo*)
{
	displayprefs.fullscreen = !displayprefs.fullscreen;
	if (gi) gi->setmouseshape(DCUR_WORKING);
	di->setdevice(-1);
	if (gi) gi->restoremouseshape();
}
#endif

void MTDisplayInterface::start()
{
#ifdef _DEBUG
	int x;
	MTDesktop *dsk = 0;
	MTMenuItem *item;
	MTMenu *menu;
	if (gi) dsk = gi->getdesktop(0);
	if (dsk){
		menu = (MTMenu*)gi->newcontrol(MTC_MENU,0,dsk,0,0,0,0,0);
		dsk->popup->additem("|MTDisplay",0,0,false,0);
		item = (MTMenuItem*)dsk->popup->additem("Graphical Device",60,0,false,0);
		item->submenu = menu;
		for (x=0;x<ndevices;x++){
			item = (MTMenuItem*)menu->additem(devices[x]->devicename,60,0,false,0);
			item->tag = x;
			item->command = menusetdevice;
		};
		item = (MTMenuItem*)dsk->popup->additem("Fullscreen",60,0,false,0);
		item->command = menusetfullscreen;
	};
#endif
}

void MTDisplayInterface::stop()
{
}

void MTDisplayInterface::processcmdline(void *params)
{

}

void MTDisplayInterface::showusage(void *out)
{

}

int MTDisplayInterface::config(int command,int param)
{
	if (command<0) return (int)&displayprefs;
	return 0;
}

int MTDisplayInterface::getnumdevices()
{
	return ndevices;
}

const char* MTDisplayInterface::getdevicename(int id)
{
	if ((id<0) || (id>=ndevices)) return 0;
	return devices[id]->devicename;
}

void MTDisplayInterface::setdevice(int id,bool silent)
{
	int x;
	bool oldfullscreen = displayprefs.fullscreen;
	MTDisplayDevice *newdevice;
	MTDisplayDeviceManager *newmanager;
	MTCMessage msg = {MTCM_NOTIFY,0,0,1};

	FENTER2("MTDisplayInterface::setdevice(%d,%d)",id,silent);
	if (gi) gi->setmouseshape(DCUR_WORKING);
	if (id>=0) displayprefs.device = id;
	else{
		if (cdevice){
			if ((!silent) && (gi)) gi->freedisplay();
			if (screen){
				delbitmap(screen);
				screen = 0;
			};
			cdevice->setfullscreen(displayprefs.fullscreen);
			screen = newbitmap(MTB_SCREEN,0,0);
			goto leave;
		};
		id = displayprefs.device;
	};
	if ((id<0) || (id>=ndevices)) id = 0;
retry:
	LOGD("%s - [Display] Changing device: ");
	LOG(devices[id]->devicename);
	LOG(NL);
	for (x=0;x<ndesktops;x++){
		desktops[x]->flags |= MTCF_DONTDRAW|MTCF_DONTFLUSH;
	};
	if ((!silent) && (gi)) gi->freedisplay();
	if (screen){
		delbitmap(screen);
		screen = 0;
	};
	if (cdevice) cdevice->uninit();
	newmanager = devices[id]->manager;
	newdevice = newmanager->newdevice(devices[id]->id);
	if (newdevice){
		if (!newdevice->init(displayprefs.fullscreen)){
			LOGD("%s - [Dispay] ERROR: Cannot initialize devices!");
			if ((displayprefs.device) || (displayprefs.fullscreen)){
				LOG(" (Retrying in safe mode)"NL);
				id = 0;
				displayprefs.device = 0;
				displayprefs.fullscreen = false;
				goto retry;
			};
			LOG(NL);
		};
		screen = newdevice->newbitmap(MTB_SCREEN,0,0);
		if (cdevice){
			if (!cdevice->switchto(newdevice)){
				LOGD("%s - [Display] ERROR: Cannot switch between devices!"NL);
				newmanager->deldevice(newdevice);
				newdevice = cdevice;
				newmanager = cmanager;
				cdevice->init(displayprefs.fullscreen);
				screen = cdevice->newbitmap(MTB_SCREEN,0,0);
			}
			else{
				cmanager->deldevice(cdevice);
			};
		};
		cdevice = newdevice;
		cmanager = newmanager;
	};
leave:
	di->fullscreen = displayprefs.fullscreen;
	cdevice->checkbitmaps();
	if (silent){
		displayprefs.fullscreen = oldfullscreen;
		for (x=0;x<ndesktops;x++){
			desktops[x]->flags &= (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
		};
	}
	else{
		if (gi) gi->setdisplay(screen);
		for (x=0;x<ndesktops;x++){
			MTDesktop &cdsk = *desktops[x];
			cdsk.message(msg);
			HWND wnd = (HWND)cdsk.mwnd;
			if (wnd){
				RECT r;
				GetClientRect(wnd,&r);
				cdsk.setbounds(0,0,r.right,r.bottom);
			};
			cdsk.flags &= (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
			cdsk.draw(NORECT);
		};
	};
	if (gi) gi->restoremouseshape();
	LEAVE();
}

bool MTDisplayInterface::adddevicemanager(MTDisplayDeviceManager *manager)
{
	int x,y;

	if (ndevices==MAX_DISPLAYDEVICES) return false;
	FENTER1("MTDisplayInterface::adddevicemanager(%.8X)",manager);
	y = ndevices;
	for (x=0;x<MAX_DISPLAYDEVICES,y<MAX_DISPLAYDEVICES;x++){
		if (!manager->devicename[x]) break;
		MTDevice *cdevice = mtnew(MTDevice);
		cdevice->devicename = manager->devicename[x];
		cdevice->id = x;
		cdevice->manager = manager;
		devices[y++] = cdevice;
	};
	ndevices = y;
	LEAVE();
	return true;
}

void MTDisplayInterface::deldevicemanager(MTDisplayDeviceManager *manager)
{
	int x,y;

	FENTER1("MTDisplayInterface::deldevicemanager(%.8X)",manager);
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

MTBitmap *MTDisplayInterface::newbitmap(int flags,int w,int h)
{
	return cdevice->newbitmap(flags,w,h);
}

MTBitmap *MTDisplayInterface::newresbitmap(int flags,MTResources *res,int resid,int colorkey)
{
	return cdevice->newresbitmap(flags,res,resid,colorkey);
}

MTBitmap *MTDisplayInterface::newfilebitmap(int flags,const char *filename,int colorkey)
{
	return cdevice->newfilebitmap(flags,filename,colorkey);
}

MTBitmap *MTDisplayInterface::newbmpbitmap(int flags,MTBitmap &orig,int colorkey)
{
	return cdevice->newbmpbitmap(flags,orig,colorkey);
}

void MTDisplayInterface::setskinbitmap(int bmpid,MTBitmap *newskin)
{
	skinbmp[bmpid] = newskin;
}

MTMask *MTDisplayInterface::newmask(int w,int h)
{
	return cdevice->newmask(w,h);
}

void MTDisplayInterface::delbitmap(MTBitmap *bmp)
{
	cdevice->delbitmap(bmp);
}

void MTDisplayInterface::delmask(MTMask *mask)
{
	cdevice->delmask(mask);
}

void MTDisplayInterface::adddesktop(MTWinControl *dsk)
{
	desktops[ndesktops++] = (MTDesktop*)dsk;
}

void MTDisplayInterface::deldesktop(MTWinControl *dsk)
{
	int x;
	
	for (x=0;x<ndesktops;x++){
		if (desktops[x]==dsk){
			desktops[x] = desktops[--ndesktops];
			break;
		};
	};
	desktops[ndesktops] = 0;
}

MTWinControl *MTDisplayInterface::getdefaultdesktop()
{
	if (ndesktops==1) return desktops[0];
	return 0;
}

void MTDisplayInterface::checkbitmaps()
{
	if (gi) gi->setmouseshape(DCUR_WORKING);
	if (cdevice) cdevice->checkbitmaps();
	if (gi) gi->restoremouseshape();
}

MTBitmap* MTDisplayInterface::getscreen()
{
	return screen;
}

void MTDisplayInterface::setfocus(bool focused)
{
	if (cdevice) cdevice->setfocus(focused);
}

void MTDisplayInterface::sync()
{
	if (cdevice) cdevice->sync();
}
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = (MTInterface*)mti;
	if (!di) di = new MTDisplayInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)di;
	return &i;
}

}
//---------------------------------------------------------------------------
