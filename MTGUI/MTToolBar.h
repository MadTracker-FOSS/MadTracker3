//---------------------------------------------------------------------------
#ifndef MTTOOLBAR_INCLUDED
#define MTTOOLBAR_INCLUDED
//---------------------------------------------------------------------------
class MTToolBar;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTToolBar : public MTWinControl{
public:
	MTToolBar(int tg,MTWinControl *p,int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	void MTCT addcontrol(MTControl *control);
	void MTCT delcontrol(MTControl *control);
};
//---------------------------------------------------------------------------
#endif
