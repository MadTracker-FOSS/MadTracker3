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
#include "MTButton.h"
#include "MTTabControl.h"
#include "MTSysControls.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
// MTControl
//   MTButton
//---------------------------------------------------------------------------
MTButton::MTButton(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_BUTTON,tag,p,l,t,w,h),
imageindex(-1),
modalresult(MTDR_NULL),
shortcut(0),
hotkey(0),
autosize(false),
downtime(0),
hotkeyoffset(-1)
{
	flags |= (MTCF_TRANSPARENT|MTCF_ACCEPTINPUT);
	if (h==0){
		if (w==0) width = 64;
		else width = w;
		height = 22;
	}
	else{
		height = h;
		if (w==0){
			width = 64;
			autosize = true;
		}
		else width = w;
	};
	if (width<16) width = 16;
	if (height<16) height = 16;
	autopopup = false;
	caption = (char*)si->memalloc(256,MTM_ZERO);
	if (candesign){
		gi->setcontrolname(this,"button");
		strcpy(caption,name);
	};
}

MTButton::~MTButton()
{
	if (window) window->delhotkey(this);
	si->memfree(caption);
	if (popup) gi->delcontrol(popup);
}

int MTButton::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	int l,x;
	char newcap[256];

	f->readln(newcap,256);
	newcap[255] = 0;
	l = strlen(newcap)+1;
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	f->read(&imageindex,8);
	setcaption(newcap);
	return csize+l+8;
}

int MTButton::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	int l = strlen(caption)+1;
	int o,zero;
	
	zero = 0;
	o = ((l+3)>>2)<<2;
	f->write(caption,l);
	f->write(&zero,o-l);
	f->write(&imageindex,8);
	return csize+o+8;
}

#define ButtonNP (ControlNP+3)
int MTButton::getnumproperties(int id)
{
	if (id==-1) return ButtonNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTButton::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[3] = {"Caption","ImageIndex","ModalResult"};
	static int propflags[3] = {MTP_TEXT,MTP_INT,MTP_INT};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=ButtonNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTButton::getproperty(int id,void *value)
{
	int l,o;
	char *e,*e2,*c;

	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		o = hotkeyoffset;
		c = (char*)value;
		e = caption;
		while (true){
			e2 = strchr(e,'&');
			if (e2){
				l = e2-e+1;
				if ((o>=0) && (l>=o)){
					strncpy(c,e,o);
					c += o;
					*c++ = '&';
					strncpy(c,e+o,l-o);
					c += l-o;
				}
				else{
					strncpy(c,e,l);
					c += l;
				};
				*c++ = '&';
				*c = 0;
				e = e2+1;
				o -= l;
			}
			else{
				l = strlen(e);
				if ((o>=0) && (l>=o)){
					strncpy(c,e,o);
					c += o;
					*c++ = '&';
					strcpy(c,e+o);
				}
				else{
					strcpy(c,e);
				};
				break;
			};
		};
		break;
	case 1:
		*(int*)value = imageindex;
		break;
	case 2:
		*(int*)value = modalresult;
		break;
	default:
		return false;
	};
	return true;
}

bool MTButton::setproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		setcaption((char*)value);
		return true;
	case 1:
		imageindex = *(int*)value;
		break;
	case 2:
		modalresult = *(int*)value;
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

void MTButton::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

bool MTButton::message(MTCMessage &msg)
{
	float vx,vy;

	if (popup){
		if (msg.msg==MTCM_MOUSEDOWN){
			MTPoint p = {left,top};
			downtime = si->syscounter();
			gi->getmousevector(vx,vy);
			if (vy>=-0.4) p.y += height;
			else p.y -= popup->height;
			popup->popup(parent,p);
		}
		else if (msg.msg==MTCM_MOUSEUP){
			if (si->syscounter()-downtime>500){
				parent->focus(this);
			};
		};
	};
	if ((msg.msg==MTCM_HOTKEY) || ((msg.msg==MTCM_MOUSECLICK) && (msg.button==DB_LEFT)) || ((msg.msg==MTCM_KEYDOWN) && (msg.key==KB_SPACE))){
		if (parent){
			MTCMessage msg = {MTCM_ACTION,0,this};
			parent->message(msg);
			if (modalresult!=MTDR_NULL){
//				if (parent->modalparent) parent->modalparent->modalresult = modalresult;
				parent->modalresult = modalresult;
				MTCMessage msg = {MTCM_CLOSE,0,parent};
				parent->message(msg);
			};
			return true;
		};
	};
	return MTControl::message(msg);
}

void MTButton::setcaption(const char *c)
{
	char *e,*e1,*e2;
	int w;

	hotkeyoffset = -1;
	strcpy(caption,c);
	e = caption;
	while ((e=strchr(e,'&'))){
		e1 = e;
		e2 = e+1;
		if (*e2=='&') e += 2;
		else{
			hotkey = *e2;
			hotkeyoffset = e-caption;
		};
		while (*e2){
			*e1++ = *e2++;
		};
		*e1 = 0;
	};
	if ((hotkey>='a') && (hotkey<='z')) hotkey += 'A'-'a';
	if (autosize){
		w = getautowidth();
		if (w<64) w = 64;
		setbounds(left,top,w,height);
	};
	if ((hotkey) && (window)) window->addhotkey(this,hotkey);
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTButton::setimage(int index)
{
	if (imageindex==index) return;
	imageindex = index;
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTButton::setautosize(bool autosize)
{
	this->autosize = autosize;
}

int MTButton::getautowidth()
{
	MTPoint bs;
	
	if (!parent) return 0;
	if (caption[0]){
		if (!skin->gettextsize(this,caption,-1,&bs)) return width;
		if (imageindex>=0) bs.x += sysimages.iw+2;
	}
	else bs.x = sysimages.iw;
	bs.x += 16;
	return bs.x;
}
//---------------------------------------------------------------------------
