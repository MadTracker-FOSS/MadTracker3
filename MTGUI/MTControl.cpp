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
#include "MTControl.h"
#include "../Headers/MTXSkin.h"
#include "MTWindow.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
MTControl *overctrl,*btnctrl;
MTCMessage leavemsg = {MTCM_LEAVE};
MTCMessage entermsg = {MTCM_ENTER};
MTCMessage btnupmsg = {MTCM_MOUSEUP,0,0,0,0,DB_LEFT};
//---------------------------------------------------------------------------
// MTControl
//---------------------------------------------------------------------------
MTControl::MTControl(int id,int tg,MTWinControl *p,int l,int t,int w,int h):
parent(p),
guiid(id),
uid(0),
name(0),
tag(tg),
flags(0),
left(l),
top(t),
width(w),
height(h),
align(MTCA_TOPLEFT),
direct(false),
timercount(0),
popup(0),
autopopup(true),
skindata(0),
cborder(-1),
moving(false),
sizing(false),
triggered(false)
{
#ifdef _DEBUG
	name = (char*)si->memalloc(64,MTM_ZERO);
#else
	if (candesign) name = (char*)si->memalloc(64,MTM_ZERO);
#endif
	if ((guiid==MTC_WINDOW) || (guiid==MTC_TABSHEET)) window = (MTWindow*)this;
	else if ((p) && (p->window)) window = p->window;
	else if ((p) && ((p->guiid==MTC_WINDOW) || (p->guiid==MTC_TABSHEET))) window = (MTWindow*)p;
	else window = 0;
}

MTControl::~MTControl()
{
	if (this==overctrl) overctrl = 0;
	if (this==btnctrl) btnctrl = 0;
	if (name) si->memfree(name);
}

int MTControl::loadfromstream(MTFile *f,int size,int flags)
{
	int l,x;
	struct{
		int l,t,w,h;
	} n;
	char buf[64];
	
	f->read(&guiid,8);
	f->readln(buf,64);
	buf[63] = 0;
	l = strlen(buf)+1;
	if (name) strcpy(name,buf);
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	x = this->flags & (MTCF_TRANSPARENT|MTCF_ACCEPTINPUT|MTCF_ACCEPTCTRLS|MTCF_DONTRESIZE|MTCF_NOTIFYPOS);
	f->read(&tag,8);
	this->flags |= x;
	f->read(&n,sizeof(n));
	f->read(&align,8);
	direct = false;
	if (guiid==MTC_WINDOW) this->flags |= MTCF_HIDDEN;
	x = this->flags;
	this->flags |= MTCF_DONTDRAW;
	setbounds(n.l,n.t,n.w,n.h);
	this->flags = x;
	return 40+l;
}

int MTControl::savetostream(MTFile *f,int flags)
{
	int l = (name)?strlen(name)+1:1;
	int bflags;
	int zero = 0;
	
	l = ((l+3)>>2)<<2;
	f->write(&guiid,8);
	if (name){
		bflags = strlen(name)+1;
		f->write(name,bflags);
		bflags = l-bflags;
		f->write(&zero,bflags);
	}
	else f->write(&zero,4);
	bflags = this->flags;
	this->flags &= (~(MTCF_FOCUSED|MTCF_SELECTED|MTCF_OVER|MTCF_NEEDUPDATE|MTCF_TRANSPARENT));
	f->write(&tag,28);
	this->flags = bflags;
	bflags = (int)direct;
	f->write(&bflags,4);
	return 40+l;
}

int MTControl::getnumproperties(int id)
{
	if (id==-1) return ControlNP;
	if (id==7) return 9;
	return 0;
}

bool MTControl::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[9] = {"UID","Name","Tag","Left","Top","Width","Height","Align","Enabled"};
	static int propflags[9] = {MTP_INT,MTP_TEXT,MTP_INT,MTP_INT,MTP_INT,MTP_INT,MTP_INT,MTP_LIST,MTP_BOOL};
	static char *subname[9] = {"Top-left","Top-right","Bottom-left","Bottom-right","Left","Right","Top","Bottom","Client"};
	
	if ((id>>8)==7){
		id &= 0xFF;
		if (id>=9) return false;
		*name = subname[id];
		flags = -1;
		return true;
	};
	if (id>=ControlNP) return false;
	*name = propname[id];
	flags = propflags[id];
	return true;
}

bool MTControl::getproperty(int id,void *value)
{
	int &iv = *((int*)value);
	static const int revalignmap[16] = {0,1,2,3,4,5,0,0,6,0,7,0,8,0,0,0};
	
	if ((id>=3) && (id<=6) && (guiid & 2) && (flags & MTCF_BORDER)){
		switch (id){
		case 3:
			iv = left-2;
			break;
		case 4:
			iv = top-2;
			break;
		case 5:
			iv = width+4;
			break;
		case 6:
			iv = height+4;
			break;
		};
	}
	else{
		switch (id){
		case 0:
			iv = uid;
			break;
		case 1:
			strcpy((char*)value,name);
			break;
		case 2:
			iv = tag;
			break;
		case 3:
			iv = left;
			break;
		case 4:
			iv = top;
			break;
		case 5:
			iv = width;
			break;
		case 6:
			iv = height;
			break;
		case 7:
			iv = revalignmap[align];
			break;
		case 8:
			iv = (flags & MTCF_DISABLED)?0:1;
			break;
		default:
			return false;
		};
	};
	return true;
}

bool MTControl::setproperty(int id,void *value)
{
	int &iv = *((int*)value);
	static const int alignmap[16] = {0,1,2,3,4,5,8,10,12,0,0,0};
	
	if (window) window->modified = true;
	if ((id>=3) && (id<=6) && (guiid & 2) && (flags & MTCF_BORDER)){
		switch (id){
		case 3:
			setbounds(iv+2,top,width,height);
			break;
		case 4:
			setbounds(left,iv+2,width,height);
			break;
		case 5:
			setbounds(left,top,iv-4,height);
			break;
		case 6:
			setbounds(left,top,width,iv-4);
			break;
		};
	}
	else{
		switch (id){
		case 0:
			uid = iv;
			break;
		case 1:
			strcpy(name,(char*)value);
			break;
		case 2:
			tag = iv;
			break;
		case 3:
			setbounds(iv,top,width,height);
			break;
		case 4:
			setbounds(left,iv,width,height);
			break;
		case 5:
			setbounds(left,top,iv,height);
			break;
		case 6:
			setbounds(left,top,width,iv);
			break;
		case 7:
			if (guiid==MTC_WINDOW) return false;
			align = alignmap[iv];
			break;
		case 8:
			flags &= (~MTCF_DISABLED);
			if (iv==0) flags |= MTCF_DISABLED;
			break;
		default:
			return false;
		};
	};
	return true;
}

void MTControl::setbounds(int l,int t,int w,int h)
{
	int ol,ot,ow,oh;
	int cl,ct,cw,ch;
	MTCMessage msg = {MTCM_BOUNDS,0,this};
	
	ol = left;
	ot = top;
	ow = width;
	oh = height;
	left = l;
	top = t;
	if (design){
		if ((w>0) && (w<8)) w = 8;
		if ((h>0) && (h<8)) h = 8;
	};
	if (w>0) width = w;
	if (h>0) height = h;
	msg.x = left;
	msg.y = top;
	msg.w = width;
	msg.h = height;
	if (parent){
		if ((!design) && (align) && ((parent->flags & MTCF_DONTRESIZE)==0)){
			MTControl &cctrl = *parent;
			flags |= MTCF_DONTRESIZE;
			cl = cctrl.left;
			ct = cctrl.top;
			cw = cctrl.width;
			ch = cctrl.height;
			if (align & 0x1) cl += w-ow;
			if (align & 0x2) ct += h-oh;
			if (align & 0x4) ch += h-oh;
			if (align & 0x8) cw += w-ow;
			cctrl.setbounds(cl,ct,cw,ch);
			flags &= (~MTCF_DONTRESIZE);
		};
		if ((flags & MTCF_CANTDRAW)==0){
			parent->message(msg);
			msg.ctrl = parent;
			msg.dr.left = ol;
			msg.dr.top = ot;
			msg.dr.right = ol+ow;
			msg.dr.bottom = ot+oh;
			msg.msg = MTCM_CHANGE;
			parent->message(msg);
		};
	};
	if ((guiid & 2) && ((flags & MTCF_CANTDRAW)==0)) message(msg);
}

bool MTControl::checkbounds(int &l,int &t,int &w,int &h)
{
	bool ok = true;

	if (w<16){
		w = 16;
		ok = false;
	};
	if (h<16){
		h = 16;
		ok = false;
	};
	return ok;
}

void MTControl::getrect(MTRect &r,int client)
{
	r.left = left;
	r.top = top;
	r.right = left+width;
	r.bottom = top+height;
}

void MTControl::switchflags(int f,bool set)
{
	int newflags = (flags & (~f));

	if (set) newflags |= f;
	if (newflags!=flags){
		if (f & MTCF_FOCUSED) message((set)?entermsg:leavemsg);
		else if ((f & MTCF_HIDDEN) && (set)) message(leavemsg);
		if ((design) && (flags & MTCF_DONTSAVE) && (f==MTCF_SELECTED)) return;
		flags = newflags;
		if (f & MTCF_NOTIFYPOS){
			if (parent) parent->switchflags(MTCF_NOTIFYPOS,set);
			if (f==MTCF_NOTIFYPOS) return;
		};
		if (guiid & 2){
			//   if (guiid==MTC_WINDOW){
			if (f & MTCF_OVER) return;
			/*   }
			else{
			if (f & (MTCF_OVER|MTCF_FOCUSED)) return;
		};*/
		};
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this,f,set};
			parent->message(msg);
		};
	};
}

void MTControl::draw(MTRect &rect)
{
	if ((flags & MTCF_CANTDRAW)==0){
		if ((direct) || ((parent) && (parent->direct))) flags |= MTCF_NEEDUPDATE;
	};
	if (flags & MTCF_DONTDRAW) return;
	if ((parent) && (design) && (guiid!=MTC_WINDOW) && ((flags & MTCF_DONTSAVE)==0)){
		if (flags & MTCF_SELECTED){
			if (&rect){
				MTRect r = {0,0,width,height};
				cliprect(r,rect);
				parent->fillcolor(r.left+left,r.top+top,r.right-r.left,r.bottom-r.top,skin->getcolor(SC_EDIT_SELECTION),128);
			}
			else parent->fillcolor(left,top,width,height,skin->getcolor(SC_EDIT_SELECTION),128);
		};
		if (parent->open(0)){
			parent->point(left,top,skin->getcolor(SC_EDIT_SELECTED));
			parent->point(left+width-1,top,skin->getcolor(SC_EDIT_SELECTED));
			parent->point(left,top+height-1,skin->getcolor(SC_EDIT_SELECTED));
			parent->point(left+width-1,top+height-1,skin->getcolor(SC_EDIT_SELECTED));
			if (flags & MTCF_OVER){
				parent->setpen(skin->getcolor(SC_EDIT_SELECTED));
				parent->moveto(left,top);
				parent->lineto(left+width-1,top);
				parent->lineto(left+width-1,top+height-1);
				parent->lineto(left,top+height-1);
				parent->lineto(left,top);
			};
			parent->close(0);
		};
	};
}

bool MTControl::message(MTCMessage &msg)
{
	switch (msg.msg){
	case MTCM_KEYDOWN:
		if (msg.key==KB_APPS){	// Context-menu key
			MTPoint p = {8,8};
			if (popup) popup->popup(this,p);
			return true;
		}
		else if ((msg.msg==MTCM_KEYDOWN) && (guiid!=MTC_WINDOW) && (parent) && (parent->focused==this)){
			if ((msg.key==KB_TAB) && !(msg.buttons & DB_CONTROL) && (flags & MTCF_ACCEPTINPUT)){
				MTWinControl *p = parent;
				while ((p->parent) && (p->parent->guiid!=MTC_DESKTOP)) p = p->parent;
				p->nextcontrol(this,(msg.buttons & DB_SHIFT)!=0);
				return true;
			}
			else if ((msg.key==KB_RETURN) || (msg.key==KB_ESCAPE)){
				switchflags(MTCF_FOCUSED,false);
				if (parent->focused==this){
					parent->focused = 0;
				};
				return true;
			}
		};
		break;
	case MTCM_MOUSECLICK:
		if ((msg.button==DB_RIGHT) && (popup) && (autopopup)){
			popup->popup(this,msg.p);
			return true;
		};
		break;
	case MTCM_TIMER:
		if (msg.param1 & MTTF_SKINTIMER){
			skin->timercontrol(msg.ctrl);
			return true;
		};
		break;
	case MTCM_NOTIFY:
		skin->resetcontrol(this,(msg.param1==0));
		break;
	};
	return false;
}

void MTControl::preparedraw(MTBitmap **b,int &ox,int &oy)
{
	if ((parent->mb) && (!direct)) *b = parent->mb;
	else{
		screen->setwindow(parent);
		*b = screen;
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	ox += parent->box;
	oy += parent->boy;
}

void MTControl::setparent(MTWinControl *newparent)
{
	MTWinControl *wc;
	bool hidden;

	if (!newparent) return;
	hidden = ((flags & MTCF_HIDDEN)!=0);
	if (parent) parent->delcontrol(this);
	parent = newparent;
	if (window!=(MTWindow*)this){
		window = 0;
		wc = parent;
		do{
			if ((wc->guiid==MTC_WINDOW) || (wc->guiid==MTC_TABSHEET)){
				window = (MTWindow*)wc;
				break;
			};
			wc = wc->parent;
		} while (wc);
	};
	if (!hidden) flags &= (~MTCF_HIDDEN);
	newparent->addcontrol(this);
	MTCMessage msg = {MTCM_CHANGE,0,this};
	parent->message(msg);
}

bool MTControl::designmessage(MTCMessage &msg)
{
	int ow,oh,ol2,ot2,ow2,oh2;

	if ((guiid==MTC_DESKTOP) || (guiid==MTC_WINDOW) || (guiid==MTC_TABSHEET)) return false;
	switch (msg.msg){
	case MTCM_MOUSEDOWN:
		if (msg.button!=DB_LEFT) break;
		moving = false;
		mox = msg.x;
		moy = msg.y;
		mow = width;
		moh = height;
		if (msg.y<8){
			if (msg.x<8){
				cborder = 0;
				sizing = true;
				return true;
			}
			else if (msg.x>=width-8){
				cborder = 1;
				sizing = true;
				return true;
			};
		}
		else if (msg.y>=height-8){
			if (msg.x<8){
				cborder = 2;
				sizing = true;
				return true;
			}
			else if (msg.x>=width-8){
				cborder = 3;
				sizing = true;
				return true;
			};
		};
		moving = true;
		return true;
	case MTCM_MOUSEMOVE:
		if (((moving) || (sizing)) && (!triggered)){
			if ((abs(msg.x-mox)>dragx) || (abs(msg.y-moy)>dragy)) triggered = true;
			else return true;
		};
		ow = ((msg.x-mox)/gridx)*gridx;
		oh = ((msg.y-moy)/gridy)*gridy;
		if (sizing){
			switch (cborder){
			case 0:
				ol2 = left+ow;
				ot2 = top+oh;
				ow2 = width-ow;
				oh2 = height-oh;
				if (!checkbounds(ol2,ot2,ow2,oh2)){
					ow = width-ow2;
					oh = height-oh2;
					ol2 = left+ow;
					ot2 = top+oh;
				};
				setbounds(ol2,ot2,ow2,oh2);
				break;
			case 1:
				ol2 = left;
				ot2 = top+oh;
				ow2 = mow+ow;
				oh2 = height-oh;
				if (!checkbounds(ol2,ot2,ow2,oh2)){
					oh = height-oh2;
					ot2 = top+oh;
				};
				setbounds(left,ot2,ow2,oh2);
				break;
			case 2:
				ol2 = left+ow;
				ot2 = top;
				ow2 = width-ow;
				oh2 = moh+oh;
				if (!checkbounds(ol2,ot2,ow2,oh2)){
					ow = width-ow2;
					ol2 = left+ow;
				};
				setbounds(ol2,top,ow2,oh2);
				break;
			case 3:
				ol2 = left;
				ot2 = top;
				ow2 = mow+ow;
				oh2 = moh+oh;
				checkbounds(ol2,ot2,ow2,oh2);
				setbounds(left,top,ow2,oh2);
				break;
			};
			return true;
		};
		break;
	case MTCM_MOUSEUP:
		triggered = false;
		if (moving){
			moving = false;
			return true;
		}
		else if (sizing){
			sizing = false;
			return true;
		};
		break;
	};
	return false;
}
//---------------------------------------------------------------------------
