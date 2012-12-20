//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: x86
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSystem1.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSYSTEM1_INCLUDED
#define MTSYSTEM1_INCLUDED
//---------------------------------------------------------------------------
#include "../MT3Config.h"
//---------------------------------------------------------------------------
#define MTS_WINNT    0x00001
#define MTS_MMX      0x00002
#define MTS_SSE      0x00004
#define MTS_SSE2     0x00008
#define MTS_SSE3     0x00010
#define MTS_CX8      0x00100
#define MTS_CMOV     0x00200
#define MTS_HT       0x01000
#define MTS_DEBUGGED 0x10000

#define MTM_ZERO     1
#define MTM_PHYSICAL 2

#define MTD_OK          (char*)0
#define MTD_OKCANCEL    (char*)1
#define MTD_YESNO       (char*)2
#define MTD_YESNOCANCEL (char*)3

#define MTD_INFORMATION 1
#define MTD_QUESTION    2
#define MTD_EXCLAMATION 3
#define MTD_ERROR       4
#define MTD_BUTTON1     0x00
#define MTD_BUTTON2     0x10
#define MTD_BUTTON3     0x20
#define MTD_BUTTON4     0x30
#define MTD_BUTTON5     0x40
#define MTD_MODAL       256

#define MTD_ICONMASK    15
#define MTD_BUTTONMASK  240

#define MTDR_NULL       -1
#define MTDR_CANCEL     -2
#define MTDR_TIMEOUT    -4

#if defined (_DEBUG) && defined (MTSYSTEM_EXPORTS)
#	define LOG(T)												si->log(T,0)
#	define LOGD(T)												si->log(T,1)
#	define FLOG1(T,P1)										si->flog(T,0,P1)
#	define FLOG2(T,P1,P2)								si->flog(T,0,P1,P2)
#	define FLOG3(T,P1,P2,P3)							si->flog(T,0,P1,P2,P3)
#	define FLOG4(T,P1,P2,P3,P4)					si->flog(T,0,P1,P2,P3,P4)
#	define FLOG5(T,P1,P2,P3,P4,P5)				si->flog(T,0,P1,P2,P3,P4,P5)
#	define FLOGD1(T,P1)									si->flog(T,1,P1)
#	define FLOGD2(T,P1,P2)								si->flog(T,1,P1,P2)
#	define FLOGD3(T,P1,P2,P3)						si->flog(T,1,P1,P2,P3)
#	define FLOGD4(T,P1,P2,P3,P4)					si->flog(T,1,P1,P2,P3,P4)
#	define FLOGD5(T,P1,P2,P3,P4,P5)			si->flog(T,1,P1,P2,P3,P4,P5)
#	define DUMP(V,L,O)										si->dump((unsigned char*)V,L,O)
#	define ENTER(F)											si->enter(F)
#	define FENTER1(F,P1)									si->fenter(F,P1)
#	define FENTER2(F,P1,P2)							si->fenter(F,P1,P2)
#	define FENTER3(F,P1,P2,P3)						si->fenter(F,P1,P2,P3)
#	define FENTER4(F,P1,P2,P3,P4)				si->fenter(F,P1,P2,P3,P4)
#	define FENTER5(F,P1,P2,P3,P4,P5)			si->fenter(F,P1,P2,P3,P4,P5)
#	define FENTER6(F,P1,P2,P3,P4,P5,P6)	si->fenter(F,P1,P2,P3,P4,P5,P6)
#	define LEAVE()												si->leave()
#	define CALLSTACK											si->getcallstack()
#else
#	define LOG(T)
#	define LOGD(T)
#	define FLOG1(T,P1)
#	define FLOG2(T,P1,P2)
#	define FLOG3(T,P1,P2,P3)
#	define FLOG4(T,P1,P2,P3,P4)
#	define FLOGD1(T,P1)
#	define FLOGD2(T,P1,P2)
#	define FLOGD3(T,P1,P2,P3)
#	define FLOGD4(T,P1,P2,P3,P4)
#	define DUMP(V,L,O)
#	define ENTER(F)
#	define FENTER1(F,P1)
#	define FENTER2(F,P1,P2)
#	define FENTER3(F,P1,P2,P3)
#	define FENTER4(F,P1,P2,P3,P4)
#	define FENTER5(F,P1,P2,P3,P4,P5)
#	define LEAVE()
#	define CALLSTACK
#endif
#define A(_A,_T) ((_T**)_A->a)
#define D(_A,_T) ((_T*)_A->d)

//---------------------------------------------------------------------------
class MTConfigFile;
class MTFileHook;
class MTFile;
class MTFolder;
class MTLock;
class MTEvent;
class MTThread;
class MTProcess;
class MTCPUMonitor;
class MTResources;
class MTArray;
class MTHash;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXGUI.h"
#include "MTXDisplay.h"
#ifdef MTSYSTEM_INTERNET
#	include "MTSocket.h"
#endif
#ifdef MTSYSTEM_CONFIG
#	include "MTConfig.h"
#endif
#ifdef MTSYSTEM_MINICONFIG
#	include "MTMiniConfig.h"
#endif
#include "MTFile.h"
#include "MTKernel.h"
#include "MTResources.h"
#include "MTStructures.h"
#include "MTXSystem.h"
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#else
#	include <setjmp.h>
#endif
//---------------------------------------------------------------------------
#ifdef MTSYSTEM_EXPORTS
/*--SDK--*/
class MTSystemInterface : public MTXInterface{
public:
	int sysflags;
	char *platform;
	char *build;
	char *processor;
	char *hostname;
	int ncpu;
	int cpufrequ;
	void *onerror;

	MTSystemInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual int MTCT getlasterror();
	virtual void MTCT setlasterror(int error);
	virtual void MTCT addfilehook(char *type,MTFileHook *hook);
	virtual void MTCT delfilehook(char *type,MTFileHook *hook);

	void* (MTCT *memalloc)(int size,int flags = 0);
	bool (MTCT *memfree)(void *mem);
	void* (MTCT *memrealloc)(void *mem,int size);
	MTThread* (MTCT *getsysthread)();
	MTThread* (MTCT *getcurrentthread)();
	bool (MTCT *issysthread)();
	bool (MTCT *setprivatedata)(int id,void *data);
	void* (MTCT *getprivatedata)(int id);
	int (MTCT *localalloc)();
	bool (MTCT *localfree)(int id);
	bool (MTCT *localset)(int id,void *value);
	void* (MTCT localget)(int id);
	MTThread* (MTCT *threadcreate)(ThreadProc proc,bool autofree,bool autostart,void *param,int priority = MTT_NORMAL,char *name = 0);
	MTProcess* (MTCT *processcreate)(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name = 0);
	MTFile* (MTCT *fileopen)(char *url,int flags);
	void (MTCT *fileclose)(MTFile *file);
	bool (MTCT *fileexists)(char *filename);
	bool (MTCT *filecopy)(char *filename,char *destination);
	bool (MTCT *filedelete)(char *filename);
	bool (MTCT *filerename)(char *filename,char *newname);
	void (MTCT *filetype)(const char *filename,char *filetype,int length);
	void (MTCT *filemaketemp)(char *filename,int length);
	MTFolder* (MTCT *folderopen)(char *path);
	void (MTCT *folderclose)(MTFolder *folder);
	MTArray* (MTCT *arraycreate)(int nallocby,int itemsize = 0);
	void (MTCT *arraydelete)(MTArray *array);
	MTHash* (MTCT *hashcreate)(int nallocby);
	void (MTCT *hashdelete)(MTHash *array);
	MTResources* (MTCT *resfind)(const char *filename,bool write = false);
	MTResources* (MTCT *resopen)(MTFile *f,bool ownfile);
	void (MTCT *resclose)(MTResources *res);
#ifdef MTSYSTEM_CONFIG
	MTConfigFile* (MTCT *configfind)(const char *filename);
	MTConfigFile* (MTCT *configopen)(const char *filename);
	void (MTCT* configclose)(MTConfigFile* file);
#else
	void* configfind;
	void* configopen;
	void* configclose;
#endif
#ifdef MTSYSTEM_MINICONFIG
	MTMiniConfig* (MTCT *miniconfigcreate)();
	void (MTCT *miniconfigdelete)(MTMiniConfig *cfg);
#else
	void* miniconfigcreate;
	void* miniconfigdelete;
#endif
	MTLock* (MTCT *lockcreate)();
	void (MTCT *lockdelete)(MTLock *lock);
	MTEvent* (MTCT *eventcreate)(bool autoreset,int interval = 0,int resolution = 0,bool periodic = true,bool pulse = false);
	void (MTCT *eventdelete)(MTEvent *event);
	MTTimer* (MTCT *timercreate)(int interval,int resolution,bool periodic,int param,TimerProc proc);
	MTTimer*(MTCT *timerevent)(int interval,int resolution,bool periodic,MTEvent *event,bool pulse = false);
	void (MTCT *timerdelete)(MTTimer *timer);
	MTCPUMonitor *(MTCT *cpumonitorcreate)(int ncounters);
	int (MTCT *syscounter)();
	bool (MTCT *syscounterex)(double *count);
	void (MTCT *syswait)(int ms);
	int (MTCT *syswaitmultiple)(int count,MTEvent **events,bool all,int timeout = -1);
	int (MTCT *dialog)(char *message,char *caption,char *buttons,int flags,int timeout);
#ifdef MTSYSTEM_RESOURCES
	int (MTCT *resdialog)(MTResources *res,int id,char *caption,char *buttons,int timeout,int flags,...);
	int (MTCT *authdialog)(char *message,char *login,char *password);
#else
	void* resdialog;
	void* authdialog;
#endif
	void (MTCT *showoserror)(int error);
	void (MTCT *showlastoserror)();
	void (MTCT *log)(const char *log,char date);
	void (MTCT *flog)(const char *log,char date,...);
	void (MTCT *dump)(unsigned char *address,int length,int offset = 0);
	void (MTCT *enter)(const char *func);
	void (MTCT *fenter)(const char *func,...);
	void (MTCT *leave)();
	const char* (MTCT *getcallstack)();
	void (MTCT *getlibmemoryrange)(void *lib,int flags,void **start,int *length);
	int (MTCT *sync_inc)(int *value);
	int (MTCT *sync_dec)(int *value);
};
//---------------------------------------------------------------------------
#endif

extern "C"
{
void MTCT mtlog(const char *log,char date = 0);
void MTCT mtflog(const char *log,char date = 0,...);
void MTCT mtdump(unsigned char *address,int length,int offset = 0);
void MTCT mtenter(const char *func);
void MTCT mtfenter(const char *func,...);
void MTCT mtleave();
const char* MTCT mtgetcallstack();
void mtgetlibmemoryrange(void *lib,int flags,void **start,int *length);
void* MTCT mtmemalloc(int size,int flags = 0);
bool MTCT mtmemfree(void *mem);
void* MTCT mtmemrealloc(void *mem,int size);
int MTCT mtdialog(char *message,char *caption,char *buttons,int flags,int timeout);
#ifdef MTSYSTEM_RESOURCES
int MTCT mtresdialog(MTResources *res,int id,char *caption,char *buttons,int timeout,int flags,...);
int MTCT mtauthdialog(char *message,char *login,char *password);
#endif
void MTCT mtshowoserror(int error);
void MTCT mtshowlastoserror();
int MTCT mtsync_inc(int *value);
int MTCT mtsync_dec(int *value);
}
//---------------------------------------------------------------------------
// FIXME: flibit commented this #ifdef out
//#ifdef MTSYSTEM_EXPORTS
	extern MTSystemInterface *si;
//#endif
extern MTInterface *mtinterface;
extern MTGUIInterface *gi;
extern MTDisplayInterface *di;
#ifdef MTSYSTEM_RESOURCES
	extern MTResources *sysres;
#endif
#ifdef _WIN32
	extern SYSTEM_INFO sysinfo;
#endif
//---------------------------------------------------------------------------
#endif
