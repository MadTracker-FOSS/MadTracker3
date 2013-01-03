//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattManager.h 79 2005-08-28 21:01:45Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTPATTMANAGER_INCLUDED
#define MTPATTMANAGER_INCLUDED
//---------------------------------------------------------------------------
class MTPattern;
class MTPattManager;
//---------------------------------------------------------------------------
#include "MTPattern.h"
#include <MTXAPI/MTXControls.h>
//---------------------------------------------------------------------------
class MTPattManager : public MTCustomWinBehaviours{
public:
	MTPattern *patt;
	bool selecting;
	int otrack;
	int oline;
	int ntracks;
	int nlines;
	int pattx,patty;
	int cellwidth[MAX_PATT_TRACKS];
	int cellheight;
	MTPoint hl;
	MTPoint selstart;
	MTPoint selend;
	MTPoint cursor;
	int ccol,cpos;
	
	MTPattManager(MTCustomWinControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	bool MTCT oncheckbounds(int &l,int &t,int &w,int &h);
	void MTCT ondraw(MTRect &rect);
	bool MTCT onmessage(MTCMessage &msg);
	void* MTCT ongetoffsetrgn(int type);
	void MTCT onoffset(int ox,int oy);
	virtual void MTCT setpattern(MTPattern *newpatt);
	virtual void MTCT setcursor(int l,int t,int p);
	virtual void MTCT setotrack(int value);
	virtual void MTCT setoline(int value);
	virtual void MTCT sethighlight(MTPoint value);
	virtual void MTCT getsel(MTRect &sel);
	virtual void MTCT setsel(MTRect value);
	virtual void MTCT clearsel();
	virtual void MTCT setselstart(MTPoint value);
	virtual void MTCT setselend(MTPoint value);
	virtual int MTCT clienttodata(MTPoint &pos);
	virtual void MTCT datatoclient(MTPoint &pos);
	virtual void MTCT stepit();
private:
	MTScroller *hs,*vs;
	int width,height;
	int tw,cw;
	int colwidth[MAX_PATT_TRACKS][MAX_PATT_COLS];
	int colncpos[MAX_PATT_TRACKS][MAX_PATT_COLS];
	int colsize[MAX_PATT_TRACKS][MAX_PATT_COLS];
	MTPoint m;
	bool drag;
	int colors[2][32];
	int clpb;
	void MTCT updatemetrics(bool passive = false);
	void MTCT updatescroller(bool passive = false);
	void MTCT drawpos(int l,int h);
	void MTCT drawtrack(int t,int w);
	void MTCT drawcells(int t,int l,int w,int h);
	void MTCT getposrect(int l,int h,MTRect &r);
	void MTCT gettrackrect(int t,int w,MTRect &r);
	void MTCT getcellsrect(int t,int l,int w,int h,MTRect &r);
	void MTCT checkcolors();
};
//---------------------------------------------------------------------------
#endif
