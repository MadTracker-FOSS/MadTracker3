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
#include "MTSign.h"
#include "../Headers/MTXSkin.h"
#include "MTGUI1.h"
#include "MTWindow.h"
//---------------------------------------------------------------------------
// MTControl
//   MTSign
//---------------------------------------------------------------------------
MTSign::MTSign(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_SIGN,tag,p,l,t,w,h),
sign(0)
{
	flags |= MTCF_TRANSPARENT;
	gi->setcontrolname(this,"sign");
	if ((w==0) || (h==0)){
		skin->getcontrolsize(MTC_SIGN,0,width,height);
	};
}

int MTSign::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	
	f->read(&sign,4);
	return csize+4;
}

int MTSign::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	
	f->write(&sign,4);
	return csize+4;
}

#define SignNP (ControlNP+1)
int MTSign::getnumproperties(int id)
{
	if (id==-1) return SignNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	if (id==ControlNP) return 3;
	return 0;
}

bool MTSign::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[1] = {"Sign"};
	static int propflags[1] = {MTP_LIST};
	static char *subname[11] = {"None","Information","Question","Exclamation","Error","Working","Loading","Saving","Downloading","Uploading","Authenticating"};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if ((id>>8)==ControlNP){
		id &= 0xFF;
		if (id>=3) return false;
		*name = subname[id];
		flags = -1;
		return true;
	};
	if (id>=SignNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTSign::getproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		*(int*)value = sign;
		break;
	default:
		return false;
	};
	return true;
}

bool MTSign::setproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		setsign(*(int*)value);
		return true;
	default:
		return false;
	};
}

void MTSign::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

void MTSign::setsign(int s)
{
	if (sign==s) return;
	skin->resetcontrol(this,false);
	sign = s;
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
