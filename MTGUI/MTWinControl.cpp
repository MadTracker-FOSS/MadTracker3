//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//    MTControl
//      MTScroller
//      MTWinControl
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
#include "MTWinControl.h"
#include "MTTabControl.h"
//---------------------------------------------------------------------------
MTBitmap *db,*tb;		// Drag bitmaps
MTControl *dc;			// Drag control
MTRect dcr;					// Drag constrain recrangle
struct DragContext{
	int bx,by,ox,oy;
} *dctrl;						// Drag context
//---------------------------------------------------------------------------
// MTControl
//   MTScroller
//---------------------------------------------------------------------------
MTScroller::MTScroller(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_SCROLLER,tag,p,l,t,w,h),
pos(0),
maxpos(1),
incr(1),
page(1),
os(1.0),
slide(false),
bs(4),
minsize(4),
f(1),
carrow(-1),
ctimer(0),
speed(0)
{
	flags |= MTCF_ACCEPTINPUT|MTCF_TRANSPARENT;
	if (w<h){
		skin->getcontrolsize(MTC_SCROLLER,1,width,height);
		if (h) height = h;
		else height = 128;
		type = MTST_VBAR;
	}
	else{
		skin->getcontrolsize(MTC_SCROLLER,0,width,height);
		if (w) width = w;
		else width = 128;
		type = MTST_HBAR;
	};
	gi->setcontrolname(this,"scroller");
	updatemetrics();
}

MTScroller::~MTScroller()
{
	if (ctimer) gi->deltimer(this,ctimer);
}

int MTScroller::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	
	f->read(&type,28);
	return csize+28;
}

int MTScroller::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	
	f->write(&type,28);
	return csize+28;
}

#define ScrollerNP (ControlNP+5)
int MTScroller::getnumproperties(int id)
{
	if (id==-1) return ScrollerNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	if (id==ControlNP) return 2;
	return 0;
}

bool MTScroller::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[5] = {"Type","Position","Maximum","Increment","PageSize"};
	static int propflags[5] = {MTP_LIST,MTP_INT,MTP_INT,MTP_INT,MTP_INT};
	static char *subname[2] = {"Horizontal","Vertical"};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if ((id>>8)==ControlNP){
		id &= 0xFF;
		if (id>=2) return false;
		*name = subname[id];
		flags = -1;
		return true;
	};
	if (id>=ScrollerNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTScroller::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		iv = type;
		break;
	case 1:
		iv = pos;
		break;
	case 2:
		iv = maxpos;
		break;
	case 3:
		iv = incr;
		break;
	case 4:
		iv = page;
		break;
	default:
		return false;
	};
	return true;
}

bool MTScroller::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		if (type!=iv){
			int swap = width;
			width = height;
			height = swap;
			type = iv;
		};
		break;
	case 1:
		setposition(iv);
		return true;
	case 2:
		maxpos = iv;
		break;
	case 3:
		incr = iv;
		break;
	case 4:
		page = iv;
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

void MTScroller::setbounds(int l,int t,int w,int h)
{
	int tmp;

	if (design){
		if (w>h) type = MTST_HBAR;
		else if (h>w) type = MTST_VBAR;
	};
	if (type==MTST_HBAR) skin->getcontrolsize(MTC_SCROLLER,0,tmp,h);
	else skin->getcontrolsize(MTC_SCROLLER,1,w,tmp);
	MTControl::setbounds(l,t,w,h);
}

void MTScroller::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

void MTScroller::setaction(int action)
{
	int factor;

	carrow = action;
	if (parent){
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
	if (action<0){
		if (ctimer){
			gi->deltimer(this,ctimer);
			ctimer = 0;
		};
		return;
	};
	if (speed){
		factor = ((action>2)?guiprefs.scroll2:guiprefs.scroll1)/speed;
		if (factor<1) factor = 1;
	}
	else{
		factor = 1;
	};
	ctimer = gi->ctrltimer(this,0,factor,true);
}

void MTScroller::updatemetrics()
{
	int tmp;

	if (type==MTST_VBAR){
		skin->getcontrolsize(MTC_SCROLLER,1,width,tmp);
		skin->getcontrolsize(MTC_SCROLLER,3,tmp,minsize);
	}
	else{
		skin->getcontrolsize(MTC_SCROLLER,0,tmp,height);
		skin->getcontrolsize(MTC_SCROLLER,2,minsize,tmp);
	};
	if (minsize<4) minsize = 4;
}

bool MTScroller::message(MTCMessage &msg)
{
	int bw,bh;
	int p;
	
	skin->getcontrolsize(MTC_SCROLLER,(type==MTST_HBAR)?4:5,bw,bh);
	switch (msg.msg){
	case MTCM_MOUSEDOWN:
		speed = 0;
		if (msg.buttons & DB_LEFT) speed |= 1;
		if (msg.buttons & DB_RIGHT) speed |= 2;
		if ((msg.x>=0) && (msg.x<bw) && (msg.y>=0) && (msg.y<bh)){
			setaction(0);
			return true;
		};
		if (type==MTST_HBAR){
			if ((msg.x>=width-bw) && (msg.x<width) && (msg.y>=0) && (msg.y<bh)){
				setaction(1);
				return true;
			};
			p = bw+(int)(pos*f);
			if (msg.x<p){
				setaction(2);
				return true;
			}
			else if (msg.x<p+bs){
				if (page>=maxpos) return false;
				slide = true;
				slidepoint = msg.x;
				slideorigin = pos;
				return true;
			}
			else{
				setaction(3);
				return true;
			};
		}
		else{
			if ((msg.x>=0) && (msg.x<bw) && (msg.y>=height-bh) && (msg.y<height)){
				setaction(1);
				return true;
			};
			p = bh+(int)(pos*f);
			if (msg.y<p){
				setaction(2);
				return true;
			}
			else if (msg.y<p+bs){
				if (page>=maxpos) return false;
				slide = true;
				slidepoint = msg.y;
				slideorigin = pos;
				return true;
			}
			else{
				setaction(3);
				return true;
			};
		};
	case MTCM_MOUSEMOVE:
		if (slide){
			setposition(slideorigin+(int)((((type==MTST_HBAR)?msg.x:msg.y)-slidepoint)/f));
			return true;
		};
		break;
	case MTCM_MOUSEUP:
		speed = 0;
		if (msg.buttons & DB_LEFT) speed |= 1;
		if (msg.buttons & DB_RIGHT) speed |= 2;
		if (speed==0) setaction(-1);
		else setaction(carrow);
		slide = 0;
		break;
	case MTCM_TIMER:
		if ((timercount>1) && (timercount<6)) break;
		switch (carrow){
		case 0:
			setposition(pos-incr);
			break;
		case 1:
			setposition(pos+incr);
			break;
		case 2:
			setposition(pos-page);
			break;
		case 3:
			setposition(pos+page);
			break;
		};
		break;
	case MTCM_NOTIFY:
		updatemetrics();
		break;
	};
	return MTControl::message(msg);
}

void MTScroller::setposition(int p)
{
	int dx = 0;
	int dy = 0;
	
	if (p>maxpos-page) p = maxpos-page;
	if (p<0) p = 0;
	if (pos==p) return;
	if (parent){
		if (type==MTST_HBAR) dx = (int)((pos-p)*os);
		else dy = (int)((pos-p)*os);
		pos = p;
		MTCMessage msg = {MTCM_TOUCHED,0,this};
		parent->message(msg);
		if (flags & MTCF_SYSTEM) parent->offset(dx,dy);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//---------------------------------------------------------------------------
MTWinControl::MTWinControl(int id,int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(id,tag,p,l,t,w,h),
dsk(0),
mb(0),
ncontrols(0),
controls(0),
focused(0),
hs(0),
vs(0),
messageproc(0),
oprgn(0),
trrgn(0),
modalparent(0),
modalresult(MTDR_NULL),
cmoving(false),
frgn(0),
notifycount(0)
{
	flags |= MTCF_ACCEPTINPUT;
	if (parent) direct = parent->direct;
	else direct = false;
	if ((w==0) || (h==0)){
		width = 160;
		height = 96;
	};
	if ((guiid!=MTC_WINDOW) && (guiid!=MTC_DESKTOP)) flags |= MTCF_BORDER;
	controls = (MTControl**)si->memalloc(256*4,0);
	if (p){
		if (p->dsk) dsk = p->dsk;
		else if (p->guiid==MTC_DESKTOP) dsk = (MTDesktop*)p;
		else dsk = 0;
		messageproc = p->messageproc;
		if (p->mb) mb = p->mb;
		else direct = true;
		box = p->box+left;
		boy = p->boy+top;
	}
	else{
		if (!direct){
			mb = di->newbitmap(MTB_DRAW,width,height);
			mb->changeproc = mbchange;
			mb->param = this;
		};
		box = boy = 0;
	};
	updateregions();
}

MTWinControl::~MTWinControl()
{
	MTCMessage msg = {MTCM_DESTROY,0,this};
	if (messageproc) messageproc(this,msg);
	if ((window) && (window->wrapper)) window->wrapper->onmessage(msg);
	flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
	delcontrols(true);
	if ((!parent) && (mb)) di->delbitmap(mb);
	deletergn(oprgn);
	deletergn(trrgn);
	if (frgn) deletergn(frgn);
}

int MTWinControl::loadfromstream(MTFile *f,int size,int flags)
{
	this->flags |= MTCF_DONTRESIZE;
	int csize = MTControl::loadfromstream(f,size,flags);
	int nc,x,psize,ptype,ppos;
	
	f->read(&box,8);
	f->read(&nc,4);
	csize += 12;
	for (x=0;x<nc;x++){
		f->read(&psize,4);
		ppos = psize+f->pos();
		f->read(&ptype,4);
		f->seek(-4,MTF_CURRENT);
		MTControl *nctrl = gi->newcontrol(ptype,0,this,0,0,128,64,0);
		if (nctrl) nctrl->loadfromstream(f,psize,flags);
		f->seek(ppos,MTF_BEGIN);
		csize += psize+4;
	};
	this->flags &= ~MTCF_DONTRESIZE;
	updateregions();
	return csize;
}

int MTWinControl::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	int x,l,o,nc;
	
	nc = ncontrols;
	for (x=0;x<ncontrols;x++){
		if (controls[x]->flags & MTCF_DONTSAVE) nc--;
	};
	f->write(&box,8);
	f->write(&nc,4);
	csize += 12;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (!(cctrl.flags & MTCF_DONTSAVE)){
			o = f->pos();
			f->seek(4,MTF_CURRENT);
			l = cctrl.savetostream(f,flags);
			f->seek(o,MTF_BEGIN);
			f->write(&l,4);
			f->seek(l,MTF_CURRENT);
			csize += l+4;
		};
	};
	return csize;
}

void MTWinControl::setbounds(int l,int t,int w,int h)
{
	int x,n,ow,oh,cl,ct,cw,ch,pf,bflags,dflags;
	bool moved,resized;
	bool bdesign = design;
	void *flushrgn,*drawrgn;
	MTRect or = {0,0,0,0};
	MTRect dr,or2 = {0,0,0,0};

	FENTER4("MTWinControl::setbounds(%d,%d,%d,%d)",l,t,w,h);
	if (w<=0) w = width;
	if (h<=0) h = height;
	checkbounds(l,t,w,h);
	bflags = flags;
	moved = ((l!=left) || (t!=top));
	resized = ((w!=width) || (h!=height));
	ow = width;
	oh = height;
/*
	if (screen){
		if (w>screen->width){
			LOGD("%s - [GUI] WARNING: Too big width!"NL);
			w = screen->width;
		};
		if (h>screen->height){
			LOGD("%s - [GUI] WARNING: Too big height!"NL);
			h = screen->height;
		};
	};
*/
	if (flags & MTCF_HIDDEN){
		if ((moved) || (resized)){
			left = l;
			top = t;
			width = w;
			height = h;
			if (parent) boffset();
			else{
				flags &= ~MTCF_DONTFLUSH;
				createbitmap();
				flags |= MTCF_DONTFLUSH;
			};
			if (resized){
				updateregions();
				flags |= MTCF_DONTRESIZE;
				if ((!design) && (align) && (parent) && ((parent->flags & MTCF_DONTRESIZE)==0)){
					MTControl &cctrl = *parent;
					cl = cctrl.left;
					ct = cctrl.top;
					cw = cctrl.width;
					ch = cctrl.height;
					if (cctrl.align & 0x1) cl += w-ow;
					if (cctrl.align & 0x2) ct += h-oh;
					if (cctrl.align & 0x4) ch += h-oh;
					if (cctrl.align & 0x8) cw += w-ow;
					cctrl.setbounds(cl,ct,cw,ch);
				};
				design = false;
				for (x=0;x<ncontrols;x++){
					MTControl &cctrl = *controls[x];
					if ((cctrl.align) && ((cctrl.flags & MTCF_DONTRESIZE)==0)){
						cl = cctrl.left;
						ct = cctrl.top;
						cw = cctrl.width;
						ch = cctrl.height;
						if (cctrl.align & 0x1) cl += w-ow;
						if (cctrl.align & 0x2) ct += h-oh;
						if (cctrl.align & 0x4) ch += h-oh;
						if (cctrl.align & 0x8) cw += w-ow;
						cctrl.setbounds(cl,ct,cw,ch);
					};
				};
				flags &= (~MTCF_DONTRESIZE);
				design = bdesign;
			};	
		}
		else if (parent){
			box = parent->box+left;
			boy = parent->boy+top;
		};
		MTCMessage msg = {MTCM_BOUNDS,0,this,0,0,width,height};
		message(msg);
		if (parent){
			msg.x = left;
			msg.y = top;
			pf = parent->flags;
			parent->flags |= MTCF_DONTDRAW;
			parent->message(msg);
			parent->flags = pf;
		};
		if (notifycount>0){
			msg.msg = MTCM_POSCHANGED;
			msg.x = left;
			msg.y = top;
			for (x=0;x<ncontrols;x++){
				MTControl &cctrl = *controls[x];
				if (cctrl.flags & MTCF_NOTIFYPOS){
					cctrl.message(msg);
				};
			};
		};
		LEAVE();
		return;
	};
	flushrgn = recttorgn(or);
	drawrgn = copyrgn(flushrgn);
	getrect(or2,0);
	if (parent){
		pf = parent->flags;
		parent->flags |= MTCF_DONTFLUSH;
	};
	flags |= MTCF_DONTFLUSH;
	if ((moved) || (resized)){
		or.left = left;
		or.top = top;
		if (w<width) or.right = left+w;
		else or.right = left+width;
		if (h<height) or.bottom = top+h;
		else or.bottom = top+height;
		if (flags & MTCF_BORDER){
			or.left -= 2;
			or.top -= 2;
			or.right += 2;
			or.bottom += 2;
		};
		left = l;
		top = t;
		width = w;
		height = h;
		if (parent){
			boffset();
			if ((flags & MTCF_CANTDRAW)==0){
				dr.left = left;
				dr.top = top;
				dr.right = left+width;
				dr.bottom = top+height;
				if (flags & MTCF_BORDER){
					dr.left -= 2;
					dr.top -= 2;
					dr.right += 2;
					dr.bottom += 2;
				};
				void *cliprgn;                     // Update region
				void *brdrgn;                      // Borders region
				void *bltrgn;                      // Source client area being moved
				void *cdrawrgn;                    // Area to be redrawn
				void *corgn = recttorgn(or);       // Source client area
				void *cdrgn = recttorgn(dr);       // Destination client area
				void *drgn;                        // Destination area
				cl = dr.left-or.left;
				ct = dr.top-or.top;
				cliprgn = getvisiblergn(false);
				brdrgn = getvisiblergn(true);
				subtractrgn(brdrgn,cliprgn);
				offsetrgn(cliprgn,left,top);
				offsetrgn(brdrgn,left,top);
				bltrgn = copyrgn(oprgn);
				offsetrgn(bltrgn,left,top);
				if (resized) updateregions();
				if ((cl) || (ct)){                 // Blit
					drgn = copyrgn(oprgn);
					offsetrgn(drgn,left,top);
					intersectrgn(bltrgn,drgn);
					deletergn(drgn);
					intersectrgn(bltrgn,cliprgn);
					offsetrgn(bltrgn,-cl,-ct);
					intersectrgn(bltrgn,cliprgn);
					if ((!isemptyrgn(cliprgn)) && (!isemptyrgn(bltrgn))){
						n = rgngetnrects(bltrgn);
						if (ct>0){
							for (x=n-1;x>=0;x--){
								rgngetrect(bltrgn,x,&dr);
								parent->blt(dr.left+cl,dr.top+ct,dr.right-dr.left,dr.bottom-dr.top,dr.left,dr.top);
							};
						}
						else{
							for (x=0;x<n;x++){
								rgngetrect(bltrgn,x,&dr);
								parent->blt(dr.left+cl,dr.top+ct,dr.right-dr.left,dr.bottom-dr.top,dr.left,dr.top);
							};
						};
/*						parent->cliprgn(cliprgn);
						rgntorect(bltrgn,dr);
						parent->blt(dr.left+cl,dr.top+ct,dr.right-dr.left,dr.bottom-dr.top,dr.left,dr.top);
						parent->unclip();*/
					};
					offsetrgn(bltrgn,cl,ct);
				};
				deletergn(corgn);
				getrect(dr,1);
				corgn = recttorgn(dr);
				cdrawrgn = recttorgn(or2);         // Source area
				getrect(dr,0);
				drgn = recttorgn(dr);
				addrgn(cdrawrgn,drgn);
				deletergn(drgn);
				drgn = copyrgn(cdrawrgn);          // Borders
				intersectrgn(drgn,brdrgn);
//				subtractrgn(cdrawrgn,cdrgn);
				subtractrgn(cdrawrgn,corgn);
				deletergn(corgn);
				subtractrgn(cdrgn,bltrgn);         // Destination client area to be drawn
				addrgn(cdrawrgn,cdrgn);
				deletergn(cdrgn);
				intersectrgn(cdrawrgn,cliprgn);
				if (guiid==MTC_WINDOW) addrgn(cdrawrgn,drgn);
				deletergn(drgn);
				deletergn(cliprgn);
				deletergn(brdrgn);
				addrgn(drawrgn,cdrawrgn);
				addrgn(cdrawrgn,bltrgn);
				deletergn(bltrgn);
				addrgn(flushrgn,cdrawrgn);
				deletergn(cdrawrgn);
			};
		}
		else{
			flags &= ~MTCF_DONTFLUSH;
			createbitmap();
			flags |= MTCF_DONTFLUSH;
		};
		// Update child controls
		if (resized){
			if (((flags & MTCF_CANTDRAW)==0) && (parent) && ((w>ow) || (h>oh))){
				or.left = or.top = 0;
				or.right = w;
				or.bottom = h;
				dr = or;
				dr.right = ow;
				dr.bottom = oh;
				void *clrgn = getvisiblergn(false);
				void *orgn = recttorgn(or);
				void *drgn = recttorgn(dr);
				subtractrgn(orgn,drgn);
				offsetrgn(clrgn,left,top);
				offsetrgn(orgn,left,top);
				intersectrgn(orgn,clrgn);
				deletergn(clrgn);
				addrgn(drawrgn,orgn);
				deletergn(orgn);
				deletergn(drgn);
			};
			getrect(or2,0);
			void *orgn = recttorgn(or2);
			addrgn(flushrgn,orgn);
			deletergn(orgn);
			void *clrgn = getvisiblergn(false);
			cliprgn(clrgn);
			deletergn(clrgn);
			flags |= MTCF_DONTRESIZE;
			if ((!design) && (align) && (parent) && ((parent->flags & MTCF_DONTRESIZE)==0)){
				MTControl &cctrl = *parent;
				cl = cctrl.left;
				ct = cctrl.top;
				cw = cctrl.width;
				ch = cctrl.height;
				if (cctrl.align & 0x1) cl += w-ow;
				if (cctrl.align & 0x2) ct += h-oh;
				if (cctrl.align & 0x4) ch += h-oh;
				if (cctrl.align & 0x8) cw += w-ow;
				cctrl.setbounds(cl,ct,cw,ch);
			};
			flags |= MTCF_DONTDRAW;
			design = false;
			for (x=0;x<ncontrols;x++){
				MTControl &cctrl = *controls[x];
				if ((cctrl.align) && ((cctrl.flags & MTCF_DONTRESIZE)==0)){
					cl = cctrl.left;
					ct = cctrl.top;
					cw = cctrl.width;
					ch = cctrl.height;
					cctrl.getrect(or,0);
					clrgn = recttorgn(or);
					offsetrgn(clrgn,box,boy);
					addrgn(drawrgn,clrgn);
					addrgn(flushrgn,clrgn);
					deletergn(clrgn);
					if (cctrl.align & 0x1) cl += w-ow;
					if (cctrl.align & 0x2) ct += h-oh;
					if (cctrl.align & 0x4) ch += h-oh;
					if (cctrl.align & 0x8) cw += w-ow;
					cctrl.setbounds(cl,ct,cw,ch);
					cctrl.getrect(or,0);
					clrgn = recttorgn(or);
					offsetrgn(clrgn,box,boy);
					addrgn(drawrgn,clrgn);
					addrgn(flushrgn,clrgn);
					deletergn(clrgn);
				};
			};
			flags = bflags;
			unclip();
			design = bdesign;
		};
	}
	else if (parent){
		box = parent->box+left;
		boy = parent->boy+top;
	};
/*
if (guiid==MTC_WINDOW){
	if (parent){
		offsetrgn(drawrgn,parent->box,parent->boy);
		screen->open(0);
		tracergn(drawrgn,screen);
		screen->close(0);
		offsetrgn(drawrgn,-parent->box,-parent->boy);
	};
};
*/
	if (parent){
		for (x=rgngetnrects(drawrgn)-1;x>=0;x--){
			rgngetrect(drawrgn,x,&dr);
			parent->draw(dr);                   // Draw new area
		};
		parent->flags = pf;
		if (!isemptyrgn(flushrgn)){
			rgntorect(flushrgn,or);
			parent->flush(or);
		};
	};
	deletergn(drawrgn);
	deletergn(flushrgn);
	if (dsk){
		dflags = dsk->flags;
		dsk->flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
	};
	flags |= MTCF_DONTDRAW;
	MTCMessage msg = {MTCM_BOUNDS,0,this,0,0,width,height};
	message(msg);
	if (dsk) dsk->flags = (dflags|MTCF_DONTFLUSH);
	if (parent){
		msg.x = left;
		msg.y = top;
		parent->flags |= MTCF_DONTDRAW;
		parent->message(msg);
		parent->flags &= ~MTCF_DONTDRAW;
	};
	if (dsk) dsk->flags = dflags;
	if (notifycount>0){
		msg.msg = MTCM_POSCHANGED;
		msg.x = left;
		msg.y = top;
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_NOTIFYPOS){
				cctrl.message(msg);
			};
		};
	};
	LEAVE();
	if (bflags & MTCF_CANTDRAW){
		flags = bflags & (~(MTCF_DONTFLUSH));
		return;
	};
	flags = bflags & (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
}

bool MTWinControl::checkbounds(int &l,int &t,int &w,int &h)
{
	int x,cl,ct,cw,ch,ow,oh;
	bool ok = true;

	ow = width;
	oh = height;
	for (x=ncontrols-1;x>=0;x--){
		MTControl &cctrl = *controls[x];
		if ((cctrl.align) && ((cctrl.flags & MTCF_DONTRESIZE)==0)){
			cl = cctrl.left; ct = cctrl.top; cw = cctrl.width; ch = cctrl.height;
			if (cctrl.align & 0x1) cl += w-ow;
			if (cctrl.align & 0x2) ct += h-oh;
			if (cctrl.align & 0x4) ch += h-oh;
			if (cctrl.align & 0x8) cw += w-ow;
			if ((!cctrl.checkbounds(cl,ct,cw,ch)) && (cctrl.align>=MTCA_LEFT)){
				if (cctrl.align & 0x1) w = cl-cctrl.left+ow;
				if (cctrl.align & 0x2) h = ct-cctrl.top+oh;
				if (cctrl.align & 0x4) h = ch-cctrl.height+oh;
				if (cctrl.align & 0x8) w = cw-cctrl.width+ow;
				ok = false;
			};
		};
	};
	return ok & MTControl::checkbounds(l,t,w,h);
}


void MTWinControl::getrect(MTRect &r,int client)
{
	r.left = left;
	r.top = top;
	r.right = left+width;
	r.bottom = top+height;
	if ((client<=0) && (flags & MTCF_BORDER)){
		r.left -= 2;
		r.top -= 2;
		r.right += 2;
		r.bottom += 2;
	};
}

void *MTWinControl::getemptyrgn()
{
	int x;
	MTRect r = {0,0,width,height};
	void *rgn,*op;
	
	rgn = recttorgn(r);
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & MTCF_HIDDEN) continue;
		if (cctrl.guiid==MTC_WINDOW){
			r.left = cctrl.left;
			r.top = cctrl.top;
			r.right = cctrl.left+cctrl.width;
			r.bottom = cctrl.top+cctrl.height;
			op = recttorgn(r);
			subtractrgn(rgn,op);
			deletergn(op);
		}
		else if (cctrl.guiid & 2){
			if (cctrl.flags & MTCF_TRANSPARENT) continue;
			cctrl.getrect(r,0);
			op = recttorgn(r);
			subtractrgn(rgn,op);
			deletergn(op);
		};
	};
	return rgn;
}

void *MTWinControl::getvisiblergn(bool client,MTControl *control)
{
	MTRect cr,r;
	void *rgn,*op;
	int x;
	
	if (control){
		cr.left = cr.top = 0;
		cr.right = width;
		cr.bottom = height;
		rgn = recttorgn(cr);
		for (x=0;x<ncontrols;x++){
			MTControl &pctrl = *controls[x];
			if (&pctrl==control) break;
			if (pctrl.flags & MTCF_HIDDEN) continue;
			if ((pctrl.guiid!=MTC_WINDOW) && (pctrl.flags & MTCF_TRANSPARENT)){
				if ((client) || (guiid!=MTC_WINDOW)) continue;
			};
			pctrl.getrect(r,client);
			op = recttorgn(r);
			subtractrgn(rgn,op);
			deletergn(op);
		};
		return rgn;
	};
	if (parent){
		cr.left = -left;
		cr.top = -top;
		cr.right = parent->width-left;
		cr.bottom = parent->height-top;
		rgn = recttorgn(cr);
		for (x=0;x<parent->ncontrols;x++){
			MTControl &pctrl = *parent->controls[x];
			if (&pctrl==this) break;
			if (pctrl.flags & MTCF_HIDDEN) continue;
			if ((pctrl.guiid!=MTC_WINDOW) && (pctrl.flags & MTCF_TRANSPARENT)){
				if ((client) || (guiid!=MTC_WINDOW)) continue;
			};
			if (pctrl.guiid & 2){
				pctrl.getrect(r,client);
				op = recttorgn(r);
				offsetrgn(op,-left,-top);
				subtractrgn(rgn,op);
				deletergn(op);
			};
		};
		op = parent->getvisiblergn(client);
		offsetrgn(op,-left,-top);
		intersectrgn(rgn,op);
		deletergn(op);
	}
	else{
		cr.left = cr.top = 0;
		cr.right = width;
		cr.bottom = height;
		rgn = recttorgn(cr);
	};
	return rgn;
}

void* MTWinControl::getfixedrgn()
{
	int x;
	void *rgn,*op;
	bool fixedbkg = true;
	MTRect r = {0,0,0,0};

	rgn = recttorgn(r);
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.align==MTCA_TOPLEFT){
			cctrl.getrect(r,0);
			op = recttorgn(r);
			addrgn(rgn,op);
			deletergn(op);
		};
	};
	return rgn;
}

void MTWinControl::switchflags(int f,bool set)
{
	int x;
	
//	if (f & MTCF_FOCUSED) message((set)?entermsg:leavemsg);
	if (f & MTCF_FOCUSED){
		MTCMessage msg = {MTCM_ACTIVE,0,this,set};
		message(msg);
	};

	if ((focused) && (((f & MTCF_FOCUSED) && (!set)) || ((f & MTCF_HIDDEN) && (set)))){
		MTControl *old = focused;
		focused = 0;
		old->switchflags(MTCF_FOCUSED,false);
	};
	if (f & MTCF_SELECTED){
		if (set){
			if (guiid==MTC_TABSHEET){
				((MTTabControl*)parent)->setpage((MTWindow*)this);
			};
		}
		else{
			for (x=0;x<ncontrols;x++) controls[x]->switchflags(MTCF_SELECTED,false);
		};
	};
	MTControl::switchflags(f,set);
	if (f & MTCF_NOTIFYPOS){
		if (set) notifycount++;
		else{
			if (notifycount<=0) return;
			if (--notifycount){
				flags |= MTCF_NOTIFYPOS;
			};
		};
	};
}

void MTWinControl::draw(MTRect &rect)
{
	int x;
	int bflags;
	bool wasmodal = false;
	void *vis,*op;
	MTRect cr = {0,0,width,height};
	MTRect cr2;
	
	if (flags & MTCF_CANTDRAW) return;
	if (flags & MTCF_BORDER){
		cr.left -= 2;
		cr.top -= 2;
		cr.right += 2;
		cr.bottom += 2;
	};
	if (&rect){
		if (!cliprect(cr,rect)) return;
	};
	ENTER("MTWinControl::draw");
	bflags = flags;
	flags |= MTCF_DONTFLUSH;
	clip(cr);
	vis = getvisiblergn(true);
	op = copyrgn(mb->getclip());
	offsetrgn(op,-box,-boy);
	intersectrgn(vis,op);
	deletergn(op);
// Draw visible child controls
	for (x=ncontrols-1;x>=0;x--){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & MTCF_CANTDRAW) continue;
// If a child control is modal, make the control look darker
		if ((!wasmodal) && (!design)){
			signed char cmodal = (cctrl.flags)>>24;
			if ((cmodal>0) && (cmodal & 4)){
				wasmodal = true;
				skin->drawmodalveil(this,cr);
			};
		};
		cctrl.getrect(cr2,0);
		if (!cliprect(cr2,cr)) continue;
		op = getvisiblergn(true,&cctrl);
		intersectrgn(op,vis);
		if (rectinrgn(cr2,op)){
			cr2.left -= cctrl.left;
			cr2.top -= cctrl.top;
			cr2.right -= cctrl.left;
			cr2.bottom -= cctrl.top;
			cctrl.draw(cr2);
		};
		deletergn(op);
	};
	if (flags & MTCF_BORDER){
		int x = -2;
		int y = -2;
		MTBitmap *b;
		preparedraw(&b,x,y);
		skin->drawborder(this,rect,b,x,y);
	};
	deletergn(vis);
	unclip();
	flags = bflags;
	MTControl::draw(rect);
	LEAVE();
}

bool MTWinControl::message(MTCMessage &msg)
{
	int x,y,bflags;
	void *cl,*op;
	signed char cmodal = 0,modal = 0;
	bool candraw,ok;
	MTRect cr = {0,0,width,height};
	MTRect r;
	MTCMessage clk;
	
	FENTER2("MTWinControl::message(%.8X,'%s')",msg.msg,(msg.ctrl)?msg.ctrl->name:"N/A");
	if (messageproc){
		if (messageproc(this,msg)){
			LEAVE();
			return true;
		};
	};
	if ((window) && (window->wrapper)){
		if (window->wrapper->onmessage(msg)){
			LEAVE();
			return true;
		};
	};
	MTControl &cctrl = *msg.ctrl;
	if (msg.msg & MTCM_CHANGE){
		if ((design) && (guiid==MTC_WINDOW)) ((MTWindow*)this)->modified = true;
		MTWinControl *check = this;
		while (check){
			if (check->flags & MTCF_CANTDRAW){
				LEAVE();
				return MTControl::message(msg);
			};
			check = check->parent;
		};
		if (guiid!=MTC_WINDOW){
			bflags = flags;
			flags |= MTCF_DONTFLUSH;
		};
		if (&cctrl==this){
			if (msg.dr.right) cr = msg.dr;
			draw(cr);
		}
		else if (!(cctrl.flags & MTCF_DONTDRAW)){
			MTDesktop *p = (MTDesktop*)this;
			if (p->dsk) p = p->dsk;
			if (msg.dr.right){
				cr.left = msg.dr.left+cctrl.left;
				cr.right = msg.dr.right+cctrl.left;
				cr.top = msg.dr.top+cctrl.top;
				cr.bottom = msg.dr.bottom+cctrl.top;
			}
			else{
				cctrl.getrect(cr,0);
			};
			cl = recttorgn(cr);
			op = getvisiblergn(true);
			intersectrgn(cl,op);
			deletergn(op);
			op = getvisiblergn(true,&cctrl);
			intersectrgn(cl,op);
			deletergn(op);
			if (!isemptyrgn(cl)){
				cliprgn(cl);
				cr.left += box;
				cr.top += boy;
				cr.right += box;
				cr.bottom += boy;
				if (cctrl.flags & (MTCF_TRANSPARENT|MTCF_HIDDEN)){
					p->draw(cr);
				}
				else{
					if (msg.dr.right) cctrl.draw(msg.dr);
					else cctrl.draw(NORECT);
					p->drawover(this,cr);
					if (guiid==MTC_WINDOW) p->flush(cr);
				};
				unclip();
				cr.left -= box;
				cr.top -= boy;
				cr.right -= box;
				cr.bottom -= boy;
			};
			deletergn(cl);
		};
		if (guiid!=MTC_WINDOW){
			flags = bflags;
			flush(cr);
		};
		LEAVE();
		return MTControl::message(msg);
	};
	if ((msg.msg>=MTCM_BOUNDS) && (msg.msg<=MTCM_FLUSH)){
		candraw = !(flags & MTCF_CANTDRAW);
		if (dsk) candraw &= !(dsk->flags & MTCF_CANTDRAW);
		if ((&cctrl==this) && (flags & MTCF_BORDER)){
			cr.left -= 2;
			cr.top -= 2;
			cr.right += 2;
			cr.bottom += 2;
		};
		cl = recttorgn(cr);
		op = getvisiblergn(false);
		intersectrgn(cl,op);
		deletergn(op);
		cliprgn(cl);
		deletergn(cl);
	};
	switch (msg.msg){
	case MTCM_BOUNDS:
		if ((design) && (guiid==MTC_WINDOW)) ((MTWindow*)this)->modified = true;
/*
		if (candraw){
			cctrl.draw(cr);
			if (msg.w) flush(msg.x,msg.y,msg.w,msg.h);
			else flush(cctrl.left,cctrl.top,cctrl.width,cctrl.height);
		};
*/
		break;
	case MTCM_FLUSH:
		if (msg.w) msg.result = flush(cctrl.left+msg.x,cctrl.top+msg.y,msg.w,msg.h);
		else msg.result = flush(cctrl.left,cctrl.top,cctrl.width,cctrl.height);
		break;
	case MTCM_MOUSEDOWN:
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_CANTTOUCH) continue;
			if (!design){
				cmodal = (cctrl.flags>>24);
				if (modal==0) modal = cmodal;
				else if ((modal & 4) && (cmodal<modal)) continue;
				if ((cmodal>1) && (cmodal<8) && (cmodal<modal)) continue;
			};
			cctrl.getrect(r,-1);
			if ((msg.x>=r.left) && (msg.x<r.right) && (msg.y>=r.top) && (msg.y<r.bottom)){
				if (cctrl.guiid & 2){
					msg.x -= cctrl.left;
					msg.y -= cctrl.top;
					focus(&cctrl);
					if ((design) && (cctrl.guiid!=MTC_MENU)){
						ok = cctrl.message(msg);
						if (cctrl.designmessage(msg)){
							if ((cctrl.moving) && (!dc)){
								initdrag(cctrl);
							};
						};
						LEAVE();
						return ok;
					};
					LEAVE();
					return cctrl.message(msg);
				};
				if (cctrl.guiid!=MTC_MENUITEM){
					if ((design) && (cctrl.flags & MTCF_DONTSAVE)){
						if ((cctrl.guiid==MTC_BUTTON) && (guiid==MTC_TABCONTROL)){
							((MTTabControl*)this)->setpageid(cctrl.tag);
						};
						continue;
					};
					msg.x -= cctrl.left;
					msg.y -= cctrl.top;
					if (design){
						if (msg.button==DB_LEFT){
							if (msg.buttons & DB_SHIFT) cctrl.switchflags(MTCF_SELECTED,(cctrl.flags & MTCF_SELECTED)?false:true);
							else{
								if ((cctrl.flags & MTCF_SELECTED)==0){
									switchflags(MTCF_SELECTED,false);
									cctrl.switchflags(MTCF_SELECTED,true);
									focused = &cctrl;
								};
								if (cctrl.designmessage(msg)){
									if (cctrl.moving){
										initdrag(cctrl);
										LEAVE();
										return true;
									};
								};
							};
						};
						LEAVE();
						if (cctrl.flags & MTCF_DONTSAVE) return false;
						return false;
					};
				};
				if (msg.button==btnupmsg.button){
					if (btnctrl){
						btnupmsg.ctrl = btnctrl;
						btnupmsg.buttons = msg.buttons;
						btnctrl->message(btnupmsg);
						btnctrl = 0;
					};
				};
				btnctrl = &cctrl;
				focus(&cctrl);
				if (msg.button==DB_LEFT){
					cctrl.switchflags(MTCF_DOWN,true);
				};
				LEAVE();
				return cctrl.message(msg);
			};
		};
		if (design){
			if (msg.button==DB_LEFT){
				if (msg.buttons & DB_SHIFT) switchflags(MTCF_SELECTED,(flags & MTCF_SELECTED)?false:true);
				else if (((flags & MTCF_SELECTED)==0) || (guiid==MTC_DESKTOP)){
					if (parent) parent->switchflags(MTCF_SELECTED,false);
					switchflags(MTCF_SELECTED,false);
					switchflags(MTCF_SELECTED,true);
				};
			};
			if (focused){
				MTControl *old = focused;
				focused = 0;
				if (old) old->switchflags(MTCF_FOCUSED,false);
			};
			switchflags(MTCF_FOCUSED,true);
			designmessage(msg);
			LEAVE();
			return true;
		}
		else{
			if (msg.button==btnupmsg.button){
				if (btnctrl){
					btnupmsg.ctrl = btnctrl;
					btnupmsg.buttons = msg.buttons;
					btnctrl->message(btnupmsg);
					btnctrl = 0;
				};
			};
			if (focused){
				MTControl *old = focused;
				focused = 0;
				if (old) old->switchflags(MTCF_FOCUSED,false);
			};
			switchflags(MTCF_FOCUSED,true);
		};
		break;
	case MTCM_MOUSEMOVE:
		if (cmoving){
			drag(msg.x,msg.y);
			LEAVE();
			return true;
		};
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_CANTTOUCH) continue;
			cctrl.getrect(r,-1);
			if ((msg.x>=r.left) && (msg.x<r.right) && (msg.y>=r.top) && (msg.y<r.bottom)){
				if (((cctrl.guiid & 0xF)==MTC_CONTROL) && (overctrl!=&cctrl)){
					if ((design) && (cctrl.flags & MTCF_DONTSAVE) && (cctrl.guiid!=MTC_MENUITEM)) continue;
					if (overctrl) overctrl->switchflags(MTCF_OVER,false);
					cctrl.switchflags(MTCF_OVER,true);
					overctrl = &cctrl;
				};
				break;
			};
		};
		if ((focused) && (msg.buttons & (DB_LEFT|DB_RIGHT|DB_MIDDLE))){
			MTControl &cctrl = *focused;
			msg.x -= cctrl.left;
			msg.y -= cctrl.top;
			LEAVE();
			if ((design) && (cctrl.designmessage(msg))) return true;
			return cctrl.message(msg);
		};
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_CANTTOUCH) continue;
			cctrl.getrect(r,-1);
			if ((msg.x>=r.left) && (msg.x<r.right) && (msg.y>=r.top) && (msg.y<r.bottom)){
				msg.x -= cctrl.left;
				msg.y -= cctrl.top;
				LEAVE();
				return cctrl.message(msg);
			};
		};
		if (overctrl!=this){
			if (overctrl) overctrl->switchflags(MTCF_OVER,false);
			switchflags(MTCF_OVER,true);
			overctrl = this;
		};
		if ((design) && (designmessage(msg))){
			LEAVE();
			return true;
		};
		break;
	case MTCM_MOUSEUP:
		if (dc){
			dc->parent->enddrag();
			LEAVE();
			return true;
		};
		if ((msg.button==btnupmsg.button) && (btnctrl)){
//			if ((btnctrl!=focused) && (btnctrl->parent==this)){
				btnctrl->switchflags(MTCF_DOWN,false);
				msg.x -= btnctrl->left;
				msg.y -= btnctrl->top;
				btnctrl->message(msg);
				msg.x += btnctrl->left;
				msg.y += btnctrl->top;
//			};
			btnctrl = 0;
		};
		clk.msg = MTCM_MOUSECLICK;
		clk.result = 0;
		if (focused){
			MTControl &cctrl = *focused;
			bool clicked = false;
			cctrl.getrect(r,-1);
			if ((msg.x>=r.left) && (msg.x<r.right) && (msg.y>=r.top) && (msg.y<r.bottom)) clicked = true;
			MTCMessage msg2 = msg;
			msg2.x -= cctrl.left;
			msg2.y -= cctrl.top;
			if (msg.button==DB_LEFT){
				cctrl.switchflags(MTCF_DOWN,false);
			};
			LEAVE();
			if ((design) && (cctrl.guiid!=MTC_MENUITEM) && (cctrl.designmessage(msg))) return true;
			if (cctrl.message(msg2)) return true;
			if (clicked){
				clk.ctrl = &cctrl;
				clk.button = msg.button;
				clk.buttons = msg.buttons;
				clk.x = msg2.x;
				clk.y = msg2.y;
				return cctrl.message(clk);
			};
			return false;
		};
		if ((design) && (designmessage(msg))){
			LEAVE();
			return true;
		};
		clk.ctrl = this;
		clk.button = msg.button;
		clk.buttons = msg.buttons;
		clk.x = msg.x;
		clk.y = msg.y;
		LEAVE();
		return message(clk);
	case MTCM_ACTION:
		break;
	case MTCM_MOUSEWHEEL:
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_CANTTOUCH) continue;
			if (!design){
				cmodal = (cctrl.flags>>24);
				if (modal==0) modal = cmodal;
				else if ((modal & 4) && (cmodal<modal)) continue;
				if ((cmodal>1) && (cmodal<8) && (cmodal<modal)) continue;
			};
			if (cctrl.guiid & 2){
				cctrl.getrect(r,-1);
				if ((msg.x>=r.left) && (msg.x<r.right) && (msg.y>=r.top) && (msg.y<r.bottom)){
					msg.x -= cctrl.left;
					msg.y -= cctrl.top;
					LEAVE();
					return cctrl.message(msg);
				};
			};
		};
		if ((hs) || (vs)){
			if (vs){
				vs->setposition(vs->pos-msg.param3*vs->incr);
				LEAVE();
				return true;
			}
			else{
				hs->setposition(hs->pos-msg.param3*hs->incr);
				LEAVE();
				return true;
			};
		};
		break;
	case MTCM_NOTIFY:
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.guiid & 2){
				y = cctrl.flags;
				cctrl.flags |= (flags & MTCF_DONTDRAW);
				cctrl.message(msg);
				cctrl.flags = y;
			};
		};
		break;
	case MTCM_ACTIVE:
	case MTCM_REFRESH:
		for (x=0;x<ncontrols;x++){
			controls[x]->message(msg);
		};
	case MTCM_MOUSECLICK:
		break;
	case MTCM_POSCHANGED:
		if (notifycount==0) break;
		for (x=0;x<ncontrols;x++){
			MTControl &cctrl = *controls[x];
			if (cctrl.flags & MTCF_NOTIFYPOS){
				cctrl.message(msg);
			};
		};
		break;
	default:
		if ((!focused) && (msg.msg==MTCM_KEYDOWN)){
			nextcontrol(0,false);
		};
		if (focused){
			if (focused->message(msg)){
				LEAVE();
				return true;
			};
		};
	};
	if ((msg.msg>=MTCM_BOUNDS) && (msg.msg<=MTCM_FLUSH)) unclip();
	LEAVE();
	return MTControl::message(msg);
}

void MTWinControl::preparedraw(MTBitmap **b,int &ox,int &oy)
{
	if (mb) *b = mb;
	else{
		screen->setwindow(this);
		*b = screen;
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	ox += box;
	oy += boy;
}

void MTWinControl::addcontrol(MTControl *control)
{
	int nacontrols = ((ncontrols+15)>>4)<<4;
	int nacontrols2 = ((ncontrols+16)>>4)<<4;
	int x;
	signed char cmodal;
	
	if (control->flags & MTCF_NOTIFYPOS){
		switchflags(MTCF_NOTIFYPOS,true);
	};
	if (design){
		if (guiid==MTC_WINDOW) ((MTWindow*)this)->modified = true;
		else if (window) window->modified = true;
	};
	if (nacontrols<32) nacontrols = 32;
	if (nacontrols2<32) nacontrols2 = 32;
	if (!controls) nacontrols = 0;
	if (nacontrols2>nacontrols){
		if (controls)
			controls = (MTControl**)si->memrealloc(controls,nacontrols2*4);
		else
			controls = (MTControl**)si->memalloc(nacontrols2*4,0);
	};
	if (guiid==MTC_DESKTOP){
		cmodal = (control->flags>>24);
		for (x=ncontrols;x>0;x--){
			MTWinControl &cw = *(MTWinControl*)controls[x-1];
			if ((signed char)(cw.flags>>24)>cmodal) break;
			controls[x] = &cw;
		};
		if (x<0) x = 0;
		controls[x] = control;
	}
	else controls[ncontrols] = control;
	if (control->guiid & 2){
		MTWinControl &cw = *(MTWinControl*)control;
		cw.box = box+cw.left;
		cw.boy = boy+cw.top;
	};
	ncontrols++;
}

void MTWinControl::delcontrol(MTControl *control)
{
	int nacontrols = ((ncontrols+15)>>4)<<4;
	int nacontrols2 = ((ncontrols+14)>>4)<<4;
	int x,y;
	bool f = false;
	
	if (control->flags & MTCF_NOTIFYPOS){
		switchflags(MTCF_NOTIFYPOS,false);
	};
	control->flags |= MTCF_HIDDEN;
	control->flags &= (~MTCF_DONTDRAW);
	MTCMessage msg = {MTCM_CHANGE,0,control};
	control->getrect(msg.dr,0);
	msg.dr.left -= control->left;
	msg.dr.right -= control->left;
	msg.dr.top -= control->top;
	msg.dr.bottom -= control->top;
	message(msg);
	if (nacontrols<256) nacontrols = 256;
	if (nacontrols2<256) nacontrols2 = 256;
	if (focused==control){
		f = true;
		focused = 0;
	};
	for (x=0;x<ncontrols;x++){
		if (controls[x]==control){
			for (y=x;y<ncontrols-1;y++)
				controls[y] = controls[y+1];
			controls[ncontrols-1] = 0;
			if (nacontrols2<nacontrols){
				if (nacontrols2)
					controls = (MTControl**)si->memrealloc(controls,nacontrols2*4);
				else{
					si->memfree(controls);
					controls = 0;
				};
			};
			ncontrols--;
			break;
		};
	};
	if (f){
		for (x=0;x<ncontrols;x++){
			if ((!(controls[x]->flags & MTCF_HIDDEN)) && (controls[x]->flags & MTCF_ACCEPTINPUT)){
				focused = controls[x];
				focused->switchflags(MTCF_FOCUSED,true);
				break;
			};
		};
	};
}

void MTWinControl::delcontrols(bool del)
{
	if (del){
		while (ncontrols>0) gi->delcontrol(controls[0]);
	};
	si->memfree(controls);
	controls = 0;
	ncontrols = 0;
}

int MTWinControl::getnumcontrols()
{
	int x,n;
	
	n = 0;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if ((cctrl.flags & MTCF_DONTSAVE)==0){
			if (cctrl.guiid & 2) n += ((MTWinControl*)controls[x])->getnumcontrols();
			n++;
		};
	};
	return n;
}

MTControl* MTWinControl::getcontrol(int id)
{
	int x,n,cn;
	
	n = 0;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if ((cctrl.flags & MTCF_DONTSAVE)==0){
			if (id==n++) return &cctrl;
			if (cctrl.guiid & 2){
				cn = ((MTWinControl*)&cctrl)->getnumcontrols();
				if (id<n+cn) return ((MTWinControl*)&cctrl)->getcontrol(id-n);
				n += cn;
			};
		};
	};
	return 0;
}

int MTWinControl::getcontrolid(MTControl *ctrl)
{
	int x,id,cn,n;
	
	id = 0;
	if (ctrl->flags & MTCF_DONTSAVE) return -1;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (ctrl==&cctrl) return id;
		if ((cctrl.flags & MTCF_DONTSAVE)==0){
			if (cctrl.guiid & 2){
				cn = ((MTWinControl*)&cctrl)->getnumcontrols();
				n = ((MTWinControl*)&cctrl)->getcontrolid(ctrl);
				if (n>=0) return id+n+1;
				id += cn;
			};
			id++;
		};
	};
	return -1;
}

MTControl* MTWinControl::getcontrolfromuid(int uid)
{
	int x;
	MTControl *res;
	
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
//		if (cctrl.flags & MTCF_DONTSAVE) continue;
		if (cctrl.uid==uid) return &cctrl;
		if (cctrl.guiid & 2){
			res = ((MTWinControl*)&cctrl)->getcontrolfromuid(uid);
			if (res) return res;
		};
	};
	return 0;
}

MTControl* MTWinControl::getcontrolfrompoint(MTPoint &p)
{
	int x;
	MTRect r;
	
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & (MTCF_DONTSAVE|MTCF_HIDDEN)) continue;
		cctrl.getrect(r,0);
		if (pointinrect(p,r)){
			if (cctrl.guiid & 2){
				MTPoint p2 = {p.x-cctrl.left,p.y-cctrl.top};
				return ((MTWinControl*)&cctrl)->getcontrolfrompoint(p2);
			};
			return &cctrl;
		};
	};
	return this;
}

void MTWinControl::nextcontrol(MTControl *start,bool reverse)
{
	int x,sx,n;
	MTControl *cctrl;
	MTControl *old;
	MTWinControl *p;
	bool ok = false;
	
	n = getnumcontrols();
	if (!n) return;
	if (reverse) x = n-1;
	else x = 0;
	if (!start) ok = true;
	sx = 0;
	while (true){
		cctrl = getcontrol(x);
		if ((ok) && (cctrl->flags & MTCF_ACCEPTINPUT) && !(cctrl->flags & MTCF_HIDDEN) && !(cctrl->parent->flags & MTCF_HIDDEN)) break;
		if (cctrl==start){
			if (ok){
				cctrl = 0;
				break;
			};
			ok = true;
			sx = 0;
		};
		if (++sx==n){
			cctrl = 0;
			break;
		};
		if (reverse){
			if (x--==0) x = n-1;
		}
		else{
			if (++x==n) x = 0;
		};
	};
	if (!cctrl) return;
	p = cctrl->parent;
	old = p->focused;
	p->focused = cctrl;
	if (old) old->switchflags(MTCF_FOCUSED,false);
	cctrl->switchflags(MTCF_FOCUSED,true);
	while ((p!=parent) && (p->parent)){
		old = p->parent->focused;
		if (old!=p){
			p->parent->focused = p;
			if (old) old->switchflags(MTCF_FOCUSED,false);
			p->switchflags(MTCF_FOCUSED,true);
		};
		p = p->parent;
	};
	return;
}

void *MTWinControl::getoffsetrgn(int type)
{
	MTRect r = {0,0,width,height};
	void *rgn,*op;
	
	rgn = recttorgn(r);
	if (hs){
		hs->getrect(r,0);
		op = recttorgn(r);
		subtractrgn(rgn,op);
		deletergn(op);
	};
	if (vs){
		vs->getrect(r,0);
		op = recttorgn(r);
		subtractrgn(rgn,op);
		deletergn(op);
	};
	return rgn;
}

void MTWinControl::offset(int ox,int oy)
{
	if (ox) moffset(ox,0);
	if (oy) moffset(0,oy);
	flush();
}

void MTWinControl::moffset(int ox,int oy)
{
	int x,n;
	void *rgn,*crgn,*crgn2,*rgn2;
	MTRect r,r2;
	
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if ((cctrl.guiid==MTC_SCROLLER) && (cctrl.flags & MTCF_SYSTEM)) continue;
		cctrl.left += ox;
		cctrl.top += oy;
	};
	if (flags & MTCF_CANTDRAW) return;
	if (ox) rgn = getoffsetrgn(0);
	else rgn = getoffsetrgn(1);
	crgn = getvisiblergn(false);
	crgn2 = getvisiblergn(true);
	subtractrgn(crgn2,crgn);
	intersectrgn(rgn,crgn);
	cliprgn(rgn);
	rgntorect(rgn,r);
	r2 = r;
	r2.left += ox;
	r2.top += oy;
	if (r2.left<0){
		r.left -= r2.left;
		r2.left = 0;
	};
	if (r2.top<0){
		r.top -= r2.top;
		r2.top = 0;
	};
	blt(r2.left,r2.top,r.right-r.left,r.bottom-r.top,r.left,r.top);
	unclip();
	rgn2 = copyrgn(rgn);
	offsetrgn(rgn2,ox,oy);
	intersectrgn(rgn2,crgn);
	subtractrgn(rgn,rgn2);
	n = rgngetnrects(rgn);
	for (x=0;x<n;x++){
		rgngetrect(rgn,x,&r);
		draw(r);
	};
	if (!isemptyrgn(crgn2)){
		MTWinControl *cparent = this;
		for (x=rgngetnrects(crgn2)-1;x>=0;x--){
			rgngetrect(crgn2,x,&r);
			cparent->draw(r);
			r.left += box;
			r.top += boy;
			r.right += box;
			r.bottom += boy;
			dsk->drawover(parent,r);
		};
	};
	deletergn(rgn);
	deletergn(crgn);
	deletergn(crgn2);
	deletergn(rgn2);
}

void MTWinControl::boffset(bool children)
{
	int x;
	
	if (!children){
		if (parent){
			box = parent->box+left;
			boy = parent->boy+top;
		}
		else{
			box = left;
			boy = top;
		};
	};
	for (x=0;x<ncontrols;x++){
		MTWinControl &cctrl = *(MTWinControl*)controls[x];
		if (cctrl.guiid & 2) cctrl.boffset();
	};
}

void MTWinControl::createbitmap()
{
	if ((!parent) && (mb)) mb->setsize(width,height);
	draw(NORECT);
}

void MTWinControl::deletebitmap()
{
	if ((!parent) && (mb)) mb->unload();
}

void MTWinControl::mbchange(MTBitmap *oldbitmap,MTBitmap *newbitmap,void *param)
{
	MTWinControl *wnd = (MTWinControl*)param;
	int x;

	if (wnd->mb!=oldbitmap) return;
	wnd->mb = newbitmap;
	for (x=0;x<wnd->ncontrols;x++){
		MTWinControl *cwnd = (MTWinControl*)wnd->controls[x];
		if (cwnd->guiid & 2){
			if (cwnd->mb==oldbitmap) cwnd->mbchange(oldbitmap,newbitmap,cwnd);
		};
	};
}

bool MTWinControl::flush()
{
	return flush(0,0,width,height);
}

bool MTWinControl::flush(MTRect &rect)
{
	return flush(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
}

bool MTWinControl::flush(int x,int y,int w,int h)
{
	int a;
	bool ok;
	
	if ((!screen) || (flags & MTCF_DONTFLUSH)) return false;
	if (!mb) return true;
	FENTER4("MTWinControl::flush(%d,%d,%d,%d)",x,y,w,h);
	flags |= MTCF_DONTFLUSH;
	for (a=0;a<ncontrols;a++){
		MTControl &cctrl = *controls[a];
		if (cctrl.flags & MTCF_NEEDUPDATE){
			cctrl.flags &= (~MTCF_NEEDUPDATE);
			ok = cctrl.direct;
			cctrl.direct = false;
			cctrl.draw(NORECT);
			cctrl.direct = ok;
		};
	};
	flags &= ~MTCF_DONTFLUSH;
	if (x<0){
		w += x;
		x = 0;
	};
	if (y<0){
		h += y;
		y = 0;
	};
	if (x+w>width) w = width-x;
	if (y+h>height) h = height-y;
	if ((w<=0) || (h<=0)){
		LEAVE();
		return true;
	};
	if (parent){
		MTCMessage msg = {MTCM_FLUSH,0,this,x,y,w,h};
		parent->message(msg);
		LEAVE();
		return (msg.result!=0);
	};
	if (frgn){
		MTRect r = {x,y,x+w,y+h};
		void *op = recttorgn(r);
		addrgn(frgn,op);
		deletergn(op);
		LEAVE();
		return true;
	};
	screen->setwindow(this);
#ifdef _DEBUG
/*	if (GetKeyState(VK_CONTROL)<0){
		screen->setbrush(-1);
		screen->setpen(0xFFFFFF);
		screen->rectangle(screen->wr.left+left+x,screen->wr.top+top+y,w,h);
		si->syswait(100);
		FLOGD1("%s [GUI] Flush from %s"NL,CALLSTACK);
	};*/
#endif
	ok = bltbmp(screen,screen->wr.left+left+x,screen->wr.top+top+y,w,h,x,y);
#ifdef _DEBUG
/*	if (GetKeyState(VK_CONTROL)<0){
		si->syswait(100);
	};*/
#endif
	LEAVE();
	return ok;
}

void MTWinControl::flushstart()
{
	MTRect r = {0,0,0,0};

	if (frgn) flushend();
	frgn = recttorgn(r);
}

bool MTWinControl::flushend()
{
	int x;
	bool ok;
	MTRect r;

	if (!frgn) return false;
	ENTER("MTWinControl::flushend");
	screen->setwindow(this);
#ifdef _DEBUG
/*	if (GetKeyState(VK_CONTROL)<0){
		screen->setbrush(-1);
		screen->setpen(0xFFFFFF);
		offsetrgn(frgn,screen->wr.left+left,screen->wr.top+top);
		tracergn(frgn,screen);
		offsetrgn(frgn,-screen->wr.left-left,-screen->wr.top-top);
		si->syswait(100);
		FLOGD1("%s [GUI] Region flush from %s"NL,CALLSTACK);
	};*/
#endif
	di->sync();
	ok = true;
	for (x=rgngetnrects(frgn)-1;x>=0;x--){
		rgngetrect(frgn,x,&r);
		ok &= bltbmp(screen,screen->wr.left+left+r.left,screen->wr.top+top+r.top,r.right-r.left,r.bottom-r.top,r.left,r.top);
	};
	deletergn(frgn);
	frgn = 0;
#ifdef _DEBUG
/*	if (GetKeyState(VK_CONTROL)<0){
		si->syswait(100);
	};*/
#endif
	LEAVE();
	return ok;
}

void *MTWinControl::open(int type)
{
	if (mb) return mb->open(type);
	return screen->open(type);
}

void MTWinControl::close(void *o)
{
	if (mb) mb->close(o);
	else screen->close(o);
}

void MTWinControl::clip(MTRect &rect)
{
	MTRect nr = rect;
	
	if (!mb){
		screen->setwindow(this);
		nr.left += screen->wr.left;
		nr.right += screen->wr.left;
		nr.top += screen->wr.top;
		nr.bottom += screen->wr.top;
	};
	nr.left += box;
	nr.right += box;
	nr.top += boy;
	nr.bottom += boy;
	if (mb) mb->clip(&nr);
	else screen->clip(&nr);
}

void MTWinControl::cliprgn(void *rgn)
{
	void *nrgn;
	
	nrgn = copyrgn(rgn);
	if (mb){
		offsetrgn(nrgn,box,boy);
		mb->cliprgn(nrgn);
	}
	else{
		screen->setwindow(this);
		offsetrgn(nrgn,screen->wr.left+box,screen->wr.top+boy);
		screen->cliprgn(nrgn);
	};
	deletergn(nrgn);
}

void MTWinControl::unclip()
{
	if (mb) mb->unclip();
	else screen->unclip();
}

bool MTWinControl::bmpblt(MTBitmap *src,int x,int y,int w,int h,int ox,int oy,int mode)
{
	if (mb){
#ifdef _DEBUG
/*		if (GetKeyState(VK_CONTROL)<0){
			screen->setbrush(-1);
			screen->setpen(0xFF00FF);
			screen->rectangle(screen->wr.left+box+x,screen->wr.top+boy+y,w,h);
			si->syswait(200);
			bool ok = src->blt(mb,box+x,boy+y,w,h,ox,oy,mode);
			flush(box+x,boy+y,w,h);
			return ok;
		};*/
#endif
		return src->blt(mb,box+x,boy+y,w,h,ox,oy,mode);
	}
	else{
		screen->setwindow(this);
		return src->blt(screen,x+screen->wr.left+box,y+screen->wr.top+boy,w,h,ox,oy,mode);
	};
}

bool MTWinControl::bltbmp(MTBitmap *dest,int x,int y,int w,int h,int ox,int oy,int mode)
{
	if (mb){
		return mb->blt(dest,x,y,w,h,box+ox,boy+oy,mode);
	}
	else{
		screen->setwindow(this);
		return screen->blt(dest,x,y,w,h,ox+screen->wr.left+box,oy+screen->wr.top+boy,mode);
	};
}

bool MTWinControl::sbmpblt(MTBitmap *src,int x,int y,int w,int h,int ox,int oy,int ow,int oh,int mode)
{
	if (mb){
		return src->sblt(mb,box+x,boy+y,w,h,ox,oy,ow,oh,mode);
	}
	else{
		screen->setwindow(this);
		return src->sblt(screen,ox+screen->wr.left+box,y+screen->wr.top+boy,w,h,ox,oy,ow,oh,mode);
	};
}

bool MTWinControl::sbltbmp(MTBitmap *dest,int x,int y,int w,int h,int ox,int oy,int ow,int oh,int mode)
{
	if (mb){
		return mb->sblt(dest,x,y,w,h,box+ox,boy+oy,ow,oh,mode);
	}
	else{
		screen->setwindow(this);
		return screen->sblt(dest,x,y,w,h,ox+screen->wr.left+box,oy+screen->wr.top+boy,ow,oh,mode);
	};
}

bool MTWinControl::blt(int x,int y,int w,int h,int ox,int oy,int mode)
{
	if (mb){
#ifdef _DEBUG
/*		if (GetKeyState(VK_CONTROL)<0){
			screen->setbrush(-1);
			screen->setpen(0x00FF00);
			screen->rectangle(screen->wr.left+box+ox,screen->wr.top+boy+oy,w,h);
			screen->setpen(0x00FFFF);
			screen->rectangle(screen->wr.left+box+x,screen->wr.top+boy+y,w,h);
			si->syswait(200);
			bool ok = mb->blt(mb,box+x,boy+y,w,h,box+ox,boy+oy,mode);
			flush(box+ox,boy+oy,w,h);
			flush(box+x,boy+y,w,h);
			return ok;
		};*/
#endif
		return mb->blt(mb,box+x,boy+y,w,h,box+ox,boy+oy,mode);
	}
	else{
		screen->setwindow(this);
		return screen->blt(screen,x+screen->wr.left+box,y+screen->wr.top+boy,w,h,ox+screen->wr.left+box,oy+screen->wr.top+boy,mode);
	};
}

bool MTWinControl::skinblt(int x,int y,int w,int h,MTSkinPart &o)
{
	if (mb){
#ifdef _DEBUG
/*		if (GetKeyState(VK_CONTROL)<0){
			screen->setbrush(-1);
			screen->setpen(0xFF00FF);
			screen->rectangle(screen->wr.left+box+x,screen->wr.top+boy+y,w,h);
			si->syswait(200);
			bool ok = mb->skinblt(box+x,boy+y,w,h,o);
			flush(box+x,boy+y,w,h);
			return ok;
		};*/
#endif
		return mb->skinblt(box+x,boy+y,w,h,o);
	}
	else{
		screen->setwindow(this);
		return screen->skinblt(x+screen->wr.left+box,y+screen->wr.top+boy,w,h,o);
	};
}

void MTWinControl::setpen(int color)
{
	((mb)?mb:screen)->setpen(color);
}

void MTWinControl::setbrush(int color)
{
	((mb)?mb:screen)->setbrush(color);
}

bool MTWinControl::fillcolor(int x,int y,int w,int h,int color,int opacity)
{
	if (mb) return mb->fill(box+x,boy+y,w,h,color,opacity);
	else{
		screen->setwindow(this);
		return screen->fill(x+screen->wr.left+box,y+screen->wr.top+boy,w,h,color,opacity);
	};
}

bool MTWinControl::fillrect(int x,int y,int w,int h,int mode)
{
	if (mb) return mb->fillex(box+x,boy+y,w,h,mode);
	else return screen->fillex(x+screen->wr.left+box,y+screen->wr.top+boy,w,h,mode);
}

void MTWinControl::point(int x,int y,int color)
{
	if (mb) mb->point(box+x,boy+y,color);
	else screen->point(x+screen->wr.left+box,y+screen->wr.top+boy,color);
}

void MTWinControl::moveto(int x,int y)
{
	if (mb) mb->moveto(box+x,boy+y);
	else screen->moveto(x+screen->wr.left+box,y+screen->wr.top+boy);
}

void MTWinControl::lineto(int x,int y)
{
	if (mb) mb->lineto(box+x,boy+y);
	else screen->lineto(x+screen->wr.left+box,y+screen->wr.top+boy);
}

void MTWinControl::polygon(const MTPoint *pt,int np)
{
	MTPoint p[32];
	int x;
	int ox = box;
	int oy = boy;
	
	if (!mb){
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	for (x=0;x<np;x++){
		p[x].x = pt[x].x+ox;
		p[x].y = pt[x].y+oy;
	};
	if (mb) mb->polygon(p,np);
	else screen->polygon(p,np);
}

void MTWinControl::polyline(const MTPoint *pt,int np)
{
	MTPoint p[32];
	int x;
	int ox = box;
	int oy = boy;
	
	if (!mb){
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	for (x=0;x<np;x++){
		p[x].x = pt[x].x+ox;
		p[x].y = pt[x].y+oy;
	};
	if (mb) mb->polyline(p,np);
	else screen->polyline(p,np);
}

void MTWinControl::rectangle(int x,int y,int w,int h)
{
	x += box;
	y += boy;
	if (mb) mb->rectangle(x,y,w,h);
	else screen->rectangle(x+screen->wr.left,y+screen->wr.top,w,h);
}

void MTWinControl::ellipse(int x,int y,int w,int h)
{
	x += box;
	y += boy;
	if (mb) mb->ellipse(x,y,w,h);
	else screen->ellipse(x+screen->wr.left,y+screen->wr.top,w,h);
}

void MTWinControl::settextcolor(int color)
{
	if (mb) mb->settextcolor(color);
	else screen->settextcolor(color);
}

void MTWinControl::setfont(void *font)
{
	if (mb) mb->setfont(font);
	else screen->setfont(font);
}

void MTWinControl::drawtext(const char *text,int length,MTRect &rect,int flags)
{
	MTRect nr = rect;
	int ox = box;
	int oy = boy;
	
	if (!mb){
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	nr.left += ox;
	nr.right += ox;
	nr.top += oy;
	nr.bottom += oy;
	if (mb) mb->drawtext(text,length,nr,flags);
	else screen->drawtext(text,length,nr,flags);
}

bool MTWinControl::gettextsize(const char *text,int length,MTPoint *size,int maxwidth)
{
	if (mb) return mb->gettextsize(text,length,size,maxwidth);
	else return screen->gettextsize(text,length,size,maxwidth);
}

int MTWinControl::gettextextent(const char *text,int length,int maxextent)
{
	if (mb) return mb->gettextextent(text,length,maxextent);
	else return screen->gettextextent(text,length,maxextent);
}

int MTWinControl::gettextheight()
{
	if (mb) return mb->gettextheight();
	else return screen->gettextheight();
}

int MTWinControl::getcharwidth(char c)
{
	if (mb) return mb->getcharwidth(c);
	else return screen->getcharwidth(c);
}

void MTWinControl::toscreen(MTPoint &p)
{
	p.x += box;
	p.y += boy;
	screen->setwindow(this);
	screen->toscreen(p);
}

void MTWinControl::updateregions()
{
	MTRect r = {0,0,width,height};
	
	if (oprgn) deletergn(oprgn);
	if (trrgn) deletergn(trrgn);
	if (flags & MTCF_TRANSPARENT){
		trrgn = recttorgn(r);
		r.right = r.bottom = 0;
		oprgn = recttorgn(r);
	}
	else{
		oprgn = recttorgn(r);
		r.right = r.bottom = 0;
		trrgn = recttorgn(r);
	};
}

int MTCT syncshow(MTSync *s)
{
	MTDesktop *cdsk = 0;
	MTWinControl *t = (MTWinControl*)s->param[0];
	MTWinControl *w = (MTWinControl*)s->param[1];
	bool modal = (s->param[2]!=0);
	int res,modalresult;
	
	if (t->dsk) cdsk = t->dsk;
	else if (t->guiid==MTC_DESKTOP) cdsk = (MTDesktop*)t;
	modalresult = MTDR_NULL;
	w->modalparent = &modalresult;
//	t->modalresult = MTDR_NULL;
	if (modal){
		cdsk->flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
		w->switchflags(MTCF_STAYONTOP|MTCF_MODAL,true);
	};
	if ((w->left+w->width<16) || (w->left>cdsk->width-16) || (w->top+w->height<16) || (w->top>cdsk->height-16)){
		w->setbounds((cdsk->width-w->width)/2,(cdsk->height-w->height)/2,0,0);
	};
	if (w->parent) w->parent->bringtofront(w);
	t->focus(w);
	w->switchflags(MTCF_HIDDEN,false);
	if (modal){
		cdsk->flags &= (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
		cdsk->draw(NORECT);
	};
	while (modalresult==MTDR_NULL){
		res = gi->processmessages(true);
		if (res<0) return -1;
		else if (res==0) break;
	};
	if (modal){
		cdsk->draw(NORECT);
	};
	return modalresult;
}

int MTWinControl::show(MTWinControl *w,int modal)
{
	MTSync sync;
	
	sync.proc = syncshow;
	sync.param[0] = (int)this;
	sync.param[1] = (int)w;
	if (modal==2) sync.param[2] = 1;
	else if (modal==0) sync.param[2] = 0;
	else sync.param[2] = (si->issysthread())?1:0;
	return gi->synchronize(&sync);
}

void MTWinControl::focus(MTControl *ctrl)
{
	MTControl *old;
	
	if ((parent) && (parent->focused!=this)){
		parent->focus(this);
	};
	if ((ctrl->flags & MTCF_ACCEPTINPUT)==0) return;
	if (focused!=ctrl){
		old = focused;
		focused = ctrl;
		if (old) old->switchflags(MTCF_FOCUSED,false);
		ctrl->switchflags(MTCF_FOCUSED,true);
		focused = ctrl;
		if ((ctrl->guiid & 2)==0){
			gi->resetcursor();
		};
		if (ctrl->guiid!=MTC_SCROLLER) showcontrol(ctrl);
	};
}

void MTWinControl::showcontrol(MTControl *ctrl)
{
	if (vs){
		if (ctrl->top<0) vs->setposition(vs->pos+ctrl->top);
		else if (ctrl->top+ctrl->height>vs->page) vs->setposition(vs->pos+ctrl->top+ctrl->height-vs->page);
	};
	if (hs){
		if (ctrl->left<0) hs->setposition(hs->pos+ctrl->left);
		else if (ctrl->left+ctrl->width>hs->page) hs->setposition(hs->pos+ctrl->left+ctrl->width-hs->page);
	};
}

void MTWinControl::showrect(MTRect &rect)
{
	if (vs){
		if (rect.top<vs->pos) vs->setposition(rect.top);
		else if (rect.bottom>vs->pos+vs->page) vs->setposition(rect.bottom-vs->page);
	};
	if (hs){
		if (rect.left<hs->pos) hs->setposition(rect.left);
		else if (rect.right>hs->pos+hs->page) hs->setposition(rect.right-hs->page);
	};
}

void MTWinControl::bringtofront(MTControl *c)
{
	int x,y;
	MTRect r;
	void *urgn,*op;
	signed char cmodal = (design)?127:c->flags>>24;
	
	if (c==controls[0]) return;
	for (x=ncontrols-1;x>0;x--){
		if (controls[x]==c){
			y = x;
			while (y>0){
				MTControl &cc = *controls[y-1];
				if ((signed char)(cc.flags>>24)>cmodal) break;
				controls[y] = &cc;
				y--;
			};
			if (x==y) break;
			controls[y]->getrect(r,0);
			controls[y] = c;
			if (c->flags & MTCF_HIDDEN) return;
			urgn = recttorgn(r);
			c->getrect(r,0);
			op = recttorgn(r);
			addrgn(urgn,op);
			deletergn(op);
			for (y=rgngetnrects(urgn)-1;y>=0;y--){
				rgngetrect(urgn,y,&r);
				draw(r);
			};
			deletergn(urgn);
			break;
		};
	};
}

void MTWinControl::puttoback(MTControl *c)
{
	int x,y;
	MTRect r;
	void *urgn,*op;
	signed char cmodal = c->flags>>24;
	
	if (c==controls[ncontrols-1]) return;
	for (x=0;x<ncontrols;x++){
		if (controls[x]==c){
			y = x;
			while (y<ncontrols-1){
				MTControl &cc = *controls[y+1];
				if (((signed char)(cc.flags>>24)<cmodal) || (cmodal<0)) break;
				controls[y] = &cc;
				y++;
			};
			if (x==y) break;
			controls[y]->getrect(r,0);
			controls[y] = c;
			if (c->flags & MTCF_HIDDEN) return;
			urgn = recttorgn(r);
			c->getrect(r,0);
			op = recttorgn(r);
			addrgn(urgn,op);
			deletergn(op);
			for (y=rgngetnrects(urgn)-1;y>=0;y--){
				rgngetrect(urgn,y,&r);
				draw(r);
			};
			deletergn(urgn);
			break;
		};
	};
}

void MTWinControl::initdrag(MTControl &cctrl)
{
	if (!dsk) return;
	cmoving = true;
	dc = &cctrl;
	cmox = ((cctrl.mox+cctrl.left)/gridx)*gridx;
	cmoy = ((cctrl.moy+cctrl.top)/gridx)*gridx;
}

void MTWinControl::startdrag()
{
	int aw,ah,mw,mh,_box,_boy;
	int x,y,z,n,error,cerror,bn,style;
	void *cl,*op;
	MTBitmap *_mb;
	MTRect r,cr;

	if (!dsk) return;
	aw = ah = mw = mh = y = 0;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & MTCF_SELECTED){
			if (cctrl.width>mw) mw = cctrl.width;
			if (cctrl.height>mh) mh = cctrl.height;
			aw += cctrl.width;
			ah += cctrl.height;
			y++;
		};
	};
	if (!y) return;
	triggered = true;
	dctrl = (DragContext*)si->memalloc(sizeof(DragContext)*y,0);
	aw = (aw+y-1)/y;
	ah = (ah+y-1)/y;
	r.left = r.top = 0;
	r.right = (mw*aw)/y;
	r.bottom = (mh*aw)/y;
	cl = recttorgn(r);
	y = 0;
	r.right = r.bottom = 0;
	dcr.left = dcr.right = screen->width;
	dcr.top = dcr.bottom = screen->height;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & MTCF_SELECTED){
			error = 1000000;
			bn = 0;
			if (cctrl.left+cctrl.width<dcr.left) dcr.left = cctrl.left+cctrl.width;
			if (cctrl.top+cctrl.height<dcr.top) dcr.top = cctrl.top+cctrl.height;
			if (width-cctrl.left<dcr.right) dcr.right = width-cctrl.left;
			if (height-cctrl.top<dcr.bottom) dcr.bottom = height-cctrl.top;
			n = rgngetnrects(cl);
			for (z=0;z<n;z++){
				rgngetrect(cl,z,&cr);
				if ((cr.right-cr.left>=cctrl.width) && (cr.bottom-cr.top>=cctrl.height)){
					cerror = (cr.right-cr.left)*(cr.bottom-cr.top)-(cctrl.width*cctrl.height);
					if (cerror<error){
						bn = z;
						error = cerror;
					};
				};
			};
			rgngetrect(cl,bn,&cr);
			dctrl[y].bx = cr.left;
			dctrl[y].by = cr.top;
			dctrl[y].ox = cctrl.left-cmox;
			dctrl[y].oy = cctrl.top-cmoy;
			cr.right = cr.left+cctrl.width;
			cr.bottom = cr.top+cctrl.height;
			if (cr.right>r.right) r.right = cr.right;
			if (cr.bottom>r.bottom) r.bottom = cr.bottom;
			op = recttorgn(cr);
			subtractrgn(cl,op);
			deletergn(op);
			y++;
		};
	};
	deletergn(cl);
	db = di->newbitmap(MTB_DRAW|MTB_SKIN,r.right,r.bottom);
	tb = di->newbitmap(MTB_DRAW,r.right,r.bottom);
	if (guiid==MTC_WINDOW) style = ((MTWindow*)this)->style;
	else if (window) style = window->style;
	else style = 0;
	skin->drawdragbkg(db,r,style);
	tb->clip(&tb->wr);
	y = 0;
	_box = box;
	_boy = boy;
	_mb = mb;
	mbchange(_mb,db,this);
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if (cctrl.flags & MTCF_SELECTED){
			box = dctrl[y].bx-cctrl.left;
			boy = dctrl[y].by-cctrl.top;
			boffset(true);
			cctrl.flags &= (~MTCF_SELECTED);
			cctrl.draw(NORECT);
			cctrl.flags |= MTCF_SELECTED;
//			bltbmp(db,dctrl[y].bx,dctrl[y].by,cctrl.width,cctrl.height,cctrl.left,cctrl.top,MTBM_COPY);
			_mb->blt(tb,dctrl[y].bx,dctrl[y].by,cctrl.width,cctrl.height,cctrl.left+_box,cctrl.top+_boy);
			y++;
		};
	};
	mbchange(db,_mb,this);
	box = _box;
	boy = _boy;
	boffset(true);
//	db->blt(tb,0,0,db->width,db->height,0,0);
//	db->blt(screen,128,128,db->width,db->height,0,0);
}

void MTWinControl::drag(int mx,int my)
{
	int x,y;
	int cx,cy,nx,ny;
	int alpha = 192;
	MTRect fr;

	if (!dsk) return;
	if (!db){
		if ((abs(mx-cmox)>dragx) || (abs(my-cmoy)>dragy)) startdrag();
		else return;
	};
	mx = (mx/gridx)*gridx;
	my = (my/gridy)*gridy;
	dcr.left += mx-cmox;
	dcr.top += my-cmoy;
	dcr.right -= mx-cmox;
	dcr.bottom -= my-cmoy;
	if ((dcr.left<0) || (dcr.top<0) || (dcr.right<0) || (dcr.bottom<0)) alpha = 64;
	fr.left = fr.right = mx+box;
	fr.top = fr.bottom = my+boy;
	y = 0;
	dsk->mb->clip(&dsk->mb->wr);
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if ((cctrl.flags & MTCF_SELECTED)==0) continue;
		cx = cmox+dctrl[y].ox+box;
		cy = cmoy+dctrl[y].oy+boy;
		nx = mx+dctrl[y].ox+box;
		ny = my+dctrl[y].oy+boy;
		tb->blt(dsk->mb,cx,cy,cctrl.width,cctrl.height,dctrl[y].bx,dctrl[y].by);
		if (cx<fr.left) fr.left = cx;
		if (cy<fr.top) fr.top = cy;
		if (cx+cctrl.width>fr.right) fr.right = cx+cctrl.width;
		if (cy+cctrl.height>fr.bottom) fr.bottom = cy+cctrl.height;
		if (nx<fr.left) fr.left = nx;
		if (ny<fr.top) fr.top = ny;
		if (nx+cctrl.width>fr.right) fr.right = nx+cctrl.width;
		if (ny+cctrl.height>fr.bottom) fr.bottom = ny+cctrl.height;
		y++;
	};
	y = 0;
	for (x=0;x<ncontrols;x++){
		MTControl &cctrl = *controls[x];
		if ((cctrl.flags & MTCF_SELECTED)==0) continue;
		nx = mx+dctrl[y].ox+box;
		ny = my+dctrl[y].oy+boy;
		dsk->bltbmp(tb,dctrl[y].bx,dctrl[y].by,cctrl.width,cctrl.height,nx,ny);
		db->blendblt(dsk->mb,mx+dctrl[y].ox+box,my+dctrl[y].oy+boy,cctrl.width,cctrl.height,dctrl[y].bx,dctrl[y].by,alpha);
//		db->blt(dsk->mb,nx,ny,cctrl.width,cctrl.height,dctrl[y].bx,dctrl[y].by);
		y++;
	};
	dsk->mb->unclip();
	dsk->flush(fr);
/*
	dsk->flush();
	tb->blt(screen,64,64,db->width,db->height,0,0);
*/
	cmox = mx;
	cmoy = my;
}

void MTWinControl::enddrag()
{
	int x,y;
	int cx,cy;
	bool move = true;
	MTRect fr;

	if (cmoving){
		cmoving = false;
		dc = 0;
		if (db){
			if ((dcr.left<0) || (dcr.top<0) || (dcr.right<0) || (dcr.bottom<0)) move = false;
			fr.left = fr.right = cmox+box;
			fr.top = fr.bottom = cmoy+boy;
			y = 0;
			dsk->mb->clip(&dsk->mb->wr);
			for (x=0;x<ncontrols;x++){
				MTControl &cctrl = *controls[x];
				if ((cctrl.flags & MTCF_SELECTED)==0) continue;
				cx = cmox+dctrl[y].ox+box;
				cy = cmoy+dctrl[y].oy+boy;
				tb->blt(dsk->mb,cx,cy,cctrl.width,cctrl.height,dctrl[y].bx,dctrl[y].by);
				if (cx<fr.left) fr.left = cx;
				if (cy<fr.top) fr.top = cy;
				if (cx+cctrl.width>fr.right) fr.right = cx+cctrl.width;
				if (cy+cctrl.height>fr.bottom) fr.bottom = cy+cctrl.height;
				y++;
			};
			dsk->mb->unclip();
			if (move){
				y = 0;
				for (x=0;x<ncontrols;x++){
					MTControl &cctrl = *controls[x];
					if ((cctrl.flags & MTCF_SELECTED)==0) continue;
					cctrl.setbounds(cmox+dctrl[y].ox,cmoy+dctrl[y].oy,0,0);
					y++;
				};
			};
			dsk->flush(fr);
			di->delbitmap(db);
			di->delbitmap(tb);
			si->memfree(dctrl);
			db = tb = 0;
			dctrl = 0;
		};
	};
}
//---------------------------------------------------------------------------
