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
#include "MTToolBar.h"
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTToolBar
//---------------------------------------------------------------------------
MTToolBar::MTToolBar(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_TOOLBAR,tag,p,l,t,w,h)
{
	flags |= MTCF_ACCEPTCTRLS|MTCF_TRANSPARENT;
	flags &= (~MTCF_BORDER);
}

void MTToolBar::draw(MTRect &rect)
{
	MTWinControl::draw(rect);
}

void MTToolBar::addcontrol(MTControl *control)
{
	if (ncontrols==0){
		control->left = control->top = 0;
	}
	else{
		control->left = controls[ncontrols-1]->left+controls[ncontrols-1]->width;
		control->top = 0;
	};
	MTWinControl::addcontrol(control);
}

void MTToolBar::delcontrol(MTControl *control)
{
	int x,l;

	MTWinControl::delcontrol(control);
	l = 0;
	for (x=0;x<ncontrols;x++){
		MTControl &c = *controls[x];
		c.setbounds(l,0,c.width,c.height);
		l += c.width;
	};
}
//---------------------------------------------------------------------------
