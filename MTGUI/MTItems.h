//---------------------------------------------------------------------------
#ifndef MTITEMS_INCLUDED
#define MTITEMS_INCLUDED

#define MTVF_IMAGES   1
#define MTVF_CHECKBOX 2
#define MTVF_RADIOBOX 4
//---------------------------------------------------------------------------
class MTUserList;
class MTItem;
class MTMenuItem;
class MTMenu;
//---------------------------------------------------------------------------
#include "MTWinControl.h"
#include "../Headers/MTXSystem.h"
//---------------------------------------------------------------------------
typedef void (MTCT *MTCItemDraw)(MTItem *item,const MTRect &rect,MTBitmap *b);
typedef void (MTCT *MTCUserItemDraw)(MTUserList *list,int id,const MTRect &rect,MTBitmap *b);
typedef int (MTCT *MTCGetItemInfo)(MTUserList *list,int id,char **caption,int *imageindex,int *flags,bool *editable);
typedef void (MTCT *MTCItemMessage)(MTUserList *list,int id,MTCMessage &msg);

class MTList : public MTWinControl{
public:
	MTControl *owner;
	int viewflags;
	int selected;
	int numitems;
	int itemheight;
	
	MTList(int id,int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTList();
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	virtual bool MTCT message(MTCMessage &msg);
	virtual bool MTCT getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable);
	virtual int MTCT searchitem(const char *search,char **caption);
	virtual void MTCT setitem(int id);
};

class MTUserList : public MTList{
public:
	int over;
	MTCUserItemDraw userdrawproc;
	MTCGetItemInfo getiteminfoproc;
	MTCItemMessage itemmessageproc;
	
	MTUserList(int tg,MTWinControl *p,int l,int t,int w,int h);
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	bool MTCT getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable);
	int MTCT searchitem(const char *search,char **caption);
	void MTCT setitem(int id);
	virtual void MTCT setnumitems(int n);
private:
	void MTCT updatescroller();
};

class MTItem : public MTControl{
public:
	int index;
	char *caption;
	int imageindex;
	int itemflags;
	void *data;
	bool editable;
	bool autosize;
	
	MTItem(int tg,MTWinControl *p,int l,int t,int w,int h);
	virtual ~MTItem();
	virtual bool MTCT message(MTCMessage &msg);
	virtual void MTCT setcaption(const char *c);
};

class MTItemView : public MTList{
public:
	MTItem *selecteditem;
	MTCItemDraw userdrawproc;
	
	MTItemView(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTItemView();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	bool MTCT getiteminfo(int id,char **caption,int *imageindex,int *flags,bool *editable);
	int MTCT searchitem(const char *search,char **caption);
	void MTCT setitem(int id);
	void MTCT addcontrol(MTControl *control);
	void MTCT delcontrol(MTControl *control);
	virtual bool MTCT message(MTCMessage &msg);
	virtual MTItem* MTCT additem(const char *caption,int image,int flags,bool editable,void *data);
	virtual void MTCT clearitems();
	virtual void MTCT beginupdate();
	virtual void MTCT endupdate();
	virtual void MTCT sort(int f);
	virtual MTItem* MTCT getitem(int id);
	virtual MTItem* MTCT getitemfromtag(int tag);
	virtual void MTCT removeitem(MTItem *item);
protected:
	bool updating;
	void MTCT quicksort(int f,int lo,int hi);
};

class MTListItem : public MTItem{
public:
	MTListItem(int tg,MTWinControl *p,int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
};

class MTListBox : public MTItemView{
public:
	MTListBox(int tg,MTWinControl *p,int l,int t,int w,int h);
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	MTItem* MTCT additem(const char *caption,int image,int flags,bool editable,void *data);
	void MTCT clearitems();
	void MTCT endupdate();
private:
	void MTCT updatescroller();
};

class MTMenuItem : public MTItem{
public:
	MTCommand command;
	MTMenu *submenu;
	MTShortcut *shortcut;
	char hotkey;
	int hotkeyoffset;
	
	MTMenuItem(int tg,MTWinControl *p,int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	void MTCT setcaption(const char *c);
};

class MTMenu : public MTItemView{
public:
	MTControl *caller;
	union{
		MTPoint mouse;
		MTRect area;
	};

	MTMenu(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTMenu();
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT switchflags(int f,bool set);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	MTItem* MTCT additem(const char *caption,int image,int flags,bool editable,void *data);
	void MTCT removeitem(MTItem *item);
	virtual void MTCT popup(MTControl *newcaller,MTPoint pos);
	virtual void MTCT popup(MTControl *newcaller,MTRect area);
protected:
	MTBitmap *ib;
};

struct MTFileEntry{
	int type;
	int	imageindex;
	char *filename;
};

class MTFileListBox : public MTUserList{
public:
	char *path;
	
	MTFileListBox(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTFileListBox();
	virtual void MTCT setpath(const char *p);
	virtual void MTCT setfilter(int f);
private:
	MTArray	*entries;
	MTProcess *process;
	void MTCT additem(const char *c,int type,int imageindex);
	void MTCT sort(int f);
	static int MTCT flb_natsort(MTFileEntry *a,MTFileEntry *b);
	static int MTCT flb_process(MTThread *thread,void *param);
	static void MTCT flb_progress(MTProcess *process,void *param,float p);
	static int MTCT flb_getiteminfo(MTUserList *list,int id,char **caption,int *imageindex,int *flags,bool *editable);
	static void MTCT flb_itemmessage(MTUserList *list,int id,MTCMessage &msg);
};
//---------------------------------------------------------------------------
#endif
