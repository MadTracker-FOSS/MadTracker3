//---------------------------------------------------------------------------
#ifndef MTSYSCONTROLS_INCLUDED
#define MTSYSCONTROLS_INCLUDED
//---------------------------------------------------------------------------
#include "MTControl.h"
#include "../Headers/MTXTrack.h"
//---------------------------------------------------------------------------
class MTImageList{
public:
	int iw,ih;
	
	MTImageList();
	virtual void MTCT setmetrics(MTSQMetrics *m);
	virtual void MTCT drawimage(int id,MTBitmap *dest,int x,int y,int opacity = 255);
private:
	MTSQMetrics mm;
};

class MTSlider : public MTControl{
public:
	int type,orientation;
	int minpos,maxpos,value;
	
	MTSlider(int tag,MTWinControl *p,int l,int t,int type);
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	virtual void MTCT setminmax(int newmin,int newmax);
	virtual void MTCT setvalue(int newvalue);
private:
	int morigv;
	int morigx,morigy;
	int mincr;
	int ctimer,ctouch;
	MTSLMetrics *cm;
};

class MTOscillo : public MTControl{
public:
	MTOscillo(int tag,MTWinControl *p,int l,int t,int w,int h,Track *trk);
	void MTCT draw(MTRect &rect);
	virtual void MTCT settrack(Track *trk);
private:
	Track *mtrk;
};
//---------------------------------------------------------------------------
extern MTImageList sysimages;
//---------------------------------------------------------------------------
#endif
