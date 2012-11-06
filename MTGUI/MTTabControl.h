//---------------------------------------------------------------------------
#ifndef MTTABCONTROL_INCLUDED
#define MTTABCONTROL_INCLUDED
//---------------------------------------------------------------------------
class MTTabControl;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
#include "MTWindow.h"
//---------------------------------------------------------------------------
class MTTabControl : public MTWinControl{
public:
	int style,cstyle;
	MTWindow *page;
	MTRect br;
	int btnx,btny,btnw,btnh,btno,btnd;
	
	MTTabControl(int tg,MTWinControl *p,int l,int t,int w,int h);
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT getrect(MTRect &r,int client);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT addcontrol(MTControl *control);
	void MTCT delcontrol(MTControl *control);
	void MTCT bringtofront(MTWinControl *w);
	void MTCT puttoback(MTWinControl *w);
	virtual void MTCT setpage(MTWindow *p);
	virtual void MTCT setpageid(int id);
	virtual int MTCT getpageid(MTWindow *p);
	virtual MTWindow* MTCT loadpage(MTResources *res,int id,bool autosave);
	virtual void MTCT setautohidetabs(bool autohide);
	void MTCT updatecaption(MTWindow *p);
private:
	bool autohidetabs;
	int npages;
	void MTCT updateborders();
	void MTCT updatebuttons();
};
//---------------------------------------------------------------------------
#endif
