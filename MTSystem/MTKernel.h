//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTKernel.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------

/*
 * A whole bunch of lowlevel routines, mainly related to threading
 * and process creation. Unfortunately it's pretty hard to figure
 * out what's going on in this code.
 * But my best guess is that most of it can be easily replaced with C++11
 * threading library and thread_local state.
 */
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
#define MTPF_CANCANCEL   1
#define MTPF_HASPROGRESS 2
#define MTPS_INIT     -1
#define MTPS_WAITING  0
#define MTPS_WORKING  1
#define MTPS_FINISHED 2

#define MAX_STACK 1024
//---------------------------------------------------------------------------
#include <MTXAPI/MTXSystem.h>
#include <MTXAPI/MTXExtension.h>

//---------------------------------------------------------------------------
class MTLock;

class MTEvent;

class MTThread;

class MTProcess;

class MTTimer;

class MTCPUMonitor;

typedef int (MTCT* ThreadProc)(MTThread* thread, void* param);

typedef void (MTCT* ProcessProc)(MTProcess* process, void* param, float p);

typedef void (MTCT* TimerProc)(MTTimer* timer, int param);
//---------------------------------------------------------------------------
#include "MTSystem.h"

#ifdef _WIN32
#	include <windows.h>
#else

#	include <pthread.h>
#	include <signal.h>

#endif

#include <MTXAPI/MTXGUI.h>
#include <MTXAPI/MTXSystem.h>
//---------------------------------------------------------------------------
extern "C" {
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
bool MTCT mtsetprivatedata(int id, void* data);
void* MTCT mtgetprivatedata(int id);
int MTCT mtlocalalloc();
bool MTCT mtlocalfree(int id);
bool MTCT mtlocalset(int id, void* value);
void* MTCT mtlocalget(int id);
MTThread* MTCT mtthreadcreate(ThreadProc proc, bool autofree, bool autostart, void* param, int priority, const char* name);
MTProcess* MTCT mtprocesscreate(ThreadProc tproc, void* param, int type, int priority, void* data, ProcessProc pproc, bool silent, const char* name);
MTLock* MTCT mtlockcreate();
void MTCT mtlockdelete(MTLock* lock);
MTEvent* MTCT mteventcreate(bool autoreset, int interval, int resolution, bool periodic, bool pulse);
void MTCT mteventdelete(MTEvent* event);
MTTimer* MTCT mttimercreate(int interval, int resolution, bool periodic, int param, TimerProc proc);
MTTimer* MTCT mttimerevent(int interval, int resolution, bool periodic, MTEvent* event, bool pulse);
void MTCT mttimerdelete(MTTimer* timer);
MTCPUMonitor* MTCT mtcpumonitorcreate(int ncounters);
int MTCT mtsyscounter();
bool MTCT mtsyscounterex(double* count);
void MTCT mtsyswait(int ms);
int MTCT mtsyswaitmultiple(int count, void** events, bool all, int timeout);
int MTCT mtgetlasterror();
void MTCT mtsetlasterror(int error);
}
//---------------------------------------------------------------------------
#endif
