//---------------------------------------------------------------------------
#ifndef MTBUTTON_INCLUDED
#define MTBUTTON_INCLUDED
//---------------------------------------------------------------------------
class MTButton;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
class MTButton : public MTControl{
public:
	char *caption;
	int imageindex;
	int modalresult;
	MTShortcut *shortcut;
	char hotkey;
	int hotkeyoffset;
	
	MTButton(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTButton();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	virtual void MTCT setcaption(const char *c);
	virtual void MTCT setimage(int index);
	virtual void MTCT setautosize(bool autosize);
	int MTCT getautowidth();
private:
	bool autosize;
	int downtime;
};
//---------------------------------------------------------------------------
#endif
