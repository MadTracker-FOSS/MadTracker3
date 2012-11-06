//---------------------------------------------------------------------------
#ifndef MTDESKTOP_INCLUDED
#define MTDESKTOP_INCLUDED
//---------------------------------------------------------------------------
class MTDesktop;
class MTComboBox;
class MTMenu;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTDesktop : public MTWinControl{
public:
	void *mwnd;
	MTMenu *newctrl;
	
	MTDesktop(int tg,void *pw,int l,int t,int w,int h);
	~MTDesktop();
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT setmenu(MTMenu *m);
	void MTCT clearmenu(MTMenu *m);
	void MTCT setcombo(MTComboBox *c);
	void MTCT drawover(MTWinControl *wnd,MTRect &rect);
private:
	MTComboBox *ccombo;
};
//---------------------------------------------------------------------------
#include "MTEdit.h"
#include "MTItems.h"
//---------------------------------------------------------------------------
#endif
