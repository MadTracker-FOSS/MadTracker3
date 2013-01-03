//---------------------------------------------------------------------------
//
//  MadTracker Interface Manager
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTInterface.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#	ifdef _DEBUG
#		include <windows.h>
#		include <commdlg.h>
#	endif
#endif
#include "MTInterface.h"
#include "MTExtensions.h"
#include "MTData.h"
#include "MTExtensions.h"
#include "MTWC_Main.h"
#include "MTWC_Main2.h"
#include "../MTGUI/MTGUITools.h"
#include <MTXAPI/MTXSystem.h>
#include <MTXAPI/MTXInput.h>
#include <MTXAPI/MTXSystem2.h>
#include <MTXAPI/RES/MT3RES.h>
//---------------------------------------------------------------------------
void MTCT designmode(MTShortcut*,MTControl*,MTUndo*);
//---------------------------------------------------------------------------
void *wnd;
MTDesktop *mtdsk;
MTTabControl *mtdock;
MTWindow *mtmain,*mtmain2;
MTResources *mtres,*skinres;
Skin *skin;
MTImageList *sysimages;
MTThread *refreshthread;
MTEvent *refreshevent;
bool candesign = false;
bool canrefresh = false;
int dragx,dragy;
MTModule *cmodule;
MTBitmap *logo;
MTCustomWinControl *mtlogo;
MTSplashLogo *mtsplash;
int logotime = 5;
MTTimer *logotimer;
MTArray *refreshprocs;
#ifdef _DEBUG
	char *help;
	MTShortcut s_openres = {MTSF_CONTROL|MTSF_UICONTROL,'O',0,0,"Load a resources"};
	MTShortcut s_openskin = {MTSF_CONTROL|MTSF_UICONTROL,'K',0,0,"Load a skin"};
	MTShortcut s_playmod = {MTSF_CONTROL|MTSF_UICONTROL,'M',0,0,"Play a module"};
	MTShortcut s_reset = {MTSF_CONTROL|MTSF_UICONTROL,'R',0,0,"Reset"};
#endif
MTShortcut s_design = {MTSF_CONTROL|MTSF_ALT,'D',0,designmode,"Design mode"};
//---------------------------------------------------------------------------
void MTCT designmode(MTShortcut*,MTControl*,MTUndo*)
{
	bool d;

	if (gi){
		d = gi->getdesign();
		gi->setdesign(!d);
	};
}

int MTCT loadprocess(MTThread *thread,void *param)
{
	MTProcess *p = (MTProcess*)thread;

	FENTER2("loadprocess(%.8X,%.8X)",thread,param);
	if (!oi->loadobject((MTModule*)param,(char*)p->data,(void*)p)) thread->result = -1;
	LEAVE();
	return 0;
}

void MTCT loadprogress(MTProcess *process,void *param,float p)
{
	int x;
	bool locked = false;

	FENTER3("loadprogress(%.8X,%.8X,%f)",process,param,p);
	if (p==-2.0){
		LOGD("%s - [MT3] ERROR: An error occured while loading the module!"NL);
	}
	else if (p==-1.0){
		MTModule *m = (MTModule*)param;
		m->lock(MTOL_LOCK,false);
		if (process->result<0){
			MTTRY
				oi->deleteobject(m);
			MTCATCH
			MTEND
			si->memfree(process->data);
			LEAVE();
			return;
		};
		if (gi) setmodule(m);
		MTTRY
			if (output){
				output->lock->lock();
				locked = true;
			};
			for (x=0;x<16;x++){
				if ((module[x]) && (module[x]!=m)){
					MTModule *oldmodule = module[x];
					module[x] = 0;
					if (locked){
						output->lock->unlock();
						locked = false;
					};
					oi->deleteobject(oldmodule);
				};
			};
		MTCATCH
		MTEND
		if (locked) output->lock->unlock();
		mi->editobject(m,false);
		si->memfree(process->data);
	};
	LEAVE();
}

void loadmodule(const char *filename)
{
	int x;
	char *file;

	if (!oi) return;
	FENTER1("loadmodule(%s)",filename);
	MTTRY
		if (output) output->lock->lock();
		for (x=0;x<16;x++){
			if (module[x]==0) break;
		};
		module[x] = (MTModule*)oi->newobject(MTO_MODULE,0,0,0,true);
	MTCATCH
	MTEND
	if (output) output->lock->unlock();
	file = (char*)si->memalloc(strlen(filename)+1,0);
	strcpy(file,filename);
	si->processcreate(loadprocess,module[x],MTP_LOADMODULE,MTT_LOWER,file,loadprogress,false,"Load Module");
	LEAVE();
}

#ifdef _DEBUG
void MTCT openresources(MTShortcut *s,MTControl *c,MTUndo*)
{
#	ifdef __linux
		return;
#	else
		MTFile *f;
		MTResources *res;
		MTWindow *mtwnd;
		int x,type,uid,flags;
		OPENFILENAME open;
		char filename[512];
		
		if (!gi) return;
		FENTER2("openresources(%.8X,%.8X)",s,c);
		mtmemzero(&open,sizeof(open));
		open.lStructSize = sizeof(open);
		open.hwndOwner = (HWND)wnd;
		open.lpstrFilter = "MadTracker Resource (*.mtr)\0*.mtr\0\0";
		open.lpstrFile = (char*)si->memalloc(1024,MTM_ZERO);
		open.nMaxFile = 1024;
		open.Flags = OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		open.lpstrDefExt = ".mtr";
		open.lpstrInitialDir = prefs.syspath[SP_INTERFACE];
		if (GetOpenFileName(&open)){
			strcpy(filename,open.lpstrFile);
			flags = MTF_READ|MTF_SHAREREAD;
			if (candesign) flags |= MTF_WRITE;
			f = si->fileopen(filename,flags);
			if (f){
				res = si->resopen(f,false);
				if (res){
					x = res->getnumresources();
					while (--x>=0){
						res->getresourceinfo(x,&type,&uid,0);
						if (type==MTR_WINDOW){
							mtwnd = gi->loadwindow(res,uid,mtdsk);
							if (mtwnd){
								mtwnd->switchflags(MTCF_HIDDEN,false);
							};
						};
					};
					si->resclose(res);
				};
				si->fileclose(f);
			};
		};
		si->memfree(open.lpstrFile);
		LEAVE();
#	endif
}

void MTCT openskin(MTShortcut *s,MTControl *c,MTUndo*)
{
#	ifdef __linux
		return;
#	else
		MTFile *sf;
		MTResources *res;
		OPENFILENAME open;
		char filename[512];
		
		if (!gi) return;
		FENTER2("openskin(%.8X,%.8X)",s,c);
		mtmemzero(&open,sizeof(open));
		open.lStructSize = sizeof(open);
		open.hwndOwner = (HWND)wnd;
		open.lpstrFilter = "MadTracker Resource (*.mtr)\0*.mtr\0\0";
		open.lpstrFile = (char*)si->memalloc(1024,MTM_ZERO);
		open.nMaxFile = 1024;
		open.Flags = OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		open.lpstrDefExt = ".mtr";
		open.lpstrInitialDir = prefs.syspath[SP_SKINS];
		if (GetOpenFileName(&open)){
			strcpy(filename,open.lpstrFile);
			sf = si->fileopen(filename,MTF_READ|MTF_SHAREREAD);
			if (sf){
				res = si->resopen(sf,true);
				if (res){
					gi->loadskin(res);
					if (skinres) si->resclose(skinres);
					skinres = res;
				};
			};
		};
		si->memfree(open.lpstrFile);
		LEAVE();
#	endif
}

void MTCT playmodule(MTShortcut *s,MTControl *c,MTUndo*)
{
#	ifdef __linux
		return;
#	else
		OPENFILENAME open;
		
		if (!oi) return;
		FENTER2("playmodule(%.8X,%.8X)",s,c);
		mtmemzero(&open,sizeof(open));
		open.lStructSize = sizeof(open);
		open.hwndOwner = (HWND)wnd;
		open.lpstrFilter = "MadTracker 2 Module (*.mt2)\0*.mt2\0\0";
		open.lpstrFile = (char*)si->memalloc(1024,MTM_ZERO);
		open.nMaxFile = 1024;
		open.Flags = OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		open.lpstrDefExt = ".mt2";
//		open.lpstrInitialDir = prefs.syspath[SP_ROOT];
		if (GetOpenFileName(&open)){
			loadmodule(open.lpstrFile);
		};
		si->memfree(open.lpstrFile);
		LEAVE();
#	endif
}

void MTCT reset(MTShortcut*,MTControl*,MTUndo*)
{
	wantreset = true;
}

void MTCT showall(MTShortcut *s,MTControl *c,MTUndo*)
{
	int x;
	
	FENTER2("showall(%.8X,%.8X)",s,c);
	for (x=0;x<mtdsk->ncontrols;x++){
		MTControl &cctrl = *mtdsk->controls[x];
		if (cctrl.guiid==MTC_WINDOW) cctrl.switchflags(MTCF_HIDDEN,false);
	};
	LEAVE();
}

void MTCT setresolution(MTShortcut *s,MTControl *c,MTUndo*)
{
#	ifdef __linux
		return;
#	else
		MTMenuItem *item = (MTMenuItem*)c;
		static const int res[4][2] = {{640,480},{800,600},{1024,768},{1280,1024}};

		FENTER2("setresolution(%.8X,%.8X)",s,c);
		if (IsZoomed((HWND)wnd)) ShowWindow((HWND)wnd,SW_RESTORE);
		SetWindowPos((HWND)wnd,HWND_TOP,64,64,res[(int)item->data][0],res[(int)item->data][1],0);
		LEAVE();
#	endif
}
#endif

bool MTCT msgproc(MTWinControl *window,MTCMessage &msg)
{
#	ifdef _DEBUG
		if (msg.msg==MTCM_KEYDOWN){
			if (msg.repeat) return false;
			if (msg.buttons & DB_CONTROL){
				if ((msg.key>=KB_NUMPAD0) && (msg.key<=KB_NUMPAD9)){
					di->setdevice(msg.key-KB_NUMPAD0);
					return true;
				};
			}
			else if (msg.key==KB_F1){
				si->dialog(help,"Help",MTD_OK,MTD_INFORMATION|MTD_MODAL,30000);
				return true;
			};
		};
#	endif
	if ((msg.msg==MTCM_BOUNDS) && (msg.ctrl==mtdsk)){
		if (mtlogo){
			mtlogo->setbounds((window->width-mtlogo->width)/2,(window->height-mtlogo->height)/2,mtlogo->width,mtlogo->height);
		};
	};
	if (window==mtdsk){
		if ((mtlogo) && (mtsplash) && (msg.msg==MTCM_MOUSEMOVE)){
			int alpha;
			MTRect r = {mtlogo->left,mtlogo->top,mtlogo->left+mtlogo->width,mtlogo->top+mtlogo->height};
			alpha = (pointinrect(msg.p,r))?16:128;
			if (pointinrect(msg.p,r)){
				alpha = 16;
			}
			else{
				r.left -= mtlogo->width/2;
				r.top -= mtlogo->height/2;
				r.right += mtlogo->width/2;
				r.bottom += mtlogo->height/2;
				if (pointinrect(msg.p,r)){
					alpha = 64;
				};
			};
			if (mtlogo->tag!=alpha){
				mtsplash->setalpha(alpha,20,8);
			};
		};
	};
	return false;
}

int MTCT refreshsync(MTSync *sync)
{
	RefreshStruct *rs;
	static MTCMessage msg = {MTCM_REFRESH};

	FENTER1("refreshsync(%.8X)",sync);
	if (lastseq){
		lastseq->message(msg);
	};
	refreshprocs->reset();
	while ((rs = (RefreshStruct*)refreshprocs->next())){
		rs->proc(rs->param);
	};
	LEAVE();
	return 0;
}

MTSync rsync = {refreshsync};

int MTCT refreshproc(MTThread *thread,void *param)
{
	while ((!thread->terminated) && (canrefresh)){
		if (refreshevent->wait(500)){
			if (!canrefresh) break;
			gi->synchronize(&rsync);
		};
	};
	refreshthread = 0;
	return 0;
}

void setmodule(void* module)
{
	MTCMessage msg = {MTCM_CHANGE};
	MTButton *bstop,*bplaypause,*brecord;

	FENTER1("setmodule(%.8X)",module);
	cmodule = (MTModule*)module;
	msg.ctrl = w_main2->list1;
	mtmain2->message(msg);
	msg.ctrl = w_main2->list2;
	mtmain2->message(msg);
	bstop = (MTButton*)mtmain2->getcontrolfromuid(MTC_bstop);
	bplaypause = (MTButton*)mtmain2->getcontrolfromuid(MTC_bplaypause);
	brecord = (MTButton*)mtmain2->getcontrolfromuid(MTC_brecord);
	if (cmodule){
		bstop->switchflags(MTCF_DISABLED,false);
		bplaypause->switchflags(MTCF_DISABLED,false);
		brecord->switchflags(MTCF_DISABLED,false);
		bplaypause->setimage((cmodule->playstatus.flags)?22:21);
	}
	else{
		bstop->switchflags(MTCF_DISABLED,true);
		bplaypause->switchflags(MTCF_DISABLED,true);
		brecord->switchflags(MTCF_DISABLED,true);
		bplaypause->setimage(21);
	};
	LEAVE();
}

int MTCT logosync(MTSync*)
{
	mtsplash->setalpha(0,20,16);
	mtsplash = 0;
	return 0;
}

void MTCT logoproc(MTTimer *timer,int param)
{
	MTSync sync;

	sync.proc = logosync;
	gi->synchronize(&sync);
	si->timerdelete(logotimer);
	logotimer = 0;
}

MTSplashLogo::MTSplashLogo(MTCustomWinControl *control):
MTCustomWinBehaviours(control),
timer(0)
{
	control->flags |= MTCF_TRANSPARENT|MTCF_STAYONTOP|MTCF_GHOST;
	control->flags &= (~MTCF_BORDER);
	control->updateregions();
}

void MTSplashLogo::ondestroy()
{
	if (timer){
		gi->deltimer(parent,timer);
		timer = 0;
	};
	if (logo){
		di->delbitmap(logo);
		logo = 0;
	};
}

void MTSplashLogo::ondraw(MTRect &rect)
{
	MTBitmap *b;
	MTRect r = {0,0,parent->width,parent->height};
	int x = 0;
	int y = 0;

	if (&rect) r = rect;
	x = parent->left+(parent->width-logo->width)/2;
	y = parent->top+(parent->height-logo->height)/2;
	parent->parent->preparedraw(&b,x,y);
	r.left += x;
	r.top += y;
	r.right += x;
	r.bottom += y;
	b->clip(&r);
	if (parent->tag>=255) logo->blt(b,x,y,logo->width,logo->height,0,0);
	else logo->blendblt(b,x,y,logo->width,logo->height,0,0,parent->tag);
	b->unclip();
}

bool MTSplashLogo::onmessage(MTCMessage &msg)
{
	MTRect r;
	MTCMessage cmsg = {MTCM_CHANGE,0,mtlogo};

	parent->getrect(r,0);
	switch (msg.msg){
	case MTCM_TIMER:
		if (parent->tag<destalpha){
			parent->tag += increment;
			if (parent->tag>destalpha) parent->tag = destalpha;
		}
		else{
			parent->tag -= increment;
			if (parent->tag<destalpha) parent->tag = destalpha;
		};
		if (parent->tag==destalpha){
			if (timer){
				gi->deltimer(parent,timer);
				timer = 0;
			};
		};
		mtdsk->message(cmsg);
		mtdsk->flush(r);
		if (parent->tag==0){
			gi->delcontrol(mtlogo);
			mtlogo = 0;
		};
		return true;
	};
	return false;
}

void MTSplashLogo::setalpha(int alpha,int interval,int increment)
{
	if ((destalpha==alpha) || (destalpha==0)) return;
	destalpha = alpha;
	this->interval = interval;
	this->increment = increment;
	if (parent->tag==destalpha) return;
	if (!timer) timer = gi->ctrltimer(parent,0,interval,true,true);
}

bool initInterface()
{
	int x,y,size;
	MTRect cr;
	MTBitmap *screen;
	MTConfigFile *conf;
	
	ENTER("initInterface");
// Read the configuration
	refreshprocs = si->arraycreate(4,sizeof(RefreshStruct));

	if ((conf = (MTConfigFile*)mi->getconf("Global",true))){
		if (conf->setsection("MT3")){
			if (conf->getparameter("LogoTime",&logotime,MTCT_UINTEGER,sizeof(logotime))){
				logotime *= 1000;
			};
		};
		mi->releaseconf(conf);
	};
	if ((conf = (MTConfigFile*)mi->getconf("Global",false))){
		if (conf->setsection("MT3")){
			conf->getparameter("EnableDesign",&candesign,MTCT_BOOLEAN,sizeof(candesign));
		};
		mi->releaseconf(conf);
	};
	if (candesign){
		gi->setdesign(true);
		gi->setdesign(false);
	};
	dragx = gi->getmetric(MTGM_DRAGX);
	dragy = gi->getmetric(MTGM_DRAGY);
// Create the main window
	wnd = gi->createwindow(32,32,800,600,"MadTracker 3",MTW_CAPTION|MTW_RESIZABLE,0);
	if (!wnd){
		LOGD("%s - [MT3] ERROR: Cannot create the main window!"NL);
		LEAVE();
		return false;
	};
	gi->windowaction(wnd,MTWA_MAXIMIZE);
	di->setdevice(-1);
	screen = di->getscreen();
// Create the desktop
	if (di->fullscreen){
		cr.left = cr.top = 0;
		cr.right = screen->width;
		cr.bottom = screen->height;
	}
	else{
		gi->getwindowrect(wnd,cr,true);
	};
	mtdsk = (MTDesktop*)gi->newcontrol(MTC_DESKTOP,0,(MTWinControl*)wnd,0,0,cr.right-cr.left,cr.bottom-cr.top,0);
	mtdsk->messageproc = msgproc;
	gi->monitordesktop(mtdsk);
	mtdsk->draw(NORECT);
// Initialize the skin
	skin = gi->getskin();
	sysimages = (MTImageList*)gi->getimagelist(0);
	if (exitasap){
		LEAVE();
		return true;
	};
// Create the dock
	mtdock = (MTTabControl*)gi->newcontrol(MTC_TABCONTROL,0,mtdsk,0,160,mtdsk->width,mtdsk->height-160-200,0);
	mtdock->flags |= MTCF_DONTSAVE;
	mtdock->align = MTCA_CLIENT;
// Load the resources
	mtres = si->resfind("MT3.mtr",candesign);
	if (!mtres){
		LOGD("%s - [MT3] ERROR: Cannot find MT3.mtr!"NL);
		LEAVE();
		return false;
	};
// Create the splash screen
	logo = di->newresbitmap(MTB_SKIN,mtres,MTB_mt3logo,0xFF00FF);
	logo->param = &logo;
	cr.left = (cr.right-cr.left-logo->width)/2;
	cr.top = (cr.bottom-cr.top-logo->height)/2;
	mtlogo = (MTCustomWinControl*)gi->newcontrol(MTC_CUSTOMWINCTRL,0,mtdsk,cr.left,cr.top,logo->width,logo->height,0);
	mtlogo->flags |= MTCF_DONTSAVE;
	mtsplash = new MTSplashLogo(mtlogo);
	mtlogo->behaviours = mtsplash;
	mtlogo->switchflags(MTCF_HIDDEN,false);
	mtdsk->flags |= MTCF_DONTDRAW;
// Create the main windows
	mtmain = gi->loadwindow(mtres,MTW_main,mtdsk);
	if (mtmain){
		mtmain->setbounds(0,-mtmain->br.top,mtdsk->width,160+mtmain->br.top);
		mtmain->flags |= MTCF_STAYONBACK;
		mtmain->align = MTCA_TOP;
		w_main = new MTWC_main(mtmain);
	};
	mtmain2 = gi->loadwindow(mtres,MTW_main2,mtdsk);
	if (mtmain2){
		mtmain2->setbounds(0,mtdsk->height-200,mtdsk->width,200);
		mtmain2->flags |= MTCF_STAYONBACK;
		mtmain2->align = MTCA_BOTTOM;
		w_main2 = new MTWC_main2(mtmain2);
	};
	gi->loadwindow(mtres,MTW_file,mtdsk);

#	ifdef _DEBUG
		MTResources *res;
		char *e;
		int type,uid;

		res = si->resfind("Debug.mtr",candesign);
		if (res){
			x = res->getnumresources();
			while (--x>=0){
				res->getresourceinfo(x,&type,&uid,&size);
				switch (type){
				case MTR_WINDOW:
					gi->loadwindow(res,uid,mtdsk);
					break;
				case MTR_TEXT:
					help = (char*)si->memalloc(size+1,0);
					e = (char*)res->getresource(type,uid,&size);
					if (e) strcpy(help,e);
					res->releaseresource(e);
					break;
				};
			};
			si->resclose(res);
		};
		MTMenu &cmenu = *mtdsk->popup;
		MTMenuItem *m_openres,*m_openskin,*m_playmod,*m_reset;
		if (candesign) ((MTMenuItem*)cmenu.additem("Design Mode",-1,0,false,0))->command = designmode;
		m_openres = (MTMenuItem*)cmenu.additem("Load Resources...",-1,0,false,0);
		m_openskin = (MTMenuItem*)cmenu.additem("Load Skin...",-1,0,false,0);
		m_playmod = (MTMenuItem*)cmenu.additem("Play a Module...",-1,0,false,0);
		m_reset = (MTMenuItem*)cmenu.additem("Reset",-1,0,false,0);
		m_openres->command = openresources;
		m_openskin->command = openskin;
		m_playmod->command = playmodule;
		m_reset->command = reset;
		s_openres.control = m_openres;
		s_openskin.control = m_openskin;
		s_playmod.control = m_playmod;
		s_reset.control = m_reset;
		m_openres->shortcut = &s_openres;
		m_openskin->shortcut = &s_openskin;
		m_playmod->shortcut = &s_playmod;
		m_reset->shortcut = &s_reset;
		((MTMenuItem*)cmenu.additem("Show All Windows",-1,0,false,0))->command = showall;
		MTMenuItem &mmenu = *(MTMenuItem*)cmenu.additem("Resolution",-1,0,false,0);
		MTMenu &menu = *(MTMenu*)gi->newcontrol(MTC_MENU,0,mtdsk,0,0,0,0,0);
		menu.flags |= MTCF_DONTSAVE;
		mmenu.submenu = &menu;
		((MTMenuItem*)menu.additem("640x480",-1,0,false,0))->command = setresolution;
		((MTMenuItem*)menu.additem("800x600",-1,0,false,(void*)1))->command = setresolution;
		((MTMenuItem*)menu.additem("1024x768",-1,0,false,(void*)2))->command = setresolution;
		gi->registershortcut(&s_openres);
		gi->registershortcut(&s_openskin);
		gi->registershortcut(&s_playmod);
		gi->registershortcut(&s_reset);
#	endif
	if (candesign) gi->registershortcut(&s_design);
	logotimer = si->timercreate(logotime,100,false,0,logoproc);

	mtdsk->flags &= (~MTCF_DONTDRAW);
/*	mtdsk->flags &= (~MTCF_HIDDEN);
	mtdsk->draw(NORECT);*/
	MTCMessage msg = {MTCM_CHANGE,0,mtlogo};
	for (x=64;x<=128;x+=64){
		mtlogo->tag = x;
		mtdsk->message(msg);
		si->syswait(20);
	};

	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		for (y=0;y<cext.i->ninterfaces;y++){
			cext.i->interfaces[y]->config(0,(int)mtdsk);
		};
	};
	LEAVE();
	return true;
}

void uninitInterface()
{
	ENTER("uninitInterface");
#	ifdef _DEBUG
		si->memfree(help);
		help = 0;
#	endif
	if (logotimer){
		si->timerdelete(logotimer);
		logotimer = 0;
	};
	if (wnd){
		if (di) gi->deletewindow(wnd);
		wnd = 0;
	};
	if (mtdsk) mtdsk->flags |= (MTCF_DONTDRAW|MTCF_DONTFLUSH);
	if (mtdock){
		gi->delcontrol(mtdock);
		mtdock = 0;
	};
	if (mtmain){
		gi->delcontrol(mtmain);
		mtmain = 0;
	};
	if (mtmain2){
		gi->delcontrol(mtmain2);
		mtmain2 = 0;
	};
	if (w_main){
		delete w_main;
		w_main = 0;
	};
	if (w_main2){
		delete w_main2;
		w_main2 = 0;
	};
	if (mtdsk){
		mtdsk->delcontrols(true);
		gi->delcontrol(mtdsk);
		mtdsk = 0;
	};
	if (mtres){
		si->resclose(mtres);
		mtres = 0;
	};
	if (skinres){
		si->resclose(skinres);
		skinres = 0;
	};
	if (refreshprocs){
		si->arraydelete(refreshprocs);
		refreshprocs = 0;
	};
	LEAVE();
}

bool startInterface()
{
	ENTER("startInterface");
	refreshevent = si->eventcreate(true,20,0,true,true);
	canrefresh = true;
	refreshthread = si->threadcreate(refreshproc,true,true,0,MTT_LOWER,"Refresh");
	gi->start();
	LEAVE();
	return (refreshthread!=0);
}

void stopInterface()
{
	ENTER("stopInterface");
	canrefresh = false;
	if (refreshevent){
		refreshevent->wait(100);
		si->eventdelete(refreshevent);
		refreshevent = 0;
	};
	if (refreshthread){
		refreshthread->terminate();
		refreshthread = 0;
	};
	if (gi) gi->stop();
	LEAVE();
}

void showInterface()
{
	int x;

	ENTER("showInterface");
	mtdsk->flags |= MTCF_DONTDRAW|MTCF_DONTFLUSH;
	for (x=0;x<mtdsk->ncontrols;x++){
		MTControl &cctrl = *mtdsk->controls[x];
		if (cctrl.guiid==MTC_WINDOW) cctrl.switchflags(MTCF_HIDDEN,false);
	};
	mtdsk->flags &= (~(MTCF_DONTDRAW|MTCF_DONTFLUSH));
	mtdsk->draw(NORECT);
#	ifdef _WIN32
		if (outmsg){
			if (outmsg->seek(0,MTF_BEGIN)==0){
				int l = outmsg->length();
				if (l){
					char *msg;
					msg = (char*)si->memalloc(l+1,MTM_ZERO);
					outmsg->read(msg,l);
					si->dialog(msg,"MadTracker",MTD_OK,MTD_INFORMATION,0);
					si->memfree(msg);
				};
			};
			si->fileclose(outmsg);
			outmsg = 0;
			if (exitasap) PostQuitMessage(0);
		}
		else{
#	endif
#	ifdef _DEBUG
		char file[256];
		strcpy(file,prefs.syspath[SP_ROOT]);
		strcat(file,"Debug.mt2");
		loadmodule(file);
#	endif
#	ifdef _WIN32
		};
#	endif
	LEAVE();
}
//---------------------------------------------------------------------------
