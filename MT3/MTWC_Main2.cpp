//---------------------------------------------------------------------------
//
//  MadTracker 3 Window Class Wrapper
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTWC_Main2.cpp 68 2005-08-26 22:19:12Z Yannick $
//
//---------------------------------------------------------------------------
//	Needed includes and variables
//---------------------------------------------------------------------------
#include "MTWC_Main2.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
#include "MT3RES.h"
//---------------------------------------------------------------------------
extern MTGUIInterface *gi;
MTWC_main2 *w_main2;
//---------------------------------------------------------------------------
//	User includes, variables and functions
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTInterface.h"
#include "MTExtensions.h"
//---------------------------------------------------------------------------
extern MT3Interface *mi;
char *freeslot;
//---------------------------------------------------------------------------
void MTCT useritemdraw(MTUserList *list,int id,const MTRect &rect,MTBitmap *b)
{
	char *caption;
	int l,imageindex,color,w,h,mid;
	MTObject *object;
	MTRect r = rect;
	MTPoint p;
	char idt[4];

	if (!cmodule) return;
	if (list==w_main2->list1){
		mid = id+1;
		object = A(cmodule->instr,Instrument)[mid];
		imageindex = 54;
	}
	else if (list==w_main2->list2){
		mid = id;
		object = A(cmodule->spl,Oscillator)[mid];
		imageindex = 55;
	}
	else return;
	if (!object){
		imageindex = -1;
		caption = freeslot;
	}
	else{
		caption = object->name;
	};
	if (id==list->selected) b->fill(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,skin->getcolor(SC_EDIT_SELECTION));
	skin->getcontrolsize(MTC_STATUS,0,h,w);
	skin->drawcontrol(MTC_STATUS,0,r,b,r.left,r.top,1);
	skin->drawcontrol(MTC_STATUS,1,r,b,r.left+w,r.top,0);
	r.left += w*2+4;
	if (id==list->over) color = skin->getcolor(SC_TEXT_FOCUSED);
	else if (id==list->selected) color = skin->getcolor(SC_EDIT_SELECTED);
	else color = skin->getcolor(SC_TEXT_NORMAL);
	if (b->open(0)){
		b->settextcolor(color);
		b->setfont(skin->getfont(1));
		sprintf(idt,"%.3d",mid);
		b->drawtext(idt,3,r,0);
		r.left += b->getcharwidth('0')*3;
		r.left += 24;
		l = b->gettextextent(caption,-1,r.right-r.left);
		if (l<strlen(caption)){
			if (b->gettextsize("...",-1,&p)){
				l = b->gettextextent(caption,-1,r.right-r.left-p.x);
			}
			else{
				l -= 3;
			};
		}
		else l = -1;
		b->drawtext(caption,l,r,DTXT_VCENTER);
		if (l>0){
			if (b->gettextsize(caption,l,&p)){
				r.left += p.x;
				b->drawtext("...",-1,r,DTXT_VCENTER);
				r.left -= p.x;
			};
		};
		b->close(0);
	};
	r.left -= 20;
	sysimages->drawimage(imageindex,b,r.left,r.top);
}

int MTCT getiteminfo(MTUserList *list,int id,char **caption,int *imageindex,int *flags,bool *editable)
{
	if (!cmodule) return -1;
	if (list==w_main2->list1){
		Instrument &cinstr = *A(cmodule->instr,Instrument)[id+1];
		if (&cinstr){
			if (caption) *caption = cinstr.name;
			if (imageindex) *imageindex = 54;
		}
		else{
			if (caption) *caption = freeslot;
		};
		return id+1;
	}
	else if (list==w_main2->list2){
		Oscillator &cosc = *A(cmodule->spl,Oscillator)[id];
		if (&cosc){
			if (caption) *caption = cosc.name;
			if (imageindex) *imageindex = 55;
		}
		else{
			if (caption) *caption = freeslot;
		};
		return id;
	};
	return -1;
}

void MTCT listmessage(MTUserList *list,int id,MTCMessage &msg)
{
	switch (msg.msg){
	case MTCM_MOUSEDOWN:
		if ((msg.buttons & DB_DOUBLE)==0) break;
		switch (list->uid){
		case MTC_list1:
			mi->editobject(cmodule->instr->a[id+1],false);
			break;
		case MTC_list2:
			mi->editobject(cmodule->spl->a[id],false);
			break;
		};
	};
}
//---------------------------------------------------------------------------
//	Wrapper class code
//---------------------------------------------------------------------------
MTWC_main2::MTWC_main2(MTWindow *w):
MTWrapper(w)
{
	list1 = (MTUserList*)wthis->getcontrolfromuid(MTC_list1);
	list2 = (MTUserList*)wthis->getcontrolfromuid(MTC_list2);
	panel1 = (MTPanel*)wthis->getcontrolfromuid(MTC_panel1);
	panel2 = (MTPanel*)wthis->getcontrolfromuid(MTC_panel2);
	bnew = (MTButton*)wthis->getcontrolfromuid(MTC_bnew);
	bfile = (MTButton*)wthis->getcontrolfromuid(MTC_bfile);
	binfo = (MTButton*)wthis->getcontrolfromuid(MTC_binfo);
	bview = (MTButton*)wthis->getcontrolfromuid(MTC_bview);
	btools = (MTButton*)wthis->getcontrolfromuid(MTC_btools);
	bhelp = (MTButton*)wthis->getcontrolfromuid(MTC_bhelp);
	bnetwork = (MTButton*)wthis->getcontrolfromuid(MTC_bnetwork);
	bconfig = (MTButton*)wthis->getcontrolfromuid(MTC_bconfig);
	bbegin = (MTButton*)wthis->getcontrolfromuid(MTC_bbegin);
	brewind = (MTButton*)wthis->getcontrolfromuid(MTC_brewind);
	bfastforward = (MTButton*)wthis->getcontrolfromuid(MTC_bfastforward);
	bend = (MTButton*)wthis->getcontrolfromuid(MTC_bend);
	bstop = (MTButton*)wthis->getcontrolfromuid(MTC_bstop);
	bplaypause = (MTButton*)wthis->getcontrolfromuid(MTC_bplaypause);
	brecord = (MTButton*)wthis->getcontrolfromuid(MTC_brecord);
	w->wrapper = this;
//---------------------------------------------------------------------------
//	User construction code
//---------------------------------------------------------------------------
	freeslot = (char*)mtres->getresource(MTR_TEXT,MTT_freeslot,0);
	list1->userdrawproc = list2->userdrawproc = useritemdraw;
	list1->getiteminfoproc = list2->getiteminfoproc = getiteminfo;
	list1->itemmessageproc = list2->itemmessageproc = listmessage;
	list1->setnumitems(MAX_INSTRS-1);
	list2->setnumitems(MAX_SAMPLES);
	MTMenu *menu = (MTMenu*)gi->newcontrol(MTC_MENU,0,mtdsk,0,0,0,0,0);
	menu->flags |= MTCF_DONTSAVE;
	menu->additem("|New...",-1,0,false,0);
	menu->additem("Module",53,0,false,0);
	menu->additem("Instrument",54,0,false,0);
	menu->additem("Oscillator",55,0,false,0);
	menu->additem("Pattern",56,0,false,0);
	menu->additem("Controller",57,0,false,0);
	bnew->popup = menu;
	menu = (MTMenu*)gi->newcontrol(MTC_MENU,0,mtdsk,0,0,0,0,0);
	menu->flags |= MTCF_DONTSAVE;
	menu->additem("|Show...",-1,0,false,0);
	menu->additem("Sequencer",4,0,false,0);
	menu->additem("Mixer",5,0,false,0);
	menu->additem("Browser",6,0,false,0);
	bview->popup = menu;
	menu = (MTMenu*)gi->newcontrol(MTC_MENU,0,mtdsk,0,0,0,0,0);
	menu->flags |= MTCF_DONTSAVE;
	menu->additem("Help",8,0,false,0);
	menu->additem("About",8,0,false,0);
	menu->additem("Register",8,0,false,0);
	menu->additem("Links",8,0,false,0);
	menu->additem("News",8,0,false,0);
	bhelp->popup = menu;
}

MTWC_main2::~MTWC_main2()
{
//---------------------------------------------------------------------------
//	User destruction code
//---------------------------------------------------------------------------
	mtres->releaseresource(freeslot);
}

bool MTWC_main2::onmessage(MTCMessage &msg)
{
//---------------------------------------------------------------------------
//	User messages handler
//---------------------------------------------------------------------------
	if (msg.msg==MTCM_ACTION){
		switch (msg.ctrl->uid){
		case MTC_bbegin:
			if (cmodule){
				cmodule->setpos(0.0);
			};
			return true;
		case MTC_brewind:
			if (cmodule){
				cmodule->setpos(cmodule->playstatus.pos-4.0);
			};
			return true;
		case MTC_bfastforward:
			if (cmodule){
				cmodule->setpos(cmodule->playstatus.pos+4.0);
			};
			return true;
		case MTC_bend:
			if (cmodule){
				cmodule->setpos(cmodule->loope);
			};
			return true;
		case MTC_bplaypause:
			if (cmodule){
				if (cmodule->playstatus.flags==PLAY_SONG){
					cmodule->play(PLAY_STOP);
					bplaypause->setimage(21);
				}
				else{
					cmodule->play(PLAY_SONG);
					bplaypause->setimage(22);
				};
			};
			return true;
		case MTC_bstop:
			if (cmodule){
				cmodule->play(PLAY_STOP);
				bplaypause->setimage(21);
			};
			return true;
		case MTC_brecord:
			return true;
		};
	};
	return false;
}
//---------------------------------------------------------------------------
