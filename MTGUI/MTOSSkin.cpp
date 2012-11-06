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
#include "../Headers/MTXSystem.h"
#include "MTGUI1.h"
#include "MTOSSkin.h"
#include "MTSysControls.h"
#include "MTSign.h"
#include "MTItems.h"
#include "MTLabel.h"
#include "MTTabControl.h"
#include "MTButton.h"
#include "MTCheckBox.h"
#include "MTProgress.h"
//---------------------------------------------------------------------------
unsigned char MTOSSkin::fontmap[256];
static const char *passtxt = {"********************************"};
//---------------------------------------------------------------------------
MTOSSkin::MTOSSkin():
Skin()
{
	unsigned char x;

	updatemetrics();
	mtmemzero(fontmap,sizeof(fontmap));
	for (x=0;x<96;x++){
		fontmap[fnm.fontseq[x]] = x;
	};
	mtmemzero(bmpid,sizeof(bmpid));
	mtmemzero(skinbmp,sizeof(skinbmp));
	mtmemzero(mask,sizeof(mask));
	mtmemzero(mr,sizeof(mr));
	mtmemzero(hskfont,sizeof(hskfont));
}

MTOSSkin::~MTOSSkin()
{
	int x;

	delfonts();
	for (x=0;x<16;x++){
		if (skinbmp[x]){
			di->delbitmap(skinbmp[x]);
			skinbmp[x] = 0;
			di->setskinbitmap(x,0);
		};
	};
}

void MTOSSkin::loadfromres(MTResources *res)
{
}

void MTOSSkin::savetores(MTResources *res)
{
}

void MTOSSkin::skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param)
{
	int id = (int)param;

	if (skin->skinbmp[id]==oldskin){
		skin->skinbmp[id] = newskin;
		di->setskinbitmap(id,newskin);
	};
}

void MTOSSkin::initcontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	default:
		{
		};
		break;
	};
}

void MTOSSkin::uninitcontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	default:
		{
		};
		break;
	};
	if (ctrl->skindata){
		si->memfree(ctrl->skindata);
		ctrl->skindata = 0;
	};
}

void MTOSSkin::resetcontrol(MTControl *ctrl,bool skinchange)
{
	switch (ctrl->guiid){
	default:
		{
		};
		break;
	};
}

void MTOSSkin::timercontrol(MTControl *ctrl)
{
	switch (ctrl->guiid){
	default:
		{
		};
		break;
	};
}

void MTOSSkin::drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags)
{
	switch (ctrl->guiid){
	case MTC_SIGN:
//	Sign
		{

		};
		break;
	case MTC_LABEL:
//	Label
		{

		};
		break;
	case MTC_SCROLLER:
//	Scroller
		{

		};
		break;
	case MTC_DESKTOP:
//	Desktop
		{

		};
		break;
	case MTC_USERLIST:
	case MTC_FILELISTBOX:
//	List
		{

		};
		break;
	case MTC_LISTITEM:
//	List Item
		{

		};
		break;
	case MTC_TABCONTROL:
//	Tab Control
		{

		};
		break;
	case MTC_LISTBOX:
//	List Box
		{

		};
		break;
	case MTC_MENUITEM:
//	Menu Item
		{

		};
		break;
	case MTC_MENU:
//	Menu
		{

		};
		break;
	case MTC_WINDOW:
//	Window
		{

		};
		break;
	case MTC_EDIT:
	case MTC_ITEMCOMBOBOX:
	case MTC_USERCOMBOBOX:
// Edit Box / Combo Box
		{

		};
		break;
	case MTC_BUTTON:
//	Button
		{

		};
		break;
	case MTC_CHECKBOX:
//	Check box
		{

		};
		break;
	case MTC_PROGRESS:
//	Progress bar
		{

		};
		break;
	case MTC_SLIDER:
//	Slider
		{

		};
		break;
	};
}

void MTOSSkin::drawcontrol(int guiid,int id,MTRect &rect,MTBitmap *b,int x,int y,int flags)
{
	switch (guiid){
	case MTC_STATUS:
		{

		};
		break;
	case MTC_SEQUENCE:
		{

		};
		break;
	};
}

void MTOSSkin::drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y)
{

}

void MTOSSkin::drawmodalveil(MTWinControl *ctrl,MTRect &rect)
{

}

void MTOSSkin::updatemetrics()
{
	MTSQMetrics &csq = fnm.pattfont;

	fontwidth = csq.a.b.w/csq.nx;
	fontheight = csq.a.b.h/csq.ny;
}

MTBitmap* MTOSSkin::getbitmap(int id)
{
	return skinbmp[id];
}

MTSLMetrics* MTOSSkin::getslider(int type,int orientation)
{
//FIXME
	return 0;
}

void MTOSSkin::getcontrolsize(int guiid,int id,int &width,int &height)
{
	switch (guiid){
	case MTC_SIGN:

		break;
	case MTC_SCROLLER:
		switch (id){
		case 0:	// Horizontal

			break;
		case 1:	// Vertical

			break;
		// Minimum slider size
		case 2:	// Horizontal

			break;
		case 3:	// Vertical

			break;
		// Button size
		case 4:	// Horizontal

			break;
		case 5:	// Vertical

			break;
		};
		break;
	case MTC_WINDOW:
		switch (id){
		case 16:

			break;
		case 17:

			break;
		case 18:

			break;
		default:
			break;
		};
		break;
	case MTC_ITEMCOMBOBOX:
	case MTC_USERCOMBOBOX:

		break;
	case MTC_PROGRESS:

		break;
	case MTC_STATUS:

		break;
	};
}

void MTOSSkin::getwindowborders(int style,MTRect *borders)
{

	}

void MTOSSkin::getwindowoffsets(int style,MTRect *borders)
{

}

void MTOSSkin::getregions(MTControl *ctrl,void **opaque,void **transparent)
{

}

int MTOSSkin::getcolor(int id)
{
	if (id<0xFF000000) return id;
	return fnm.colors[id-0xFF000000];
}

void* MTOSSkin::getfont(int id)
{
	return hskfont[id];
}

bool MTOSSkin::gettextsize(MTControl *ctrl,const char *text,int length,MTPoint *size,int maxwidth)
{
	bool ok = false;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		switch (ctrl->guiid){
		case MTC_EDIT:
		case MTC_ITEMCOMBOBOX:
		case MTC_USERCOMBOBOX:
			{
				if (!text) ok = ctrl->parent->gettextsize(passtxt,length,size,maxwidth);
				else ok = ctrl->parent->gettextsize(text,length,size,maxwidth);
			}
			break;
		default:
			ok = ctrl->parent->gettextsize(text,length,size,maxwidth);
			break;
		};
		ctrl->parent->close(0);
	};
	return ok;
}

int MTOSSkin::gettextextent(MTControl *ctrl,const char *text,int length,int maxextent)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		switch (ctrl->guiid){
		case MTC_EDIT:
		case MTC_ITEMCOMBOBOX:
		case MTC_USERCOMBOBOX:
			{
				if (!text) res = ctrl->parent->gettextextent(passtxt,length,maxextent);
				else res = ctrl->parent->gettextextent(text,length,maxextent);
			}
			break;
		default:
			res = ctrl->parent->gettextextent(text,length,maxextent);
			break;
		};
		ctrl->parent->close(0);
	};
	return res;
}

int MTOSSkin::gettextheight(MTControl *ctrl)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		res = ctrl->parent->gettextheight();
		ctrl->parent->close(0);
	};
	return res;
}

int MTOSSkin::getcharwidth(MTControl *ctrl,char c)
{
	int res = 0;

	if (ctrl->parent->open(0)){
		if (ctrl->guiid==MTC_BUTTON) ctrl->parent->setfont(hskfont[2]);
		else ctrl->parent->setfont(hskfont[1]);
		res = ctrl->parent->getcharwidth(c);
		ctrl->parent->close(0);
	};
	return res;
}

void MTOSSkin::drawitem(MTUserList *list,int i,MTRect &rect,MTBitmap *b,const char *caption,int imageindex,int itemflags,bool editable)
{

}

void MTOSSkin::drawchar(unsigned char c,MTBitmap *bmp,int &x,int y,int color)
{
	MTSQMetrics &csq = fnm.pattfont;

	bmp->skinblta(x,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c],color);
	x += fontwidth;
}

void MTOSSkin::drawtext(unsigned char *text,MTBitmap *bmp,int &x,int y,int color)
{
	unsigned char *p = text;
	MTSQMetrics &csq = fnm.pattfont;

	while (*p){
		bmp->skinblta(x,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[*p],color);
		x += fontwidth;
		p++;
	};
}

void MTOSSkin::drawdec(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color)
{
	int z,x2;
	char c;
	MTSQMetrics &csq = fnm.pattfont;
	
	x += fontwidth*n;
	x2 = x-fontwidth;
	for (z=0;z<n;z++){
		c = val%10;
		val /= 10;
		bmp->skinblta(x2,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c+'0'],color);
		if ((!val) && (!zeroes)) return;
		x2 -= fontwidth;
	};
}

void MTOSSkin::drawhex(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color)
{
	int z,x2;
	char c;
	MTSQMetrics &csq = fnm.pattfont;
	
	x += fontwidth*n;
	x2 = x-fontwidth;
	for (z=0;z<n;z++){
		c = val%16;
		val >>= 4;
		if (c<10) c += '0';
		else c += 'A'-10;
		bmp->skinblta(x2,y,fontwidth,fontheight,csq.a,csq.nx,csq.ny,fontmap[c],color);
		if ((!val) && (!zeroes)) return;
		x2 -= fontwidth;
	};
}

void MTOSSkin::calcbounds(int m)
{

}

void MTOSSkin::setshadows(bool enabled)
{
}

void MTOSSkin::drawframe(MTBitmap *bmp,int x,int y,int w,int h,int flags)
{

}

void MTOSSkin::setfonts()
{
	hskfont[0] = createfont(fnm.caption.name,fnm.caption.size,(fnm.caption.flags & SF_BOLD)!=0);
	hskfont[1] = createfont(fnm.text.name,fnm.text.size,(fnm.text.flags & SF_BOLD)!=0);
	hskfont[2] = createfont(fnm.button.name,fnm.button.size,(fnm.button.flags & SF_BOLD)!=0);
	hskfont[3] = createfont(fnm.edit.name,fnm.edit.size,(fnm.edit.flags & SF_BOLD)!=0);
}

void MTOSSkin::delfonts()
{
	deletefont(hskfont[0]);
	deletefont(hskfont[1]);
	deletefont(hskfont[2]);
	deletefont(hskfont[3]);
}
//---------------------------------------------------------------------------
