//---------------------------------------------------------------------------
#ifndef MTGDIBITMAP_INCLUDED
#define MTGDIBITMAP_INCLUDED
//---------------------------------------------------------------------------
class MTGDIBitmap;
class MTGDIMask;
//---------------------------------------------------------------------------
#include <windows.h>
#include "MTDisplayASM.h"
#include "MTDisplayMMX.h"
#include "MTBitmap.h"
//---------------------------------------------------------------------------
const int bltmode[4] = {SRCCOPY,SRCPAINT,SRCINVERT,0xEA02E9};
const int trmode[4] = {SRCPAINT,SRCPAINT,SRCINVERT,0xEA02E9};
const int patmode[4] = {PATCOPY,0xFA0089,PATINVERT,PATCOPY};
//---------------------------------------------------------------------------
class MTGDIBitmap : public MTBitmap{
public:
	MTGDIBitmap(MTDisplayDevice *d,int f,int w,int h);
	MTGDIBitmap(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey);
	MTGDIBitmap(MTDisplayDevice *d,int f,const char *filename,int colorkey);
	MTGDIBitmap(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey);
	~MTGDIBitmap();
	bool MTCT load();
	void MTCT unload();
	bool MTCT setsize(int w,int h);
	void* MTCT open(int wantedtype);
	void MTCT close(void *o);
	void MTCT clip(MTRect *rect);
	void MTCT cliprgn(void *rgn);
	void MTCT unclip();
	void* MTCT getclip();
	bool MTCT openbits(MTRect &rect,void **bits,int *pitch,void **maskbits = 0,int *maskpitch = 0);
	void MTCT closebits();
	bool MTCT blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	bool MTCT sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode = MTBM_COPY);
	bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o,int color);
	bool MTCT skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color);
	bool MTCT maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o);
	bool MTCT shade(int x,int y,int w,int h,MTMask *mask,int mx,int my);
	bool MTCT tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity);
	int MTCT translatecolor(int color);
	void MTCT setpen(int color);
	void MTCT setbrush(int color);
	bool MTCT fill(int x,int y,int w,int h,int color,int opacity = 255);
	bool MTCT fillex(int x,int y,int w,int h,int mode = MTBM_COPY);
	void MTCT point(int x,int y,int color);
	void MTCT moveto(int x,int y);
	void MTCT lineto(int x,int y);
	void MTCT polygon(const MTPoint *pt,int np);
	void MTCT polyline(const MTPoint *pt,int np);
	void MTCT rectangle(int x,int y,int w,int h);
	void MTCT ellipse(int x,int y,int w,int h);
	void MTCT settextcolor(int color);
	void MTCT setfont(void *font);
	void MTCT drawtext(const char *text,int length,MTRect &rect,int flags);
	bool MTCT gettextsize(const char *text,int length,MTPoint *size,int maxwidth = -1);
	int MTCT gettextextent(const char *text,int length,int maxextent);
	int MTCT gettextheight();
	int MTCT getcharwidth(char c);
	int MTCT getbaseline();
	void MTCT setwindow(MTWinControl *window);
	void MTCT toscreen(MTPoint &p);
protected:
	int mcount;
	void *mbits;
	unsigned char *mmbits;
	int mpitch;
	MTRect ob;
	bool allocated;
	HDC mdc,mmdc;
	HBITMAP bmp,oldbmp,mask;
	int clipped;
	HRGN mrgn;
	HRGN crgn[MAX_CLIP];
	BITMAPINFOHEADER bmi;
	int mtextinit;
	int mtxtcolor;
	HFONT mfont;
	int baseline;
	int textheight;
	void MTCT initialize();
};

class MTGDIMask : public MTMask{
public:
	MTGDIMask(MTDisplayDevice *d,int w,int h);
	~MTGDIMask();
	void MTCT blur(int amount);
	void MTCT opacity(int amount);
	void MTCT fill(int x,int y,int w,int h,int amount);
protected:
	friend MTGDIBitmap;
	void *mbits;
	int mpitch;
	HDC mdc;
	HBITMAP bmp,oldbmp;
	BITMAPINFOHEADER bmi;
	RGBQUAD pal[256];
};
//---------------------------------------------------------------------------
extern ShadeProc asm_shade;
extern SShadeProc asm_sshade;
extern BlendProc asm_blend;
extern BlendTProc asm_blendt;
extern BlendCKProc asm_blendck;
extern BlurProc asm_blur;
extern OpacityProc asm_opacity;
extern FillProc asm_fill;
//---------------------------------------------------------------------------
#endif
