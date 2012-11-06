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
//      MTCustomControl
//      MTWinControl
//        MTCustomWinControl
//
//---------------------------------------------------------------------------
#include "MTCustControls.h"
#include "MTGUI1.h"
//---------------------------------------------------------------------------
// MTControl
//   MTCustomControl
//---------------------------------------------------------------------------
MTCustomControl::MTCustomControl(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_CUSTOMCTRL,tag,p,l,t,w,h),
behaviours(0)
{
	gi->setcontrolname(this,"custcontrol");
}

MTCustomControl::~MTCustomControl()
{
	if (behaviours) behaviours->ondestroy();
}

int MTCustomControl::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTControl::loadfromstream(f,size,flags);
	
	if (behaviours) csize += behaviours->onload(f,size,flags);
	return csize;
}

int MTCustomControl::savetostream(MTFile *f,int flags)
{
	int csize = MTControl::savetostream(f,flags);
	
	if (behaviours) csize += behaviours->onsave(f,flags);
	return csize;
}

void MTCustomControl::setbounds(int l,int t,int w,int h)
{
	if (behaviours) behaviours->onsetbounds(l,t,w,h);
	MTControl::setbounds(l,t,w,h);
}

bool MTCustomControl::checkbounds(int &l,int &t,int &w,int &h)
{
	if (behaviours) return behaviours->oncheckbounds(l,t,w,h);
	return MTControl::checkbounds(l,t,w,h);
}

void MTCustomControl::switchflags(int f,bool set)
{
	if (behaviours) behaviours->onswitchflags(f,set);
	MTControl::switchflags(f,set);
}

void MTCustomControl::draw(MTRect &rect)
{
	if (flags & MTCF_CANTDRAW) return;
	if (behaviours) behaviours->ondraw(rect);
	MTControl::draw(rect);
}

void MTCustomControl::preparedraw(MTBitmap **b,int &ox,int &oy)
{
	if ((parent->mb) && (!direct)) *b = parent->mb;
	else{
		screen->setwindow(parent);
		*b = screen;
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	ox += parent->box+left;
	oy += parent->boy+top;
}

bool MTCustomControl::message(MTCMessage &msg)
{
	if ((behaviours) && (behaviours->onmessage(msg))) return true;
	return MTControl::message(msg);
}

bool MTCustomControl::processmessage(MTCMessage &msg)
{
	return MTControl::message(msg);
}
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTCustomWinControl
//---------------------------------------------------------------------------
MTCustomWinControl::MTCustomWinControl(int tg,void *pw,int l,int t,int w,int h):
MTWinControl(MTC_CUSTOMWINCTRL,tg,(MTWinControl*)pw,l,t,w,h),
behaviours(0)
{
	gi->setcontrolname(this,"custwincontrol");
}

MTCustomWinControl::~MTCustomWinControl()
{
	if (behaviours) behaviours->ondestroy();
}

int MTCustomWinControl::loadfromstream(MTFile *f,int size,int flags)
{
	int csize = MTWinControl::loadfromstream(f,size,flags);
	
	if (behaviours) csize += behaviours->onload(f,size,flags);
	return csize;
}

int MTCustomWinControl::savetostream(MTFile *f,int flags)
{
	int csize = MTWinControl::savetostream(f,flags);
	
	if (behaviours) csize += behaviours->onsave(f,flags);
	return csize;
}

void MTCustomWinControl::setbounds(int l,int t,int w,int h)
{
	if (behaviours) behaviours->onsetbounds(l,t,w,h);
	MTWinControl::setbounds(l,t,w,h);
}

bool MTCustomWinControl::checkbounds(int &l,int &t,int &w,int &h)
{
	if (behaviours) return behaviours->oncheckbounds(l,t,w,h);
	return MTWinControl::checkbounds(l,t,w,h);
}

void MTCustomWinControl::switchflags(int f,bool set)
{
	if (behaviours) behaviours->onswitchflags(f,set);
	MTWinControl::switchflags(f,set);
}

void MTCustomWinControl::draw(MTRect &rect)
{
	if (flags & MTCF_CANTDRAW) return;
	if (behaviours) behaviours->ondraw(rect);
	MTWinControl::draw(rect);
}

bool MTCustomWinControl::message(MTCMessage &msg)
{
	if ((behaviours) && (behaviours->onmessage(msg))) return true;
	return MTWinControl::message(msg);
}

void MTCustomWinControl::preparedraw(MTBitmap **b,int &ox,int &oy)
{
	if ((parent->mb) && (!direct)) *b = parent->mb;
	else{
		screen->setwindow(parent);
		*b = screen;
		ox += screen->wr.left;
		oy += screen->wr.top;
	};
	ox += parent->box+left;
	oy += parent->boy+top;
}

void* MTCustomWinControl::getoffsetrgn(int type)
{
	if (behaviours){
		void *rgn = behaviours->ongetoffsetrgn(type);
		if (rgn) return rgn;
	};
	return MTWinControl::getoffsetrgn(type);
}

void MTCustomWinControl::offset(int ox,int oy)
{
	if (behaviours) behaviours->onoffset(ox,oy);
	MTWinControl::offset(ox,oy);
}

bool MTCustomWinControl::processmessage(MTCMessage &msg)
{
	return MTWinControl::message(msg);
}
//---------------------------------------------------------------------------
