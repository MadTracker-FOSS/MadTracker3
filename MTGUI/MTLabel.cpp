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
#include <math.h>
#include <stdio.h>
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
#include "MTLabel.h"
#include "MTWindow.h"
//---------------------------------------------------------------------------
// MTControl
//   MTLabel
//---------------------------------------------------------------------------
MTLabel::MTLabel(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_LABEL,tag,p,l,t,w,h),
autosize(false),
alength(128)
{
	flags |= MTCF_TRANSPARENT;
	if ((w==0) || (h==0)){
		width = 64;
		height = 20;
	};
	if (width<16) width = 16;
	if (height<16) height = 16;
	caption = (char*)si->memalloc(alength,MTM_ZERO);
	if (candesign){
		gi->setcontrolname(this,"label");
		strcpy(caption,name);
	};
}

MTLabel::~MTLabel()
{
	si->memfree(caption);
}

int MTLabel::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	char *c;
	int l,x,r;
	
	l = 0;
	c = caption;
	r = alength;
	while (true){
		x = f->readln(c,r);
		l += x;
		if (x==alength){
			alength += 64;
			caption = (char*)si->memrealloc(caption,alength);
			c = caption+l;
			r = 64;
		}
		else break;
	};
	l++;
/*
	alength = strlen(caption)+1;
	caption = (char*)si->memrealloc(caption,alength);
*/
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	f->read(&autosize,4);
	return csize+l+4;
}

int MTLabel::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	int l = strlen(caption)+1;
	int o,zero;
	
	zero = 0;
	o = ((l+3)>>2)<<2;
	f->write(caption,l);
	f->write(&zero,o-l);
	f->write(&autosize,4);
	return csize+o+4;
}

#define LabelNP (ControlNP+2)
int MTLabel::getnumproperties(int id)
{
	if (id==-1) return LabelNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTLabel::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[2] = {"Caption","AutoSize"};
	static int propflags[2] = {MTP_TEXT,MTP_BOOL};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=LabelNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTLabel::getproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		strcpy((char*)value,caption);
		break;
	case 1:
		*(int*)value = autosize;
		break;
	default:
		return false;
	};
	return true;
}

bool MTLabel::setproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		setcaption((const char*)value);
		return true;
	case 1:
		autosize = *(bool*)value;
		return true;
	default:
		return false;
	};
}

void MTLabel::draw(MTRect &rect)
{
	if (flags & MTCF_CANTDRAW) return;
	MTRect r = {left,top,left+width,top+height};
	skin->drawcontrol(this,r,0,0,0);
	MTControl::draw(rect);
}

void MTLabel::setcaption(const char *c)
{
	MTPoint ts;
	int w,ml;
	double r;
	
	if (!c) return;
	if (strlen(c)+1>alength){
		alength = strlen(c)+1;
		caption = (char*)si->memrealloc(caption,alength);
	};
	strcpy(caption,c);
	if (parent){
		if (autosize){
			if ((skin->gettextsize(this,caption,-1,&ts)) && (ts.y>0)){
				w = ts.x;
				ml = (int)((double)parent->dsk->width/NICE_RATIO);
				if (w>ml) w = ml;
				r = (double)w/(double)ts.y;
				if (r>NICE_RATIO*1.25){
					w = (int)(sqrt((double)(w*ts.y)*NICE_RATIO));
				};
				if (w!=ts.x) skin->gettextsize(this,caption,-1,&ts,w);
				setbounds(left,top,ts.x,ts.y);
			};
		};
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
