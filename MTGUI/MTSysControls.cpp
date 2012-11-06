//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//    MTImageList
//    MTControl
//      MTScroller
//      MTSlider
//      MTOscillo
//
//---------------------------------------------------------------------------
#include "MTSysControls.h"
#include "MTGUI1.h"
#include "../Headers/MTXSkin.h"
#include "MTWindow.h"
//---------------------------------------------------------------------------
MTImageList sysimages;
//---------------------------------------------------------------------------
// MTImageList
//---------------------------------------------------------------------------
MTImageList::MTImageList()
{
}

void MTImageList::setmetrics(MTSQMetrics *m)
{
	mm = *m;
	if ((mm.nx>0) && (mm.ny>0)){
		iw = mm.a.b.w/mm.nx;
		ih = mm.a.b.h/mm.ny;
	}
	else{
		iw = ih = 0;
	};
}

void MTImageList::drawimage(int id,MTBitmap *dest,int x,int y,int opacity)
{
	if ((id<0) || (id>=mm.nx*mm.ny)) return;
	if (opacity>=255){
		dest->skinblta(x,y,iw,ih,mm.a,mm.nx,mm.ny,id);
	}
	else if (opacity>0){
		int ox,oy;
		ox = mm.a.b.x+iw*(id%mm.nx);
		oy = mm.a.b.y+ih*(id/mm.nx);
		skin->getbitmap(mm.a.bmpid)->blendblt(dest,x,y,iw,ih,ox,oy,opacity);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTSlider
//---------------------------------------------------------------------------
MTSlider::MTSlider(int tag,MTWinControl *p,int l,int t,int type):
MTControl(MTC_SLIDER,tag,p,l,t,16,16),
type(type),
orientation(0),
minpos(0),
maxpos(256),
value(128),
mincr(2),
morigx(0),
morigy(0),
ctimer(0),
ctouch(-1)
{
	flags |= (MTCF_TRANSPARENT|MTCF_ACCEPTINPUT);
	cm = skin->getslider(type,orientation);
	switch (cm->type){
	case SKIN_BUTTON:
	case SKIN_PROGR:
	case SKIN_VUMETER:
		width = cm->a.s.b.w;
		height = cm->a.s.b.h;
		break;
	};
	gi->setcontrolname(this,"slider");
}

int MTSlider::loadfromstream(MTFile *f,int size,int flags)
{
	type = -1;
	int csize = MTControl::loadfromstream(f,size,flags);
	
	f->read(&type,4);
	f->read(&orientation,4);
	cm = skin->getslider(type,orientation);
	switch (cm->type){
	case SKIN_BUTTON:
	case SKIN_PROGR:
	case SKIN_VUMETER:
		setbounds(left,top,cm->a.s.b.w,cm->a.s.b.h);
		break;
	};
	return csize+8;
}

int MTSlider::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	
	f->write(&type,4);
	f->write(&orientation,4);
	return csize+8;
}

#define SliderNP (ControlNP+2)
int MTSlider::getnumproperties(int id)
{
	if (id==-1) return SliderNP;
	if (id<ControlNP) return MTControl::getnumproperties(id);
	if (id==ControlNP) return 4;
	else if (id==ControlNP+1) return 2;
	return 0;
}

bool MTSlider::getpropertytype(int id,char **name,int &flags)
{
	static char *propname[2] = {"Type","Orientation"};
	static int propflags[2] = {MTP_LIST,MTP_LIST};
	static char *subname[4] = {"Slider","Knob","Volume","Vu-meter"};
	static char *subname2[2] = {"Horizontal","Vertical"};
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getpropertytype(id,name,flags);
	if ((id>>8)==ControlNP){
		id &= 0xFF;
		if (id>=4) return false;
		*name = subname[id];
		flags = -1;
		return true;
	}
	else if ((id>>8)==ControlNP+1){
		id &= 0xFF;
		if (id>=2) return false;
		*name = subname2[id];
		flags = -1;
		return true;
	};
	if (id>=SliderNP) return false;
	*name = propname[id-ControlNP];
	flags = propflags[id-ControlNP];
	return true;
}

bool MTSlider::getproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::getproperty(id,value);
	switch (id-ControlNP){
	case 0:
		iv = type;
		break;
	case 1:
		iv = orientation;
		break;
	default:
		return false;
	};
	return true;
}

bool MTSlider::setproperty(int id,void *value)
{
	int &iv = *(int*)value;
	
	if ((id<ControlNP) || ((id & 0xFF00) && ((id>>8)<ControlNP))) return MTControl::setproperty(id,value);
	if (window) window->modified = true;
	switch (id-ControlNP){
	case 0:
		type = iv;
		cm = skin->getslider(type,orientation);
		switch (cm->type){
		case SKIN_BUTTON:
		case SKIN_PROGR:
		case SKIN_VUMETER:
			setbounds(left,top,cm->a.s.b.w,cm->a.s.b.h);
			break;
		};
		break;
	case 1:
		orientation = iv;
		cm = skin->getslider(type,orientation);
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

void MTSlider::setbounds(int l,int t,int w,int h)
{
	if (type>=0){
		switch (cm->type){
		case SKIN_BUTTON:
		case SKIN_PROGR:
		case SKIN_VUMETER:
			w = cm->a.s.b.w;
			h = cm->a.s.b.h;
			break;
		};
	};
	MTControl::setbounds(l,t,w,h);
}

void MTSlider::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	skin->drawcontrol(this,rect,b,x,y);
	MTControl::draw(rect);
}

bool MTSlider::message(MTCMessage &msg)
{
	int b;
	
	if (type!=3){
		MTBSLM &btn = cm->a;
		switch (msg.msg){
		case MTCM_MOUSEDOWN:
			switch (cm->type){
			case SKIN_BUTTON:
			case SKIN_XBUTTON:
				if ((msg.x>=btn.sa.left) && (msg.x<width-btn.sa.right) && (msg.y>=btn.sa.top) && (msg.y<height-btn.sa.bottom)){
					if (cm->orientation==SKIN_HORIZ){
						b = width-btn.sa.right-btn.sa.left-btn.b.b.w;
						b = b-((value-minpos)*b)/(maxpos-minpos);
						if (msg.x<btn.sa.left+b) ctouch = 0;
						else if (msg.x<btn.sa.left+b+btn.b.b.w){
							morigx = msg.x-b;
							ctouch = 1;
						}
						else ctouch = 2;
						if (ctouch!=1) ctimer = gi->ctrltimer(this,0,50,true);
						return true;
					}
					else{
						b = height-btn.sa.bottom-btn.sa.top-btn.b.b.h;
						b = b-((value-minpos)*b)/(maxpos-minpos);
						if (msg.y<btn.sa.top+b) ctouch = 0;
						else if (msg.y<btn.sa.top+b+btn.b.b.h){
							morigy = msg.y-b;
							ctouch = 1;
						}
						else ctouch = 2;
						if (ctouch!=1) ctimer = gi->ctrltimer(this,0,50,true);
						return true;
					};
				};
				break;
			default:
				morigv = value;
				morigx = msg.x;
				morigy = msg.y;
				ctouch = 3;
				break;
			};
			break;
		case MTCM_MOUSEMOVE:
			if (ctouch==1){
				if (cm->orientation==SKIN_HORIZ){
					b = width-btn.sa.right-btn.sa.left-btn.b.b.w;
					setvalue((b-(msg.x-morigx))*(maxpos-minpos)/b+minpos);
				}
				else{
					b = height-btn.sa.bottom-btn.sa.top-btn.b.b.h;
					setvalue((b-(msg.y-morigy))*(maxpos-minpos)/b+minpos);
				};
				return true;
			}
			else if (ctouch==3){
				int cincr = mincr;
				if (msg.buttons & DB_SHIFT){
					cincr >>= 1;
					if (cincr==0) cincr = 1;
				};
				if (cm->orientation==SKIN_HORIZ){
					setvalue(morigv+(msg.x-morigx)*cincr);
				}
				else if (cm->orientation==SKIN_VERT){
					setvalue(morigv+(morigy-msg.y)*cincr);
				}
				else{
					setvalue(morigv+(+msg.x-msg.y+morigy-morigx)*cincr);
				};
			};
			break;
		case MTCM_MOUSEUP:
			if (ctimer){
				gi->deltimer(this,ctimer);
				ctimer = 0;
			};
			ctouch = -1;
			break;
		case MTCM_TIMER:
			if (ctouch==0) setvalue(value+((maxpos-minpos)>>4));
			else setvalue(value-((maxpos-minpos)>>4));
			return true;
		};
	};
	return MTControl::message(msg);
}

void MTSlider::setminmax(int newmin,int newmax)
{
	minpos = newmin;
	maxpos = newmax;
	if (maxpos<minpos) maxpos = minpos;
	mincr = (maxpos-minpos)/128;
	if (mincr<=0) mincr = 1;
	if (value<minpos) setvalue(minpos);
	else if (value>maxpos) setvalue(maxpos);
	else{
		MTCMessage msg = {MTCM_CHANGE,0,this};
		parent->message(msg);
	};
}

void MTSlider::setvalue(int newvalue)
{
	if (newvalue!=value){
		value = newvalue;
		if (value>maxpos) value = maxpos;
		if (value<minpos) value = minpos;
		MTCMessage msg = {MTCM_TOUCHED,0,this};
		parent->message(msg);
	};
}
//---------------------------------------------------------------------------
// MTControl
//   MTOscillo
//---------------------------------------------------------------------------
MTOscillo::MTOscillo(int tag,MTWinControl *p,int l,int t,int w,int h,Track *trk):
MTControl(MTC_OSCILLO,tag,p,l,t,w,h),
mtrk(trk)
{
	if ((w==0) || (h==0)){
		width = 64;
		height = 32;
	};
	gi->setcontrolname(this,"oscillo");
}

void MTOscillo::draw(MTRect &rect)
{
	int x = left;
	int y = top;
	MTBitmap *b;
	int z;
	char *bits,*lines;
	int pitch,mh2,mtop,last,color;
	int *start,*pos,*end;
	float sy;
	
	if (flags & MTCF_CANTDRAW) return;
	preparedraw(&b,x,y);
	b->fill(x,y,width,height,skin->getcolor(SC_EDIT_BACKGROUND));
	skin->drawframe(b,x,y,width,height);
	/* if (mtrk){
	Track &ctrk = *(Track*)mtrk;
	if (ctrk.buffer){
	if (b->openbits(0,height,(void**)&bits,&pitch)){
	lines = bits;
	start = (int*)ctrk.buffer;
	pos = start+ctrk.oldoffset*2;
	end = start+ctrk.length*2;
	mh2 = height-4;
	sy = (float)mh2/(32768*256);
	mtop = (y+2)*pitch;
	last = (y+mh2)*pitch;
	color = b->translatecolor(C_MTCOLOR|C_FTXT);
	x += 2;
	switch (b->bitcount){
	case 8:
	for (z=0;z<width-4;z++,x++){
	y = (height/2)+((*(pos++)+*(pos++))*sy);
	if (y<1) lines[x+mtop] = 0x2D;
	else if (y>=mh2) lines[x+last] = 0x2D;
	else lines[x+mtop+y*pitch] = 0x2D;
	if (pos==end) pos = start;
	};
	break;
	case 15:
	case 16:
	mtop >>= 1;
	last >>= 1;
	pitch >>= 1;
	for (z=0;z<width-4;z++,x++){
	y = (height/2)+((*(pos++)+*(pos++))*sy);
	if (y<1) ((short*)lines)[x+mtop] = (short)color;
	else if (y>=mh2) ((short*)lines)[x+last] = (short)color;
	else ((short*)lines)[x+mtop+y*pitch] = (short)color;
	if (pos==end) pos = start;
	};
	break;
	case 24:
	x *= 3;
	for (z=0;z<width-4;z++,x+=3){
	y = (height/2)+((*(pos++)+*(pos++))*sy);
	if (y<1){
	*(short*)lines[x+mtop] = (short)color;
	lines[x+mtop+1] = HIWORD(color);
	}
	else if (y>=mh2){
	*(short*)lines[x+last] = (short)color;
	lines[x+last+1] = (color>>16);
	}
	else{
	*(short*)lines[x+mtop+y*pitch] = (short)color;
	lines[x+mtop+y*pitch+1] = (color>>16);
	};
	if (pos==end) pos = start;
	};
	break;
	case 32:
	break;
	};
	b->closebits();
	};
	};
};*/
	MTControl::draw(rect);
}

void MTOscillo::settrack(Track *trk)
{
	mtrk = trk;
}
//---------------------------------------------------------------------------
