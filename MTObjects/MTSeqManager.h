//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSeqManager.h 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSEQMANAGER_INCLUDED
#define MTSEQMANAGER_INCLUDED
//---------------------------------------------------------------------------
#include "MTModule.h"
#include "MTXControls.h"
//---------------------------------------------------------------------------
class MTSeqManager : public MTCustomWinBehaviours{
public:
	MTModule *module;
	double zoom;
	double cursor;
	double offsetx;
	double nbeats;
	int offsety;
	int nlayers;
	int pattx;
	int patty;
	int patth;
	MTPoint hl;
	int selecting;
	bool followsong;
	
	MTSeqManager(MTCustomWinControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	bool MTCT oncheckbounds(int &l,int &t,int &w,int &h);
	void MTCT ondraw(MTRect &rect);
	bool MTCT onmessage(MTCMessage &msg);
	void* MTCT ongetoffsetrgn(int type);
	void MTCT onoffset(int ox,int oy);
	virtual void MTCT setmodule(MTModule *newmodule);
	virtual void MTCT setcursor(double value);
	virtual void MTCT setoffsetx(double value);
	virtual void MTCT setoffsety(int value);
	virtual void MTCT setzoom(int value);
	virtual void MTCT sethighlight(int x,int layer);
	virtual void MTCT updatepos();
	virtual double MTCT clienttodata(int x,int *layer);
	virtual int MTCT datatoclient(double x,int *layer);
	virtual void MTCT clienttosequence(int &x,int &layer);
	virtual void MTCT selectsequence(int layer,int s,int how);
	virtual void MTCT clearselection(bool refresh);
	virtual void MTCT deletesequence(int layer,int s,bool adapt);
	virtual void MTCT drawseqp(int layer,int s,double sl,double el);
	virtual bool MTCT sequencetorect(int layer,int s,MTRect &r);
private:
	MTScroller *hs,*vs;
	int width,height;
	void MTCT updatemetrics();
	void MTCT updatescroller();
	void MTCT drawlayer(int sy,int sh);
	void MTCT drawseq(double sx,double sw,int sy,int sh);
};
//---------------------------------------------------------------------------
bool MTCT ModuleEdit(MTObject *object,MTWindow *window,int flags,MTUser *user);
//---------------------------------------------------------------------------
#endif
