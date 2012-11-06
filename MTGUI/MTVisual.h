//---------------------------------------------------------------------------
#ifndef MTVISUAL_INCLUDED
#define MTVISUAL_INCLUDED
//---------------------------------------------------------------------------
#include "MTWinControl.h"
#include "MTSysControls.h"
//---------------------------------------------------------------------------
class MTVisual : public MTWinControl{
public:
	MTVisual(int tag,MTWinControl *p,int l,int t,int w,int h);
	~MTVisual();
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	virtual void MTCT setmodule(MTModule *newmodule);
	virtual void MTCT settype(int type);
	virtual void MTCT update();
private:
	int mtype;
	MTModule *module;
	int ntracks;
	MTOscillo *oscillo[64];
};
//---------------------------------------------------------------------------
#endif
