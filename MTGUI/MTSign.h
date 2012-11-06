//---------------------------------------------------------------------------
#ifndef MTSIGN_INCLUDED
#define MTSIGN_INCLUDED
//---------------------------------------------------------------------------
class MTSign;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTSign : public MTControl{
public:
	int sign;
	
	MTSign(int tg,MTWinControl *p,int l,int t,int w,int h);
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT draw(MTRect &rect);
	virtual void MTCT setsign(int s);
};
//---------------------------------------------------------------------------
#endif
