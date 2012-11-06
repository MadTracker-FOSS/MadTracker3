//---------------------------------------------------------------------------
#ifndef MTCUSTCONTROLS_INCLUDED
#define MTCUSTCONTROLS_INCLUDED
//---------------------------------------------------------------------------
class MTCustomControl;
class MTCustomWinControl;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
class MTCustomBehaviours{
public:
	MTCustomControl *parent;
	int customid;

	MTCustomBehaviours(MTCustomControl *control){
		parent = control; customid = 0;
	};
	virtual void MTCT ondestroy(){
	};
	virtual int MTCT onload(MTFile*,int,int){
		return 0;
	};
	virtual int MTCT onsave(MTFile*,int){
		return 0;
	};
	virtual void MTCT onsetbounds(int,int,int,int){
	};
	virtual bool MTCT oncheckbounds(int&,int&,int&,int&){
		return true;
	};
	virtual void MTCT onswitchflags(int,bool){
	};
	virtual void MTCT ondraw(MTRect&){
	};
	virtual bool MTCT onmessage(MTCMessage&){
		return false;
	};
};

class MTCustomControl : public MTControl{
public:
	MTCustomBehaviours *behaviours;

	MTCustomControl(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTCustomControl();
	
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	void MTCT setbounds(int l,int t,int w,int h);
	bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT preparedraw(MTBitmap **b,int &ox,int &oy);
	virtual bool MTCT processmessage(MTCMessage &msg);
};

class MTCustomWinBehaviours{
public:
	MTCustomWinControl *parent;
	int customid;

	MTCustomWinBehaviours(MTCustomWinControl *control){
		parent = control; customid = 0;
	};
	virtual void MTCT ondestroy(){
	};
	virtual int MTCT onload(MTFile*,int,int){
		return 0;
	};
	virtual int MTCT onsave(MTFile*,int){
		return 0;
	};
	virtual void MTCT onsetbounds(int,int,int,int){
	};
	virtual bool MTCT oncheckbounds(int&,int&,int&,int&){
		return true;
	};
	virtual void MTCT onswitchflags(int,bool){
	};
	virtual void MTCT ondraw(MTRect&){
	};
	virtual bool MTCT onmessage(MTCMessage&){
		return false;
	};
	virtual void* MTCT ongetoffsetrgn(int){
		return 0;
	};
	virtual void MTCT onoffset(int,int){
	};
};

class MTCustomWinControl : public MTWinControl{
public:
	MTCustomWinBehaviours *behaviours;

	MTCustomWinControl(int tg,void *pw,int l,int t,int w,int h);
	~MTCustomWinControl();

	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	void MTCT setbounds(int l,int t,int w,int h);
	bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT preparedraw(MTBitmap **b,int &ox,int &oy);
	void* MTCT getoffsetrgn(int type);
	void MTCT offset(int ox,int oy);
	virtual bool MTCT processmessage(MTCMessage &msg);
};
//---------------------------------------------------------------------------
#endif
