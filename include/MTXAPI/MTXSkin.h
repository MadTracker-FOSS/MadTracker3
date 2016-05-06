//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXSkin.h 67 2005-08-26 22:18:51Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXSKIN_INCLUDED
#define MTXSKIN_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"

//---------------------------------------------------------------------------
class MTControl;

class MTWinControl;

class MTUserList;
//---------------------------------------------------------------------------
#include "MTXDisplay.h"
#include "MTXControls.h"
#include "MTXSystem.h"

//---------------------------------------------------------------------------
enum
{
    SC_BACKGROUND = 0xFF000000,
    SC_TEXT_NORMAL,
    SC_TEXT_FOCUSED,
    SC_WINDOW_NORMAL,
    SC_WINDOW_FOCUSED,
    SC_EDIT_BACKGROUND,
    SC_EDIT_NORMAL,
    SC_EDIT_FOCUSED,
    SC_EDIT_SELECTION,
    SC_EDIT_SELECTED,
    SC_BUTTON_NORMAL,
    SC_BUTTON_FOCUSED,
    SC_BUTTON_DISABLED,
    SC_LINK_NORMAL,
    SC_LINK_FOCUSED,
    SC_TAB_NORMAL,
    SC_TAB_FOCUSED,
    SC_CTRL_HL,
    SC_CTRL_L,
    SC_CTRL_BKG,
    SC_CTRL_S,
    SC_CTRL_DS,
    SC_CURSOR,
    SC_PATT_TEXT1,
    SC_PATT_TEXT2,
    SC_PATT_TEXT3,
    SC_PATT_TEXT4,
    SC_PATT_TEXT5,
    SC_PATT_TEXT6,
    SC_PATT_TEXT7,
    SC_PATT_TEXT8,
    SC_PATT_BACK1,
    SC_PATT_BACK2,
    SC_PATT_BACK3,
    SC_PATT_BACK4,
    SC_PATT_BACK5,
    SC_PATT_BACK6,
    SC_PATT_BACK7,
    SC_PATT_BACK8,
    SC_MAX
};

enum
{
    SKIN_FIXED = 0x0000, SKIN_STRETCH, SKIN_TILE, SKIN_COLOR
};

#define SKIN_TRANSP 0x01
#define SKIN_ALPHA  0x02

enum
{
    SKIN_BUTTON = 0, SKIN_PROGR, SKIN_ANIM, SKIN_CIRCLE, SKIN_VUMETER, SKIN_XBUTTON, SKIN_XPROGR, SKIN_XVUMETER
};

enum
{
    SKIN_HORIZ = 0, SKIN_VERT, SKIN_BOTH
};

#define SF_BOLD       0x01
#define SF_ITALIC     0x02
#define SF_UNDERLINED 0x04
#define SF_CENTER     0x10
#define SF_RIGHT      0x20
//---------------------------------------------------------------------------
#ifndef MTGUITypes
#define MTGUITypes
struct MTPoint{
    int x,y;
};

struct MTRect{
    int left,top,right,bottom;
};

struct MTBounds{
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
};
#endif

#ifndef MTSkinTypes
#define MTSkinTypes
struct MTSkinPart{
    unsigned char mode,flags,bmpid,reserved;
    union{
        MTBounds b;
        int color;
    };
};
#endif

struct MTSQMetrics
{
    MTSkinPart a;
    int nx, ny;
    int speed;
};

struct MTCANIMetrics
{
    MTSkinPart a;
    MTSkinPart fa;
    int nx;
    int ny;
};

struct MTFontDesc
{
    unsigned short flags;
    unsigned short size;
    char name[48];
};

struct MTFNMetrics
{
    MTFontDesc caption;
    MTFontDesc text;
    MTFontDesc button;
    MTFontDesc edit;
    unsigned char fontseq[96];
    MTSQMetrics pattfont;
    int colors[SC_MAX - SC_BACKGROUND];
};

struct MTBTMetrics
{
    MTSkinPart bkg;
    MTSkinPart tl;
    MTSkinPart t;
    MTSkinPart tr;
    MTSkinPart bl;
    MTSkinPart b;
    MTSkinPart br;
    MTSkinPart l;
    MTSkinPart r;
};

struct MTWNMetrics: public MTBTMetrics
{
    MTPoint tlo;
    MTPoint tro;
    MTPoint blo;
    MTPoint bro;
    int to;
    int bo;
    int lo;
    int ro;
    MTRect co;
    MTPoint btno;
};

struct MTBSLM
{
    MTSkinPart s;
    MTSkinPart b;
    MTSkinPart fb;
    MTRect sa;
};

struct MTPSLM
{
    MTSkinPart s;
    MTSkinPart ps;
    MTSkinPart pm;
    MTSkinPart pe;
    MTRect sa;
};

struct MTCSLM
{
    MTSkinPart s;
    MTSkinPart b;
    MTSkinPart fb;
    MTPoint cp;
    int r;
};

struct MTVSLM
{
    MTSkinPart s;
    MTSkinPart v;
    MTRect sa;
};

struct MTXBSLM
{
    MTBTMetrics s;
    MTSkinPart b;
    MTSkinPart fb;
    MTRect sa;
};

struct MTXPSLM
{
    MTBTMetrics s;
    MTSkinPart ps;
    MTSkinPart pm;
    MTSkinPart pe;
    MTRect sa;
};

struct MTXVSLM
{
    MTBTMetrics s;
    MTSkinPart v;
    MTRect sa;
};

struct MTSLMetrics
{
    int type;
    int orientation;
    union
    {
        MTBSLM a;
        MTPSLM b;
        MTCANIMetrics c;
        MTCSLM d;
        MTVSLM e;
        MTXBSLM xa;
        MTXPSLM xb;
        MTXVSLM xe;
    };
};

struct MTSkinInfo
{
    int colorkey;
    char skinname[128];
    char skinauthor[64];
    char skinemail[64];
    char skinurl[256];
};

class Skin
{
public:
    MTBitmap *skinbmp[16];
    int fontwidth;
    int fontheight;

    virtual void MTCT loadfromres(MTResources *res) = 0;

    virtual void MTCT savetores(MTResources *res) = 0;

//	Controls data
    virtual void MTCT initcontrol(MTControl *ctrl) = 0;

    virtual void MTCT uninitcontrol(MTControl *ctrl) = 0;

    virtual void MTCT resetcontrol(MTControl *ctrl, bool skinchange) = 0;

    virtual void MTCT timercontrol(MTControl *ctrl) = 0;

//	Control-specific skin notifications
    virtual void MTCT notify(MTControl *ctrl, int type, int param1, int param2, void *param3 = 0) = 0;

//	Controls drawing
    virtual void MTCT drawcontrol(MTControl *ctrl, MTRect &rect, MTBitmap *b, int x, int y, int flags = 0) = 0;

    virtual void MTCT drawcontrol(int guiid, int id, MTRect &rect, MTBitmap *b, int x, int y, int flags = 0) = 0;

    virtual void MTCT drawborder(MTWinControl *ctrl, MTRect &rect, MTBitmap *b, int x, int y) = 0;

    virtual void MTCT drawmodalveil(MTWinControl *ctrl, MTRect &rect) = 0;

    virtual void MTCT drawdragbkg(MTBitmap *b, MTRect &rect, int style) = 0;

//	Dimensions
    virtual void MTCT updatemetrics() = 0;

    virtual MTBitmap *MTCT getbitmap(int id) = 0;

    virtual MTSLMetrics *MTCT getslider(int type, int orientation) = 0;

    virtual void MTCT calcbounds(int m) = 0;

    virtual void MTCT setshadows(bool enabled) = 0;

    virtual void MTCT getcontrolsize(int guiid, int id, int &width, int &height) = 0;

    virtual void MTCT getwindowborders(int style, MTRect *borders) = 0;

    virtual void MTCT getwindowoffsets(int style, MTRect *borders) = 0;

    virtual void MTCT getregions(MTControl *ctrl, void **opaque, void **transparent) = 0;

    virtual int MTCT getcolor(int id) = 0;

    virtual void *MTCT getfont(int id) = 0;

    virtual bool MTCT gettextsize(MTControl *ctrl, const char *text, int length, MTPoint *size, int maxwidth = -1) = 0;

    virtual int MTCT gettextextent(MTControl *ctrl, const char *text, int length, int maxextent) = 0;

    virtual int MTCT gettextheight(MTControl *ctrl) = 0;

    virtual int MTCT getcharwidth(MTControl *ctrl, char c) = 0;

//	Miscellaneous drawing
    virtual void MTCT drawitem(MTUserList *list, int i, MTRect &rect, MTBitmap *b, const char *caption, int imageindex, int itemflags, bool editable) = 0;

    virtual void MTCT drawchar(unsigned char c, MTBitmap *bmp, int &x, int y, int color) = 0;

    virtual void MTCT drawtext(unsigned char *text, MTBitmap *bmp, int &x, int y, int color) = 0;

    virtual void MTCT drawdec(int val, bool zeroes, int n, MTBitmap *bmp, int &x, int y, int color) = 0;

    virtual void MTCT drawhex(int val, bool zeroes, int n, MTBitmap *bmp, int &x, int y, int color) = 0;

    virtual void MTCT drawframe(MTBitmap *bmp, int x, int y, int w, int h, int flags = 0) = 0;
};
//---------------------------------------------------------------------------
#endif
