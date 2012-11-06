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
#include "MTDesktop.h"
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
extern MTDesktop *desktops[32];
extern int ndesktops;
//---------------------------------------------------------------------------
void MTCT new_control(MTShortcut*,MTControl *c,MTUndo*)
{
	MTMenuItem *mi = (MTMenuItem*)c;
	MTMenu *popup = (MTMenu*)mi->parent;
	MTWinControl *wctrl;

	while ((popup->caller) && (popup->caller->guiid==MTC_MENU)) popup = (MTMenu*)popup->caller;
	if ((!popup->caller) || ((popup->caller->guiid & 2)==0)) return;
	wctrl = (MTWinControl*)popup->caller;
	wctrl->bringtofront(gi->newcontrol(mi->tag,0,wctrl,popup->mouse.x,popup->mouse.y,0,0,0));
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTDesktop
//---------------------------------------------------------------------------
MTDesktop::MTDesktop(int tag,void *p,int l,int t,int w,int h):
MTWinControl(MTC_DESKTOP,tag,0,l,t,w,h),
mwnd(p),
newctrl(0),
ccombo(0)
{
	if (!popup){
		popup = (MTMenu*)gi->newcontrol(MTC_MENU,0,this,0,0,0,0,0);
		popup->flags |= MTCF_DONTSAVE;
	}
	else{
		if (popup->numitems>0) popup->additem("|Desktop",0,0,false,0);
	};
	desktops[ndesktops++] = this;
	di->adddesktop(this);
	if (candesign){
		int x,n,type;
		char name[256];

		newctrl = (MTMenu*)gi->newcontrol(MTC_MENU,0,this,0,0,0,0,0);
		n = gi->getnumcontrols();
		for (x=0;x<n;x++){
			if (gi->getcontroltype(x,name,type)){
				MTMenuItem *mi = (MTMenuItem*)newctrl->additem(name,-1,0,false,0);
				mi->tag = type;
				mi->command = new_control;
			};
		};
	};
}

MTDesktop::~MTDesktop()
{
	int x;
	
	flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
	di->deldesktop(this);
	for (x=0;x<ncontrols;x++){
		if (controls[x]->guiid==MTC_LISTBOX){
			delcontrol(controls[x]);
			x--;
		};
	};
	for (x=0;x<ndesktops;x++){
		if (desktops[x]==this){
			desktops[x] = desktops[--ndesktops];
			break;
		};
	};
}

void MTDesktop::draw(MTRect &rect)
{
	void *rgn = getemptyrgn();
	
	if (flags & MTCF_CANTDRAW) return;
	if (&rect){
		void *crgn = recttorgn(rect);
		intersectrgn(rgn,crgn);
		deletergn(crgn);
	};
	if (!isemptyrgn(rgn)){
		if ((flags & MTCF_TRANSPARENT)==0){
//			DUMPRGN(rgn);
			cliprgn(rgn);
			skin->drawcontrol(this,rect,0,0,0);
			unclip();
		}
		else{
			gi->invalidatergn(mwnd,rgn);
		};
	};
	deletergn(rgn);
	MTWinControl::draw(rect);
	if (&rect) flush(rect);
	else flush();
}

bool MTDesktop::message(MTCMessage &msg)
{
	if (msg.msg==MTCM_ACTION){
		if (ccombo){
			ccombo->popuptick = guitick;
			ccombo->mlb->switchflags(MTCF_FOCUSED,false);
		};
		return true;
	}
	else if ((msg.msg==MTCM_CHANGE) && (msg.param1 & MTCF_FOCUSED)){
		MTWinControl &cw = *(MTWinControl*)msg.ctrl;
		if ((cw.flags & MTCF_FOCUSED)==0){
			if ((ccombo) && (&cw==ccombo->mlb)){
				ccombo->popuptick = guitick;
				cw.switchflags(MTCF_HIDDEN,true);
				cw.flags &= (~MTCF_POPUP);
				puttoback(&cw);
				msg.msg = MTCM_ACTION;
				msg.param1 = 0;
				ccombo->message(msg);
				ccombo = 0;
			}
			else if (cw.guiid==MTC_MENU){
				if ((!focused) || (focused->guiid!=MTC_MENU)) clearmenu(0);
			};
		};
	};
	if (msg.msg<MTCM_USERINPUT){
		if ((blockinput) || (flags & MTCF_HIDDEN)) return false;
	};
	return MTWinControl::message(msg);
}

void MTDesktop::setmenu(MTMenu *m)
{
	focus(m);
	m->flags |= MTCF_POPUP;
	bringtofront(m);
	m->switchflags(MTCF_HIDDEN,false);
}

void MTDesktop::clearmenu(MTMenu *m)
{
	while (ncontrols>0){
		MTWinControl &cctrl = *(MTWinControl*)controls[0];
		if ((MTMenu*)&cctrl==m) break;
		if ((cctrl.guiid!=MTC_MENU) || ((cctrl.flags & MTCF_POPUP)==0)) break;
		cctrl.switchflags(MTCF_HIDDEN,true);
		cctrl.flags &= (~MTCF_POPUP);
		puttoback(&cctrl);
	};
}

void MTDesktop::setcombo(MTComboBox *c)
{
	MTList *lb = c->mlb;
	int h;
	
	h = c->dropcount*lb->itemheight;
	ccombo = c;
	lb->setbounds(c->parent->box+c->left+2,c->parent->boy+c->top+c->height+2,c->width-4,h);
	focus(lb);
	lb->flags |= MTCF_POPUP;
	bringtofront(lb);
	lb->switchflags(MTCF_HIDDEN,false);
}

void MTDesktop::drawover(MTWinControl *wnd,MTRect &rect)
{
	int x;
	MTRect cr = {0,0,width,height};
	MTRect cr2;
	int bflags;
	bool wasmodal = false;
	bool ok = false;
	
	if (flags & MTCF_CANTDRAW) return;
	while (wnd->parent!=this){
		wnd = wnd->parent;
		if (wnd==0) return;
	};
	if (&rect){
		if (!cliprect(cr,rect)) return;
	};
	bflags = flags;
	flags |= MTCF_DONTFLUSH;
	clip(cr);
	for (x=ncontrols-1;x>=0;x--){
		MTControl &cctrl = *controls[x];
		if ((!wasmodal) && (!design)){
			signed char cmodal = (cctrl.flags)>>24;
			if ((cmodal>0) && (cmodal & 4)){
				wasmodal = true;
				fillcolor(cr.left,cr.top,cr.right-cr.left,cr.bottom-cr.top,0,64);
			};
		};
		if (!ok){
			if (&cctrl==wnd) ok = true;
			continue;
		};
		if (cctrl.flags & MTCF_HIDDEN) continue;
		cctrl.getrect(cr2,0);
		if (cliprect(cr2,cr)){
			cr2.left -= cctrl.left;
			cr2.top -= cctrl.top;
			cr2.right -= cctrl.left;
			cr2.bottom -= cctrl.top;
			cctrl.draw(cr2);
		};
	};
	unclip();
	flags = bflags;
	MTControl::draw(rect);
}
//---------------------------------------------------------------------------
