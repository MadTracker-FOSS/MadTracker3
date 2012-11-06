//---------------------------------------------------------------------------
#ifndef MTCHECKBOX_INCLUDED
#define MTCHECKBOX_INCLUDED
//---------------------------------------------------------------------------
class MTCheckBox;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
class MTCheckBox : public MTControl{
public:
	char *caption;
	bool radio;
	int state;
	
	MTCheckBox(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTCheckBox();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	virtual void MTCT setstate(int c,bool touched = false);
private:
	bool undef;
};
//---------------------------------------------------------------------------
#endif
