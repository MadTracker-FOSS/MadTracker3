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
#include "MTWindow.h"
#include "MTTabControl.h"
#include "MTSysControls.h"
#include "MTButton.h"
#include "../Headers/MTXInput.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTWindow
//---------------------------------------------------------------------------
MTWindow::MTWindow(int tag,MTWinControl *p,int l,int t,int w,int h,int s):
MTWinControl(MTC_WINDOW,tag,p,l,t,w,h),
style(s),
imageindex(-1),
wrapper(0),
modified(false),
btnx(0),
btny(0),
btnw(16),
btnh(16),
btno(-1),
btnd(-1),
res(0),
resid(0)
{
	flags |= (MTCF_ACCEPTCTRLS|MTCF_TRANSPARENT|MTCF_HIDDEN);
	if ((parent) && (parent->guiid==MTC_TABCONTROL)){
		guiid = MTC_TABSHEET;
		align = MTCA_CLIENT;
	};
	caption = (char*)si->memalloc(256,MTM_ZERO);
	if (candesign){
		gi->setcontrolname(this,(guiid==MTC_TABSHEET)?"page":"window");
		if (design){
			MTWinControl *oldp = parent;
			parent = 0;
			setcaption(name);
			parent = oldp;
			if (guiid==MTC_TABSHEET) ((MTTabControl*)parent)->updatecaption(this);
		};
	};
	updateborders();
	minsize[0] = 128;
	if (style==5) minsize[1] = 96;
	else minsize[1] = 64;
	screen->setwindow(0);
	maxsize[0] = screen->wr.right;
	maxsize[1] = screen->wr.bottom;
	prev.right = prev.bottom = 0;
	hotcontrols = si->arraycreate(4,sizeof(HotControl));
	if (candesign){
		if (!popup){
			popup = (MTMenu*)gi->newcontrol(MTC_MENU,0,dsk,0,0,0,0,0);
			popup->flags |= MTCF_DONTSAVE;
		}
		else{
			if (popup->numitems>0) popup->additem("|Design",0,0,false,0);
		};
		MTMenuItem *newctrl = (MTMenuItem*)popup->additem("New control...",-1,0,false,0);
		newctrl->submenu = dsk->newctrl;
		newctrl->tag = 99999;
		autopopup = true;
	};
}

void nomorewindow(MTWinControl *w)
{
	int x;

	for (x=0;x<w->ncontrols;x++){
		MTControl *c = w->controls[x];
		c->window = 0;
		if (c->guiid & 2) nomorewindow((MTWinControl*)c);
	};
}

MTWindow::~MTWindow()
{
	si->memfree(caption);
	si->arraydelete(hotcontrols);
	nomorewindow(this);
}

int MTWindow::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTWinControl::loadfromstream(f,size,flags);
	char cap[256];
	int l,x;
	
	if ((parent) && (parent->guiid==MTC_TABCONTROL)){
		guiid = MTC_TABSHEET;
		align = MTCA_CLIENT;
	};
	f->read(&style,4);
	f->readln(cap,256);
	cap[255] = 0;
	l = strlen(cap)+1;
	MTWinControl *oldp = parent;
	parent = 0;
	setcaption(cap);
	parent = oldp;
	x = (4-(l & 3)) & 3;
	f->seek(x,MTF_CURRENT);
	l += x;
	f->read(&imageindex,4); 
	if ((parent) && (parent->guiid==MTC_TABCONTROL)){
		((MTTabControl*)parent)->updatecaption(this);
	};
	box = parent->box+left;
	boy = parent->boy+top;
	updateborders();
	if (style==5) minsize[1] = 96;
	else minsize[1] = 64;
	modified = false;
	return csize+8+l;
}

int MTWindow::savetostream(MTFile *f,int flags)
{
	int oalign = align;
	align = 0;
	int csize = MTWinControl::savetostream(f,flags);
	int l = strlen(caption)+1;
	int o,zero;
	
	align = oalign;
	zero = 0;
	o = ((l+3)>>2)<<2;
	f->write(&style,4);
	f->write(caption,l);
	f->write(&zero,o-l);
	f->write(&imageindex,4);
	modified = false;
	return csize+8+o;
}

#define WindowNP (ControlNP+4)
int MTWindow::getnumproperties(int id)
{
	if (id==-1) return WindowNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	if (id==ControlNP) return 8;
	return 0;
}

bool MTWindow::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[4] = {"Style","Caption","ImageIndex","Stay-on-top"};
	static int propflags[4] = {MTP_LIST,MTP_TEXT,MTP_INT,MTP_BOOL};
	static char *subname[8] = {"Fixed","Fixed+Caption","Sizable","Sizable+Caption","Dialog","Tabbed","Dock","Main"};
	
	if ((id<ControlNP) || ((id & 0xFF00) && (id>>8<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if ((id>>8)==ControlNP){
		id &= 0xFF;
		if (id>=8) return false;
		*name = subname[id];
		flags = -1;
		return true;
	};
	if (id>=WindowNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTWindow::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && (id>>8<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		iv = style & 0xFF;
		break;
	case 1:
		strcpy((char*)value,caption);
		break;
	case 2:
		iv = imageindex;
		break;
	case 3:
		iv = ((flags & MTCF_STAYONTOP)!=0);
		break;
	default:
		return false;
	};
	return true;
}

bool MTWindow::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	modified = true;
	if ((parent) && (parent->guiid==MTC_DESKTOP)){
		if (id==5){
			parent->setbounds(parent->left,parent->top,parent->width-width+iv,parent->height);
		}
		else if (id==6){
			parent->setbounds(parent->left,parent->top,parent->width,parent->height-height+iv);
		};
	};
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	switch (id-ControlNP){
	case 0:
		switch (iv){
		case MTWS_SIZABLECAP:
		case MTWS_MAIN:
			iv |= MTWS_MAXIMIZE;
		case MTWS_FIXEDCAP:
		case MTWS_DIALOG:
			iv |= MTWS_MINIMIZE;
			iv |= MTWS_CLOSE;
		default:
			break;
		};
		setstyle(iv);
		return true;
	case 1:
		setcaption((const char*)value);
		return true;
	case 2:
		imageindex = iv;
		setcaption(caption);
		return true;
	case 3:
		switchflags(MTCF_STAYONTOP,(iv!=0));
		return true;
	default:
		return false;
	};
}

bool MTWindow::checkbounds(int &l,int &t,int &w,int &h)
{
	bool ok = true;

	if (w<minsize[0]){
		w = minsize[0];
		ok = false;
	}
	else if (w>maxsize[0]){
		w = maxsize[0];
		ok = false;
	};
	if (h<minsize[1]){
		h = minsize[1];
		ok = false;
	}
	else if (h>maxsize[1]){
		h = maxsize[1];
		ok = false;
	};
	return ok && MTWinControl::checkbounds(l,t,w,h);
}

void MTWindow::getrect(MTRect &r,int client)
{
	r.left = left;
	r.top = top;
	r.right = left+width;
	r.bottom = top+height;
	if ((client<=0) && (guiid==MTC_WINDOW)){
		r.left += br.left;
		r.top += br.top;
		r.right += br.right;
		r.bottom += br.bottom;
	};
	if ((client==0) && ((guiprefs.shadows) || ((guiprefs.dialogshadows) && ((style & 0xFF)==MTWS_DIALOG)))){
		int dx = guiprefs.shadowx;
		int dy = guiprefs.shadowy;
		if (8-dx>0) r.left -= 8-dx;
		if (8-dy>0) r.top -= 8-dy;
		if (8+dx>0) r.right += dx+8;
		if (8+dy>0) r.bottom += dy+8;
	};
}

void MTWindow::switchflags(int f,bool set)
{
	int oldo;

	if (guiid==MTC_WINDOW){
/*
		if (f & MTCF_FOCUSED){
			MTCMessage msg = {MTCM_ACTIVE,0,this,set};
			message(msg);
		};
*/
		if ((btno>=0) && (f & MTCF_OVER) && (!set)){
			oldo = btno;
			btno = -1;
			if (parent){
				MTCMessage msg = {MTCM_CHANGE,0,this};
				msg.dr.top = -btny;
				msg.dr.bottom = -btny+btnh;
				msg.dr.left = width-btnx+oldo*btnw;
				msg.dr.right = msg.dr.left+btnw;
				parent->message(msg);
			};
		};
	};
	MTWinControl::switchflags(f,set);
}

void MTWindow::draw(MTRect &rect)
{
	int x = 0;
	int y = 0;
	MTBitmap *b;
	MTRect cr = {0,0,width,height};
	
	if (flags & MTCF_HIDDEN) return;
	if (&rect){
		cliprect(cr,rect);
	};
	if (parent){
		preparedraw(&b,x,y);
		skin->drawcontrol(this,rect,b,x,y);
	};
	if (flags & MTCF_CANTDRAW) return;
	if (design){
		int x;
		int y;
		int color = skin->getcolor(SC_CTRL_S);
		open(0);
		for (y=(cr.top>>3)<<3;y<=cr.bottom;y+=8){
			for (x=(cr.left>>3)<<3;x<=cr.right;x+=8) point(x,y,color);
		};
		close(0);
	};
	if (guiid==MTC_WINDOW){
		flags |= MTCF_DONTFLUSH;
		MTWinControl::draw(rect);
		flags &= ~MTCF_DONTFLUSH;
		flush(cr);
	}
	else MTWinControl::draw(rect);
}

bool MTWindow::message(MTCMessage &msg)
{
	int x,y,ow,oh,ol2,ot2,ow2,oh2,cstyle,cbtno;
	MTBitmap *tmp,*tmp2;
	MTControl *hctrl,*fctrl;
	bool multi;
	int focusid;
	char key;
	MTRect r;
	MTMenu *popup;
	MTMenuItem *newctrl;
	
	if (parent){
		if (guiid==MTC_WINDOW){
			switch (msg.msg){
			case MTCM_MOUSEDOWN:
				if (msg.button!=DB_LEFT) break;
				parent->bringtofront(this);
				if (btno>=0){
					btnd = btno;
					MTCMessage msg = {MTCM_CHANGE,0,this};
					msg.dr.top = -btny;
					msg.dr.bottom = -btny+btnh;
					msg.dr.left = width-btnx+btnd*btnw;
					msg.dr.right = msg.dr.left+btnw;
					parent->message(msg);
					return true;
				};
				if (design){
					mox = (msg.x/gridx)*gridx;
					moy = (msg.y/gridy)*gridy;
				}
				else{
					mox = msg.x;
					moy = msg.y;
				};
				mow = width;
				moh = height;
				cstyle = style & 0xFF;
				if ((cstyle==2) || (cstyle==3) || (design)){
					if (msg.y<0){
						if (msg.x<0){
							cborder = 0;
							sizing = true;
							return true;
						}
						else if (msg.x>=width){
							cborder = 1;
							sizing = true;
							return true;
						}
						else{
							if ((msg.buttons & DB_DOUBLE) && (style & MTWS_MAXIMIZE)){
								MTCMessage msg = {MTCM_MAXIMIZE,0,this};
								message(msg);
							}
							else{
								moving = true;
							};
							return true;
						};
					}
					else if (msg.y>=height){
						if (msg.x<0){
							cborder = 2;
							sizing = true;
							return true;
						}
						else if (msg.x>=width){
							cborder = 3;
							sizing = true;
							return true;
						}
						else{
							moving = true;
							return true;
						};
					}
					else if ((msg.x<0) || (msg.x>=width)){
						moving = true;
						return true;
					};
				}
				else if ((cstyle!=7) && ((msg.x<0) || (msg.x>=width) || (msg.y<0) || (msg.y>=height))){
					moving = true;
					return true;
				};
				break;
			case MTCM_MOUSEMOVE:
				cbtno = -1;
				if ((msg.x>=width-btnx) && (msg.x<width) && (msg.y>=-btny) && (msg.y<-btny+btnh)){
					cbtno = (msg.x-width+btnx)/btnw;
				};
				if (cbtno!=btno){
					MTCMessage msg = {MTCM_CHANGE,0,this};
					msg.dr.top = -btny;
					msg.dr.bottom = -btny+btnh;
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
				if (btno>=0) break;
				if (((moving) || (sizing)) && (!triggered)){
					if ((abs(msg.x-mox)>dragx) || (abs(msg.y-moy)>dragy)) triggered = true;
					else return true;
				};
				if (moving){
					setbounds(left+msg.x-mox,top+msg.y-moy,width,height);
					return true;
				}
				else if (sizing){
					if (design){
						ow = (msg.x/gridx)*gridx-mox;
						oh = (msg.y/gridy)*gridy-moy;
					}
					else{
						ow = msg.x-mox;
						oh = msg.y-moy;
					};
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
				if (msg.button!=DB_LEFT) break;
				triggered = false;
				if (btnd>=0){
					MTCMessage msg = {MTCM_CHANGE,0,this};
					msg.dr.top = -btny;
					msg.dr.bottom = -btny+btnh;
					msg.dr.left = width-btnx+btnd*btnw;
					msg.dr.right = msg.dr.left+btnw;
					ow = btnd;
					btnd = -1;
					parent->message(msg);
					if (btno==ow){
						MTCMessage msg = {MTCM_STAYONTOP,0,this};
						ow = MTWS_STAYONTOP;
						while (ow>=MTWS_CLOSE){
							if ((style & ow) && (btno--==0)) break;
							ow >>= 1;
							msg.msg++;
						};
						message(msg);
					};
					return true;
				}
				else{
					if (moving){
						moving = false;
						return true;
					}
					else if (sizing){
						sizing = false;
						return true;
					};
				};
				break;
			case MTCM_KEYDOWN:
				if (((msg.key==KB_ESCAPE) && (focused==0)) || ((modalresult!=MTDR_NULL) && (msg.key==KB_RETURN))){
					if ((focused) && (focused->guiid==MTC_BUTTON) && (msg.key==KB_RETURN)){
						modalresult = ((MTButton*)focused)->modalresult;
					};
					MTCMessage msg = {MTCM_CLOSE,0,this};
					message(msg);
					return true;
				};
				break;
			};
		};
		switch (msg.msg){
		case MTCM_CHAR:
			if (MTWinControl::message(msg)) return true;
			if (hotcontrols->nitems==0) return false;
			key = msg.key;
			if ((key>='a') && (key<='z')) key += 'A'-'a';
			if (msg.buttons==0){
				hctrl = 0;
				focusid = -1;
				multi = false;
				fctrl = focused;
				while ((fctrl) && (fctrl->guiid & 2)){
					if (((MTWinControl*)fctrl)->focused){
						fctrl = ((MTWinControl*)fctrl)->focused;
					};
				};
				for (x=0;x<hotcontrols->nitems;x++){
					HotControl &chot = D(hotcontrols,HotControl)[x];
					if (chot.hotkey==key){
						if (chot.ctrl==fctrl){
							focusid = x;
						};
						if (hctrl){
							multi = true;
							if (focusid>=0) break;
						};
						hctrl = chot.ctrl;
					};
				};
				if (!hctrl) return false;
				if (multi){
					focusid++;
					for (y=0;y<hotcontrols->nitems-1;y++){
						HotControl &chot = D(hotcontrols,HotControl)[focusid];
						if (chot.hotkey==key){
							focus(chot.ctrl);
						};
						if (++focusid>=hotcontrols->nitems) focusid = 0;
					};
				}
				else{
					MTCMessage msg = {MTCM_HOTKEY,0,hctrl};
					hctrl->message(msg);
					return true;
				};
			};
			return false;
		case MTCM_NOTIFY:
			if (msg.param1==0) updateborders();
			break;
		case MTCM_STAYONTOP:
			return true;
		case MTCM_HELP:
			return true;
		case MTCM_MINIMIZE:
			return true;
		case MTCM_ONPOPUP:
			popup = (MTMenu*)msg.ctrl;
			newctrl = (MTMenuItem*)popup->getitemfromtag(99999);
			if (newctrl){
				if (design){
					newctrl->switchflags(MTCF_HIDDEN,false);
				}
				else{
					newctrl->switchflags(MTCF_HIDDEN,true);
					msg.result = 0;
				};
			};
			return true;
		case MTCM_MAXIMIZE:
			if ((!parent) || (msg.ctrl!=this)) break;
			if (MTWinControl::message(msg)) return true;
			if (parent->guiid==MTC_TABCONTROL){
				setparent(dsk);
			}
			else{
				for (x=parent->ncontrols-1;x>=0;x--){
					MTTabControl &ct = *(MTTabControl*)parent->controls[x];
					if ((MTWindow*)&ct==this) break;
					if (ct.guiid==MTC_TABCONTROL){
						MTRect cr;
						ct.getrect(cr,1);
						getrect(r,-1);
						if (cliprect(r,cr)){
							setparent(&ct);
							ct.setpage(this);
							return true;
						};
					};
				};
			};
			return true;
		case MTCM_CLOSE:
			if (msg.ctrl!=this) break;
			if (MTWinControl::message(msg)) return true;
/*			if ((modalparent) && (modalparent->modalresult==MTDR_NULL)){
				if (modalresult==MTDR_NULL) modalparent->modalresult = 0;
				else modalparent->modalresult = modalresult;
			};*/
			if ((modalparent) && (*modalparent==MTDR_NULL)){
				*modalparent = modalresult;
			};
			if ((guiprefs.fadeout) && (mb) && (guiid==MTC_WINDOW)){
				getrect(r,0);
				if (r.left<0) r.left = 0;
				if (r.top<0) r.top = 0;
				r.right -= r.left;
				r.bottom -= r.top;
				r.left -= left-box;
				r.top -= top-boy;
				tmp = di->newbitmap(MTB_SKIN|MTB_DRAW,r.right,r.bottom);
				tmp2 = di->newbitmap(MTB_DRAW,r.right,r.bottom);
				mb->blt(tmp,0,0,r.right,r.bottom,r.left,r.top);
				ow = parent->flags;
				parent->flags |= MTCF_DONTFLUSH;
				switchflags(MTCF_HIDDEN,true);
				mb->blt(tmp2,0,0,r.right,r.bottom,r.left,r.top);
				parent->flags = ow;
				for (ow=224;ow>0;ow-=32){
					oh = si->syscounter();
					tmp2->blt(mb,r.left,r.top,r.right,r.bottom,0,0);
					tmp->blendblt(mb,r.left,r.top,r.right,r.bottom,0,0,ow);
					parent->flush(r.left,r.top,r.right,r.bottom);
					parent->flushend();
					oh -= si->syscounter();
					oh += 20;
					if (oh>0) si->syswait(oh);
					else if (oh<5) break;
				};
				tmp2->blt(mb,r.left,r.top,r.right,r.bottom,0,0);
				parent->flush(r.left,r.top,r.right,r.bottom);
				di->delbitmap(tmp);
				di->delbitmap(tmp2);
			}
			else{
				switchflags(MTCF_HIDDEN,true);
			};
			if (flags & MTCF_FREEONCLOSE) gi->delcontrol(this);
			return true;
		};
	};
	return MTWinControl::message(msg);
}

void MTWindow::setparent(MTWinControl *newparent)
{
	if (!newparent) return;
	if ((parent) && (parent->guiid==MTC_TABCONTROL) && (newparent->guiid!=MTC_TABCONTROL)){
		guiid = MTC_WINDOW;
		align = MTCA_TOPLEFT;
		flags |= MTCF_HIDDEN;
		if (prev.right==0){
			prev.left = left+parent->box-newparent->box;
			prev.top = top+parent->boy-newparent->boy;
			prev.right = prev.left+width;
			prev.bottom = prev.top+height;
		};
		MTWinControl::setparent(newparent);
		setbounds(prev.left,prev.top,prev.right-prev.left,prev.bottom-prev.top);
		updateborders();
		switchflags(MTCF_HIDDEN,false);
		return;
	};
	prev.left = left;
	prev.top = top;
	prev.right = left+width;
	prev.bottom = top+height;
	MTWinControl::setparent(newparent);
}

void MTWindow::setstyle(int s)
{
	style = s;
	updateborders();
	if (style==5) minsize[1] = 96;
	else minsize[1] = 64;
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
		parent->flush();
	};
}

void MTWindow::setcaption(const char *c)
{
	strcpy(caption,c);
	if (parent){
		if (guiid==MTC_TABSHEET){
			((MTTabControl*)parent)->updatecaption(this);
		}
		else{
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
			parent->flush();
		};
	};
}

void MTWindow::updateregions()
{
	MTRect r = {0,0,width,height};
	
	if (oprgn) deletergn(oprgn);
	if (trrgn) deletergn(trrgn);
	if (guiid==MTC_TABSHEET){
		oprgn = recttorgn(r);
		r.right = r.bottom = 0;
		trrgn = recttorgn(r);
	}
	else{
		skin->getregions(this,&oprgn,&trrgn);
	};
}

void MTWindow::addhotkey(MTControl *ctrl,char hotkey)
{
	HotControl hc = {hotkey,ctrl};

	delhotkey(ctrl);
	hotcontrols->push(&hc);
}

void MTWindow::delhotkey(MTControl *ctrl)
{
	int x;

	for (x=0;x<hotcontrols->nitems;x++){
		if (D(hotcontrols,HotControl)[x].ctrl==ctrl){
			hotcontrols->delitems(x,1);
			x--;
		};
	};
}

void MTWindow::setminsize(int width,int height)
{
	if (width>128) minsize[0] = width;
	else minsize[0] = 128;
	if (style==5) minsize[1] = 96;
	else minsize[1] = 64;
	if (height>minsize[1]) minsize[1] = height;
}

void MTWindow::setmaxsize(int width,int height)
{
	if (width<screen->wr.right) maxsize[0] = width;
	if (height<screen->wr.bottom) maxsize[1] = height;
}

void MTWindow::updateborders()
{
	int cstyle = style & 0xFF;

	if (guiid==MTC_TABSHEET){
		br.left = br.top = br.right = br.bottom = 0;
	}
	else{
		skin->getwindowborders(cstyle,&br);
	};
	skin->getcontrolsize(MTC_WINDOW,cstyle,btnx,btny);
	skin->getcontrolsize(MTC_WINDOW,(cstyle==7)?18:17,btnw,btnh);
	btny += btnh;
	if (style & MTWS_CLOSE){
		btnx += btnw;
	};
	if (style & MTWS_MAXIMIZE){
		btnx += btnw;
	};
	if (style & MTWS_MINIMIZE){
		btnx += btnw;
	};
	if (style & MTWS_HELP){
		btnx += btnw;
	};
	if (style & MTWS_STAYONTOP){
		btnx += btnw;
	};
	updateregions();
}
//---------------------------------------------------------------------------
