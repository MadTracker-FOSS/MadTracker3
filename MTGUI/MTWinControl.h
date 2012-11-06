//---------------------------------------------------------------------------
#ifndef MTWINCONTROLS_INCLUDED
#define MTWINCONTROLS_INCLUDED
//---------------------------------------------------------------------------
#define MTST_HBAR 0
#define MTST_VBAR 1
//---------------------------------------------------------------------------
class MTScroller;
class MTWinControl;
class MTDesktop;
//---------------------------------------------------------------------------
#include "MTControl.h"
//---------------------------------------------------------------------------
typedef bool (MTCT *MTCMessageProc)(MTWinControl *window,MTCMessage &msg);

class MTScroller : public MTControl{
public:
	int type;
	int pos;
	int maxpos;
	int incr,page;
	float os;
	bool slide;
	
	MTScroller(int tag,MTWinControl *p,int l,int t,int w,int h);
	~MTScroller();
	int MTCT loadfromstream(MTFile *f,int size,int flags);
	int MTCT savetostream(MTFile *f,int flags);
	int MTCT getnumproperties(int id);
	bool MTCT getpropertytype(int id,char **name,int &flags);
	bool MTCT getproperty(int id,void *value);
	bool MTCT setproperty(int id,void *value);
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage &msg);
	virtual void MTCT setposition(int p);
private:
	// FIXME use functions to get and set slider dimensions/scaling
	friend class MTSkin;
	int bs;
	int minsize;
	float f;
	int carrow;
	int slidepoint;
	int slideorigin;
	int ctimer;
	int speed;
	void MTCT setaction(int action);
	void MTCT updatemetrics();
};

class MTWinControl : public MTControl{
public:
	MTDesktop *dsk;
	int box,boy;
	MTBitmap *mb;
	int ncontrols;
	MTControl **controls;
	MTControl *focused;
	MTScroller *hs,*vs;
	MTCMessageProc messageproc;
	void *oprgn,*trrgn;
	int *modalparent;
	int modalresult;

	MTWinControl(int id,int tag,MTWinControl *p,int l,int t,int w,int h);
	virtual ~MTWinControl();
	void MTCT preparedraw(MTBitmap **b,int &ox,int &oy);
	virtual int MTCT loadfromstream(MTFile *f,int size,int flags);
	virtual int MTCT savetostream(MTFile *f,int flags);
	virtual void MTCT setbounds(int l,int t,int w,int h);
	virtual bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	virtual void MTCT getrect(MTRect &r,int client);
	virtual void* MTCT getemptyrgn();
	virtual void* MTCT getvisiblergn(bool client,MTControl *control = 0);
	virtual void* MTCT getfixedrgn();
	virtual void MTCT switchflags(int f,bool set);
	virtual void MTCT draw(MTRect &rect);
	virtual bool MTCT message(MTCMessage &msg);
	virtual void MTCT addcontrol(MTControl *control);
	virtual void MTCT delcontrol(MTControl *control);
	virtual void MTCT delcontrols(bool del = false);
	virtual int MTCT getnumcontrols();
	virtual MTControl* MTCT getcontrol(int id);
	virtual int MTCT getcontrolid(MTControl *ctrl);
	virtual MTControl* MTCT getcontrolfromuid(int uid);
	virtual MTControl* MTCT getcontrolfrompoint(MTPoint &p);
	virtual void MTCT nextcontrol(MTControl *start,bool reverse);
	virtual void* MTCT getoffsetrgn(int type);
	virtual void MTCT offset(int ox,int oy);
	virtual void MTCT createbitmap();
	virtual void MTCT deletebitmap();
	virtual bool MTCT flush();
	virtual bool MTCT flush(MTRect &rect);
	virtual bool MTCT flush(int x,int y,int w,int h);
	virtual int MTCT show(MTWinControl *w,int modal);
	virtual void MTCT focus(MTControl *ctrl);
	virtual void MTCT showcontrol(MTControl *ctrl);
	virtual void MTCT showrect(MTRect &rect);
	virtual void MTCT bringtofront(MTControl *w);
	virtual void MTCT puttoback(MTControl *w);
	virtual void MTCT updateregions();
	virtual void* MTCT open(int type);
	virtual void MTCT close(void *o);
	virtual void MTCT clip(MTRect &rect);
	virtual void MTCT cliprgn(void *rgn);
	virtual void MTCT unclip();
	virtual bool MTCT bmpblt(MTBitmap *src,int x,int y,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	virtual bool MTCT bltbmp(MTBitmap *dest,int x,int y,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	virtual bool MTCT sbmpblt(MTBitmap *src,int x,int y,int w,int h,int ox,int oy,int ow,int oh,int mode = MTBM_COPY);
	virtual bool MTCT sbltbmp(MTBitmap *dest,int x,int y,int w,int h,int ox,int oy,int ow,int oh,int mode = MTBM_COPY);
	virtual bool MTCT blt(int x,int y,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	virtual void MTCT setpen(int color);
	virtual void MTCT setbrush(int color);
	virtual bool MTCT fillcolor(int x,int y,int w,int h,int color,int opacity = 255);
	virtual bool MTCT fillrect(int x,int y,int w,int h,int mode = MTBM_COPY);
	virtual void MTCT point(int x,int y,int color);
	virtual void MTCT moveto(int x,int y);
	virtual void MTCT lineto(int x,int y);
	virtual void MTCT polygon(const MTPoint *pt,int np);
	virtual void MTCT polyline(const MTPoint *pt,int np);
	virtual void MTCT rectangle(int x,int y,int w,int h);
	virtual void MTCT ellipse(int x,int y,int w,int h);
	virtual void MTCT settextcolor(int color);
	virtual void MTCT setfont(void *font);
	virtual void MTCT drawtext(const char *text,int length,MTRect &rect,int flags);
	virtual bool MTCT gettextsize(const char *text,int length,MTPoint *size,int maxwidth = -1);
	virtual int MTCT gettextextent(const char *text,int length,int maxextent);
	virtual int MTCT gettextheight();
	virtual int MTCT getcharwidth(char c);
	virtual void MTCT toscreen(MTPoint &p);
	void flushstart();
	bool flushend();
	bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o);
	void MTCT moffset(int dx,int dy);
	void MTCT boffset(bool children = false);
	void MTCT initdrag(MTControl &cctrl);
	void MTCT startdrag();
	void MTCT drag(int mx,int my);
	void MTCT enddrag();
	static void MTCT mbchange(MTBitmap *oldbitmap,MTBitmap *newbitmap,void *param);
private:
	int cmox,cmoy;
	bool cmoving;
	void *frgn;
	int notifycount;
};
//---------------------------------------------------------------------------
#include "MTDesktop.h"
//---------------------------------------------------------------------------
#endif
