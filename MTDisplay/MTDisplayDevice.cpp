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
#include "MTDisplay1.h"
#include "MTDisplayDevice.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
MTDisplayDevice::MTDisplayDevice():
nbitmaps(0),
nmasks(0)
{
	mtmemzero(bitmaps,sizeof(bitmaps));
}

bool MTDisplayDevice::switchto(MTDisplayDevice *newdevice)
{
	int x;
	MTBitmap *res;
	bool ok = true;

	FENTER1("MTDisplayDevice::switchto(%.8X)",newdevice);
// Unload the bitmaps first
	for (x=0;x<nbitmaps;x++) bitmaps[x]->unload();
// Create new bitmaps
	for (x=0;x<nbitmaps;x++){
		MTBitmap &cbmp = *bitmaps[x];
		if (cbmp.morig) res = newdevice->newbmpbitmap(cbmp.flags & MTB_NOINHERIT,*cbmp.morig,cbmp.mck);
		else if (cbmp.mres) res = newdevice->newresbitmap(cbmp.flags & MTB_NOINHERIT,cbmp.mres,cbmp.mresid,cbmp.mck);
		else if (cbmp.mfilename) res = newdevice->newfilebitmap(cbmp.flags & MTB_NOINHERIT,cbmp.mfilename,cbmp.mck);
		else res = newdevice->newbitmap(cbmp.flags & MTB_NOINHERIT,cbmp.width,cbmp.height);
		if (res){
			if (res->loaded){
				res->changeproc = cbmp.changeproc;
				res->param = cbmp.param;
				cbmp.id = (int)res;
			}
			else{
				LOGD("%s - [Display] ERROR: Could not load bitmap!"NL);
				ok = false;
				break;
			};
		};
	};
	if (ok){
// Delete old bitmaps
		while (nbitmaps>0){
			MTBitmap &cbmp = *bitmaps[0];
			if (cbmp.changeproc) cbmp.changeproc(&cbmp,(MTBitmap*)cbmp.id,cbmp.param);
			else if (cbmp.param) *(int*)cbmp.param = cbmp.id;
			delbitmap(&cbmp);
		};
	}
	else{
// Delete new bitmaps and re-load the old ones
		while (newdevice->nbitmaps>0){
			newdevice->delbitmap(newdevice->bitmaps[0]);
		};
		for (x=0;x<nbitmaps;x++) bitmaps[x]->load();
	};
	LEAVE();
	return ok;
}

void MTDisplayDevice::add(MTBitmap *bmp)
{
	bitmaps[nbitmaps++] = bmp;
}

void MTDisplayDevice::remove(MTBitmap *bmp)
{
	int x;

	for (x=0;x<nbitmaps;x++){
		if (bitmaps[x]==bmp){
			nbitmaps--;
			bitmaps[x] = bitmaps[nbitmaps];
			bitmaps[nbitmaps] = 0;
			break;
		};
	};
}

void MTDisplayDevice::add(MTMask *mask)
{
	masks[nmasks++] = mask;
}

void MTDisplayDevice::remove(MTMask *mask)
{
	int x;

	for (x=0;x<nmasks;x++){
		if (masks[x]==mask){
			nmasks--;
			masks[x] = masks[nmasks];
			masks[nmasks] = 0;
			break;
		};
	};
}

void MTDisplayDevice::checkbitmaps()
{
}

void MTDisplayDevice::setfocus(bool focused)
{
}

void MTDisplayDevice::sync()
{
}
//---------------------------------------------------------------------------
MTDisplayDeviceManager::MTDisplayDeviceManager()
{
	mtmemzero(devicename,sizeof(devicename));
}
//---------------------------------------------------------------------------
