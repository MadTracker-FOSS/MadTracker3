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
#include "MTVisual.h"
#include "MTGUI1.h"
#include "../Headers/MTXModule.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTVisual
//---------------------------------------------------------------------------
MTVisual::MTVisual(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_VISUAL,tag,p,l,t,w,h),
mtype(0),
module(0),
ntracks(0)
{
	gi->setcontrolname(this,"visual");
	mtmemzero(oscillo,sizeof(oscillo));
}

MTVisual::~MTVisual()
{
}

void MTVisual::setbounds(int l,int t,int w,int h)
{
	int bflags = flags;
	
	flags |= MTCF_DONTDRAW;
	MTWinControl::setbounds(l,t,w,h);
	flags = bflags;
	if (mtype==1){
		ntracks = 0;
		update();
	};
	draw(NORECT);
}

void MTVisual::draw(MTRect &rect)
{
	MTRect cr = {0,0,width,height};
	
	if (&rect){
		if (!cliprect(cr,rect)) goto exit;
	};
exit:
	MTWinControl::draw(rect);
}

void MTVisual::setmodule(MTModule *newmodule)
{
	int x;
	
	if (newmodule==module) return;
	for (x=0;x<ntracks;x++){
		if (oscillo[x]){
			gi->delcontrol(oscillo[x]);
			oscillo[x] = 0;
		};
	};
	ntracks = 0;
	module = newmodule;
	if (mtype==1) update();
}

void MTVisual::settype(int type)
{
	mtype = type;
}

void MTVisual::update()
{
	int x,nt,ndiv,lasttrack,ox,oy,owidth,oheight;
	
	if (!module){
		delcontrols();
		for (x=0;x<ntracks;x++){
			if (oscillo[x]){
				gi->delcontrol(oscillo[x]);
				oscillo[x] = 0;
			};
		};
		ntracks = 0;
		return;
	};
	if (module->ntracks!=ntracks){
		nt = module->ntracks;
		for (ndiv=1;ndiv<=ceil(sqrt(nt));ndiv++){
			oheight = height/ndiv;
			lasttrack = (int)ceil((float)nt/ndiv);
			owidth = width/lasttrack;
			if ((float)owidth/oheight>=1) break;
		};
		ox = 0;
		oy = 0;
		nt = 0;
		delcontrols();
		for (x=0;x<module->ntracks;x++){
			if (!oscillo[x]){
				oscillo[x] = (MTOscillo*)gi->newcontrol(MTC_OSCILLO,x,this,ox,oy,owidth,oheight,module->trk->a[x]);
				oscillo[x]->direct = true;
			}
			else{
				oscillo[x]->settrack(A(module->trk,Track)[x]);
				oscillo[x]->setbounds(ox,oy,owidth,oheight);
			};
			ox += owidth;
			if (++nt==lasttrack){
				nt = 0;
				ox = 0;
				oy += oheight;
			};
		};
		for (x=module->ntracks;x<ntracks;x++){
			if (oscillo[x]){
				gi->delcontrol(oscillo[x]);
				oscillo[x] = 0;
			};
		};
		ntracks = module->ntracks;
	};
}
//---------------------------------------------------------------------------
