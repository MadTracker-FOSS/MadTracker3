//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTKernel.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTKERNEL_INCLUDED
#define MTKERNEL_INCLUDED
//---------------------------------------------------------------------------
#define MTT_IDLE     -15
#define MTT_LOW      -2
#define MTT_LOWER    -1
#define MTT_NORMAL   0
#define MTT_HIGHER   1
#define MTT_HIGH     2
#define MTT_REALTIME 15

enum MTProcessType{
	MTP_LOADMODULE = 0x10000,
	MTP_SAVEMODULE,
	MTP_LOADINSTRUMENT,
	MTP_SAVEINSTRUMENT,
	MTP_LOADSAMPLE,
	MTP_SAVESAMPLE,
	MTP_BUILDPEAKS = 0x20000,
	MTP_NETWORK = 0x30000,
	MTP_EXPORT = 0x40000,
	MTP_INTERNET = 0x50000
};

#define MTPF_CANCANCEL   1
#define MTPF_HASPROGRESS 2

#define MTPS_INIT     -1
#define MTPS_WAITING  0
#define MTPS_WORKING  1
#define MTPS_FINISHED 2

#define MAX_STACK 1024
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
class MTLock;
class MTEvent;
class MTThread;
class MTProcess;
class MTTimer;
class MTCPUMonitor;
typedef int (MTCT *ThreadProc)(MTThread *thread,void *param);
typedef void (MTCT *ProcessProc)(MTProcess *process,void *param,float p);
typedef void (MTCT *TimerProc)(MTTimer *timer,int param);
//---------------------------------------------------------------------------
#include "MTSystem1.h"
#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <signal.h>
#endif
#include "MTXGUI.h"
//---------------------------------------------------------------------------
class MTLock{
public:
	MTLock();
	virtual ~MTLock();
	virtual bool MTCT lock(int timeout = -1);
	virtual void MTCT unlock();
private:
#ifdef _WIN32
	CRITICAL_SECTION critical;
#else
	pthread_mutex_t mutex;
#endif
};

#ifndef _WIN32
struct _mutex_cond{
	pthread_mutex_t i_mutex;
	pthread_cond_t i_cv;
};
struct _le{
	struct _le *next;
	struct _le *prev;
	struct _mutex_cond *i_mutex_cond;
};
#endif

class MTEvent{
public:
	MTEvent(bool autoreset,int interval = 0,int resolution = 0,bool periodic = true,bool pulse = false);
	MTEvent();
	virtual ~MTEvent();
	virtual bool MTCT pulse();
	virtual bool MTCT set();
	virtual bool MTCT reset();
	virtual bool MTCT wait(int timeout = -1);
protected:
	friend int MTCT mtsyswaitmultiple(int count,MTEvent **events,bool all,int timeout);
	friend class MTTimer;
	int timer;
#ifdef _WIN32
	HANDLE event;
#else
	static void LinuxEventProc(sigval);
	bool signaled,needreset,needpulse;
	pthread_mutex_t *e_mutex;
	_le *start,*end;
	void _add(_le *list);
	void _del(_le *list);
#endif
};

class MTThread : public MTEvent{
public:
	mt_uint32 id;
	int type;
	int result;
	bool terminated;

	MTThread(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name);
	MTThread();
	virtual ~MTThread();
	bool MTCT pulse();
	bool MTCT set();
	bool MTCT reset();
	virtual void MTCT start();
	virtual bool MTCT getmessage(int &msg,int &param1,int &param2,bool wait = false);
	virtual void MTCT postmessage(int msg,int param1,int param2);
	virtual void MTCT terminate();
protected:
#ifdef _WIN32
	static DWORD WINAPI SysThread(MTThread*);
#else
	static void* SysThread(void*);
	int _p[2];
	pthread_attr_t *attr;
#endif
	ThreadProc mproc;
	char *mname;
	void *mparam;
	int mpriority;
	bool mautofree;
	bool running;
	bool hasmsg;
};

class MTProcess : public MTThread{
public:
	int status;
	int priority;
	void *data;
	void *guidata;
	float progress;
	ProcessProc mpproc;

	MTProcess(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name);
	virtual ~MTProcess();
	void MTCT start();
	virtual void MTCT setprogress(float p);
private:
	static int MTCT syncprocessproc(MTSync *s);
};

class MTTimer{
public:
	MTTimer(int interval,int resolution,bool periodic,int param,TimerProc proc);
	MTTimer(int interval,int resolution,bool periodic,MTEvent *event,bool pulse = false);
	virtual ~MTTimer();
private:
#ifdef _WIN32
	static void CALLBACK WinTimerProc(UINT,UINT,DWORD,DWORD,DWORD);
#else
	static void LinuxTimerProc(sigval);
#endif
	MTEvent *event;
	int id;
	int res;
	int mparam;
	TimerProc mproc;
};

struct MTCPUState{
	double starttime;
	double lasttime;
	double start;
	double count;
	double divider;
	double cpu;
	bool counting;
	bool used;
};

class MTCPUMonitor{
public:
	int flushid;
	MTCPUMonitor(int ncounters);
	virtual ~MTCPUMonitor();
	virtual void MTCT startslice(int id);
	virtual void MTCT endslice(int id);
	virtual void MTCT flushcpu(int id);
	virtual void MTCT startadd(int id);
	virtual void MTCT endadd(int id);
	virtual double MTCT getcpu(int id);
	virtual void* MTCT getcpustate(int id);
	virtual int MTCT getcpustateid(void *s);
	virtual int MTCT addcpustate();
	virtual void MTCT delcpustate(int id);
private:
	int n;
	MTCPUState *state;
};
//---------------------------------------------------------------------------
extern "C"
{
void initKernel();
void uninitKernel();
void stopThreads(bool processes);
#ifndef _WIN32
	void* mttry(bool pop);
	void mtsigreturn(int sig);
#endif
//---------------------------------------------------------------------------
MTThread* MTCT mtgetsysthread();
MTThread* MTCT mtgetcurrentthread();
bool MTCT mtissysthread();
bool MTCT mtsetprivatedata(int id,void *data);
void* MTCT mtgetprivatedata(int id);
int MTCT mtlocalalloc();
bool MTCT mtlocalfree(int id);
bool MTCT mtlocalset(int id,void *value);
void* MTCT mtlocalget(int id);
MTThread* MTCT mtthreadcreate(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name);
MTProcess* MTCT mtprocesscreate(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name);
MTLock* MTCT mtlockcreate();
void MTCT mtlockdelete(MTLock *lock);
MTEvent* MTCT mteventcreate(bool autoreset,int interval,int resolution,bool periodic,bool pulse);
void MTCT mteventdelete(MTEvent *event);
MTTimer* MTCT mttimercreate(int interval,int resolution,bool periodic,int param,TimerProc proc);
MTTimer* MTCT mttimerevent(int interval,int resolution,bool periodic,MTEvent *event,bool pulse);
void MTCT mttimerdelete(MTTimer *timer);
MTCPUMonitor* MTCT mtcpumonitorcreate(int ncounters);
int MTCT mtsyscounter();
bool MTCT mtsyscounterex(double *count);
void MTCT mtsyswait(int ms);
int MTCT mtsyswaitmultiple(int count,void **events,bool all,int timeout);
int MTCT mtgetlasterror();
void MTCT mtsetlasterror(int error);
}
//---------------------------------------------------------------------------
#endif
