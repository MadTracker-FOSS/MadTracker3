//---------------------------------------------------------------------------
#ifndef MTWINDOW_INCLUDED
#define MTWINDOW_INCLUDED
//---------------------------------------------------------------------------
enum{
	MTWS_FIXED = 0,
	MTWS_FIXEDCAP,
	MTWS_SIZABLE,
	MTWS_SIZABLECAP,
	MTWS_DIALOG,
	MTWS_TABBED,
	MTWS_DOCK,
	MTWS_MAIN,
	MTWS_CLOSE = 256,
	MTWS_MAXIMIZE = 512,
	MTWS_MINIMIZE = 1024,
	MTWS_HELP = 2048,
	MTWS_STAYONTOP = 4096
};
//---------------------------------------------------------------------------
class MTWindow;
class MTWrapper;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
struct HotControl{
	char hotkey;
	MTControl *ctrl;
};

class MTWindow : public MTWinControl{
public:
	int style;
	char *caption;
	int imageindex;
	MTWrapper *wrapper;
	bool modified;
	MTRect br;
	int btnx,btny,btnw,btnh,btno,btnd;
	
	MTWindow(int tg,MTWinControl *p,int l,int t,int w,int h,int s);
	~MTWindow();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	void MTCT getrect(MTRect &r,int client);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT setparent(MTWinControl *parent);
	void MTCT updateregions();
	virtual void MTCT setstyle(int s);
	virtual void MTCT setcaption(const char *c);
	virtual void MTCT addhotkey(MTControl *ctrl,char hotkey);
	virtual void MTCT delhotkey(MTControl *ctrl);
	virtual void MTCT setminsize(int width,int height);
	virtual void MTCT setmaxsize(int width,int height);
private:
	friend class MTTabControl;
	friend class MTGUIInterface;
	int minsize[2],maxsize[2];
	MTRect prev;
	MTResources *res;
	int resid;
	MTArray *hotcontrols;
	void MTCT updateborders();
};
//---------------------------------------------------------------------------
#include "../Headers/MTXWrapper.h"
//---------------------------------------------------------------------------
#endif
