//---------------------------------------------------------------------------
#ifndef MTLABEL_INCLUDED
#define MTLABEL_INCLUDED
//---------------------------------------------------------------------------
class MTLabel;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
class MTLabel : public MTControl{
public:
	char *caption;
	bool autosize;
	
	MTLabel(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTLabel();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT draw(MTRect &rect);
	virtual void MTCT setcaption(const char *c);
private:
	int alength;
};
//---------------------------------------------------------------------------
#endif
