//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <math.h>
#include <windows.h>
#include "MTGUI1.h"
#include "MTSkin.h"
#include "MTControl.h"
#include "MTWinControl.h"
#include "MTCustControls.h"
#include "MTDesktop.h"
#include "MTWindow.h"
#include "MTTabControl.h"
#include "MTPanel.h"
#include "MTButton.h"
#include "MTEdit.h"
#include "MTLabel.h"
#include "MTSign.h"
#include "MTProgress.h"
#include "MTCheckBox.h"
#include "MTItems.h"
#include "MTVisual.h"
#include "MTOSWindow.h"
#include "MTToolBar.h"
#include "../Headers/MTXSystem2.h"

#define SPLITRGB(c,r,g,b) {r=c & 0xFF;g=(c>>8) & 0xFF;b=(c>>16) && 0xFF;}
//---------------------------------------------------------------------------
static const char *guiname = {"MadTracker GUI Subsystem"};
static const int guiversion = 0x30000;
static const MTXKey guikey = {0,0,0,0};
MTXInterfaces i;
MTGUIInterface *gi;
bool candesign = false;
bool design = false;
bool blockinput = false;
void *hinst;
MTInterface *mtinterface;
MTSystemInterface *si;
MTDisplayInterface *di;
MTGUIPreferences guiprefs = {"Default.mtr",4,false,false,4,4,192,32,30,50,true,true,true,500,20,300,20};
MTBitmap *screen = 0;
int ndesktops;
MTDesktop *desktops[32];
int cursorstart = 0;
bool quit = false;
bool bitmapcheck = false;
MSG msg;
HINSTANCE instance;
HWND mainwindow;
int lastclick,lastx,lasty,clastx,clasty;
int doubleclick,dragx,dragy,gridx = 4,gridy = 4;
int guitick = 0;
int cdelta;
int lastmx = -1;
int lastmy = -1;
float vx = 0.0;
float vy = 0.0;
WINDOWPLACEMENT wp;
MTHash *shortcuts;
MTArray *cursors;
static const char* wincurmap[16] = {IDC_ARROW,IDC_ARROW,IDC_APPSTARTING,IDC_WAIT,IDC_IBEAM,IDC_HELP,IDC_SIZEALL,IDC_SIZENS,IDC_SIZEWE,IDC_SIZENWSE,IDC_SIZENESW,IDC_NO,IDC_CROSS,(char*)DCUR_DRAG,(char*)DCUR_POINT};
int ksgroup = 0;
int lastwparam,lastlparam;
Skin *skin;
//---------------------------------------------------------------------------
inline int WinButtons(int wb)
{
	int b = 0;
	
	if (wb & MK_CONTROL) b |= DB_CONTROL;
	if (wb & MK_LBUTTON) b |= DB_LEFT;
	if (wb & MK_MBUTTON) b |= DB_MIDDLE;
	if (wb & MK_RBUTTON) b |= DB_RIGHT;
	if (wb & MK_SHIFT) b |= DB_SHIFT;
	return b;
}
//---------------------------------------------------------------------------
void gradient(PALETTEENTRY *cpal,int color1,int color2,int count)
{
	register int x,f1;
	register unsigned char r1,g1,b1;
	register unsigned char r2,g2,b2;
	unsigned char cgamma[128];
	
	SPLITRGB(color1,r1,g1,b1);
	SPLITRGB(color2,r2,g2,b2);
	for (x=0;x<count;x++)
		cgamma[x] = x*255/count;
	for (x=0;x<count;x++){
		f1 = cgamma[x];
		cpal->peRed = (r1*(255-f1)+r2*f1)/255;
		cpal->peGreen = (g1*(255-f1)+g2*f1)/255;
		cpal->peBlue = (b1*(255-f1)+b2*f1)/255;
		cpal++;
	};
}

int varcolor(int color1,int color2,int level)
{
	register unsigned char r1,g1,b1;
	register unsigned char r2,g2,b2;
	int r,g,b,v;
	
	SPLITRGB(color1,r1,g1,b1);
	SPLITRGB(color2,r2,g2,b2);
	r = (r1*(255-level)+r2*level)/255;
	g = (g1*(255-level)+g2*level)/255;
	b = (b1*(255-level)+b2*level)/255;
	v = r-255;
	if (v>0){
		g += v;
		b += v;
	};
	v = g-255;
	if (v>0){
		r += v;
		b += v;
	};
	v = b-255;
	if (v>0){
		r += v;
		g += v;
	};
	if (r>255) r = 255;
	if (g>255) g = 255;
	if (b>255) b = 255;
	return r|(g<<8)|(b<<16);
}
//---------------------------------------------------------------------------
#ifdef _DEBUG
void tracergn(void *rgn,MTBitmap *bmp)
{
	int x;
	MTRect r;
	
	for (x=rgngetnrects(rgn)-1;x>=0;x--){
		rgngetrect(rgn,x,&r);
		bmp->rectangle(r.left,r.top,r.right-r.left,r.bottom-r.top);
	};
}

void tracerect(MTRect &r,MTBitmap *bmp)
{
	bmp->rectangle(r.left,r.top,r.right-r.left,r.bottom-r.top);
}
#endif
//---------------------------------------------------------------------------
MTGUIInterface::MTGUIInterface():
active(true),
visible(true),
running(false),
nsync(0),
timercount(0),
runningevent(0),
timers(0)
{
	type = guitype;
	key = &guikey;
	name = guiname;
	version = guiversion;
	status = 0;
}

bool MTGUIInterface::init()
{
	MTConfigFile *conf;
	WNDCLASSEX wndclass;
	
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	if (!si) return false;
	ENTER("MTGUIInterface::init");
	quit = false;
	LOGD("%s - [GUI] Initializing..."NL);
	di = (MTDisplayInterface*)mtinterface->getinterface(displaytype);
	shortcuts = si->hashcreate(8);
	cursors = si->arraycreate(4,0);
	if ((conf = (MTConfigFile*)mtinterface->getconf("Global",true))){
		if (conf->setsection("MTGUI")){
			conf->getparameter("Skin",guiprefs.skin,MTCT_STRING,sizeof(guiprefs.skin));
			conf->getparameter("UseShadows",&guiprefs.shadows,MTCT_BOOLEAN,sizeof(guiprefs.shadows));
			conf->getparameter("DialogShadows",&guiprefs.dialogshadows,MTCT_BOOLEAN,sizeof(guiprefs.dialogshadows));
			conf->getparameter("ShadowOffsetX",&guiprefs.shadowx,MTCT_SINTEGER,sizeof(guiprefs.shadowx));
			conf->getparameter("ShadowOffsetY",&guiprefs.shadowy,MTCT_SINTEGER,sizeof(guiprefs.shadowy));
			conf->getparameter("ShadowBlur",&guiprefs.shadowblur,MTCT_UINTEGER,sizeof(guiprefs.shadowblur));
			conf->getparameter("ShadowOpacity",&guiprefs.shadowop,MTCT_UINTEGER,sizeof(guiprefs.shadowop));
			conf->getparameter("FadingWindows",&guiprefs.fadeout,MTCT_BOOLEAN,sizeof(guiprefs.fadeout));
			conf->getparameter("TransparentMenus",&guiprefs.transpmenus,MTCT_BOOLEAN,sizeof(guiprefs.transpmenus));
			conf->getparameter("AnimatedControls",&guiprefs.animctrl,MTCT_BOOLEAN,sizeof(guiprefs.animctrl));
			conf->getparameter("FadeoutTime",&guiprefs.fadeouttime,MTCT_UINTEGER,sizeof(guiprefs.fadeouttime));
			conf->getparameter("FadeoutInterval",&guiprefs.fadeoutinterval,MTCT_UINTEGER,sizeof(guiprefs.fadeoutinterval));
			conf->getparameter("AnimationTime",&guiprefs.animtime,MTCT_UINTEGER,sizeof(guiprefs.animtime));
			conf->getparameter("AnimationInterval",&guiprefs.animinterval,MTCT_UINTEGER,sizeof(guiprefs.animinterval));
		};
		mtinterface->releaseconf(conf);
	};
	if (!initSkin()){
		if (shortcuts){
			si->hashdelete(shortcuts);
			shortcuts = 0;
		};
		if (cursors){
			si->arraydelete(cursors);
			cursors = 0;
		};
		LEAVE();
		return false;
	};
	initOSWindow();
	doubleclick = GetDoubleClickTime();
	dragx = GetSystemMetrics(SM_CXDRAG);
	dragy = GetSystemMetrics(SM_CYDRAG);
	instance = (HINSTANCE)mtinterface->getinterface(0);
	mtmemzero(&wndclass,sizeof(wndclass));
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_OWNDC|CS_BYTEALIGNCLIENT;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.hInstance = instance;
	wndclass.hIcon = (HICON)LoadImage(instance,MAKEINTRESOURCE(1),IMAGE_ICON,0,0,LR_DEFAULTSIZE);
	wndclass.hIconSm = (HICON)LoadImage(instance,MAKEINTRESOURCE(1),IMAGE_ICON,16,16,0);
	wndclass.hCursor = (HCURSOR)LoadCursor(0,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndclass.lpszClassName = "MT3GUIWindow";
	RegisterClassEx(&wndclass);
	runningevent = si->eventcreate(false,0,0,true,false);
	timers = si->hashcreate(8);
	status |= MTX_INITIALIZED;
	resetcursor();
	LEAVE();
	return true;
}

void MTGUIInterface::uninit()
{
	MTConfigFile *conf;

	ENTER("MTGUIInterface::uninit");
	quit = true;
	LOGD("%s - [GUI] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	if (runningevent){
		si->eventdelete(runningevent);
		runningevent = 0;
	};
	if (timers){
		timers->clear(true,TimerDelete);
		si->hashdelete(timers);
		timers = 0;
	};
	if ((conf = (MTConfigFile*)mtinterface->getconf("Global",true))){
		if (conf->setsection("MTGUI")){
			conf->setparameter("Skin",guiprefs.skin,MTCT_STRING,sizeof(guiprefs.skin));
			conf->setparameter("UseShadows",&guiprefs.shadows,MTCT_BOOLEAN,sizeof(guiprefs.shadows));
			conf->setparameter("DialogShadows",&guiprefs.dialogshadows,MTCT_BOOLEAN,sizeof(guiprefs.dialogshadows));
			conf->setparameter("ShadowOffsetX",&guiprefs.shadowx,MTCT_SINTEGER,sizeof(guiprefs.shadowx));
			conf->setparameter("ShadowOffsetY",&guiprefs.shadowy,MTCT_SINTEGER,sizeof(guiprefs.shadowy));
			conf->setparameter("ShadowBlur",&guiprefs.shadowblur,MTCT_UINTEGER,sizeof(guiprefs.shadowblur));
			conf->setparameter("ShadowOpacity",&guiprefs.shadowop,MTCT_UINTEGER,sizeof(guiprefs.shadowop));
			conf->setparameter("FadingWindows",&guiprefs.fadeout,MTCT_BOOLEAN,sizeof(guiprefs.fadeout));
			conf->setparameter("TransparentMenus",&guiprefs.transpmenus,MTCT_BOOLEAN,sizeof(guiprefs.transpmenus));
			conf->setparameter("AnimatedControls",&guiprefs.animctrl,MTCT_BOOLEAN,sizeof(guiprefs.animctrl));
			conf->setparameter("FadeoutTime",&guiprefs.fadeouttime,MTCT_UINTEGER,sizeof(guiprefs.fadeouttime));
			conf->setparameter("FadeoutInterval",&guiprefs.fadeoutinterval,MTCT_UINTEGER,sizeof(guiprefs.fadeoutinterval));
			conf->setparameter("AnimationTime",&guiprefs.animtime,MTCT_UINTEGER,sizeof(guiprefs.animtime));
			conf->setparameter("AnimationInterval",&guiprefs.animinterval,MTCT_UINTEGER,sizeof(guiprefs.animinterval));
		};
		mtinterface->releaseconf(conf);
	};
	if (shortcuts){
		si->hashdelete(shortcuts);
		shortcuts = 0;
	};
	if (cursors){
		si->arraydelete(cursors);
		cursors = 0;
	};
	uninitOSWindow();
	uninitSkin();
	UnregisterClass("MT3GUIWindow",instance);
	active = true;
	visible = true;
/*
	for (x=0;x<16;x++){
		if (skin->skinbmp[x]){
			di->delbitmap(skin->skinbmp[x]);
			skin->skinbmp[x] = 0;
		};
	};
*/
	candesign = false;
	design = false;
	blockinput = false;
	overctrl = 0;
	LEAVE();
}

#ifdef _DEBUG
void MTCT menusetshadows(MTShortcut *s,MTControl *c,MTUndo*)
{
	int x;
	
	guiprefs.shadows = !guiprefs.shadows;
	skin->setshadows(guiprefs.shadows);
	for (x=0;x<ndesktops;x++) desktops[x]->draw(NORECT);
}

void MTCT menusettransp(MTShortcut *s,MTControl *c,MTUndo*)
{
	guiprefs.transpmenus = !guiprefs.transpmenus;
}

void MTCT menusetanim(MTShortcut *s,MTControl *c,MTUndo*)
{
	guiprefs.animctrl = !guiprefs.animctrl;
}
#endif

void MTGUIInterface::start()
{
#ifdef _DEBUG
	MTDesktop *dsk;
//	MTMenu *menu;
	dsk = gi->getdesktop(0);
	if (dsk){
//		menu = (MTMenu*)gi->newcontrol(MTC_MENU,0,dsk,0,0,0,0,0);
		dsk->popup->additem("|MTGUI",0,0,false,0);
		((MTMenuItem*)dsk->popup->additem("Toggle Shadows",-1,0,false,0))->command = menusetshadows;
		((MTMenuItem*)dsk->popup->additem("Transparent Menus",-1,0,false,0))->command = menusettransp;
		((MTMenuItem*)dsk->popup->additem("Animated Controls",-1,0,false,0))->command = menusetanim;
	};
#endif
	running = true;
	runningevent->set();
}

void MTGUIInterface::stop()
{
	int time;

	if (!running) return;
	runningevent->reset();
	time = si->syscounter();
	while (nsync>0){
		processmessages(false);
		if (si->syscounter()-time>10000) break;
	};
	running = false;
}

void MTGUIInterface::processcmdline(void *params)
{

}

void MTGUIInterface::showusage(void *out)
{

}

int MTGUIInterface::config(int command,int param)
{
	if (command<0) return (int)&guiprefs;
	return 0;
}

int MTGUIInterface::processmessages(bool userinput)
{
	int res;
	
	if (quit){
		if (nsync==0) return 0;
		if (!PeekMessage(&msg,0,0,0,PM_NOREMOVE)){
			si->syswait(10);
			return 1;
		};
	};
//	if (MsgWaitForMultipleObjects(0,0,false,125,QS_ALLEVENTS)==WAIT_TIMEOUT) return 1;
	res = GetMessage(&msg,0,0,0);
	if (res==0){
		quit = true;
		return 0;
	}
	else if (res<0) return -1;
	blockinput = !userinput;
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	blockinput = false;
	return 1;
}

static const char *ctrlname[] = {"MTCustomControl","MTCustomWincontrol","MTVisual",
	"MTScroller","MTSlider","MTOscillo",
	"MTLabel","MTButton","MTSign","MTCheckBox","MTEdit","MTUserComboBox","MTItemComboBox","MTProgress",
	"MTUserList","MTListBox","MTFileListBox","MTPanel","MTToolBar","MTTabControl","MTOSWindow"};
static const int ctrltype[] = {MTC_CUSTOMCTRL,MTC_CUSTOMWINCTRL,MTC_VISUAL,
	MTC_SCROLLER,MTC_SLIDER,MTC_OSCILLO,
	MTC_LABEL,MTC_BUTTON,MTC_SIGN,MTC_CHECKBOX,MTC_EDIT,MTC_USERCOMBOBOX,MTC_ITEMCOMBOBOX,MTC_PROGRESS,
	MTC_USERLIST,MTC_LISTBOX,MTC_FILELISTBOX,MTC_PANEL,MTC_TOOLBAR,MTC_TABCONTROL,MTC_OSWINDOW};

int MTGUIInterface::getnumcontrols()
{
	return sizeof(ctrltype)/sizeof(int);
}

bool MTGUIInterface::getcontroltype(int id,char *name,int &type)
{
	if (id>=(sizeof(ctrlname)/sizeof(int))) return false;
	strcpy(name,ctrlname[id]);
	type = ctrltype[id];
	return true;
}

MTControl *MTGUIInterface::newcontrol(int type,int tag,MTWinControl *parent,int l,int t,int w,int h,void *param)
{
	MTControl *res = 0;
	RECT r;

	FENTER3("MTGUIInterface::newcontrol(%d,%d,%.8X,...)",type,tag,parent);
	switch (type){
	case MTC_DESKTOP:
		if ((w|h)==0){
			GetClientRect((HWND)parent,&r);
			w = r.right-r.left;
			h = r.bottom-r.top;
		};
		res = new MTDesktop(tag,(HWND)parent,l,t,w,h);
		break;
	case MTC_WINDOW:
	case MTC_TABSHEET:
		res = new MTWindow(tag,parent,l,t,w,h,(int)param);
		break;
	case MTC_LISTBOX:
		res = new MTListBox(tag,parent,l,t,w,h);
		break;
	case MTC_MENU:
		res = new MTMenu(tag,parent,l,t,w,h);
		break;
	case MTC_MENUITEM:
		res = new MTMenuItem(tag,parent,l,t,w,h);
		break;
	case MTC_USERLIST:
		res = new MTUserList(tag,parent,l,t,w,h);
		break;
	case MTC_FILELISTBOX:
		res = new MTFileListBox(tag,parent,l,t,w,h);
		break;
	case MTC_LISTITEM:
		res = new MTListItem(tag,parent,l,t,w,h);
		break;
	case MTC_PANEL:
		res = new MTPanel(tag,parent,l,t,w,h);
		break;
	case MTC_TOOLBAR:
		res = new MTToolBar(tag,parent,l,t,w,h);
		break;
	case MTC_TABCONTROL:
		res = new MTTabControl(tag,parent,l,t,w,h);
		break;
	case MTC_SCROLLER:
		res = new MTScroller(tag,parent,l,t,w,h);
		break;
	case MTC_LABEL:
		res = new MTLabel(tag,parent,l,t,w,h);
		break;
	case MTC_BUTTON:
		res = new MTButton(tag,parent,l,t,w,h);
		break;
	case MTC_SIGN:
		res = new MTSign(tag,parent,l,t,w,h);
		break;
	case MTC_CHECKBOX:
		res = new MTCheckBox(tag,parent,l,t,w,h);
		break;
	case MTC_EDIT:
		res = new MTEdit(tag,parent,l,t,w,h);
		break;
	case MTC_USERCOMBOBOX:
		res = new MTUserComboBox(tag,parent,l,t,w,h);
		break;
	case MTC_ITEMCOMBOBOX:
		res = new MTItemComboBox(tag,parent,l,t,w,h);
		break;
	case MTC_PROGRESS:
		res = new MTProgress(tag,parent,l,t,w,h);
		break;
	case MTC_SLIDER:
		res = new MTSlider(tag,parent,l,t,(int)param);
		break;
	case MTC_OSCILLO:
		res = new MTOscillo(tag,parent,l,t,w,h,(Track*)param);
		break;
	case MTC_VISUAL:
		res = new MTVisual(tag,parent,l,t,w,h);
		break;
	case MTC_CUSTOMCTRL:
		res = new MTCustomControl(tag,parent,l,t,w,h);
		break;
	case MTC_CUSTOMWINCTRL:
		res = new MTCustomWinControl(tag,parent,l,t,w,h);
		break;
	case MTC_OSWINDOW:
		res = new MTOSWindow(tag,parent,l,t,w,h);
		break;
	default:
		FLOGD1("%s - [GUI] ERROR: Unknown object type %08X!"NL,type);
		break;
	};
	if (res){
		if (skin) skin->initcontrol(res);
		if ((parent) && (type!=MTC_DESKTOP)) parent->addcontrol(res);
	};
	LEAVE();
	return res;
}

void MTGUIInterface::delcontrol(MTControl *control)
{
	if (!control) return;
	FENTER1("MTGUIInterface::delcontrol(%.8X)",control);
 	if (control->parent) control->parent->delcontrol(control);
	if (skin) skin->uninitcontrol(control);
	switch (control->guiid){
	case MTC_DESKTOP:
		delete (MTDesktop*)control;
		break;
	case MTC_WINDOW:
	case MTC_TABSHEET:
		delete (MTWindow*)control;
		break;
	case MTC_LISTITEM:
		delete (MTListItem*)control;
		break;
	case MTC_MENUITEM:
		delete (MTMenuItem*)control;
		break;
	case MTC_LISTBOX:
		delete (MTListBox*)control;
		break;
	case MTC_FILELISTBOX:
		delete (MTFileListBox*)control;
		break;
	case MTC_MENU:
		delete (MTMenu*)control;
		break;
	case MTC_USERLIST:
		delete (MTUserList*)control;
		break;
	case MTC_PANEL:
		delete (MTPanel*)control;
		break;
	case MTC_TOOLBAR:
		delete (MTToolBar*)control;
		break;
	case MTC_TABCONTROL:
		delete (MTTabControl*)control;
		break;
	case MTC_SCROLLER:
		delete (MTScroller*)control;
		break;
	case MTC_LABEL:
		delete (MTLabel*)control;
		break;
	case MTC_BUTTON:
		delete (MTButton*)control;
		break;
	case MTC_SIGN:
		delete (MTSign*)control;
		break;
	case MTC_CHECKBOX:
		delete (MTCheckBox*)control;
		break;
	case MTC_EDIT:
		delete (MTEdit*)control;
		break;
	case MTC_USERCOMBOBOX:
		delete (MTUserComboBox*)control;
		break;
	case MTC_ITEMCOMBOBOX:
		delete (MTItemComboBox*)control;
		break;
	case MTC_PROGRESS:
		delete (MTProgress*)control;
		break;
	case MTC_SLIDER:
		delete (MTSlider*)control;
		break;
	case MTC_OSCILLO:
		delete (MTOscillo*)control;
		break;
	case MTC_VISUAL:
		delete (MTVisual*)control;
		break;
	case MTC_CUSTOMCTRL:
		delete (MTCustomControl*)control;
		break;
	case MTC_CUSTOMWINCTRL:
		delete (MTCustomWinControl*)control;
		break;
	case MTC_OSWINDOW:
		delete (MTOSWindow*)control;
		break;
	};
	LEAVE();
}

int syncloadwindow(MTSync *s)
{
	MTWindow *window;
	
	window = (MTWindow*)gi->newcontrol(MTC_WINDOW,0,(MTDesktop*)s->param[2],0,0,256,128,0);
	MTTRY
		window->loadfromstream((MTFile*)s->param[0],s->param[1],0);
	MTCATCH
		gi->delcontrol(window);
		window = 0;
	MTEND
	return (int)window;
}

MTWindow* MTGUIInterface::loadwindow(MTResources *res,int id,MTDesktop *desktop,bool autosave)
{
	MTFile *f;
	MTWindow *w;
	int size;

	f = res->getresourcefile(MTR_WINDOW,id,&size);
	if (!f) return 0;
	w = loadwindowfromfile(f,size,desktop);
	res->releaseresourcefile(f);
	if (autosave){
		w->res = res;
		w->resid = id;
	};
	return w;
}

bool MTGUIInterface::savewindow(MTWindow *window)
{
	MTFile *f;

	if (!window->res) return false;
	f = si->fileopen("mem://",MTF_READ|MTF_WRITE);
	if (!f) return false;
	window->savetostream(f,0);
	window->res->addfile(MTR_WINDOW,window->resid,f);
	si->fileclose(f);
	return true;
}

MTWindow* MTGUIInterface::loadwindowfromfile(MTFile *f,int size,MTDesktop *desktop)
{
	MTSync s;
	
	if (!desktop) desktop = (MTDesktop*)di->getdefaultdesktop();
	s.proc = syncloadwindow;
	s.param[0] = (int)f;
	s.param[1] = size;
	s.param[2] = (int)desktop;
	return (MTWindow*)synchronize(&s);
}

int MTGUIInterface::savewindowtofile(MTFile *f,MTWindow *window)
{
	return window->savetostream(f,0);
}

void MTGUIInterface::loadskin(MTResources *res)
{
	int x;
	MTCMessage msg = {MTCM_NOTIFY};
	
	FENTER1("MTGUIInterface::loadskin(%.8X)",res);
	for (x=0;x<ndesktops;x++){
		desktops[x]->flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
	};
	skin->loadfromres(res);
	for (x=0;x<ndesktops;x++){
		desktops[x]->flags &= (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
		desktops[x]->message(msg);
		desktops[x]->draw(NORECT);
	};
	LEAVE();
}

void *MTGUIInterface::saveskin(int &size)
{
// FIXME
//	return ::saveskin(size);
	return 0;
}

void MTGUIInterface::setdisplay(MTBitmap *s)
{
	screen = s;
// FIXME
//	startskin();
}

void MTGUIInterface::freedisplay()
{
// FIXME
//	stopskin();
	screen = 0;
}

bool MTGUIInterface::getdesign()
{
	return design;
}

void MTGUIInterface::setdesign(bool d)
{
	int x;
	
	candesign = true;
	design = d;
	for (x=0;x<ndesktops;x++) desktops[x]->draw(NORECT);
}

MTDesktop* MTGUIInterface::getdesktop(int id)
{
	return desktops[id];
}

int MTGUIInterface::ctrltimer(MTControl *ctrl,int flags,int interval,bool immediate,bool accurate)
{
	MTWinControl *parent;
	MTDesktop *dsk;
	_MTTimer *t;
	
	ctrl->timercount = 0;
	parent = ctrl->parent;
	if (!parent) parent = (MTWinControl*)ctrl;
	if ((parent->guiid & 2)==0) return 0;
	if (immediate){
		MTCMessage msg = {MTCM_TIMER,0,ctrl};
		ctrl->message(msg);
	};
	t = mtnew(_MTTimer);
	t->flags = flags;
	t->ctrl = ctrl;
	t->accurate = accurate;
	if (accurate){
		t->id = (int)si->timercreate(interval,0,true,(int)t,TimerProc2);
	}
	else{
		dsk = parent->dsk;
		if (!dsk) dsk = (MTDesktop*)parent;
		if (dsk->guiid!=MTC_DESKTOP){
			si->memfree(t);
			return 0;
		};
		t->id = SetTimer((HWND)dsk->mwnd,(int)ctrl,interval,TimerProc);
	};
	timers->additem(timercount,t);
	return timercount++;
}

void MTGUIInterface::deltimer(MTControl *ctrl,int timerid)
{
	_MTTimer *t;

	FENTER2("MTGUIInterface::deltimer(%.8X,%d)",ctrl,timerid);
	t = (_MTTimer*)timers->getitem(timerid);
	if (!t){
		LOGD("%s - [GUI] Warning: wrong timerid!"NL);
		LEAVE();
		return;
	};
	if (t->ctrl!=ctrl){
		LOGD("%s - [GUI] ERROR: Wrong timer/control association!"NL);
		LEAVE();
		return;
	};
	timers->delitem(timerid,true,TimerDelete);
	LEAVE();
}

int MTGUIInterface::synchronize(MTSync* param)
{
	if (si->issysthread()){
		nsync++;
		sync(param);
	}
	else{
		if (!running){
			if ((status & MTX_INITIALIZED)==0) return -1;
			if (!runningevent->wait(5000)) return -1;
		};
		nsync++;
		if (SendMessage((HWND)desktops[0]->mwnd,WM_USER,(int)param,0)!=1){
			nsync--;
			FLOGD1("%s - [GUI] ERROR: MTGUIInterface::synchronize(MTSync*) error %d"NL,GetLastError());
			return -1;
		};
	};
	return param->result;
}

void MTGUIInterface::sync(MTSync *param)
{
	nsync--;
	param->result = param->proc(param);
}

void* MTGUIInterface::getimagelist(int id)
{
	if (id==0) return &sysimages;
	return 0;
}

Skin* MTGUIInterface::getskin()
{
	return skin;
}

void* MTGUIInterface::createwindow(int l,int t,int w,int h,char *caption,int flags,void *parent)
{
	int stflags,exflags;
	HWND wnd;
	
	stflags = WS_SYSMENU|WS_TABSTOP;
	exflags = WS_EX_APPWINDOW;
	if (flags & MTW_TRANSPARENT){
		stflags |= WS_POPUP;
		exflags |= WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW;
	}
	else{
		if (flags & MTW_CAPTION){
			stflags |= WS_CAPTION;
			if (flags & MTW_RESIZABLE) stflags |= WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME;
		}
		else{
			stflags |= WS_POPUPWINDOW;
//			exflags |= WS_EX_TOOLWINDOW;
			if (flags & MTW_RESIZABLE) stflags |= WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME;
//			if (flags & MTW_RESIZABLE) stflags |= WS_THICKFRAME;
		};
	};
	if (flags & MTW_TOPMOST) exflags |= WS_EX_TOPMOST;
	wnd = CreateWindowEx(exflags,"MT3GUIWindow",caption,stflags,l,t,w,h,(HWND)parent,0,instance,0);
	if (mainwindow==0) mainwindow = wnd;
	return (void*)wnd;
}

void MTGUIInterface::deletewindow(void *window)
{
	if (mainwindow==(HWND)window) mainwindow = 0;
	DestroyWindow((HWND)window);
}

void* MTGUIInterface::getwindowproc()
{
	return WindowProc;
}

void* MTGUIInterface::getappwindow()
{
	return (void*)mainwindow;
}

void MTGUIInterface::monitordesktop(MTDesktop *dsk)
{
	SetWindowLong((HWND)dsk->mwnd,GWL_USERDATA,(int)dsk);
	SetWindowLong((HWND)dsk->mwnd,GWL_WNDPROC,(int)WindowProc);
	if ((GetWindowLong((HWND)dsk->mwnd,GWL_EXSTYLE) & WS_EX_TRANSPARENT)!=0){
		dsk->direct = true;
		dsk->flags |= MTCF_TRANSPARENT;
		if (dsk->mb){
			di->delbitmap(dsk->mb);
			dsk->mb = 0;
		};
	};
}

void MTGUIInterface::invalidaterect(void *wnd,MTRect &r)
{
	InvalidateRect((HWND)wnd,(RECT*)&r,false);
}

void MTGUIInterface::invalidatergn(void *wnd,void *rgn)
{
	InvalidateRgn((HWND)wnd,(HRGN)rgn,false);
}

void MTGUIInterface::windowaction(void *wnd,int action)
{
	int command;
	
	switch (action){
	case MTWA_HIDE:
		command = SW_HIDE;
		break;
	case MTWA_SHOW:
		command = SW_SHOWNORMAL;
		break;
	case MTWA_MINIMIZE:
		command = SW_MINIMIZE;
		break;
	case MTWA_MAXIMIZE:
		command = SW_MAXIMIZE;
		break;
	case MTWA_CLOSE:
		PostMessage((HWND)wnd,WM_CLOSE,0,0);
		return;
	default:
		return;
	};
	ShowWindow((HWND)wnd,command);
}

void MTGUIInterface::windowmove(void *wnd,int x,int y,bool relative)
{
	if (relative){
		RECT r;
		GetWindowRect((HWND)wnd,&r);
		x += r.left;
		y += r.top;
	};
	SetWindowPos((HWND)wnd,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

void MTGUIInterface::resetcursor()
{
	cursorstart = si->syscounter();
}

float MTGUIInterface::getcursorphase()
{
	static const double f2pi = 6.283185307179586476925286766559;

	return (cos(f2pi*(si->syscounter()-cursorstart))+1.0f)/2.0f;
}

MTControl* MTGUIInterface::getfocusedcontrol()
{
	return 0;
}

void MTGUIInterface::setmouseshape(int cursor)
{
	if (cursors) cursors->push((void*)cursor);
	SetCursor(LoadCursor(0,wincurmap[cursor]));
}

void MTGUIInterface::restoremouseshape()
{
	int cursor = DCUR_DEFAULT;

	if (cursors){
		cursors->pop();
		if (cursors->nitems>0) cursor = (int)cursors->a[cursors->nitems-1];
	};
	SetCursor(LoadCursor(0,wincurmap[cursor]));
}

void MTGUIInterface::getmousevector(float &x,float &y)
{
	x = vx;
	y = vy;
}

void MTGUIInterface::getmousepos(MTPoint &mp)
{
	GetCursorPos((POINT*)&mp);
}

void MTGUIInterface::registershortcut(MTShortcut *s)
{
	int keylookup;

	if (!shortcuts) return;
	if (s->group==0){
		s->group = ksgroup;
		s->flags |= MTSF_GROUP;
	};
	keylookup = s->flags & (~(MTSF_UICONTROL|MTSF_GROUP));
	keylookup |= (s->key<<8);
#	if (BIG_ENDIAN==1234)
		keylookup = swap_dword(keylookup);
#	endif
	shortcuts->additem(keylookup,s);
}

void MTGUIInterface::unregistershortcut(MTShortcut *s)
{
	int keylookup;

	if (!shortcuts) return;
	keylookup = s->flags & (~MTSF_UICONTROL);
	keylookup |= (s->key<<8);
#	if (BIG_ENDIAN==1234)
		keylookup = swap_dword(keylookup);
#	endif
	shortcuts->delitem(keylookup);
}

int MTGUIInterface::registershortcutgroup()
{
	return ++ksgroup;
}

void MTGUIInterface::setgrid(int gx,int gy)
{
	gridx = gx;
	gridy = gy;
}

void MTGUIInterface::needbitmapcheck()
{
	bitmapcheck = true;
}

void MTGUIInterface::savewindowstate(void *wnd)
{
	wp.length = sizeof(wp);
	GetWindowPlacement((HWND)wnd,&wp);
}

void MTGUIInterface::restorewindowstate(void *wnd)
{
	SetWindowPos((HWND)wnd,HWND_NOTOPMOST,wp.rcNormalPosition.left,wp.rcNormalPosition.top,wp.rcNormalPosition.right-wp.rcNormalPosition.left,wp.rcNormalPosition.bottom-wp.rcNormalPosition.top,0);
	ShowWindow((HWND)wnd,wp.showCmd);
}

void MTGUIInterface::getwindowrect(void *wnd,MTRect &rect,bool client)
{
	if (client) GetClientRect((HWND)wnd,(LPRECT)&rect);
	else GetWindowRect((HWND)wnd,(LPRECT)&rect);
}

bool MTGUIInterface::isdragged(MTPoint &p1,MTPoint &p2)
{
	if ((abs(p1.x-p2.x)>dragx) || (abs(p1.y-p2.y)>dragy)) return true;
	return false;	
}

int MTGUIInterface::getmetric(int metric)
{
	switch (metric){
	case MTGM_DOUBLECLICK:
		return doubleclick;
	case MTGM_DRAGX:
		return dragx;
	case MTGM_DRAGY:
		return dragy;
	case MTGM_GRIDX:
		return gridx;
	case MTGM_GRIDY:
		return gridy;
	default:
		return 0;
	};
}

void MTGUIInterface::setcontrolname(MTControl *ctrl,const char *prefix)
{
	if (!candesign) return;
	strcpy(ctrl->name,prefix);
}
//---------------------------------------------------------------------------
VOID CALLBACK MTGUIInterface::TimerProc(HWND wnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	if (idEvent<0x10000){
		return;
	};
	_MTTimer *t = (_MTTimer*)idEvent;
	MTCMessage cmsg = {MTCM_TIMER,0,t->ctrl,t->flags};
	t->ctrl->message(cmsg);
}

void MTGUIInterface::TimerProc2(MTTimer *timer,int param)
{
	MTSync s;

	if (((_MTTimer*)param)->ctrl==0) return;
	s.proc = TimerSync;
	s.result = 0;
	s.param[0] = (int)param;
	gi->synchronize(&s);
}

int MTGUIInterface::TimerSync(MTSync *s)
{
	_MTTimer *t = (_MTTimer*)s->param[0];
	if (gi->timers->getitemid(t)<0) return 0;
	if (!t->ctrl) return 0;
	MTCMessage cmsg = {MTCM_TIMER,0,t->ctrl,t->flags};
	try{
		t->ctrl->message(cmsg);
	}
	catch(...){
		LOGD("%s - [GUI] ERROR: Exception in control timer!"NL);
		TimerDelete((void*)t,0);
	};
	return 1;
}

void MTGUIInterface::TimerDelete(void *item,void *param)
{
	_MTTimer *t = (_MTTimer*)item;
	
	if (t->accurate){
		t->ctrl = 0;
		si->timerdelete((MTTimer*)t->id);
	}
	else{
		MTWinControl *parent;
		MTDesktop *dsk;
		parent = t->ctrl->parent;
		if (!parent) parent = (MTWinControl*)t->ctrl;
		if ((parent->guiid & 2)==0) return;
		t->ctrl = 0;
		dsk = parent->dsk;
		if (!dsk) dsk = (MTDesktop*)parent;
		if (dsk->guiid==MTC_DESKTOP){
			KillTimer((HWND)dsk->mwnd,t->id);
		};
	};
}

LRESULT CALLBACK MTGUIInterface::WindowProc(HWND wnd,unsigned int msg,unsigned int wparam,long lparam)
{
	MTDesktop *dsk = (MTDesktop*)GetWindowLong(wnd,GWL_USERDATA);
	int newclick,keylookup;
	float cx,cy,cd;
	MTShortcut *cs;
	MTRect r;
	MTCMessage cmsg;
	
	if ((!dsk) || (dsk->mwnd!=(void*)wnd)){
		if (msg==WM_CREATE){
			SendMessage(wnd,WM_SETICON,ICON_BIG,(LPARAM)LoadImage(instance,MAKEINTRESOURCE(1),IMAGE_ICON,0,0,LR_DEFAULTSIZE));
			SendMessage(wnd,WM_SETICON,ICON_SMALL,(LPARAM)LoadImage(instance,MAKEINTRESOURCE(1),IMAGE_ICON,16,16,0));
			return 0;		
		};
		return DefWindowProc(wnd,msg,wparam,lparam);
	};
	FENTER4("MTGUIInterface::WindowProc(%.8X,%.8X,%.8X,%.8X)",wnd,msg,wparam,lparam);
	mtmemzero(&cmsg,sizeof(cmsg));
	cmsg.ctrl = dsk;
	if (bitmapcheck){
		di->checkbitmaps();
		bitmapcheck = false;
	};
	lastwparam = wparam;
	lastlparam = lparam;
	switch (msg){
/*
	case WM_TIMER:
		if (wparam<0x10000){
			break;
		};
		cmsg.ctrl = (MTControl*)wparam;
		cmsg.msg = MTCM_TIMER;
		cmsg.ctrl->message(cmsg);
		LEAVE();
		return 0;
*/
	case WM_USER:
		gi->sync((MTSync*)wparam);
		LEAVE();
		return 1;
	case WM_PAINT:
		if (GetUpdateRect(wnd,(RECT*)&r,false)){
			dsk->flush(r);
			dsk->flushend();
			ValidateRect(wnd,(RECT*)&r);
			LEAVE();
			return 0;
		};
		break;
	case WM_ACTIVATEAPP:
		if (gi->active==(wparam!=0)){
			LEAVE();
			return 0;
		};
		gi->active = wparam!=0;
		FLOGD1("%s - [GUI] %s focus"NL,(gi->active)?"Gaining":"Loosing");
		di->setfocus(gi->active);
		if (gi->active){
			di->checkbitmaps();
			bitmapcheck = false;
		};
		if ((gi->active) && (di->fullscreen)){
			desktops[0]->draw(NORECT);
		};
		LEAVE();
		return 0;
	case WM_SIZE:
		screen->setmodified(wnd);
		if (wparam==SIZE_MINIMIZED){
			LOGD("%s - [GUI] Minimizing..."NL);
			gi->visible = false;
			dsk->switchflags(MTCF_HIDDEN,true);
		}
		else{
			if (gi->visible==false) gi->visible = true;
			r.right = lparam & 0xFFFF;
			r.bottom = lparam>>16;
//			if (r.right>screen->width) r.right = screen->width;
//			if (r.bottom>screen->height) r.bottom = screen->height;
			if (dsk->flags & MTCF_HIDDEN){
				LOGD("%s - [GUI] Restoring desktop..."NL);
				di->checkbitmaps();
				bitmapcheck = false;
				dsk->flags &= (~MTCF_HIDDEN);
/*
				dsk->flags |= MTCF_DONTFLUSH;
				dsk->switchflags(MTCF_HIDDEN,false);
				dsk->draw(NORECT);
				dsk->flags &= (~MTCF_DONTFLUSH);
*/
			};
			FLOGD2("%s - [GUI] Resizing desktop to %d x %d"NL,r.right,r.bottom);
			InvalidateRect(wnd,0,true);
			dsk->flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
			dsk->setbounds(0,0,r.right,r.bottom);
			dsk->flags &= (~MTCF_DONTDRAW);
			dsk->draw(NORECT);
			dsk->flags &= (~MTCF_DONTFLUSH);
			LEAVE();
			return 0;
		};
		break;
	case WM_MOVING:
	case WM_MOVE:
		cmsg.msg = MTCM_POSCHANGED;
		screen->setmodified(wnd);
		if (dsk->flags & MTCF_TRANSPARENT){
			dsk->draw(NORECT);
			LEAVE();
			return 1;
		};
		break;
	case WM_MOUSEMOVE:
		cmsg.msg = MTCM_MOUSEMOVE;
		cmsg.buttons = WinButtons(wparam);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		if (lastx<0){
			lastx = cmsg.x;
			lasty = cmsg.y;
			break;
		};
		cx = (float)(cmsg.x-lastx);
		cy = (float)(cmsg.y-lasty);
		cd = (float)sqrt(cx*cx+cy*cy);
		if (cd>4){
			cx /= cd;
			cy /= cd;
			if (cd>8){
				vx = (vx+cx*3)/4;
				vy = (vy+cy*3)/4;
			}
			else{
				vx = (vx+cx)/2;
				vy = (vy+cy)/2;
			};
			lastx = cmsg.x;
			lasty = cmsg.y;
		};
		break;
	case WM_LBUTTONDOWN:
		SetCapture(wnd);
		newclick = si->syscounter();
		cmsg.msg = MTCM_MOUSEDOWN;
		cmsg.button = DB_LEFT;
		cmsg.buttons = WinButtons(wparam);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		if ((newclick-lastclick<=doubleclick) && (abs(cmsg.x-clastx)<=dragx) && (abs(cmsg.y-clasty)<=dragy)){
			cmsg.buttons |= DB_DOUBLE;
			lastclick = 0;
		}
		else lastclick = newclick;
		clastx = cmsg.x;
		clasty = cmsg.y;
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		cmsg.msg = MTCM_MOUSEUP;
		cmsg.button = DB_LEFT;
		cmsg.buttons = WinButtons(wparam);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		break;
	case WM_RBUTTONDOWN:
		SetCapture(wnd);
		newclick = si->syscounter();
		cmsg.msg = MTCM_MOUSEDOWN;
		cmsg.button = DB_RIGHT;
		cmsg.buttons = WinButtons(wparam);
		if ((newclick-lastclick<=doubleclick) && (abs(cmsg.x-clastx)<=dragx) && (abs(cmsg.y-clasty)<=dragy)){
			cmsg.buttons |= DB_DOUBLE;
			lastclick = 0;
		}
		else lastclick = newclick;
		clastx = cmsg.x;
		clasty = cmsg.y;
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		cmsg.msg = MTCM_MOUSEUP;
		cmsg.button = DB_RIGHT;
		cmsg.buttons = WinButtons(wparam);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		break;
	case WM_MBUTTONDOWN:
		SetCapture(wnd);
		newclick = si->syscounter();
		cmsg.msg = MTCM_MOUSEDOWN;
		cmsg.button = DB_MIDDLE;
		cmsg.buttons = WinButtons(wparam);
		if ((newclick-lastclick<=doubleclick) && (abs(cmsg.x-clastx)<=dragx) && (abs(cmsg.y-clasty)<=dragy)){
			cmsg.buttons |= DB_DOUBLE;
			lastclick = 0;
		}
		else lastclick = newclick;
		clastx = cmsg.x;
		clasty = cmsg.y;
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		cmsg.msg = MTCM_MOUSEUP;
		cmsg.button = DB_MIDDLE;
		cmsg.buttons = WinButtons(wparam);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		break;
	case 0x020A:
		cdelta += (short)(wparam>>16);
		if ((cdelta<40) && (cdelta>-40)) break;
		cmsg.msg = MTCM_MOUSEWHEEL;
		cmsg.buttons = WinButtons(wparam & 0xFFFF);
		cmsg.x = (signed short)lparam;
		cmsg.y = lparam>>16;
		ScreenToClient(wnd,(POINT*)&cmsg.x);
		cmsg.param3 = cdelta/40;
		cdelta -= cmsg.param3*40;
		break;
	case WM_CHAR:
		cmsg.msg = MTCM_CHAR;
		if (GetKeyState(VK_SHIFT)<0) cmsg.buttons |= DB_SHIFT;
		if (GetKeyState(VK_CONTROL)<0) cmsg.buttons |= DB_CONTROL;
		cmsg.key = wparam;
		cmsg.scancode = (lparam>>16) & 0xFF;
		cmsg.repeat = lparam & 0xFFFF;
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		cmsg.msg = MTCM_KEYDOWN;
		keylookup = 0;
		if (GetKeyState(VK_SHIFT)<0){
			cmsg.buttons |= DB_SHIFT;
			keylookup |= MTSF_SHIFT;
		};
		if (GetKeyState(VK_CONTROL)<0){
			cmsg.buttons |= DB_CONTROL;
			keylookup |= MTSF_CONTROL;
		};
		if (GetKeyState(VK_MENU)<0){
			cmsg.buttons |= DB_ALT;
			keylookup |= MTSF_ALT;
		};
		cmsg.key = wparam;
		keylookup |= (wparam<<8);
		cmsg.scancode = (lparam>>16) & 0xFF;
		cmsg.repeat = lparam>>30;
#		if (BIG_ENDIAN==1234)
			keylookup = swap_dword(keylookup);
#		endif
		cs = (MTShortcut*)shortcuts->getitem(keylookup);
		if (cs){
			cmsg.s = cs;
			if (cs->group){
				FLOGD1("%s - [GUI] Shortcut: %s"NL,cs->description);
				if (cs->flags & MTSF_UICONTROL){
					MTCMessage smsg = {MTCM_SHORTCUT,0,cs->control};
					smsg.s = cs;
					cs->control->message(smsg);
				}
				else if ((cs->flags & MTSF_GROUP)==0){
					cs->command(cs,0,0);
				};
			};
		};
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		cmsg.msg = MTCM_KEYUP;
		if (GetKeyState(VK_SHIFT)<0) cmsg.buttons |= DB_SHIFT;
		if (GetKeyState(VK_CONTROL)<0) cmsg.buttons |= DB_CONTROL;
		if (GetKeyState(VK_MENU)<0) cmsg.buttons |= DB_ALT;
		cmsg.key = wparam;
		cmsg.scancode = (lparam>>16) & 0xFF;
		break;
	case WM_CLOSE:
		if (mtinterface->canclose()) PostQuitMessage(0);
		LEAVE();
		return 0;
	};
	if (cmsg.msg){
		dsk->flushstart();
		bool ok = dsk->message(cmsg);
		dsk->flushend();
		guitick++;
		if (ok){
			LEAVE();
			return 0;
		};
	};
	LEAVE();
	return DefWindowProc(wnd,msg,wparam,lparam);
}
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!gi) gi = new MTGUIInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)gi;
	return &i;
}

}
//---------------------------------------------------------------------------
