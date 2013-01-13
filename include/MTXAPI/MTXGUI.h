//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXGUI.h 67 2005-08-26 22:18:51Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXGUI_INCLUDED
#define MTXGUI_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int guitype = FOURCC('X','G','U','I');
//---------------------------------------------------------------------------
enum{
	DB_LEFT = 1,
	DB_RIGHT = 2,
	DB_MIDDLE = 4,
	DB_DOUBLE = 8,
	DB_SHIFT = 32,
	DB_CONTROL = 64,
	DB_ALT = 128
};

enum{
	DCUR_DEFAULT = 0,
	DCUR_ARROW,
	DCUR_BACKGROUND,
	DCUR_WORKING,
	DCUR_TEXT,
	DCUR_HELP,
	DCUR_SIZE,
	DCUR_SIZE_NS,
	DCUR_SIZE_WE,
	DCUR_SIZE_NWSE,
	DCUR_SIZE_SW_NE,
	DCUR_NO,
	DCUR_CROSS,
	DCUR_DRAG,
	DCUR_POINT
};

#define MTW_CAPTION     1
#define MTW_RESIZABLE   2
#define MTW_TOPMOST     4
#define MTW_TRANSPARENT 16

enum{
	MTWA_HIDE = 0,
	MTWA_SHOW,
	MTWA_MINIMIZE,
	MTWA_MAXIMIZE,
	MTWA_CLOSE
};

enum{
	MTGM_DOUBLECLICK = 0,
	MTGM_DRAGX,
	MTGM_DRAGY,
	MTGM_GRIDX,
	MTGM_GRIDY
};

#define MTTF_SKINTIMER 0x80000000

#define MTSF_MOUSE     0x01
#define MTSF_JOYSTICK  0x02
#define MTSF_MIDI      0x04
#define MTSF_GROUP     0x08
#define MTSF_SHIFT     0x10
#define MTSF_CONTROL   0x20
#define MTSF_ALT       0x40
#define MTSF_UICONTROL 0x80
//---------------------------------------------------------------------------
struct MTUndo;
struct MTShortcut;
class MTControl;
typedef void (MTCT *MTCommand)(MTShortcut*,MTControl*,MTUndo*);
//---------------------------------------------------------------------------
#include "MTXGUITypes.h"
#include "MTXSystem.h"
#include "MTXDisplay.h"
#include "MTXControls.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------
struct MTUndo{
	bool redo;
	MTCommand command;
	char *description;
	int param1,param2,param3;
	void *param4;
};

struct MTShortcut{
	unsigned char flags;
	unsigned char key;
	unsigned short user;
	union{
		MTCommand command;
		MTControl *control;
		int group;
	};
	const char *description;
};

class MTGUIInterface : public MTXInterface{
public:
	bool active;
	bool visible;
	
	virtual int MTCT processmessages(bool userinput) = 0;
	virtual int MTCT getnumcontrols() = 0;
	virtual bool MTCT getcontroltype(int id,char *name,int &type) = 0;
	virtual MTControl* MTCT newcontrol(int type,int tag,MTWinControl *parent,int l,int t,int w,int h,void *param) = 0;
	virtual void MTCT delcontrol(MTControl *control) = 0;
	virtual MTWindow* MTCT loadwindow(MTResources *res,int id,MTDesktop *desktop,bool autosave = true) = 0;
	virtual bool MTCT savewindow(MTWindow *window) = 0;
	virtual MTWindow* MTCT loadwindowfromfile(MTFile *f,int size,MTDesktop *desktop) = 0;
	virtual int MTCT savewindowtofile(MTFile *f,MTWindow *window) = 0;
	virtual void MTCT loadskin(MTResources *res) = 0;
	virtual void* MTCT saveskin(int &size) = 0;
	virtual void MTCT setdisplay(MTBitmap *s) = 0;
	virtual void MTCT freedisplay() = 0;
	virtual bool MTCT getdesign() = 0;
	virtual void MTCT setdesign(bool d) = 0;
	virtual MTDesktop* MTCT getdesktop(int id) = 0;
	virtual int MTCT ctrltimer(MTControl *ctrl,int flags,int interval,bool immediate,bool accurate = false) = 0;
	virtual void MTCT deltimer(MTControl *ctrl,int timerid) = 0;
	virtual int MTCT synchronize(MTSync*) = 0;
	virtual void* MTCT getimagelist(int id) = 0;
	virtual Skin* MTCT getskin();
	virtual void* MTCT createwindow(int l,int t,int w,int h,const char *caption,int flags,void *parent) = 0;
	virtual void MTCT deletewindow(void *wnd) = 0;
	virtual void* MTCT getwindowproc() = 0;
	virtual void* MTCT getappwindow() = 0;
	virtual void MTCT monitordesktop(MTWinControl *dsk) = 0;
	virtual void MTCT invalidaterect(void *wnd,MTRect &r) = 0;
	virtual void MTCT invalidatergn(void *wnd,void *rgn) = 0;
	virtual void MTCT windowaction(void *wnd,int action) = 0;
	virtual void MTCT windowmove(void *wnd,int x,int y,bool relative) = 0;
	virtual void MTCT resetcursor() = 0;
	virtual float MTCT getcursorphase() = 0;
	virtual MTControl* MTCT getfocusedcontrol() = 0;
	virtual void MTCT setmouseshape(int cursor) = 0;
	virtual void MTCT restoremouseshape() = 0;
	virtual void MTCT getmousevector(float &x,float &y) = 0;
	virtual void MTCT getmousepos(MTPoint &mp) = 0;
	virtual void MTCT registershortcut(MTShortcut *s) = 0;
	virtual void MTCT unregistershortcut(MTShortcut *s) = 0;
	virtual int MTCT registershortcutgroup() = 0;
	virtual void MTCT setgrid(int gx,int gy) = 0;
	virtual void MTCT needbitmapcheck() = 0;
	virtual void MTCT savewindowstate(void *wnd) = 0;
	virtual void MTCT restorewindowstate(void *wnd) = 0;
	virtual void MTCT getwindowrect(void *wnd,MTRect &rect,bool client) = 0;
	virtual bool MTCT isdragged(MTPoint &p1,MTPoint &p2) = 0;
	virtual int MTCT getmetric(int metric) = 0;
	virtual void MTCT setcontrolname(MTControl *ctrl,const char *prefix) = 0;

/*	MTGUIInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual int MTCT processmessages(bool userinput);
	virtual int MTCT getnumcontrols();
	virtual bool MTCT getcontroltype(int id,char *name,int &type);
	virtual MTControl* MTCT newcontrol(int type,int tag,MTWinControl *parent,int l,int t,int w,int h,void *param);
	virtual void MTCT delcontrol(MTControl *control);
	virtual MTWindow* MTCT loadwindow(MTResources *res,int id,MTDesktop *desktop,bool autosave);
	virtual bool MTCT savewindow(MTWindow *window);
	virtual MTWindow* MTCT loadwindowfromfile(MTFile *f,int size,MTDesktop *desktop);
	virtual int MTCT savewindowtofile(MTFile *f,MTWindow *window);
	virtual void MTCT loadskin(MTResources *res);
	virtual void* MTCT saveskin(int &size);
	virtual void MTCT setdisplay(MTBitmap *s);
	virtual void MTCT freedisplay();
	virtual bool MTCT getdesign();
	virtual void MTCT setdesign(bool d);
	virtual MTDesktop* MTCT getdesktop(int id);
	virtual int MTCT ctrltimer(MTControl *ctrl,int flags,int interval,bool immediate,bool accurate = false);
	virtual void MTCT deltimer(MTControl *ctrl,int timerid);
	virtual int MTCT synchronize(MTSync*);
	virtual void* MTCT getimagelist(int id);
	virtual Skin* MTCT getskin();
	virtual void* MTCT createwindow(int l,int t,int w,int h,const char *caption,int flags,void *parent);
	virtual void MTCT deletewindow(void *wnd);
	virtual void* MTCT getwindowproc();
	virtual void* MTCT getappwindow();
	virtual void MTCT monitordesktop(MTDesktop *dsk);
	virtual void MTCT invalidaterect(void *wnd,MTRect &r);
	virtual void MTCT invalidatergn(void *wnd,void *rgn);
	virtual void MTCT windowaction(void *wnd,int action);
	virtual void MTCT windowmove(void *wnd,int x,int y,bool relative);
	virtual void MTCT resetcursor();
	virtual float MTCT getcursorphase();
	virtual MTControl* MTCT getfocusedcontrol();
	virtual void MTCT setmouseshape(int cursor);
	virtual void MTCT restoremouseshape();
	virtual void MTCT getmousevector(float &x,float &y);
	virtual void MTCT getmousepos(MTPoint &mp);
	virtual void MTCT registershortcut(MTShortcut *s);
	virtual void MTCT unregistershortcut(MTShortcut *s);
	virtual int MTCT registershortcutgroup();
	virtual void MTCT setgrid(int gx,int gy);
	virtual void MTCT needbitmapcheck();
	virtual void MTCT savewindowstate(void *wnd);
	virtual void MTCT restorewindowstate(void *wnd);
	virtual void MTCT getwindowrect(void *wnd,MTRect &rect,bool client);
	virtual bool MTCT isdragged(MTPoint &p1,MTPoint &p2);
	virtual int MTCT getmetric(int metric);
	virtual void MTCT setcontrolname(MTControl *ctrl,const char *prefix);
private:
	struct _MTTimer{
		int flags;
		MTControl *ctrl;
		int id;
		bool accurate;
	};
	bool isinit;
	bool running;
	int nsync;
	int timercount;
	MTEvent *runningevent;
	MTHash *timers;
	void sync(MTSync*);
	static VOID CALLBACK TimerProc(HWND wnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	static LRESULT CALLBACK WindowProc(HWND wnd,unsigned int msg,unsigned int wparam,long lparam);
	static void MTCT TimerProc2(MTTimer *timer,int param);
	static int MTCT TimerSync(MTSync *s);
	static void MTCT TimerDelete(void *item,void *param);
 * 
 * Need to merge. List in comments above.
 */
};
//---------------------------------------------------------------------------
#endif
