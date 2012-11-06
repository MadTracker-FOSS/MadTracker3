//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
#include "MTProgress.h"
#include "MTWindow.h"
//---------------------------------------------------------------------------
// MTControl
//   MTProgress
//---------------------------------------------------------------------------
MTProgress::MTProgress(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_PROGRESS,tag,p,l,t,w,h),
pos(0),
maxpos(100),
step(1)
{
	int tmp;

	flags |= MTCF_TRANSPARENT;
	skin->getcontrolsize(MTC_PROGRESS,0,tmp,height);
	if ((width==0) || (height==0)){
		width = 128;
		height = 32;
	};
	gi->setcontrolname(this,"progress");
}

int MTProgress::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	
	f->write(&pos,12);
	return csize+12;
}

int MTProgress::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	
	f->read(&pos,12);
	return csize+12;
}

#define ProgressNP (ControlNP+3)
int MTProgress::getnumproperties(int id)
{
	if (id==-1) return ProgressNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTProgress::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[3] = {"Position","Maximum","Increment"};
	static int propflags[3] = {MTP_INT,MTP_INT,MTP_INT};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=ProgressNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTProgress::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		iv = pos;
		break;
	case 1:
		iv = maxpos;
		break;
	case 2:
		iv = step;
		break;
	default:
		return false;
	};
	return true;
}

bool MTProgress::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		setposition(iv);
		return true;
	case 1:
		maxpos = iv;
		break;
	case 2:
		step = iv;
		break;
	default:
		return false;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	return true;
}

void MTProgress::setbounds(int l,int t,int w,int h)
{
	skin->getcontrolsize(MTC_PROGRESS,0,w,h);
	MTControl::setbounds(l,t,w,h);
}

void MTProgress::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

void MTProgress::setposition(int p)
{
	if (pos==p) return;
	pos = p;
	if (pos>maxpos) pos = maxpos;
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTProgress::stepit()
{
	setposition(pos+step);
}
//---------------------------------------------------------------------------
