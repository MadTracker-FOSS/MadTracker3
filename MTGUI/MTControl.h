//---------------------------------------------------------------------------
#ifndef MTCONTROL_INCLUDED
#define MTCONTROL_INCLUDED

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
#define MTCM_SHORTCUT    0x1100
#define MTCM_HOTKEY      0x1101
#define MTCM_TIMER       0x1080
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

enum{
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

enum{
	MTP_INT = 0,
	MTP_BOOL,
	MTP_TEXT,
	MTP_FLAGS,
	MTP_LIST,
	MTP_ACTION,
	MTP_ITEMS
};

#define NORECT *(MTRect*)0

#define ControlNP 9
//---------------------------------------------------------------------------
struct MTCMessage;
class MTControl;
class MTWinControl;
class MTWindow;
class MTMenu;
//---------------------------------------------------------------------------
#include "MTGUI1.h"
#include "MTGUITools.h"
#include "../Headers/MTXExtension.h"
#include "../Headers/MTXSystem.h"
#include "../Headers/MTXDisplay.h"
//---------------------------------------------------------------------------
struct MTCMessage{
	int msg;
	int result;
	MTControl *ctrl;
	union{
		struct{
			union{
				int x;
				int key;
				int param1;
			};
			union{
				int y;
				int scancode;
				int param2;
			};
			union{
				int w;
				int button;
				int repeat;
				int param3;
			};
			union{
				int h;
				int buttons;
				void* param4;
			};
		};
		MTRect dr;
		MTPoint p;
	};
	MTShortcut *s;
};

class MTControl{
public:
	MTWinControl *parent;
	MTWindow *window;
	int guiid;
	int uid;
	char *name;
	int tag;
	int flags;
	int left,top;
	int width,height;
	int align;
	bool direct;
	int timercount;
	MTMenu *popup;
	bool autopopup;
	void *skindata;
	
	MTControl(int id,int tg,MTWinControl *p,int l,int t,int w,int h);
	virtual ~MTControl();
	virtual int MTCT loadfromstream(MTFile *f,int size,int flags);
	virtual int MTCT savetostream(MTFile *f,int flags);
	virtual int MTCT getnumproperties(int id);
	virtual bool MTCT getpropertytype(int id,char **name,int &flags);
	virtual bool MTCT getproperty(int id,void *value);
	virtual bool MTCT setproperty(int id,void *value);
	virtual void MTCT setbounds(int l,int t,int w,int h);
	virtual bool MTCT checkbounds(int &l,int &t,int &w,int &h);
	virtual void MTCT getrect(MTRect &r,int client);
	virtual void MTCT switchflags(int f,bool set);
	virtual void MTCT draw(MTRect &rect);
	virtual bool MTCT message(MTCMessage &msg);
	virtual void MTCT preparedraw(MTBitmap **b,int &ox,int &oy);
	virtual void MTCT setparent(MTWinControl *newparent);
	bool MTCT designmessage(MTCMessage &msg);
protected:
	friend class MTWinControl;
	int cborder;
	int mox,moy,mow,moh;
	bool moving,sizing,triggered;
};
//---------------------------------------------------------------------------
#include "MTWinControl.h"
//---------------------------------------------------------------------------
extern MTControl *overctrl,*btnctrl;
extern MTCMessage leavemsg,entermsg,btnupmsg;
//---------------------------------------------------------------------------
#endif
