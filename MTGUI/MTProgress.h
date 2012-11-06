//---------------------------------------------------------------------------
#ifndef MTPROGRESS_INCLUDED
#define MTPROGRESS_INCLUDED
//---------------------------------------------------------------------------
class MTProgress;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
class MTProgress : public MTControl{
public:
	int pos;
	int maxpos;
	int step;
	
	MTProgress(int tg,MTWinControl *p,int l,int t,int w,int h);
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	virtual void MTCT setposition(int p);
	virtual void MTCT stepit();
};
//---------------------------------------------------------------------------
#endif
