//---------------------------------------------------------------------------
#ifndef MTMAINMENU_INCLUDED
#define MTMAINMENU_INCLUDED
//---------------------------------------------------------------------------
class MTMainMenu;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTMainMenu : public MTWinControl{
public:
	MTMainMenu(int tg,MTWinControl *p,int l,int t,int w,int h);
	bool MTCT message(MTCMessage &msg);
};
//---------------------------------------------------------------------------
#endif
