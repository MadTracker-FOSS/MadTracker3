//---------------------------------------------------------------------------
#ifndef MTPANEL_INCLUDED
#define MTPANEL_INCLUDED
//---------------------------------------------------------------------------
class MTPanel;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTPanel : public MTWinControl{
public:
	int style;
	
	MTPanel(int tg,MTWinControl *p,int l,int t,int w,int h);
	bool MTCT message(MTCMessage &msg);
};
//---------------------------------------------------------------------------
#endif
