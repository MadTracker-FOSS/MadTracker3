//---------------------------------------------------------------------------
#ifndef MTGUI1_INCLUDED
#define MTGUI1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int guitype = FOURCC('X','G','U','I');
static const int guietype = FOURCC('G','U','I','E');

enum{
	DB_LEFT = 1,
	DB_RIGHT = 2,
	DB_MIDDLE = 4,
	DB_DOUBLE = 8,
	DB_SHIFT = 32,
	DB_CONTROL = 64,
	DB_ALT = 128
};

#define NICE_RATIO 2.5

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

struct MTUndo;
struct MTShortcut;
class MTControl;
class MTWinControl;
class MTWindow;
class MTDesktop;
//---------------------------------------------------------------------------
typedef void (MTCT *MTCommand)(MTShortcut*,MTControl*,MTUndo*);
//---------------------------------------------------------------------------
#include "MTGUITools.h"
#include "MTXSystem.h"
#include "MTXDisplay.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------
struct MTGUIPreferences{
	char skin[256];
	unsigned char cursorspeed;
	bool shadows;
	bool dialogshadows;
	char shadowx,shadowy;
	unsigned char shadowop,shadowblur;
	unsigned char scroll1,scroll2;
	bool fadeout;
	bool transpmenus;
	bool animctrl;
	int fadeouttime;
	int fadeoutinterval;
	int animtime;
	int animinterval;
};

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
	char *description;
};

struct MTSync;

typedef int (MTCT *SyncProc)(MTSync*);

struct MTSync{
	SyncProc proc;
	int result;
	int param[4];
};

class MTGUIInterface : public MTXInterface{
public:
	bool active;
	bool visible;

	MTGUIInterface();
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
	virtual void* MTCT createwindow(int l,int t,int w,int h,char *caption,int flags,void *parent);
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
};
//---------------------------------------------------------------------------
#ifdef _DEBUG
void tracergn(void *rgn,MTBitmap *bmp);
void tracerect(MTRect &r,MTBitmap *bmp);
#endif
//---------------------------------------------------------------------------
extern bool candesign;
extern bool design;
extern bool blockinput;
extern void *hinst;
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTDisplayInterface *di;
extern MTGUIInterface *gi;
extern MTGUIPreferences guiprefs;
extern MTBitmap *screen;
extern int doubleclick,dragx,dragy,gridx,gridy;
extern int guitick;
extern int lastwparam,lastlparam;
extern Skin *skin;
//---------------------------------------------------------------------------
#endif
