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
#include "MTPanel.h"
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
extern MTMenu *newctrlmenu;
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTPanel
//---------------------------------------------------------------------------
MTPanel::MTPanel(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_PANEL,tag,p,l,t,w,h),
style(0)
{
	flags |= MTCF_ACCEPTCTRLS|MTCF_TRANSPARENT;
	flags &= (~MTCF_BORDER);
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

bool MTPanel::message(MTCMessage &msg)
{
	MTMenu *popup;
	MTMenuItem *newctrl;

	if ((!candesign) || (msg.msg!=MTCM_ONPOPUP)) return MTWinControl::message(msg);
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
}
//---------------------------------------------------------------------------
