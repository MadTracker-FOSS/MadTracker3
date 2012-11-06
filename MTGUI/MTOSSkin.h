//---------------------------------------------------------------------------
#ifndef MTOSSKIN_INCLUDED
#define MTOSSKIN_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
class MTOSSkin : public Skin{
public:
	MTOSSkin();
	~MTOSSkin();

	void MTCT loadfromres(MTResources *res);
	void MTCT savetores(MTResources *res);
//	Controls data
	void MTCT initcontrol(MTControl *ctrl);
	void MTCT uninitcontrol(MTControl *ctrl);
	void MTCT resetcontrol(MTControl *ctrl,bool skinchange);
	void MTCT timercontrol(MTControl *ctrl);
//	Controls drawing
	void MTCT drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	void MTCT drawcontrol(int guiid,int id,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	void MTCT drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y);
	void MTCT drawmodalveil(MTWinControl *ctrl,MTRect &rect);
//	Dimensions
	void MTCT updatemetrics();
	MTBitmap* MTCT getbitmap(int id);
	MTSLMetrics* MTCT getslider(int type,int orientation);
	void MTCT calcbounds(int m);
	void MTCT setshadows(bool enabled);
	void MTCT getcontrolsize(int guiid,int id,int &width,int &height);
	void MTCT getwindowborders(int style,MTRect *borders);
	void MTCT getwindowoffsets(int style,MTRect *borders);
	void MTCT getregions(MTControl *ctrl,void **opaque,void **transparent);
	int MTCT getcolor(int id);
	void* MTCT getfont(int id);
	bool MTCT gettextsize(MTControl *ctrl,const char *text,int length,MTPoint *size,int maxwidth = -1);
	int MTCT gettextextent(MTControl *ctrl,const char *text,int length,int maxextent);
	int MTCT gettextheight(MTControl *ctrl);
	int MTCT getcharwidth(MTControl *ctrl,char c);
//	Miscellaneous drawing
	void MTCT drawitem(MTUserList *list,int i,MTRect &rect,MTBitmap *b,const char *caption,int imageindex,int itemflags,bool editable);
	void MTCT drawchar(unsigned char c,MTBitmap *bmp,int &x,int y,int color);
	void MTCT drawtext(unsigned char *text,MTBitmap *bmp,int &x,int y,int color);
	void MTCT drawdec(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color);
	void MTCT drawhex(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color);
	void MTCT drawframe(MTBitmap *bmp,int x,int y,int w,int h,int flags = 0);
private:
	static unsigned char fontmap[256];
	int bmpid[16];
	MTMask *mask[8];
	MTRect mr[8][8];
	void *hskfont[4];
	MTFNMetrics fnm;
	void setfonts();
	void delfonts();
	static void MTCT skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param);
};
//---------------------------------------------------------------------------
#endif
