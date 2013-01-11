//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGraphs.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTGraphs.h"
#include "MTObjects1.h"
#include "../MTGUI/MTGUITools.h"
#include <MTXAPI/MTXSkin.h>
//---------------------------------------------------------------------------
MTCPUGraph::MTCPUGraph(MTCustomControl *control):
MTCustomBehaviours(control),
lastid(0),
scale(0.125)
{
	width = parent->width;
	height = parent->height;
	history[0] = (double*)si->memalloc(sizeof(double)*(width-4),MTM_ZERO);
	history[1] = (double*)si->memalloc(sizeof(double)*(width-4),MTM_ZERO);
	refreshproc = mtinterface->addrefreshproc(cpuproc,this);
}

void MTCPUGraph::ondestroy()
{
	mtinterface->delrefreshproc(refreshproc);
	si->memfree(history[0]);
	si->memfree(history[1]);
	delete this;
}

void MTCPUGraph::onsetbounds(int l,int t,int w,int h)
{
	if (w>0) width = w;
	if (h>0) height = h;
	history[0] = (double*)si->memrealloc(history[0],sizeof(double)*(width-4));
	history[1] = (double*)si->memrealloc(history[1],sizeof(double)*(width-4));
}

void MTCPUGraph::ondraw(MTRect &rect)
{
	int x,from,to,h;
	int bx = 0;
	int by = 0;
	MTBitmap *b;
	MTRect r = {0,0,width,height};

	parent->preparedraw(&b,bx,by);
	if (&rect){
		r = rect;
		r.left += bx;
		r.top += by;
		r.right += bx;
		r.bottom += by;
		from = rect.left-2;
		to = rect.right-2;
		if (from<0) from = 0;
		if (to>width-4) to = width-4;
		if (from>to) from = to;
		if (to<from) to = from;
	}
	else{
		from = 0;
		to = width-4;
		r.left += bx;
		r.top += by;
		r.right += bx;
		r.bottom += by;
	};
	b->fill(r.left,r.top,r.right-r.left,r.bottom-r.top,skin->getcolor(SC_EDIT_BACKGROUND));
	if (output){
		MTRect cr = {bx+2,by+height-2-(height-4)*output->maxcpu/scale,bx+width-2,by+height-2-(height-4)*output->mincpu/scale};
		if (cliprect(cr,r)){
			b->fill(cr.left,cr.top,cr.right-cr.left,cr.bottom-cr.top,skin->getcolor(SC_EDIT_SELECTION));
		};
	};
	if ((from>=0) && (to<=width-4)){
		if (b->open(0)){
			char st[16];
			MTRect cr = {bx+2,by+2,bx+width-2,by+height-2};
			b->setfont(skin->getfont(1));
			b->settextcolor(skin->getcolor(SC_EDIT_SELECTED));
			sprintf(st,"%.2f%%",scale*100.0);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%.2f%%",scale*75.0);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%.2f%%",scale*50.0);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%.2f%%",scale*25.0);
			b->drawtext(st,-1,cr,0);
			b->setpen(skin->getcolor(SC_EDIT_SELECTED));
			b->moveto(r.left,2+by+((height-4)*3)/4);
			b->lineto(r.right,2+by+((height-4)*3)/4);
			b->moveto(r.left,2+by+(height-4)/2);
			b->lineto(r.right,2+by+(height-4)/2);
			b->moveto(r.left,2+by+(height-4)/4);
			b->lineto(r.right,2+by+(height-4)/4);
			h = width-5-from;
			b->setpen(skin->getcolor(SC_EDIT_NORMAL));
			b->moveto(bx+from+2,by+height-2-(history[1][h--]*(height-4))/scale);
			for (x = from + 1; (x < to && h >= 0); x++, h--)
			{
				b->lineto(bx+x+2,by+height-2-(history[1][h]*(height-4))/scale);
			}
			h = width-5-from;
			b->setpen(skin->getcolor(SC_EDIT_FOCUSED));
			b->moveto(bx+from+2,by+height-2-(history[0][h--]*(height-4))/scale);
			for ( x = from + 1; (x < to && h >= 0); x++, h--)
			{
				b->lineto(bx+x+2,by+height-2-(history[0][h]*(height-4))/scale);
			}
			b->close(0);
		};
	};
	skin->drawframe(b,bx,by,width,height);
}

void MTCPUGraph::cpuproc(void *cpu)
{
	double output,objects;
	MTCPUGraph *cpumonitor = (MTCPUGraph*)cpu;
	MTModule *module = (MTModule*)mtinterface->getmodule(0);

	if (!module) return;
	if (module->cpu->flushid==cpumonitor->lastid) return;
	cpumonitor->lastid = module->cpu->flushid;
	output = module->cpu->getcpu(0);
	if (output>1.0) output = 1.0;
	objects = output-module->cpu->getcpu(3);
	if (objects>1.0) objects = 1.0;
	((MTCPUGraph*)cpu)->setcpu(output,objects);
}

void MTCPUGraph::setcpu(double output,double objects)
{
	int x;
	bool ok = true;

	for (x=width-5;x>0;x--) history[0][x] = history[0][x-1];
	for (x=width-5;x>0;x--) history[1][x] = history[1][x-1];
	if (output>scale){
		scale *= 2.0;
		ok = false;
	};
	if (objects>scale){
		scale *= 2.0;
		ok = false;
	};
	if (ok){
		if ((output<scale/8.0) && (objects<scale/8.0)){
			scale *= 0.5;
			if (scale<0.03125) scale = 0.03125;
		};
	};
	history[0][0] = history[0][2]*0.1+history[0][1]*0.3+output*0.6;
	history[1][0] = history[1][2]*0.1+history[1][1]*0.3+objects*0.6;
	MTCMessage msg = {MTCM_CHANGE,0,parent};
	parent->parent->message(msg);
}
//---------------------------------------------------------------------------
MTChannelsGraph::MTChannelsGraph(MTCustomControl *control):
MTCustomBehaviours(control),
lastid(0),
scale(32)
{
	width = parent->width;
	height = parent->height;
	history[0] = (int*)si->memalloc(sizeof(int)*(width-4),MTM_ZERO);
	history[1] = (int*)si->memalloc(sizeof(int)*(width-4),MTM_ZERO);
	refreshproc = mtinterface->addrefreshproc(chanproc,this);
}

void MTChannelsGraph::ondestroy()
{
	mtinterface->delrefreshproc(refreshproc);
	si->memfree(history[0]);
	si->memfree(history[1]);
	delete this;
}

void MTChannelsGraph::onsetbounds(int l,int t,int w,int h)
{
	if (w>0) width = w;
	if (h>0) height = h;
	history[0] = (int*)si->memrealloc(history[0],sizeof(int)*(width-4));
	history[1] = (int*)si->memrealloc(history[1],sizeof(int)*(width-4));
}

void MTChannelsGraph::ondraw(MTRect &rect)
{
	int x,from,to,h;
	int bx = 0;
	int by = 0;
	MTBitmap *b;
	MTRect r = {0,0,width,height};

	parent->preparedraw(&b,bx,by);
	if (&rect){
		r = rect;
		r.left += bx;
		r.top += by;
		r.right += bx;
		r.bottom += by;
		from = rect.left-2;
		to = rect.right-2;
		if (from<0) from = 0;
		if (to>width-4) to = width-4;
		if (from>to) from = to;
		if (to<from) to = from;
	}
	else{
		from = 0;
		to = width-4;
		r.left += bx;
		r.top += by;
		r.right += bx;
		r.bottom += by;
	};
	b->fill(r.left,r.top,r.right-r.left,r.bottom-r.top,skin->getcolor(SC_EDIT_BACKGROUND));
	if ((from>=0) && (to<=width-4)){
		if (b->open(0)){
			char st[8];
			MTRect cr = {bx+2,by+2,bx+width-2,by+height-2};
			b->setfont(skin->getfont(1));
			b->settextcolor(skin->getcolor(SC_EDIT_SELECTED));
			sprintf(st,"%d",scale);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%d",(scale*3)/4);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%d",scale/2);
			b->drawtext(st,-1,cr,0);
			cr.top += (height-4)/4;
			sprintf(st,"%d",scale/4);
			b->drawtext(st,-1,cr,0);
			b->setpen(skin->getcolor(SC_EDIT_SELECTED));
			b->moveto(r.left,2+by+((height-4)*3)/4);
			b->lineto(r.right,2+by+((height-4)*3)/4);
			b->moveto(r.left,2+by+(height-4)/2);
			b->lineto(r.right,2+by+(height-4)/2);
			b->moveto(r.left,2+by+(height-4)/4);
			b->lineto(r.right,2+by+(height-4)/4);
			h = width-5-from;
			b->setpen(skin->getcolor(SC_EDIT_NORMAL));
			b->moveto(bx+from+2,by+height-2-(history[1][h--]*(height-4))/scale);
			for (x = from + 1; (x < to && h >= 0); x++, h--)
			{
				b->lineto(bx+x+2,by+height-2-(history[1][h]*(height-4))/scale);
			}
			h = width-5-from;
			b->setpen(skin->getcolor(SC_EDIT_FOCUSED));
			b->moveto(bx+from+2,by+height-2-(history[0][h--]*(height-4))/scale);
			for (x = from + 1; (x < to && h >= 0); x++, h--)
			{
				b->lineto(bx+x+2,by+height-2-(history[0][h]*(height-4))/scale);
			}
			b->close(0);
		};
	};
	skin->drawframe(b,bx,by,width,height);
}

void MTChannelsGraph::chanproc(void *channels)
{
	int x,foreground,background;
	MTChannelsGraph *chanmonitor = (MTChannelsGraph*)channels;
	MTModule *module = (MTModule*)mtinterface->getmodule(0);

	if (!module) return;
//	if (module->cpu->flushid==chanmonitor->lastid) return;
	chanmonitor->lastid = module->cpu->flushid;
	PlayStatus &cs = module->playstatus;
	if (objectlock->lock(50)){
		foreground = cs.nchannels;
		background = 0;
		for (x=0;x<foreground;x++){
			if (cs.chan[x]->flags & IIF_BACKGROUND) background++;
		};
		objectlock->unlock();
		chanmonitor->setchannels(foreground,background);
	};
}

void MTChannelsGraph::setchannels(int foreground,int background)
{
	int x;
	bool ok = true;

	for (x=width-5;x>0;x--) history[0][x] = history[0][x-1];
	for (x=width-5;x>0;x--) history[1][x] = history[1][x-1];
	if (foreground>scale){
		scale *= 2;
		ok = false;
	};
	if (background>scale){
		scale *= 2;
		ok = false;
	};
	if (ok){
		if ((foreground<scale/8) && (background<scale/8)){
			scale *= 0.5;
			if (scale<32) scale = 32;
		};
	};
	history[0][0] = foreground;
	history[1][0] = background;
	MTCMessage msg = {MTCM_CHANGE,0,parent};
	parent->parent->message(msg);
}
//---------------------------------------------------------------------------
