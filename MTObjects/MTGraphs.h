//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGraphs.h 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTGRAPHS_INCLUDED
#define MTGRAPHS_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXControls.h>
//---------------------------------------------------------------------------
class MTCPUGraph : public MTCustomBehaviours{
public:
	MTCPUGraph(MTCustomControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	void MTCT ondraw(MTRect &rect);
private:
	int refreshproc;
	int width,height;
	double scale;
	int lastid;
	double *history[2];
	static void MTCT cpuproc(void *cpu);
	void MTCT setcpu(double output,double objects);
};

class MTChannelsGraph : public MTCustomBehaviours{
public:
	MTChannelsGraph(MTCustomControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	void MTCT ondraw(MTRect &rect);
private:
	int refreshproc;
	int width,height,scale;
	int lastid;
	int *history[2];
	static void MTCT chanproc(void *cpu);
	void MTCT setchannels(int foreground,int background);
};
//---------------------------------------------------------------------------
#endif
