//---------------------------------------------------------------------------
#ifndef MTSKIN_INCLUDED
#define MTSKIN_INCLUDED
//---------------------------------------------------------------------------
class MTControl;
class MTWinControl;
class MTUserList;
//---------------------------------------------------------------------------
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#define sk_back 'BACK'
#define sk_font 'FONT'
#define sk_wins 'WINS'
#define sk_btns 'BTNS'
#define sk_tbtn 'TBTN'
#define sk_dtbt 'DTBT'
#define sk_wbtn 'WBTN'
#define sk_dbtn 'DBTN'
#define sk_mbtn 'MBTN'
#define sk_arrw 'ARRW'
#define sk_cbbt 'CBBT'
#define sk_hscr 'HSCR'
#define sk_vscr 'VSCR'
#define sk_hscb 'HSCB'
#define sk_vscb 'VSCB'
#define sk_hdsc 'HDSC'
#define sk_vdsc 'VDSC'
#define sk_stbt 'STBT'
#define sk_patt 'PATT'
#define sk_hsli 'HSLI'
#define sk_vsli 'VSLI'
#define sk_knob 'KNOB'
#define sk_vols 'VOLS'
#define sk_vume 'VUME'
#define sk_anim 'ANIM'
#define sk_icon 'ICON'
#define sk_chkb 'CHKB'
#define sk_radb 'RADB'
#define sk_imag 'IMAG'
#define sk_prog 'PROG'
#define sk_skin 'SKIN'
#define sk_bmid	'BMID'
#else
#define sk_back 'KCAB'
#define sk_font 'TNOF'
#define sk_wins 'SNIW'
#define sk_btns 'SNTB'
#define sk_tbtn 'NTBT'
#define sk_dtbt 'TBTD'
#define sk_wbtn 'NTBW'
#define sk_dbtn 'NTBD'
#define sk_mbtn 'NTBM'
#define sk_arrw 'WRRA'
#define sk_cbbt 'TBBC'
#define sk_hscr 'RCSH'
#define sk_vscr 'RCSV'
#define sk_hscb 'BCSH'
#define sk_vscb 'BCSV'
#define sk_hdsc 'CSDH'
#define sk_vdsc 'CSDV'
#define sk_stbt 'TBTS'
#define sk_patt 'TTAP'
#define sk_hsli 'ILSH'
#define sk_vsli 'ILSV'
#define sk_knob 'BONK'
#define sk_vols 'SLOV'
#define sk_vume 'EMUV'
#define sk_anim 'MINA'
#define sk_icon 'NOCI'
#define sk_chkb 'BKHC'
#define sk_radb 'BDAR'
#define sk_imag 'GAMI'
#define sk_prog 'GORP'
#define sk_skin 'NIKS'
#define sk_bmid	'DIMB'
#endif
//---------------------------------------------------------------------------
class MTSkin : public Skin{
public:
	MTSkin();
	~MTSkin();

	void MTCT loadfromres(MTResources *res);
	void MTCT savetores(MTResources *res);
//	Controls data
	void MTCT initcontrol(MTControl *ctrl);
	void MTCT uninitcontrol(MTControl *ctrl);
	void MTCT resetcontrol(MTControl *ctrl,bool skinchange);
	void MTCT timercontrol(MTControl *ctrl);
//	Control-specific skin notifications
	void MTCT notify(MTControl *ctrl,int type,int param1,int param2,void *param3 = 0);
//	Controls drawing
	void MTCT drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	void MTCT drawcontrol(int guiid,int id,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	void MTCT drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y);
	void MTCT drawmodalveil(MTWinControl *ctrl,MTRect &rect);
	void MTCT drawdragbkg(MTBitmap *b,MTRect &rect,int style);
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
	MTSkinPart bkg;
	MTFNMetrics fnm;
	MTWNMetrics wnm[8];
	MTBTMetrics btm[5];
	MTBTMetrics tbtm[4];
	MTBTMetrics dtbtm[4];
	MTSQMetrics wbtns;
	MTSQMetrics dbtns;
	MTSQMetrics mbtns;
	MTSkinPart arm;
	MTSkinPart cbbm;
	MTBTMetrics hscm;
	MTBTMetrics vscm;
	MTBTMetrics hscbm;
	MTBTMetrics vscbm;
	MTBTMetrics hdscm;
	MTBTMetrics vdscm;
	MTSkinPart sbm;
	MTBTMetrics ptm[4];
	MTSLMetrics hslm;
	MTSLMetrics vslm;
	MTSLMetrics knm;
	MTSLMetrics vsm;
	MTSLMetrics vum;
	MTSQMetrics animm[4];
	MTSQMetrics icom;
	MTSkinPart cbm;
	MTSkinPart rbm;
	MTSQMetrics ilm;
	MTSLMetrics prm;
	MTSkinInfo info;
	MTRect br[8];
	struct SKO{
		char *k;
		int t;
		void *a;
		int s;
		int p;
	} sko[32];
	static void MTCT skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param);
	void setfonts();
	void delfonts();
	void delshadows();
	void drawwindow(MTBitmap *bmp,int x,int y,int w,int h,int style);
	void drawinnerwindow(MTBitmap *bmp,int x,int y,int w,int h,int style);
	void drawbutton(MTBitmap *bmp,int x,int y,int w,int h,MTBTMetrics *m);
	void drawslider(MTBitmap *bmp,int x,int y,int w,int h,MTSLMetrics *m,int min,int max,int pos,bool focused);
	void drawshadow(MTBitmap *bmp,int x,int y,int w,int h,int style);
};
//---------------------------------------------------------------------------
bool initSkin();
void uninitSkin();
//---------------------------------------------------------------------------
#endif
