//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXControls.h 67 2005-08-26 22:18:51Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXCONTROLS_INCLUDED
#define MTXCONTROLS_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
#define MTC_CONTROL            0x0001
#define MTC_CUSTOMCTRL        0x10001
#define  MTC_WINCONTROL        0x0003
#define   MTC_CUSTOMWINCTRL   0x10003
#define   MTC_DESKTOP          0xFF03
#define   MTC_WINDOW           0x0103
#define   MTC_VISUAL           0x0203
#define   MTC_ITEMVIEW         0x1003
#define    MTC_LISTBOX         0x2003
#define    MTC_MENU            0x4003
#define     MTC_MAINMENU       0x4007
#define   MTC_USERLIST         0x6003
#define    MTC_FILELISTBOX     0x6103
#define   MTC_PANEL            0x7003
#define   MTC_TOOLBAR          0x7403
#define   MTC_TABCONTROL       0x8003
#define   MTC_TABSHEET         0x8103
#define  MTC_SCROLLER          0x0101
#define  MTC_SLIDER            0x0201
#define  MTC_LED               0x0301
#define  MTC_OSCILLO           0x0401
#define  MTC_ITEM              0xF001
#define   MTC_LISTITEM         0xF011
#define   MTC_MENUITEM         0xF021
#define  MTC_LABEL             0x1001
#define  MTC_BUTTON            0x2001
#define  MTC_SIGN              0x3001
#define  MTC_CHECKBOX          0x4001
#define  MTC_EDIT              0x5001
#define   MTC_ITEMCOMBOBOX     0x5101
#define   MTC_USERCOMBOBOX     0x5201
#define  MTC_PROGRESS          0x6001
#define  MTC_STATUS            0x7001
#define  MTC_SEQUENCE          0x8001
#define  MTC_OSWINDOW          0xFF01

#define MTCM_MOUSEDOWN   0x0001
#define MTCM_MOUSEMOVE   0x0002
#define MTCM_MOUSEUP     0x0003
#define MTCM_MOUSECLICK  0x0004
#define MTCM_MOUSEWHEEL  0x0005
#define MTCM_KEYDOWN     0x0008
#define MTCM_KEYUP       0x0009
#define MTCM_CHAR        0x000A
#define MTCM_USERINPUT   0x0100
#define MTCM_ENTER       0x0100
#define MTCM_LEAVE       0x0101
#define MTCM_ACTIVE      0x0102
#define MTCM_BOUNDS      0x0200
#define MTCM_FLUSH       0x0300
#define MTCM_ACTION      0x1000
#define MTCM_ITEMCHANGE  0x1001
#define MTCM_ITEMSELECT  0x1002
#define MTCM_TIMER       0x1080
#define MTCM_SHORTCUT    0x1100
#define MTCM_HOTKEY      0x1101
#define MTCM_REFRESH     0x1081
#define MTCM_STAYONTOP   0x10FB
#define MTCM_HELP        0x10FC
#define MTCM_MINIMIZE    0x10FD
#define MTCM_MAXIMIZE    0x10FE
#define MTCM_CLOSE       0x10FF
#define MTCM_DESTROY     0x1100
#define MTCM_ONPOPUP     0x1200
#define MTCM_NOTIFY      0x2000
#define MTCM_POSCHANGED  0x2001
#define MTCM_CHANGE      0x4000
#define MTCM_TOUCHED     0x6000
#define MTCM_USER        0x80000000

#define MTCF_DISABLED    0x00000001
#define MTCF_FOCUSED     0x00000002
#define MTCF_SELECTED    0x00000004
#define MTCF_OVER        0x00000008
#define MTCF_DOWN        0x00000010
#define MTCF_HIDDEN      0x00000040
#define MTCF_BORDER      0x00000080
#define MTCF_ACCEPTINPUT 0x00000100
#define MTCF_ACCEPTCTRLS 0x00000120
#define MTCF_RAISED      0x00000200
#define MTCF_TRANSPARENT 0x00000400
#define MTCF_NEEDUPDATE  0x00000800
#define MTCF_SYSTEM      0x00001000
#define MTCF_DONTDRAW    0x00002000
#define MTCF_DONTFLUSH   0x00004000
#define MTCF_DONTSAVE    0x00008000
#define MTCF_DONTRESIZE  0x00010000
#define MTCF_FREEONCLOSE 0x00020000
#define MTCF_GHOST       0x00040000
#define MTCF_NOTIFYPOS   0x00080000
#define MTCF_STAYONTOP   0x01000000
#define MTCF_MODAL       0x04000000
#define MTCF_POPUP       0x08000000
#define MTCF_STAYONBACK  0xFF000000
#define MTCF_CANTDRAW    0x00002040
#define MTCF_CANTTOUCH   0x00040040

enum
{
    MTCA_TOPLEFT = 0x0,
    MTCA_TOPRIGHT,
    MTCA_BOTTOMLEFT,
    MTCA_BOTTOMRIGHT,
    MTCA_LEFT,
    MTCA_RIGHT,
    MTCA_TOP = 0x8,
    MTCA_BOTTOM = 0xA,
    MTCA_CLIENT = 0xC
};

enum
{
    MTWS_FIXED = 0,
    MTWS_FIXEDCAP,
    MTWS_SIZABLE,
    MTWS_SIZABLECAP,
    MTWS_DIALOG,
    MTWS_TABBED,
    MTWS_DOCK,
    MTWS_MAIN,
    MTWS_CLOSE = 256,
    MTWS_MAXIMIZE = 512,
    MTWS_MINIMIZE = 1024,
    MTWS_HELP = 2048,
    MTWS_STAYONTOP = 4096
};

#define MTST_HBAR 0
#define MTST_VBAR 1

enum
{
    MTP_INT = 0, MTP_BOOL, MTP_TEXT, MTP_FLAGS, MTP_LIST, MTP_ACTION, MTP_ITEMS
};

#define MTVF_IMAGES   1
#define MTVF_CHECKBOX 2
#define MTVF_RADIOBOX 4

#define NORECT *(MTRect*)0
//---------------------------------------------------------------------------
struct MTCMessage;

class MTControl;

class MTWinControl;

class MTScroller;

class MTCustomBehaviours;

class MTCustomControl;

class MTCustomWinBehaviours;

class MTCustomWinControl;

class MTList;

class MTUserList;

class MTItem;

class MTItemView;

class MTListItem;

class MTListBox;

class MTMenuItem;

class MTMenu;

class MTFileListBox;

class MTSlider;

class MTDesktop;

class MTWindow;

class MTPanel;

class MTOSWindow;

class MTTabControl;
//---------------------------------------------------------------------------
#include "MTXGUI.h"
#include "MTXDisplay.h"
#include "MTXSystem.h"
#include "MTXWrapper.h"

//---------------------------------------------------------------------------
struct MTCMessage
{
    int msg;
    int result;
    MTControl *ctrl;
    union
    {
        struct
        {
            union
            {
                int x;
                int key;
                int param1;
            };
            union
            {
                int y;
                int scancode;
                int param2;
            };
            union
            {
                int w;
                int button;
                int repeat;
                int param3;
            };
            union
            {
                int h;
                int buttons;
                void *param4;
            };
        };
        MTRect dr;
        MTPoint p;
    };
    MTShortcut *s;
};

class MTControl
{
public:
    MTWinControl *parent;
    MTWindow *window;
    int guiid;
    int uid;
    char *name;
    int tag;
    int flags;
    int left, top;
    int width, height;
    int align;
    bool direct;
    int timercount;
    MTMenu *popup;
    bool autopopup;
    void *skindata;

    MTControl(int id, int tg, MTWinControl *p, int l, int t, int w, int h);

    virtual ~MTControl() = 0;

    virtual int MTCT loadfromstream(MTFile *f, int size, int flags) = 0;

    virtual int MTCT savetostream(MTFile *f, int flags) = 0;

    virtual int MTCT getnumproperties(int id) = 0;

    virtual bool MTCT getpropertytype(int id, char **name, int &flags) = 0;

    virtual bool MTCT getproperty(int id, void *value) = 0;

    virtual bool MTCT setproperty(int id, void *value) = 0;

    virtual void MTCT setbounds(int l, int t, int w, int h) = 0;

    virtual bool MTCT checkbounds(int &l, int &t, int &w, int &h) = 0;

    virtual void MTCT getrect(MTRect &rect, int client) = 0;

    virtual void MTCT switchflags(int f, bool set) = 0;

    virtual void MTCT draw(MTRect &rect) = 0;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual void MTCT preparedraw(MTBitmap **b, int &ox, int &oy) = 0;

    virtual void MTCT setparent(MTWinControl *newparent) = 0;

    bool MTCT designmessage(MTCMessage &msg);

protected:
    friend class MTWinControl;

    int cborder;
    int mox, moy, mow, moh;
    bool moving, sizing, triggered;
};

typedef bool (MTCT *MTCMessageProc)(MTWinControl *window, MTCMessage &msg);

class MTWinControl: public MTControl
{
public:
    MTDesktop *dsk;
    int box, boy;
    MTBitmap *mb;
    int ncontrols;
    MTControl **controls;
    MTControl *focused;
    MTScroller *hs, *vs;
    MTCMessageProc messageproc;
    void *oprgn, *trrgn;
    int *modalparent;
    int modalresult;

    virtual ~MTWinControl() = 0;

    virtual int MTCT loadfromstream(MTFile *f, int size, int flags) = 0;

    virtual int MTCT savetostream(MTFile *f, int flags) = 0;

    virtual void MTCT setbounds(int l, int t, int w, int h) = 0;

    virtual bool MTCT checkbounds(int &l, int &t, int &w, int &h) = 0;

    virtual void MTCT getrect(MTRect &rect, int client) = 0;

    virtual void *MTCT getemptyrgn() = 0;

    virtual void *MTCT getvisiblergn(bool client, MTControl *control = 0) = 0;

    virtual void *MTCT getfixedrgn() = 0;

    virtual void MTCT switchflags(int f, bool set) = 0;

    virtual void MTCT draw(MTRect &rect) = 0;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual void MTCT addcontrol(MTControl *control) = 0;

    virtual void MTCT delcontrol(MTControl *control) = 0;

    virtual void MTCT delcontrols(bool del = false) = 0;

    virtual int MTCT getnumcontrols() = 0;

    virtual MTControl *MTCT getcontrol(int id) = 0;

    virtual int MTCT getcontrolid(MTControl *ctrl) = 0;

    virtual MTControl *MTCT getcontrolfromuid(int uid) = 0;

    virtual MTControl *MTCT getcontrolfrompoint(MTPoint &p) = 0;

    virtual void MTCT nextcontrol(MTControl *start, bool reverse) = 0;

    virtual void *MTCT getoffsetrgn(int type) = 0;

    virtual void MTCT offset(int ox, int oy) = 0;

    virtual void MTCT createbitmap() = 0;

    virtual void MTCT deletebitmap() = 0;

    virtual bool MTCT flush() = 0;

    virtual bool MTCT flush(MTRect &rect) = 0;

    virtual bool MTCT flush(int x, int y, int w, int h) = 0;

    virtual int MTCT show(MTWinControl *w, int modal) = 0;

    virtual void MTCT focus(MTControl *ctrl) = 0;

    virtual void MTCT showcontrol(MTControl *ctrl) = 0;

    virtual void MTCT showrect(MTRect &rect) = 0;

    virtual void MTCT bringtofront(MTControl *c) = 0;

    virtual void MTCT puttoback(MTControl *c) = 0;

    virtual void MTCT updateregions() = 0;

    virtual void *MTCT open(int type) = 0;

    virtual void MTCT close(void *o) = 0;

    virtual void MTCT clip(MTRect &rect) = 0;

    virtual void MTCT cliprgn(void *rgn) = 0;

    virtual void MTCT unclip() = 0;

    virtual bool MTCT bmpblt(MTBitmap *src, int x, int y, int w, int h, int ox, int oy, int mode = MTBM_COPY) = 0;

    virtual bool MTCT bltbmp(MTBitmap *dest, int x, int y, int w, int h, int ox, int oy, int mode = MTBM_COPY) = 0;

    virtual bool MTCT sbmpblt(MTBitmap *src, int x, int y, int w, int h, int ox, int oy, int ow, int oh, int mode = MTBM_COPY) = 0;

    virtual bool MTCT sbltbmp(MTBitmap *dest, int x, int y, int w, int h, int ox, int oy, int ow, int oh, int mode = MTBM_COPY) = 0;

    virtual bool MTCT blt(int x, int y, int w, int h, int ox, int oy, int mode = MTBM_COPY) = 0;

    virtual void MTCT setpen(int color) = 0;

    virtual void MTCT setbrush(int color) = 0;

    virtual bool MTCT fillcolor(int x, int y, int w, int h, int color, int opacity = 255) = 0;

    virtual bool MTCT fillrect(int x, int y, int w, int h, int mode = MTBM_COPY) = 0;

    virtual void MTCT point(int x, int y, int color) = 0;

    virtual void MTCT moveto(int x, int y) = 0;

    virtual void MTCT lineto(int x, int y) = 0;

    virtual void MTCT polygon(const MTPoint *pt, int np) = 0;

    virtual void MTCT polyline(const MTPoint *pt, int np) = 0;

    virtual void MTCT rectangle(int x, int y, int w, int h) = 0;

    virtual void MTCT ellipse(int x, int y, int w, int h) = 0;

    virtual void MTCT settextcolor(int color) = 0;

    virtual void MTCT setfont(void *font) = 0;

    virtual void MTCT drawtext(const char *text, int length, MTRect &rect, int flags) = 0;

    virtual bool MTCT gettextsize(const char *text, int length, MTPoint *size, int maxwidth = -1) = 0;

    virtual int MTCT gettextextent(const char *text, int length, int maxextent) = 0;

    virtual int MTCT gettextheight() = 0;

    virtual int MTCT getcharwidth(char c) = 0;

    virtual void MTCT toscreen(MTPoint &p) = 0;

private:
    int cmox, cmoy;
    bool cmoving;
    void *frgn;
    int notifycount;
};

class MTLabel: public MTControl
{
public:
    char *caption;
    bool autosize;

    virtual void MTCT setcaption(const char *c) = 0;
};

class MTButton: public MTControl
{
public:
    char *caption;
    int imageindex;
    int modalresult;
    MTShortcut *shortcut;
    char hotkey;
    int hotkeyoffset;

    virtual void MTCT setcaption(const char *c) = 0;

    virtual void MTCT setimage(int index) = 0;

    virtual void MTCT setautosize(bool autosize) = 0;
};

class MTIcon: public MTControl
{
public:
    int icon;
};

class MTCheckBox: public MTControl
{
public:
    char *caption;
    bool radio;
    int state;

    virtual void MTCT setstate(int c, bool touched = false) = 0;
};

class MTEdit: public MTControl
{
public:
    char *text;
    int maxlength;
    bool password;

    virtual int MTCT getnumproperties(int id) = 0;

    virtual bool MTCT getpropertytype(int id, char **name, int &flags) = 0;

    virtual bool MTCT getproperty(int id, void *value) = 0;

    virtual bool MTCT setproperty(int id, void *value) = 0;

    virtual void MTCT draw(MTRect &rect) = 0;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual void MTCT settext(const char *t) = 0;

    virtual void MTCT setselstart(int ss) = 0;

    virtual void MTCT setselend(int se) = 0;

    virtual void MTCT setcursor(int c) = 0;

    virtual void MTCT undo() = 0;

protected:
    int lblank, rblank;
    bool md, focused, isnew;
    int cursor, offset;
    int selstart, selend;
    int mss, mse;
    char *oldtext;
    int oldselstart, oldselend;
};

class MTComboBox: public MTEdit
{
public:
    int dropcount;

    virtual void MTCT pulldown() = 0;

    virtual int MTCT getselected() = 0;

    virtual bool MTCT getiteminfo(int id, char **caption, int *imageindex, int *flags, bool *editable) = 0;

    virtual void MTCT setitem(int id) = 0;

    virtual int MTCT searchitem(const char *search, char **caption) = 0;

protected:
    friend class MTDesktop;

    MTList *mlb;
    bool modified;
};

class MTUserComboBox: public MTComboBox
{
public:
    virtual void MTCT setnumitems(int n) = 0;
};

class MTItemComboBox: public MTComboBox
{
public:
    virtual MTItem *MTCT additem(const char *caption, int image, int flags, bool editable, void *data) = 0;

    virtual void MTCT removeitem(MTItem *item) = 0;

    virtual void MTCT clearitems() = 0;

    virtual void MTCT beginupdate() = 0;

    virtual void MTCT endupdate() = 0;

    virtual void MTCT sort(int f) = 0;

    virtual MTItem *MTCT getitem(int id) = 0;

    virtual MTItem *MTCT getitemfromtag(int tag) = 0;
};

class MTProgress: public MTControl
{
public:
    int pos;
    int maxpos;
    int step;

    virtual void MTCT setposition(int p) = 0;

    virtual void MTCT stepit() = 0;
};

class MTSign: public MTControl
{
public:
    int sign;

    virtual void MTCT setsign(int s) = 0;
};

class MTCustomBehaviours
{
public:
    MTCustomControl *parent;
    int customid;

    MTCustomBehaviours(MTCustomControl *control)
    {
        parent = control;
        customid = 0;
    };

    virtual void MTCT ondestroy()
    {
    };

    virtual int MTCT onload(MTFile *, int, int)
    {
        return 0;
    };

    virtual int MTCT onsave(MTFile *, int)
    {
        return 0;
    };

    virtual void MTCT onsetbounds(int, int, int, int)
    {
    };

    virtual bool MTCT oncheckbounds(int &, int &, int &, int &)
    {
        return true;
    };

    virtual void MTCT onswitchflags(int, bool)
    {
    };

    virtual void MTCT ondraw(MTRect &)
    {
    };

    virtual bool MTCT onmessage(MTCMessage &)
    {
        return false;
    };
};

class MTCustomControl: public MTControl
{
public:
    MTCustomBehaviours *behaviours;

    virtual bool MTCT processmessage(MTCMessage &msg) = 0;
};

class MTCustomWinBehaviours
{
public:
    MTCustomWinControl *parent;
    int customid;

    MTCustomWinBehaviours(MTCustomWinControl *control)
    {
        parent = control;
        customid = 0;
    };

    virtual void MTCT ondestroy()
    {
    };

    virtual int MTCT onload(MTFile *, int, int)
    {
        return 0;
    };

    virtual int MTCT onsave(MTFile *, int)
    {
        return 0;
    };

    virtual void MTCT onsetbounds(int, int, int, int)
    {
    };

    virtual bool MTCT oncheckbounds(int &, int &, int &, int &)
    {
        return true;
    };

    virtual void MTCT onswitchflags(int, bool)
    {
    };

    virtual void MTCT ondraw(MTRect &)
    {
    };

    virtual bool MTCT onmessage(MTCMessage &)
    {
        return false;
    };

    virtual void *MTCT ongetoffsetrgn(int)
    {
        return 0;
    };

    virtual void MTCT onoffset(int, int)
    {
    };
};

class MTCustomWinControl: public MTWinControl
{
public:
    MTCustomWinBehaviours *behaviours;

    virtual bool MTCT processmessage(MTCMessage &msg) = 0;
};

typedef void (MTCT *MTCItemDraw)(MTItem *item, const MTRect &rect, MTBitmap *b);

typedef void (MTCT *MTCUserItemDraw)(MTUserList *list, int id, const MTRect &rect, MTBitmap *b);

typedef int (MTCT *MTCGetItemInfo)(MTUserList *list, int id, char **caption, int *imageindex, int *flags, bool *editable);

typedef void (MTCT *MTCItemMessage)(MTUserList *list, int id, MTCMessage &msg);

class MTList: public MTWinControl
{
public:
    MTControl *owner;
    int viewflags;
    int selected;
    int numitems;
    int itemheight;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual bool MTCT getiteminfo(int id, char **caption, int *imageindex, int *flags, bool *editable) = 0;

    virtual int MTCT searchitem(const char *search, char **caption) = 0;

    virtual void MTCT setitem(int id) = 0;
};

class MTUserList: public MTList
{
public:
    int over;
    MTCUserItemDraw userdrawproc;
    MTCGetItemInfo getiteminfoproc;
    MTCItemMessage itemmessageproc;

    virtual void MTCT setnumitems(int n) = 0;
};

class MTItem: public MTControl
{
public:
    int index;
    char *caption;
    int imageindex;
    int itemflags;
    void *data;
    bool editable;
    bool autosize;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual void MTCT setcaption(const char *c) = 0;
};

class MTItemView: public MTList
{
public:
    MTItem *selecteditem;
    MTCItemDraw userdrawproc;

    virtual bool MTCT message(MTCMessage &msg) = 0;

    virtual MTItem *MTCT additem(const char *caption, int image, int flags, bool editable, void *data) = 0;

    virtual void MTCT clearitems() = 0;

    virtual void MTCT beginupdate() = 0;

    virtual void MTCT endupdate() = 0;

    virtual void MTCT sort(int f) = 0;

    virtual MTItem *MTCT getitem(int id) = 0;

    virtual MTItem *MTCT getitemfromtag(int tag) = 0;

    virtual void MTCT removeitem(MTItem *item) = 0;

protected:
    bool updating;
};

class MTListItem: public MTItem
{
public:
    MTListItem(int tg, MTWinControl *p, int l, int t, int w, int h);

    void MTCT draw(MTRect &rect) = 0;
};

class MTListBox: public MTItemView
{
public:
};

class MTMenuItem: public MTItem
{
public:
    MTCommand command;
    MTMenu *submenu;
    MTShortcut *shortcut;
    char hotkey;
    int hotkeyoffset;
};

class MTMenu: public MTItemView
{
public:
    MTControl *caller;
    union
    {
        MTPoint mouse;
        MTRect area;
    };

    virtual void MTCT popup(MTControl *newcaller, MTPoint pos) = 0;

    virtual void MTCT popup(MTControl *newcaller, MTRect area) = 0;
};

class MTFileListBox: public MTUserList
{
public:
    char *path;

    virtual void MTCT setpath(const char *p) = 0;

    virtual void MTCT setfilter(int f) = 0;
};

class MTImageList
{
public:
    int iw, ih;

    virtual void MTCT setmetrics(void **m) = 0;

    virtual void MTCT drawimage(int id, MTBitmap *dest, int x, int y, int opacity = 255) = 0;
};

class MTSlider: public MTControl
{
public:
    int type, orientation;
    int minpos, maxpos, value;

    virtual void MTCT setminmax(int newmin, int newmax) = 0;

    virtual void MTCT setvalue(int newvalue) = 0;
};

class MTScroller: public MTControl
{
public:
    int type;
    int pos;
    int maxpos;
    int incr, page;
    float os;
    bool slide;

    virtual void MTCT setposition(int p) = 0;
};

class MTDesktop: public MTWinControl
{
public:
    void *mwnd;
};

class MTWindow: public MTWinControl
{
public:
    int style;
    char *caption;
    int imageindex;
    MTWrapper *wrapper;
    bool modified;
    MTRect br;
    int btnx, btny, btnw, btnh, btno, btnd;

    virtual void MTCT setstyle(int s) = 0;

    virtual void MTCT setcaption(const char *c) = 0;

    virtual void MTCT addhotkey(MTControl *ctrl, char hotkey) = 0;

    virtual void MTCT delhotkey(MTControl *ctrl) = 0;

    virtual void MTCT setminsize(int width, int height) = 0;

    virtual void MTCT setmaxsize(int width, int height) = 0;
};

class MTTabControl: public MTWinControl
{
public:
    int style, cstyle;
    MTWindow *page;
    int btnx, btny, btnw, btnh, btno, btnd;

    virtual void MTCT setpage(MTWindow *p) = 0;

    virtual void MTCT setpageid(int id) = 0;

    virtual int MTCT getpageid(MTWindow *p) = 0;

    virtual MTWindow *MTCT loadpage(MTResources *res, int id, bool autosave) = 0;

    virtual void MTCT setautohidetabs(bool autohide) = 0;
};

class MTPanel: public MTWinControl
{
public:
    int style;
};

class MTToolBar: public MTWinControl
{
public:
};

class MTOSWindow: public MTControl
{
public:
    virtual void MTCT setcompatible(bool compatible) = 0;

    virtual void *MTCT getoshandle() = 0;

    virtual bool MTCT patchoscode(void *lib) = 0;
};
//---------------------------------------------------------------------------
#endif
