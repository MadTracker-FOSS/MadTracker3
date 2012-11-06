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
#include "MTCheckBox.h"
#include "MTWindow.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
// MTControl
//   MTCheckBox
//---------------------------------------------------------------------------
MTCheckBox::MTCheckBox(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_CHECKBOX,tag,p,l,t,w,h),
radio(false),
state(0),
undef(false)
{
	flags |= (MTCF_TRANSPARENT|MTCF_ACCEPTINPUT);
	if ((w==0) || (h==0)){
		width = 128;
		height = 16;
	};
	if (width<16) width = 16;
	if (height<16) height = 16;
	caption = (char*)si->memalloc(256,MTM_ZERO);
	if (candesign){
		gi->setcontrolname(this,"checkbox");
		strcpy(caption,name);
	};
}

MTCheckBox::~MTCheckBox()
{
	si->memfree(caption);
}

int MTCheckBox::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	int l,x;

	f->readln(caption,256);
	caption[255] = 0;
	l = strlen(caption)+1;
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	f->read(&radio,8);
	undef = (state==2);
	return csize+l+8;
}

int MTCheckBox::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	int l = strlen(caption)+1;
	int o,zero;
	
	zero = 0;
	o = ((l+3)>>2)<<2;
	f->write(caption,l);
	f->write(&zero,o-l);
	l = (int)radio;
	f->write(&l,4);
	f->write(&state,4);
	return csize+o+8;
}

#define CheckBoxNP (ControlNP+3)
int MTCheckBox::getnumproperties(int id)
{
	if (id==-1) return CheckBoxNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	if (id==ControlNP+2) return 3;
	return 0;
}

bool MTCheckBox::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[3] = {"Caption","Radio","State"};
	static int propflags[3] = {MTP_TEXT,MTP_BOOL,MTP_LIST};
	static char *subname[3] = {"Unchecked","Checked","Undefined"};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if ((id>>8)==ControlNP+2){
		id &= 0xFF;
		if (id>=3) return false;
		*name = subname[id];
		flags = -1;
		return true;
	};
	if (id>=CheckBoxNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTCheckBox::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		strcpy((char*)value,caption);
		break;
	case 1:
		iv = radio;
		break;
	case 2:
		iv = state;
		break;
	default:
		return false;
	};
	return true;
}

bool MTCheckBox::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		strcpy(caption,(char*)value);
		break;
	case 1:
		radio = iv!=0;
		break;
	case 2:
		setstate(iv,true);
		return true;
	default:
		return false;
	};
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	return true;
}

void MTCheckBox::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

bool MTCheckBox::message(MTCMessage &msg)
{
	int newstate = state;

	if ((msg.msg==MTCM_MOUSEDOWN) || ((msg.msg==MTCM_KEYDOWN) && (msg.key==KB_SPACE))){
		if (radio){
			if (parent){
				int x;
				for (x=0;x<parent->ncontrols;x++){
					MTCheckBox &cchk = *(MTCheckBox*)parent->controls[x];
					if ((cchk.guiid==MTC_CHECKBOX) && (cchk.radio)) cchk.setstate(0,true);
				};
				newstate = 1;
			};
		}
		else{
			newstate++;
			if (undef){
				if (newstate>=3) newstate = 0;
			}
			else if (newstate>=2) newstate = 0;
		};
		setstate(newstate,true);
		return true;
	};
	return MTControl::message(msg);
}

void MTCheckBox::setstate(int c,bool touched)
{
	if (state==c) return;
	if (c==2) undef = true;
	else if (!touched) undef = false;
	skin->notify(this,0,c,touched);
	state = c;
	if (parent){
		MTCMessage msg = {(touched)?MTCM_TOUCHED:MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
