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
#include "MTTabControl.h"
#include "MTButton.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTTabControl
//---------------------------------------------------------------------------
MTTabControl::MTTabControl(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_TABCONTROL,tag,p,l,t,w,h),
style(5),
cstyle(0),
page(0),
autohidetabs(false),
npages(0),
btnx(0),
btny(0),
btnw(16),
btnh(16),
btno(-1),
btnd(-1)
{
	flags &= ~MTCF_BORDER;
	flags |= MTCF_ACCEPTINPUT|MTCF_TRANSPARENT;
	if ((parent) && (parent->guiid==MTC_DESKTOP)){
		style = 6;
		flags |= MTCF_HIDDEN;
	};
	updateborders();
	if (candesign){
		gi->setcontrolname(this,"tabcontrol");
		if (design) gi->newcontrol(MTC_WINDOW,0,this,br.left,br.top,width-br.left+br.right,height-br.top+br.bottom,0);
	};
}

int MTTabControl::loadfromstream(MTFile *f,int size,int flags)
{
	int csize;
	
	while (ncontrols>0) gi->delcontrol(controls[0]);
	csize = MTWinControl::loadfromstream(f,size,flags);
	updateborders();
	setpageid(0);
	return csize;
}

int MTTabControl::savetostream(MTFile *f,int flags)
{
	return MTWinControl::savetostream(f,flags);
}

#define TabControlNP (ControlNP+1)
int MTTabControl::getnumproperties(int id)
{
	if (id==-1) return TabControlNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	return 0;
}

bool MTTabControl::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[1] = {"New page"};
	static int propflags[1] = {MTP_ACTION};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if (id>=TabControlNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTTabControl::getproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	return true;
}

bool MTTabControl::setproperty(int id,void *value)
{
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (id>=TabControlNP) return false;
	if (window) window->modified = true;
	gi->newcontrol(MTC_WINDOW,0,this,br.left,br.top,width-br.left+br.right,height-br.top+br.bottom,0);
	return true;
}

void MTTabControl::getrect(MTRect &r,int client)
{
	r.left = left;
	r.top = top;
	r.right = left+width;
	r.bottom = top+height;
	if (client>0){
		r.left += br.left;
		r.top += br.top;
		r.right += br.right;
		r.bottom += br.bottom;
	};
}

void MTTabControl::switchflags(int f,bool set)
{
	int oldo;

	if ((style==6) && (btno>=0) && (f & MTCF_OVER) && (!set)){
		oldo = btno;
		btno = -1;
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			msg.dr.top = btny;
			msg.dr.bottom = btny+btnh;
			msg.dr.left = width-btnx+oldo*btnw;
			msg.dr.right = msg.dr.left+btnw;
			parent->message(msg);
		};
	};
//	if (f==MTCF_FOCUSED) return;
	MTWinControl::switchflags(f,set);
}

void MTTabControl::draw(MTRect &rect)
{
	int x = 0;
	int y = 0;
	MTBitmap *b;
	MTRect cr = {0,0,width,height};
	
	if (flags & MTCF_CANTDRAW) return;
	if (&rect){
		if (!cliprect(cr,rect)) return;
	};
	if ((parent) && ((npages!=1) || (!autohidetabs))){
		preparedraw(&b,x,y);
		clip(cr);
		skin->drawcontrol(this,cr,b,x,y);
		unclip();
	};
	MTWinControl::draw(rect);
}

bool MTTabControl::message(MTCMessage &msg)
{
	int id,cbtno,ow;
	
	if (cstyle){
		switch (msg.msg){
		case MTCM_MOUSEDOWN:
			if (msg.button!=DB_LEFT) break;
			if (btno>=0){
				btnd = btno;
				MTCMessage msg = {MTCM_CHANGE,0,this};
				msg.dr.top = btny;
				msg.dr.bottom = btny+btnh;
				msg.dr.left = width-btnx+btnd*btnw;
				msg.dr.right = msg.dr.left+btnw;
				parent->message(msg);
				return true;
			};
			break;
		case MTCM_MOUSEMOVE:
			cbtno = -1;
			if ((msg.x>=width-btnx) && (msg.x<width) && (msg.y>=btny) && (msg.y<btny+btnh)){
				cbtno = (msg.x-width+btnx)/btnw;
			};
			if (cbtno!=btno){
				MTCMessage msg = {MTCM_CHANGE,0,this};
				msg.dr.top = btny;
				msg.dr.bottom = btny+btnh;
				if (cbtno>=0){
					msg.dr.left = width-btnx+cbtno*btnw;
					msg.dr.right = msg.dr.left+btnw;
					if (btno>=0){
						if (btno<cbtno){
							msg.dr.left = width-btnx+btno*btnw;
						}
						else{
							msg.dr.right = width-btnx+(btno+1)*btnw;
						};
					};
				}
				else{
					msg.dr.left = width-btnx+btno*btnw;
					msg.dr.right = msg.dr.left+btnw;
				};
				btno = cbtno;
				parent->message(msg);
			};
			break;
		case MTCM_MOUSEUP:
			if (btnd>=0){
				MTCMessage msg = {MTCM_CHANGE,0,this};
				msg.dr.top = btny;
				msg.dr.bottom = btny+btnh;
				msg.dr.left = width-btnx+btnd*btnw;
				msg.dr.right = msg.dr.left+btnw;
				ow = btnd;
				btnd = -1;
				parent->message(msg);
				if ((page) && (btno==ow)){
					MTCMessage msg = {MTCM_STAYONTOP,0,page};
					ow = MTWS_STAYONTOP;
					while (ow>=MTWS_CLOSE){
						if ((cstyle & ow) && (btno--==0)) break;
						ow >>= 1;
						msg.msg++;
					};
					page->message(msg);
				};
				return true;
			};
			break;
		};
	};
	switch (msg.msg){
	case MTCM_KEYDOWN:
		if ((msg.key==KB_TAB) && (msg.buttons & DB_CONTROL)){
			id = getpageid(page);
			if (msg.buttons & DB_SHIFT){
				if (id--==0) id = (ncontrols>>1)-1;
			}
			else{
				if (++id==(ncontrols>>1)) id = 0;
			};
			setpageid(id);
			return true;
		};
		break;
	case MTCM_ACTION:
		if (msg.ctrl->guiid==MTC_BUTTON){
			setpageid(msg.ctrl->tag);
			return true;
		};
		break;
	case MTCM_NOTIFY:
		if (msg.param1==0) updateborders();
		break;
	};
	return MTWinControl::message(msg);
}

void MTTabControl::addcontrol(MTControl *control)
{
	int x,y,ox;
	MTRect b;
	
	if ((control->guiid==MTC_WINDOW) || (control->guiid==MTC_TABSHEET)){
		npages++;
		MTWindow *cp = (MTWindow*)control;
		if (!page){
			page = cp;
			control->flags &= (~MTCF_HIDDEN);
		}
		else{
			control->flags |= MTCF_HIDDEN;
		};
		MTControl *old = focused;
		focused = page;
		if (old) old->switchflags(MTCF_FOCUSED,false);
		page->switchflags(MTCF_FOCUSED,true);
		page->nextcontrol(0,false);
		cstyle = page->style;
		if ((npages<3) && (autohidetabs)) updateborders();
		else updatebuttons();
		flags |= MTCF_DONTRESIZE;
		cp->guiid = MTC_TABSHEET;
		y = cp->flags;
		cp->flags |= MTCF_DONTDRAW;
		cp->setbounds(br.left,br.top,width-br.left+br.right,height-br.top+br.bottom);
		cp->flags = y;
		cp->align = MTCA_CLIENT;
		flags &= (~MTCF_DONTRESIZE);
		skin->getwindowoffsets(style,&b);
		ox = br.left+b.left;
		y = 0;
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if ((cctrl.guiid==MTC_WINDOW) || (cctrl.guiid==MTC_TABSHEET)) y++;
			else if (cctrl.guiid==MTC_BUTTON){
				ox += cctrl.width;
			};
		};
		MTButton *cb = (MTButton*)gi->newcontrol(MTC_BUTTON,y,this,ox,b.top,0,br.top-b.top-b.bottom,0);
		cb->imageindex =( (MTWindow*)control)->imageindex;
		cb->setcaption(((MTWindow*)control)->caption);
		cb->switchflags(MTCF_DONTSAVE,true);
		cb->switchflags(MTCF_HIDDEN,autohidetabs);
		if (flags & MTCF_HIDDEN){
			switchflags(MTCF_HIDDEN,false);
		}
		else{
			MTCMessage msg = {MTCM_CHANGE,0,this};
			msg.dr.top = btny;
			msg.dr.bottom = btny+btnh;
			msg.dr.left = width-btnx;
			msg.dr.right = width;
			parent->message(msg);
		};
	};
	MTWinControl::addcontrol(control);
}

void MTTabControl::delcontrol(MTControl *control)
{
	int x,id,ox;
	MTRect b;
	
	if ((control->guiid==MTC_WINDOW) || (control->guiid==MTC_TABSHEET)){
		npages--;
		MTWindow *cp = (MTWindow*)control;
		if (page==cp) page = 0;
		id = getpageid(cp);
		for (x=0;x<ncontrols;x++){
			MTButton &cb = *(MTButton*)controls[x];
			if (cb.guiid==MTC_BUTTON){
				if (cb.tag==id) gi->delcontrol(&cb);
				else if (cb.tag>id) cb.tag--;
			};
		};
		skin->getwindowoffsets(style,&b);
		ox = br.left+b.left;
		for (x=0;x<ncontrols;x++){
			MTButton &cb = *(MTButton*)controls[x];
			if (cb.guiid==MTC_BUTTON){
				cb.setbounds(ox,cb.top,cb.width,cb.height);
				ox += cb.width;
			};
		};
	};
	MTWinControl::delcontrol(control);
	if ((autohidetabs) && (npages==1)){
		updateborders();
	};
	if ((ncontrols==0) && (!design)) switchflags(MTCF_HIDDEN,true);
}

void MTTabControl::bringtofront(MTWinControl *w)
{
	if (w->guiid==MTC_TABSHEET) setpage((MTWindow*)w);
	else MTWinControl::bringtofront(w);
}

void MTTabControl::puttoback(MTWinControl *w)
{
}

void MTTabControl::setpage(MTWindow *p)
{
	int x;
	MTControl *old;
	
	if (page==p) return;
	for (x=0;x<ncontrols;x++){
		if (controls[x]==(MTControl*)p){
			if (page) page->switchflags(MTCF_HIDDEN,true);
			page = p;
			cstyle = page->style;
			updatebuttons();
			page->switchflags(MTCF_HIDDEN,false);
			if (!design){
				old = focused;
				focused = page;
				if (old) old->switchflags(MTCF_FOCUSED,false);
				page->switchflags(MTCF_FOCUSED,true);
				page->nextcontrol(0,false);
			};
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
			break;
		};
	};
}

void MTTabControl::setpageid(int id)
{
	int x,y;
	MTControl *old;
	
	y = 0;
	for (x=0;x<ncontrols;x++){
		MTWindow &cp = *(MTWindow*)controls[x];
		if (cp.guiid==MTC_TABSHEET){
			if (id==y){
				if (page==&cp) return;
				if (page) page->switchflags(MTCF_HIDDEN,true);
				page = &cp;
				cstyle = page->style;
				updatebuttons();
				page->switchflags(MTCF_HIDDEN,false);
				if (!design){
					old = focused;
					focused = page;
					if (old){
						old->switchflags(MTCF_FOCUSED,false);
					};
					page->switchflags(MTCF_FOCUSED,true);
					page->nextcontrol(0,false);
				};
				MTCMessage msg = {MTCM_CHANGE,0,this};
				parent->message(msg);
				break;
			};
			y++;
		};
	};
}

int MTTabControl::getpageid(MTWindow *p)
{
	int x,y;
	
	y = 0;
	for (x=0;x<ncontrols;x++){
		MTWindow &cp = *(MTWindow*)controls[x];
		if ((cp.guiid==MTC_WINDOW) || (cp.guiid==MTC_TABSHEET)){
			if (&cp==p) return y;
			y++;
		};
	};
	return -1;
}

MTWindow* MTTabControl::loadpage(MTResources *res,int id,bool autosave)
{
	MTWindow *w;

	w = gi->loadwindow(res,id,dsk,autosave);
	if (!w) return 0;
	w->setparent(this);
	return w;
}

void MTTabControl::setautohidetabs(bool autohide)
{
	autohidetabs = autohide;
	if (npages==1){
		updateborders();
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
		};
	};
}

void MTTabControl::updatecaption(MTWindow *p)
{
	int x,id,ox;
	bool found = false;
	MTRect b;
	
	id = getpageid(p);
	if (id<0) return;
	skin->getwindowoffsets(style,&b);
	ox = br.left+b.left;
	for (x=0;x<ncontrols;x++){
		MTButton &cb = *(MTButton*)controls[x];
		if (cb.guiid==MTC_BUTTON){
			if (cb.tag==id){
				cb.imageindex = p->imageindex;
				cb.setcaption(p->caption);
				found = true;
			}
			else if (found) cb.setbounds(ox,cb.top,cb.width,cb.height);
			ox += cb.width;
		};
	};
}

void MTTabControl::updateborders()
{
	int x,ox;
	bool hide = false;
	MTRect b;

	if ((npages==1) && (autohidetabs)){
		br.left = br.top = br.right = br.bottom = 0;
		hide = true;
	}
	else{
		skin->getwindowborders(style,&b);
		br.left = -b.left;
		br.top = -b.top;
		br.right = -b.right;
		br.bottom = -b.bottom;
	};
	skin->getwindowoffsets(style,&b);
	ox = br.left+b.left;
	updatebuttons();
	flags |= MTCF_DONTRESIZE;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.guiid==MTC_BUTTON){
			cctrl.switchflags(MTCF_HIDDEN,hide);
			cctrl.left = ox;
			cctrl.top = b.top;
			cctrl.width = ((MTButton*)&cctrl)->getautowidth();
			cctrl.height = br.top-b.top-b.bottom;
			ox += cctrl.width;
		}
		else{
			cctrl.setbounds(br.left,br.top,width-br.left+br.right,height-br.top+br.bottom);
		};
	};
	flags &= (~MTCF_DONTRESIZE);
}

void MTTabControl::updatebuttons()
{
	if (style!=6) return;
	skin->getcontrolsize(MTC_WINDOW,style,btnx,btny);
	skin->getcontrolsize(MTC_WINDOW,16,btnw,btnh);
	if (cstyle & MTWS_CLOSE){
		btnx += btnw;
	};
	if (cstyle & MTWS_MAXIMIZE){
		btnx += btnw;
	};
	if (cstyle & MTWS_MINIMIZE){
		btnx += btnw;
	};
	if (cstyle & MTWS_HELP){
		btnx += btnw;
	};
	if (cstyle & MTWS_STAYONTOP){
		btnx += btnw;
	};
}
//---------------------------------------------------------------------------
