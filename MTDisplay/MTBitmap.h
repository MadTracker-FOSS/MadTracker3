//---------------------------------------------------------------------------
#ifndef MTBITMAP_INCLUDED
#define MTBITMAP_INCLUDED

#define MTB_DRAW         0x00000001
#define MTB_SCREEN       0x00000002
#define MTB_SKIN         0x00000004
#define MTB_NOINHERIT    0x0000FFFF
#define MTB_KEEPMASK     0x00010000

#define DTXT_RIGHT     1
#define DTXT_CENTER    2
#define DTXT_BOTTOM    4
#define DTXT_VCENTER   8
#define DTXT_PREFIX    16
#define DTXT_MULTILINE 32

enum{
	MTBM_COPY = 0,
	MTBM_PAINT,
	MTBM_INVERT,
	MTBM_COLORPAINT
};

#define MAX_CLIP 16

class MTWinControl;
class MTBitmap;
class MTMask;

#include "../Headers/MTXExtension.h"
#include "../Headers/MTXSystem.h"
#include "../Headers/MTXGUI.h"
#include "../Headers/MTXSkin.h"
#include "MTDisplayDevice.h"
//---------------------------------------------------------------------------
typedef void (MTCT *MTBitmapChangeProc)(MTBitmap *oldbitmap,MTBitmap *newbitmap,void *param);

class MTBitmap{
public:
	MTDisplayDevice *device;
	int flags;
	int width,height;
	int bitcount;
	MTRect wr;
	MTBitmapChangeProc changeproc;
	void *param;
	bool loaded;
	
	virtual bool MTCT load() = 0;
	virtual void MTCT unload() = 0;
	virtual bool MTCT setsize(int w,int h) = 0;
	virtual void* MTCT open(int wantedtype) = 0;
	virtual void MTCT close(void *o) = 0;
	virtual void MTCT clip(MTRect *rect) = 0;
	virtual void MTCT cliprgn(void *rgn) = 0;
	virtual void MTCT unclip() = 0;
	virtual void* MTCT getclip() = 0;
	virtual bool MTCT openbits(MTRect &rect,void **bits,int *pitch,void **maskbits = 0,int *maskpitch = 0) = 0;
	virtual void MTCT closebits() = 0;
	virtual bool MTCT blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode = MTBM_COPY) = 0;
	virtual bool MTCT sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode = MTBM_COPY) = 0;
	virtual bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o,int color = 0xFFFFFF) = 0;
	virtual bool MTCT skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color = 0xFFFFFF) = 0;
	virtual bool MTCT maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o) = 0;
	virtual bool MTCT shade(int x,int y,int w,int h,MTMask *mask,int mx,int my) = 0;
	virtual bool MTCT tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh) = 0;
	virtual bool MTCT sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh) = 0;
	virtual bool MTCT blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity) = 0;
	virtual int MTCT translatecolor(int color) = 0;
	virtual void MTCT setpen(int color) = 0;
	virtual void MTCT setbrush(int color) = 0;
	virtual bool MTCT fill(int x,int y,int w,int h,int color,int opacity = 255) = 0;
	virtual bool MTCT fillex(int x,int y,int w,int h,int mode = MTBM_COPY) = 0;
	virtual void MTCT point(int x,int y,int color) = 0;
	virtual void MTCT moveto(int x,int y) = 0;
	virtual void MTCT lineto(int x,int y) = 0;
	virtual void MTCT polygon(const MTPoint *pt,int np) = 0;
	virtual void MTCT polyline(const MTPoint *pt,int np) = 0;
	virtual void MTCT rectangle(int x,int y,int w,int h) = 0;
	virtual void MTCT ellipse(int x,int y,int w,int h) = 0;
	virtual void MTCT settextcolor(int color) = 0;
	virtual void MTCT setfont(void *font) = 0;
	virtual void MTCT drawtext(const char *text,int length,MTRect &rect,int flags) = 0;
	virtual bool MTCT gettextsize(const char *text,int length,MTPoint *size,int maxwidth = -1) = 0;
	virtual int MTCT gettextextent(const char *text,int length,int maxextent) = 0;
	virtual int MTCT gettextheight() = 0;
	virtual int MTCT getcharwidth(char c) = 0;
	virtual int MTCT getbaseline() = 0;
	virtual void MTCT setwindow(MTWinControl *window) = 0;
	virtual void MTCT setmodified(void *w){ if (w==(void*)mwnd) modified = true; };
	virtual void MTCT toscreen(MTPoint &p) = 0;
public:
	int id;
	MTResources *mres;
	int mresid;
	char *mfilename;
	MTBitmap *morig;
	int mck;
	int mox,moy;
	bool modified;
	void *mwnd;
	virtual void MTCT initialize() = 0;
};

class MTMask{
public:
	MTDisplayDevice *device;
	int width;
	int height;
	
	virtual void MTCT blur(int amount) =  0;
	virtual void MTCT opacity(int amount) = 0;
	virtual void MTCT fill(int x,int y,int w,int h,int amount) = 0;
};
//---------------------------------------------------------------------------
#endif
