//---------------------------------------------------------------------------
#ifndef MTEDIT_INCLUDED
#define MTEDIT_INCLUDED
//---------------------------------------------------------------------------
class MTItem;
class MTList;
class MTEdit;
class MTComboBox;
class MTUserComboBox;
class MTItemComboBox;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
class MTEdit : public MTControl{
public:
	char *text;
	int maxlength;
	bool password;
	
	MTEdit(int tg,MTWinControl *p,int l,int t,int w,int h);
	virtual ~MTEdit();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	virtual int MTCT getnumproperties(int id);
	virtual bool MTCT getpropertytype(int id,char **name,int &flags);
	virtual bool MTCT getproperty(int id,void *value);
	virtual bool MTCT setproperty(int id,void *value);
	virtual void MTCT draw(MTRect &rect);
	virtual bool MTCT message(MTCMessage &msg);
	virtual void MTCT settext(const char *t);
	virtual void MTCT setselstart(int ss);
	virtual void MTCT setselend(int se);
	virtual void MTCT setcursor(int c);
	virtual void MTCT undo();
protected:
	friend class MTSkin;
	int lblank,rblank;
	bool md,focused,isnew;
	int cursor,offset;
	int selstart,selend;
	int mss,mse;
	char *oldtext;
	int oldselstart,oldselend;
	int timer;
	void MTCT modify();
};

class MTComboBox : public MTEdit{
public:
	int dropcount;

	MTComboBox(int id,int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTComboBox();
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	virtual void MTCT pulldown();
	virtual int MTCT getselected();
	virtual bool MTCT getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable);
	virtual void MTCT setitem(int id);
	virtual int MTCT searchitem(const char *search,char **caption);
protected:
	friend class MTSkin;
	friend class MTDesktop;
	friend class MTList;
	MTList *mlb;
	bool modified;
	int popuptick;
};

class MTUserComboBox : public MTComboBox{
public:
	MTUserComboBox(int tg,MTWinControl *p,int l,int t,int w,int h);
	virtual void MTCT setnumitems(int n);
};

class MTItemComboBox : public MTComboBox{
public:
	MTItemComboBox(int tg,MTWinControl *p,int l,int t,int w,int h);
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	virtual MTItem* MTCT additem(const char *caption,int image,int flags,bool editable,void *data);
	virtual void MTCT removeitem(MTItem *item);
	virtual void MTCT clearitems();
	virtual void MTCT beginupdate();
	virtual void MTCT endupdate();
	virtual void MTCT sort(int f);
	virtual MTItem* MTCT getitem(int id);
	virtual MTItem* MTCT getitemfromtag(int tag);
};
//---------------------------------------------------------------------------
#include "MTItems.h"
//---------------------------------------------------------------------------
#endif
