//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTKernel.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTKernel.h"
#include <MTXAPI/MTXSystem2.h>
#include <MTXAPI/RES/MTSystemRES.h>

#ifdef _WIN32
#	include <mmsystem.h>
#else

#	include <stdlib.h>

#endif

//---------------------------------------------------------------------------
MTThread* systhread;

unsigned int _nthreads, _nathreads;

MTThread** threads;

MTLock* threadlock;

bool pf;

int privateid[16];

#ifdef _WIN32
double sysfrequency;
BOOL (WINAPI *tryenter)(LPCRITICAL_SECTION lpCriticalSection);
#endif

//---------------------------------------------------------------------------
MTThread* mtgetsysthread()
{
    return systhread;
}

MTThread* mtgetcurrentthread()
{
    return (MTThread*) mtlocalget(privateid[0]);
}

bool mtissysthread()
{
#	ifdef _WIN32
    return (GetCurrentThreadId()==systhread->id);
#	else
    return (pthread_self() == systhread->id);
#	endif
}

bool mtsetprivatedata(int id, void* data)
{
    return mtlocalset(privateid[id + 8], data);
}

void* mtgetprivatedata(int id)
{
    return mtlocalget(privateid[id + 8]);
}

// TODO Deprecated as c++11 introduced the thread_local storage duration specifier.
int mtlocalalloc()
{
#	ifdef _WIN32
    return TlsAlloc();
#	else
    pthread_key_t key;
    if (pthread_key_create(&key, 0))
    {
        return -1;
    }
    return (int) key;
#	endif
}

// TODO Deprecated, see above.
bool mtlocalfree(int id)
{
#	ifdef _WIN32
    return TlsFree(id);
#	else
    return (pthread_key_delete((pthread_key_t) id) == 0);
#	endif
}

//TODO Deprecated, see above.
bool mtlocalset(int id, void* value)
{
#	ifdef _WIN32
    return TlsSetValue(id,value);
#	else
    return (pthread_setspecific((pthread_key_t) id, value) == 0);
#	endif
}

//TODO Deprecated, see above.
void* mtlocalget(int id)
{
#	ifdef _WIN32
    return TlsGetValue(id);
#	else
    return pthread_getspecific((pthread_key_t) id);
#	endif
}

//TODO Use the standard-compliant std::thread instead.
MTThread* mtthreadcreate(ThreadProc proc, bool autofree, bool autostart, void* param, int priority, const char* name)
{
    return new MTThread(proc, autofree, autostart, param, priority, name);
}

MTProcess* mtprocesscreate(ThreadProc tproc, void* param, int type, int priority, void* data, ProcessProc pproc, bool silent, const char* name)
{
    return new MTProcess(tproc, param, type, priority, data, pproc, silent, name);
}

MTLock* mtlockcreate()
{
    return new MTLock();
}

void mtlockdelete(MTLock* lock)
{
    delete lock;
}

MTEvent* mteventcreate(bool autoreset, int interval, int resolution, bool periodic, bool pulse)
{
    return new MTEvent(autoreset, interval, resolution, periodic, pulse);
}

void mteventdelete(MTEvent* event)
{
    delete event;
}

MTTimer* mttimercreate(int interval, int resolution, bool periodic, int param, TimerProc proc)
{
    return new MTTimer(interval, resolution, periodic, param, proc);
}

MTTimer* mttimerevent(int interval, int resolution, bool periodic, MTEvent* event, bool pulse)
{
    return new MTTimer(interval, resolution, periodic, event, pulse);
}

void mttimerdelete(MTTimer* timer)
{
    delete timer;
}

MTCPUMonitor* mtcpumonitorcreate(int ncounters)
{
    return new MTCPUMonitor(ncounters);
}

int mtsyscounter()
{
#if defined(_WIN32)
    return GetTickCount();
#elif defined(__linux__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
#elif defined(__APPLE__)
#warning Need to implement `int mtsyscounter()` for this platform!
#else
#error Need to implement `int mtsyscounter()` for this platform!
#endif
}

bool mtsyscounterex(double* count)
{
#if defined(_WIN32)
    __int64 c;
    bool ok;

    if (pf)
    {
        ok = QueryPerformanceCounter((LARGE_INTEGER*) &c) != 0;
        if (ok)
        {
            *count = (double) c / sysfrequency;
        }
        return ok;
    }
    else
    {
        *count = (double) GetTickCount() / 1000;
        return true;
    };
#elif defined(__linux__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *count = (double) ts.tv_sec + (double) ts.tv_nsec / 1000000000.0;
    return true;
#elif defined(__APPLE__)
#warning Need to implement `bool mtsyscounterex()` for this platform!
#else
#error Need to implement `bool mtsyscounterex()` for this platform!
#endif
}

void mtsyswait(int ms)
{
#if defined(_WIN32)
    Sleep(ms);
#else
    struct timespec ts = {
        ms / 1000, (ms % 1000) * 1000
    };
    nanosleep(&ts, 0);
#endif
}

int mtsyswaitmultiple(int count, MTEvent** events, bool all, int timeout)
{
    int x, res;

    if (count <= 0)
    {
        return -1;
    }

#if defined(_WIN32)
    HANDLE *h = (HANDLE*)mtmemalloc(sizeof(HANDLE)*count);
    for (x=0;x<count;x++) h[x] = events[x]->event;
    x = WaitForMultipleObjects(count,h,all,timeout);
    if (x==WAIT_TIMEOUT) res = -1;
    else res = x-WAIT_OBJECT_0;
    mtmemfree(h);
#elif defined(__linux__)
    struct timespec to_time;
    _mutex_cond* p, * common_mutex_cond;
    _le* q;
    _mutex_cond** mutex_cond;
    _le** le;

    mutex_cond = (_mutex_cond**) malloc(sizeof(_mutex_cond*) * count);
    le = (_le**) malloc(sizeof(_le*) * count);
    if (timeout != -1)
    {
        clock_gettime(CLOCK_REALTIME, &to_time);
        to_time.tv_sec += timeout / 1000;
        to_time.tv_nsec += (timeout % 1000) * 1000000;
        if (to_time.tv_nsec > 1000000000)
        {
            to_time.tv_nsec -= 1000000000;
            to_time.tv_sec++;
        };
    };
    if (all)
    {
        res = 0;
        for(x = 0; x < count; x++)
        {
            p = (_mutex_cond*) malloc(sizeof(_mutex_cond));
            pthread_mutex_init(&p->i_mutex, 0);
            pthread_cond_init(&p->i_cv, 0);
            q = (_le*) malloc(sizeof(_le));
            mutex_cond[x] = p;
            q->i_mutex_cond = p;
            le[x] = q;
            events[x]->_add(q);
        };
        for(x = 0; x < count; x++)
        {
            pthread_mutex_lock(&mutex_cond[x]->i_mutex);
//			while (!events[x]->signaled){
            if (!events[x]->signaled)
            {
                if (timeout != -1)
                {
                    if (pthread_cond_timedwait(&mutex_cond[x]->i_cv, &mutex_cond[x]->i_mutex, &to_time) == ETIMEDOUT)
                    {
                        res = -1;
                        break;
                    };
                }
                else
                {
                    pthread_cond_wait(&mutex_cond[x]->i_cv, &mutex_cond[x]->i_mutex);
                };
            };
            pthread_mutex_unlock(&mutex_cond[x]->i_mutex);
            if (res == -1)
            {
                break;
            }
        };
        for(x = 0; x < count; x++)
        {
            events[x]->_del(le[x]);
        };
    }
    else
    {
        res = -1;
        common_mutex_cond = (_mutex_cond*) malloc(sizeof(_mutex_cond));
        pthread_mutex_init(&common_mutex_cond->i_mutex, 0);
        pthread_cond_init(&common_mutex_cond->i_cv, 0);
        for(x = 0; x < count; x++)
        {
            q = (_le*) malloc(sizeof(_le));
            q->i_mutex_cond = common_mutex_cond;
            le[x] = q;
            events[x]->_add(q);
        };
        pthread_mutex_lock(&common_mutex_cond->i_mutex);
        for(x = 0; x < count; x++)
        {
            if (events[x]->signaled)
            {
                res = 0;
                break;
            };
        };
        if (res == -1)
        {
            if (timeout != -1)
            {
                if (pthread_cond_timedwait(
                    &common_mutex_cond->i_cv, &common_mutex_cond->i_mutex, &to_time
                ) != ETIMEDOUT)
                {
                    res = 0;
                };
            }
            else
            {
                pthread_cond_wait(&common_mutex_cond->i_cv, &common_mutex_cond->i_mutex);
                res = 0;
            };
        };
        pthread_mutex_unlock(&mutex_cond[x]->i_mutex);
        for(x = 0; x < count; x++)
        {
            events[x]->_del(le[x]);
        };
    };
    free(mutex_cond);
    free(le);
#elif defined(__APPLE__)
#warning Need to implement `int mtsyswaitmultiple()` for this platform!
#else
#error Need to implement `int mtsyswaitmultiple()` for this platform!
#endif

    return res;
}

int mtgetlasterror()
{
    return (int) mtgetprivatedata(-7);
}

void mtsetlasterror(int error)
{
    mtsetprivatedata(-7, (void*) error);
}
//---------------------------------------------------------------------------
#ifndef _WIN32
struct _MTTRY_E
{
    _MTTRY_E* prev;
    sigjmp_buf jb;
};

struct _MTTRY
{
    int ne;
    _MTTRY_E* el;
};

void mttryinit()
{
    _MTTRY* mt = (_MTTRY*) mtmemalloc(sizeof(_MTTRY));
    mt->ne = 0;
    mt->el = 0;
//fprintf(stderr,"INIT> Thread: %08X Data: %08X"NL,pthread_self(),mt);
    mtsetprivatedata(-6, (void*) mt);
    si->onerror = (void*) mttry;
}

void mttryuninit()
{
    _MTTRY* mt = (_MTTRY*) mtgetprivatedata(-6);
    if ((!mt) || (mt->ne != 0) || (mt->el != 0))
    {
        LOGD("%s - [System] ERROR: Exception stack corrupted! This is bad bad bad!!"
                 NL);
    };
    mtmemfree(mt);
}

void* mttry(bool pop)
{
    _MTTRY* mt = (_MTTRY*) mtgetprivatedata(-6);
    if (!mt)
    {
        mttryinit();
        mt = (_MTTRY*) mtgetprivatedata(-6);
    };
    if (!pop)
    {
        _MTTRY_E* prev = mt->el;
        mt->el = (_MTTRY_E*) malloc(sizeof(_MTTRY_E));
        mt->el->prev = prev;
        mt->ne++;
//fprintf(stderr,"TRY> Thread: %08X Data: %08X (%d)"NL,pthread_self(),mtgetprivatedata(-6),mt->ne);
        return mt->el->jb;
/*
		if (sigsetjmp(mt->el->jb,1)==0){
fprintf(stderr,"TRY> Thread: %08X Data: %08X (%d)"NL,pthread_self(),mtgetprivatedata(-6),mt->ne);
			return 0;
		}
		else{
fprintf(stderr,"CATCH> Thread: %08X Data: %08X (%d)"NL,pthread_self(),mtgetprivatedata(-6),mt->ne);
			return 1;
		};
*/
    }
    else
    {
        _MTTRY_E* old = mt->el;
//fprintf(stderr,"<RET Thread: %08X Data: %08X (%d)"NL,pthread_self(),mt,mt->ne);
        mt->el = old->prev;
        free(old);
        mt->ne--;
        return 0;
    };
}

void mtsigreturn(int sig)
{
    _MTTRY* mt = (_MTTRY*) mtgetprivatedata(-6);
    if (mt->ne > 0)
    {
//fprintf(stderr,"<CRASH> Thread: %08X Data: %08X"NL,pthread_self(),mt);
        siglongjmp(mt->el->jb, sig);
    };
}

#endif

//---------------------------------------------------------------------------
void initKernel()
{
    int x;

    for(x = 0; x < 16; x++)
    {
        privateid[x] = mtlocalalloc();
    }
#	ifdef _WIN32
    mt_int64 intf;
    pf = QueryPerformanceFrequency((LARGE_INTEGER*)&intf)!=0;
    if (pf) sysfrequency = (double)intf;
    else sysfrequency = 1000;
#	endif
    threadlock = new MTLock();
    mtsetprivatedata(-8, new MTThread());
#	ifndef _WIN32
    mttryinit();
#	endif
#	ifdef _DEBUG
    char* callstack = (char*) mtmemalloc(MAX_STACK, MTM_ZERO);
    mtsetprivatedata(-5, callstack);
    mtsetprivatedata(-4, callstack + MAX_STACK - 1);
    mtsetprivatedata(-3, 0);
#	endif
#	ifdef _WIN32
    HMODULE hkernel = GetModuleHandle("KERNEL32.DLL");
    *(int*)&tryenter = (int)GetProcAddress(hkernel,"TryEnterCriticalSection");
#	endif
}

void uninitKernel()
{
    int x;

    stopThreads(false);
    if (threads)
    {
        mtmemfree(threads);
    }
    delete systhread;
    delete threadlock;
#	ifndef _WIN32
    mttryuninit();
#	endif
#	ifdef _DEBUG
    mtmemfree(mtgetprivatedata(-5));
#	endif
    for(x = 0; x < 16; x++)
    {
        mtlocalfree(privateid[x]);
    }
}

void stopThreads(bool processes)
{
    int x, n;

    if (_nthreads > 0)
    {
#		ifdef _DEBUG
        char buf[256];
        if (processes)
        {
            n = 0;
            for(x = 0; x < _nthreads; x++)
            {
                if (threads[x]->type)
                {
                    n++;
                }
            };
            if (n)
            {
                sprintf(buf, "%d process(s) still running!", n);
                mtdialog(buf, "Processes", MTD_OK, MTD_EXCLAMATION, 5000);
            };
        }
        else
        {
            sprintf(buf, "%d thread(s) still running!", _nthreads);
            mtdialog(buf, "Threads", MTD_OK, MTD_EXCLAMATION, 5000);
        };
#		endif
        if (processes)
        {
            for(x = 0; x < _nthreads; x++)
            {
                if (threads[x]->type)
                {
                    threads[x]->terminate();
                    x--;
                };
            };
        }
        else
        {
            while(_nthreads > 0)
            {
                threads[0]->terminate();
            }
        };
    };
}

//---------------------------------------------------------------------------
MTLock::MTLock()
{
#	ifdef _WIN32
    mtmemzero(&critical,sizeof(critical));
    InitializeCriticalSection(&critical);
#	else
    pthread_mutex_init(&mutex, 0);
#	endif
}

MTLock::~MTLock()
{
#	ifdef _WIN32
    DeleteCriticalSection(&critical);
#	else
    pthread_mutex_destroy(&mutex);
#	endif
}

bool MTLock::lock(int timeout)
{
#	ifdef _WIN32
    if ((timeout>0) && (tryenter)){
        while (--timeout>0){
            if (tryenter(&critical)) return true;
            Sleep(1);
        };
        if (tryenter(&critical)) return true;
        return false;
    };
    EnterCriticalSection(&critical);
    return true;
#	else
    if (timeout)
    {
        struct timespec ts = {0, 1000};
        while(--timeout > 0)
        {
            if (pthread_mutex_trylock(&mutex) == 0)
            {
                return true;
            }
            nanosleep(&ts, 0);
        };
        if (pthread_mutex_trylock(&mutex) == 0)
        {
            return true;
        }
        return false;
    }
    else
    {
        return (pthread_mutex_lock(&mutex) == 0);
    };
#	endif
}

void MTLock::unlock()
{
#	ifdef _WIN32
    LeaveCriticalSection(&critical);
#	else
    pthread_mutex_unlock(&mutex);
#	endif
}

//---------------------------------------------------------------------------
MTEvent::MTEvent(bool autoreset, int interval, int resolution, bool periodic, bool pulse)
{
#if defined(_WIN32)
    event = CreateEvent(0,!autoreset,false,0);
    if (interval){
        timer = timeSetEvent(interval,resolution,(LPTIMECALLBACK)event,(int)this,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
    }
    else timer = 0;
#elif defined(__linux__)
    signaled = false;
    needpulse = pulse;
    needreset = autoreset;
    e_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(e_mutex, 0);
    start = end = 0;
    if (interval)
    {
        struct sigevent se;
        struct itimerspec ts;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_signo = SIGRTMAX;
        se.sigev_value.sival_ptr = this;
        se.sigev_notify_function = LinuxEventProc;
        se.sigev_notify_attributes = 0;
        timer_create(CLOCK_REALTIME, &se, (timer_t*) &timer);
        mtmemzero(&ts, sizeof(ts));
        ts.it_value.tv_sec = interval / 1000;
        ts.it_value.tv_nsec = (interval % 1000) * 1000000;
        if (periodic)
        {
            ts.it_interval = ts.it_value;
        }
        timer_settime((timer_t) timer, 0, &ts, 0);
    }
    else
    {
        timer = 0;
    }
#elif defined(__APPLE__)
#warning Need to implement `MTEvent::MTEvent()` for this platform!
#else
#error Need to implement `MTEvent::MTEvent()` for this platform!
#endif
}

MTEvent::MTEvent():

#ifdef _WIN32

event(0),
timer(0)
#else
    signaled(false),
    needreset(false)
#endif
{
#ifndef _WIN32

    e_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(
        e_mutex,

        0
    );

    start = end = 0;

#endif
}

MTEvent::~MTEvent()
{
#if defined(_WIN32)
    if (timer){
        timeKillEvent(timer);
    };
    if (event){
        CloseHandle(event);
    };
#elif defined(__linux__)
    if (timer)
    {
        timer_delete((timer_t) timer);
    }
    pthread_mutex_destroy(e_mutex);
    free(e_mutex);
#elif defined(__APPLE__)
#warning Need to implement `MTEvent::~MTEvent()` for this platform!
#else
#error Need to implement `MTEvent::~MTEvent()` for this platform!
#endif
}

bool MTEvent::pulse()
{
#	ifdef _WIN32
    return PulseEvent(event)!=0;
#	else
    _le* p;
    signaled = true;
    if (start == 0)
    {
        return true;
    }
    p = start;
    do
    {
        pthread_mutex_lock(&p->i_mutex_cond->i_mutex);
        p = p->next;
    }
    while(p != 0);
    p = start;
    do
    {
        pthread_cond_signal(&p->i_mutex_cond->i_cv);
        p = p->next;
    }
    while(p != 0);
    p = start;
    do
    {
        pthread_mutex_unlock(&p->i_mutex_cond->i_mutex);
        p = p->next;
    }
    while(p != 0);
    signaled = false;
    return true;
#	endif
}

bool MTEvent::set()
{
#	ifdef _WIN32
    return SetEvent(event)!=0;
#	else
    _le* p;
    signaled = true;
    if (start == 0)
    {
        return true;
    }
    p = start;
    do
    {
        pthread_mutex_lock(&p->i_mutex_cond->i_mutex);
        p = p->next;
    }
    while(p != 0);
    p = start;
    do
    {
        pthread_cond_signal(&p->i_mutex_cond->i_cv);
        p = p->next;
    }
    while(p != 0);
    p = start;
    do
    {
        pthread_mutex_unlock(&p->i_mutex_cond->i_mutex);
        p = p->next;
    }
    while(p != 0);
    return true;
#	endif
}

bool MTEvent::reset()
{
#	ifdef _WIN32
    return ResetEvent(event)!=0;
#	else
    signaled = false;
    return true;
#	endif
}

bool MTEvent::wait(int timeout)
{
#	ifdef _WIN32
    if (!event) return false;
    if ((gi) && (mtissysthread())){
        int now = mtsyscounter();
        if (WaitForSingleObject(event,10)==WAIT_OBJECT_0) return true;
        while (gi->processmessages(false)>=0){
            if (WaitForSingleObject(event,10)==WAIT_OBJECT_0) return true;
            if ((timeout>=0) && (mtsyscounter()-now>timeout)) break;
        };
        return false;
    }
    else return (WaitForSingleObject(event,timeout)==WAIT_OBJECT_0);
#	else
    // TODO UI synchronization support
    MTEvent* self = this;
    return (mtsyswaitmultiple(1, &self, true, timeout) >= 0);
#	endif
}

#ifndef _WIN32

void MTEvent::_add(_le* list)
{
    pthread_mutex_lock(e_mutex);
    if (start == 0)
    {
        start = list;
        list->next = 0;
        list->prev = 0;
        end = list;
    }
    else
    {
        end->next = list;
        list->prev = end;
        list->next = 0;
        end = list;
    };
    pthread_mutex_unlock(e_mutex);
}

void MTEvent::_del(_le* list)
{
    pthread_mutex_lock(e_mutex);
    if ((start == list) && (end == list))
    {
        start = end = 0;
    }
    else if (start == list)
    {
        start = list->next;
        start->prev = 0;
    }
    else if (end == list)
    {
        end = list->prev;
        end->next = 0;
    }
    else
    {
        list->prev->next = list->next;
        list->next->prev = list->prev;
    };
    free(list);
    pthread_mutex_unlock(e_mutex);
}

void MTEvent::LinuxEventProc(sigval timer)
{
    MTEvent& cevent = *(MTEvent*) timer.sival_ptr;
    if (cevent.needpulse)
    {
        cevent.pulse();
    }
    else
    {
        cevent.set();
    }
}

#endif

//---------------------------------------------------------------------------
MTThread::MTThread(ThreadProc proc, bool autofree, bool autostart, void* param, int priority, const char* name):
    MTEvent(),
    type(0),
    result(0),
    terminated(false),
    mproc(proc),
    mname(0),
    mparam(param),
    mpriority(priority),
    mautofree(autofree),
    running(false),
    hasmsg(false)
{
    if (name)
    {
        mname = name;
    }
    threadlock->lock();
    if (_nthreads == _nathreads)
    {
        _nathreads += 4;
        if (threads)
        {
            threads = (MTThread**) mtmemrealloc(threads, 4 * _nathreads);
        }
        else
        {
            threads = (MTThread**) mtmemalloc(4 * _nathreads);
        }
    };
    threads[_nthreads++] = this;
    if (autostart)
    {
        start();
    }
    threadlock->unlock();
#	ifndef _WIN32
    bool fifo = false;
    struct sched_param rt_param;
    attr = (pthread_attr_t*) malloc(sizeof(pthread_attr_t));
    pthread_attr_init(attr);
    memset(&rt_param, 0, sizeof(rt_param));
    switch (priority)
    {
        case MTT_HIGHER:
            rt_param.sched_priority = 1;
            fifo = true;
            break;
        case MTT_HIGH:
            rt_param.sched_priority = 10;
            fifo = true;
            break;
        case MTT_REALTIME:
            rt_param.sched_priority = 99;
            fifo = true;
            break;
        default:
            rt_param.sched_priority = 0;
            break;
    };
    if (fifo)
    {
        pthread_attr_setschedpolicy(attr, SCHED_FIFO);
    }
    pthread_attr_setschedparam(attr, &rt_param);
    pipe(_p);
#	endif
}

MTThread::MTThread():
    MTEvent(),
    type(-1),
    result(0),
    terminated(false),
    mproc(0),
    mname("User"),
    mparam(0),
    mpriority(0),
    mautofree(false),
    running(false),
    hasmsg(false)
{
    systhread = this;
#	ifdef _WIN32
    event = GetCurrentThread();
    id = GetCurrentThreadId();
#	else
    id = pthread_self();
    attr = (pthread_attr_t*) malloc(sizeof(pthread_attr_t));
    pthread_attr_init(attr);
    pipe(_p);
#	endif
}

MTThread::~MTThread()
{
    unsigned int x, y;
    bool isrunning;

    terminated = true;
    threadlock->lock();
    mautofree = false;
    isrunning = running;
    running = false;
    threadlock->unlock();
    if (isrunning)
    {
        if (!wait(10000))
        {
            LOGD("%s - [System] ERROR: Thread timeout!"
                     NL);
        };
    };
    threadlock->lock();
    if (this == systhread)
    {
        systhread = 0;
    }
    else
    {
        for(x = 0; x < _nthreads; x++)
        {
            if (threads[x] == this)
            {
                for(y = x; y < _nthreads - 1; y++)
                {
                    threads[y] = threads[y + 1];
                }
                threads[--_nthreads] = 0;
                break;
            };
        };
        if (_nthreads < _nathreads - 4)
        {
            _nathreads -= 4;
            if (_nathreads <= 0)
            {
                mtmemfree(threads);
                threads = 0;
                _nathreads = 0;
            }
            else
            {
                threads = (MTThread**) mtmemrealloc(threads, 4 * _nathreads);
            }
        };
    };
    threadlock->unlock();
#	ifndef _WIN32
    close(_p[0]);
    close(_p[1]);
    pthread_attr_destroy(attr);
    free(attr);
#	endif
}

bool MTThread::pulse()
{
    return false;
}

bool MTThread::set()
{
#	ifndef _WIN32
    return MTEvent::set();
#	endif
    return false;
}

bool MTThread::reset()
{
    return false;
}

void MTThread::start()
{
#	ifdef _WIN32
    event = CreateThread(0,0,(LPTHREAD_START_ROUTINE)SysThread,this,0,(LPDWORD)&id);
#	else
    pthread_create((pthread_t*) &id, 0, SysThread, this);
#	endif
    running = true;
}

bool MTThread::getmessage(int& msg, int& param1, int& param2, bool wait)
{
#	ifdef _WIN32
    MSG cmsg;

    if (wait){
        hasmsg = true;
        if (GetMessage(&cmsg,0,0,0)>0){
            if (cmsg.message==WM_QUIT) msg = -1;
            else msg = cmsg.message;
            param1 = cmsg.wParam;
            param2 = cmsg.lParam;
            return true;
        };
        return false;
    };
    if (PeekMessage(&cmsg,0,0,0,PM_REMOVE)){
        if (cmsg.message==WM_QUIT) msg = -1;
        else msg = cmsg.message;
        param1 = cmsg.wParam;
        param2 = cmsg.lParam;
        return true;
    };
    return false;
#	else
    int cmsg[3];
    fd_set rs;

    FD_ZERO(&rs);
    FD_SET(_p[0], &rs);
    if (wait)
    {
        select(1, &rs, 0, 0, 0);
    }
    else
    {
        struct timeval tv = {0, 0};
        select(1, &rs, 0, 0, &tv);
        if (!FD_ISSET(_p[0], &rs))
        {
            return false;
        }
    };
    if (read(_p[0], &cmsg, sizeof(cmsg)) != sizeof(cmsg))
    {
        return false;
    }
    msg = cmsg[0];
    param1 = cmsg[1];
    param2 = cmsg[2];
    return true;
#	endif
}

void MTThread::postmessage(int msg, int param1, int param2)
{
#	ifdef _WIN32
    if (msg<0) msg = WM_QUIT;
    PostThreadMessage(id,msg,param1,param2);
#	else
    int cmsg[3];

    cmsg[0] = msg;
    cmsg[1] = param1;
    cmsg[2] = param2;
    write(_p[1], &cmsg, sizeof(cmsg));
#	endif
}

void MTThread::terminate()
{
    bool isrunning;

    threadlock->lock();
    terminated = true;
    mautofree = false;
    isrunning = running;
    running = false;
    threadlock->unlock();
    if (isrunning)
    {
        if (hasmsg)
        {
            postmessage(-1, 0, 0);
        }
        if (!wait(10000))
        {
            LOGD("%s - [System] ERROR: Thread timeout!"
                     NL);
        };
    };
    delete this;
}

#ifdef _WIN32
DWORD WINAPI MTThread::SysThread(MTThread *thread)
{
    int x;
    bool autofree;

    mtsetprivatedata(-8,thread);
    switch (thread->mpriority){
    case MTT_IDLE:
        x = THREAD_PRIORITY_IDLE;
        break;
    case MTT_LOW:
        x = THREAD_PRIORITY_LOWEST;
        break;
    case MTT_LOWER:
        x = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case MTT_HIGHER:
        x = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case MTT_HIGH:
        x = THREAD_PRIORITY_HIGHEST;
        break;
    case MTT_REALTIME:
        x = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    default:
        x = THREAD_PRIORITY_NORMAL;
        break;
    };
    SetThreadPriority(GetCurrentThread(),x);
    MTTRY
#		ifdef _DEBUG
            char *callstack = (char*)mtmemalloc(MAX_STACK,MTM_ZERO);
            mtsetprivatedata(-5,callstack);
            mtsetprivatedata(-4,callstack+MAX_STACK-1);
            mtsetprivatedata(-3,0);
#		endif
        x = thread->mproc(thread,thread->mparam);
#		ifdef _DEBUG
            mtmemfree(mtgetprivatedata(-5));
#		endif
        LOGD("%s - [System] Process completed."NL);
    MTCATCH
        LOGD("%s - [System] ERROR: Exception in process!"NL);
        x = -1;
    MTEND
    threadlock->lock();
    autofree = thread->mautofree;
    if (autofree) thread->mautofree = false;
    thread->running = false;
    threadlock->unlock();
    if (thread->type){
        MTProcess *p = (MTProcess*)thread;
        p->setprogress((x==-1)?-2.0:-1.0);
        if (autofree) delete thread;
    }
    else if (autofree) delete thread;
    return x;
}
#else

void* MTThread::SysThread(void* param)
{
    int x;
    bool autofree;
    MTThread* thread = (MTThread*) param;

    mtsetprivatedata(-8, thread);
    mttryinit();
    MTTRY
#		ifdef _DEBUG
        char* callstack = (char*) mtmemalloc(MAX_STACK, MTM_ZERO);
        mtsetprivatedata(-5, callstack);
        mtsetprivatedata(-4, callstack + MAX_STACK - 1);
        mtsetprivatedata(-3, 0);
#		endif
        x = thread->mproc(thread, thread->mparam);
#		ifdef _DEBUG
        mtmemfree(mtgetprivatedata(-5));
#		endif
        LOGD("%s - [System] Process completed."
                 NL);
    MTCATCH
        LOGD("%s - [System] ERROR: Exception in process!"
                 NL);
        x = -1;
    MTEND
    mttryuninit();
    threadlock->lock();
    autofree = thread->mautofree;
    if (autofree)
    {
        thread->mautofree = false;
    }
    thread->running = false;
    thread->set();
    if (thread->type > 0)
    {
        MTProcess* p = (MTProcess*) thread;
        p->setprogress((x == -1) ? -2.0 : -1.0);
        if (autofree)
        {
            delete thread;
        }
    }
    else if (autofree)
    {
        delete thread;
    }
    threadlock->unlock();
    pthread_exit((void*) x);
}

#endif

//---------------------------------------------------------------------------
MTProcess::MTProcess(ThreadProc tproc, void* param, int type, int priority, void* data, ProcessProc pproc, bool silent, const char* name):
    status(MTPS_INIT),
    MTThread(tproc, true, false, param, priority, name),
    guidata(0),
    mpproc(pproc)
{
    unsigned int x;
    MTDesktop* dsk;
    int size;

    this->type = type;
    this->data = data;
    setprogress(0.0);
    threadlock->lock();
    status = MTPS_WAITING;
    for(x = 0; x < _nthreads; x++)
    {
        if ((threads[x] != this) && (threads[x]->type))
        {
            MTProcess& cprocess = *(MTProcess*) threads[x];
            if ((cprocess.status == MTPS_WORKING) && ((cprocess.type >> 16) == (type >> 16)))
            {
                threadlock->unlock();
                return;
            };
        };
    };
#	ifdef MTSYSTEM_RESOURCES
    if ((!silent) && (sysres) && ((dsk = (MTDesktop*) di->getdefaultdesktop())) && ((dsk->flags & MTCF_HIDDEN) == 0))
    {
        MTFile* wf = sysres->getresourcefile(MTR_WINDOW, MTW_tasks, &size);
        if (wf)
        {
            MTWindow* tasks = gi->loadwindowfromfile(wf, size, dsk);
            tasks->flags |= MTCF_FREEONCLOSE;
            guidata = tasks;
            tasks->switchflags(MTCF_HIDDEN, false);
            sysres->releaseresourcefile(wf);
        };
    };
#	endif
    start();
    threadlock->unlock();
}

MTProcess::~MTProcess()
{
    unsigned int x;
    bool isrunning;

    terminated = true;
    threadlock->lock();
    mautofree = false;
    isrunning = running;
    running = false;
    threadlock->unlock();
    if (isrunning)
    {
        if (!wait(10000))
        {
            LOGD("%s - [System] ERROR: Thread timeout!"
                     NL);
        };
    };
    if ((progress != -1.0) && (progress != -2.0))
    {
        setprogress(-1.0);
    }
    threadlock->lock();
    status = MTPS_FINISHED;
    for(x = 0; x < _nthreads; x++)
    {
        if ((threads[x] != this) && (threads[x]->type))
        {
            MTProcess& cprocess = *(MTProcess*) threads[x];
            if ((cprocess.status == MTPS_WAITING) && ((cprocess.type >> 16) == (type >> 16)))
            {
                cprocess.start();
                break;
            };
        };
    };
    threadlock->unlock();
}

void MTProcess::start()
{
    status = MTPS_WORKING;
    MTThread::start();
}

int MTProcess::syncprocessproc(MTSync* s)
{
    MTProcess* p = (MTProcess*) s->param[0];
    float f;
    MTCMessage msg;

    f = *(float*) &s->param[1];
    if (p->guidata)
    {
        MTWindow* wnd = (MTWindow*) p->guidata;
        mtmemzero(&msg, sizeof(msg));
        if (f < 0)
        {
            msg.msg = MTCM_CLOSE;
            msg.ctrl = wnd;
            wnd->message(msg);
        }
        else
        {
            MTProgress* progress = (MTProgress*) wnd->getcontrolfromuid(MTC_progress);
            if (progress)
            {
                progress->setposition((int) (f * 100.0));
            };
        };
    };
    if (p->mpproc)
    {
        p->mpproc(p, p->mparam, f);
    }
    return 0;
}

void MTProcess::setprogress(float p)
{
    progress = p;
    if (gi)
    {
        MTSync sync;
        mtmemzero(&sync, sizeof(sync));
        sync.proc = syncprocessproc;
        sync.param[0] = (int) this;
        sync.param[1] = *(int*) &p;
        gi->synchronize(&sync);
    }
    else if (mpproc)
    {
        mpproc(this, mparam, p);
    }
}

//---------------------------------------------------------------------------
MTTimer::MTTimer(int interval, int resolution, bool periodic, int param, TimerProc proc):
    event(0),
    res(resolution),
    mparam(param),
    mproc(proc)
{
#if defined(_WIN32)
    timeBeginPeriod(res);
    id = timeSetEvent(interval,res,WinTimerProc,(DWORD)this,(periodic)?TIME_PERIODIC:TIME_ONESHOT);
#elif defined(__linux__)
    struct sigevent se;
    struct itimerspec ts;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_signo = SIGRTMAX;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_function = UnixTimerProc;
    se.sigev_notify_attributes = 0;
    timer_create(CLOCK_REALTIME, &se, (timer_t*) &id);
    mtmemzero(&ts, sizeof(ts));
    ts.it_value.tv_sec = interval / 1000;
    ts.it_value.tv_nsec = (interval % 1000) * 1000000;
    if (periodic)
    {
        ts.it_interval = ts.it_value;
    }
    timer_settime((timer_t) id, 0, &ts, 0);
#elif defined(__APPLE__)
#warning Need to implement `MTTimer::MTTimer()` for this platform!
#else
#error Need to implement `MTTimer::MTTimer()` for this platform!
#endif
}

MTTimer::MTTimer(int interval, int resolution, bool periodic, MTEvent* event, bool pulse):
    res(resolution)
{
    this->event = event;

#if defined(_WIN32)
    timeBeginPeriod(res);
    id = timeSetEvent(interval,res,(LPTIMECALLBACK)event->event,0,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
#elif defined(__linux__)
    struct sigevent se;
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo = SIGALRM;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_function = UnixTimerProc;
    se.sigev_notify_attributes = 0;
    timer_create(CLOCK_REALTIME, &se, (timer_t*) &id);
#elif defined(__APPLE__)
#warning Need to implement `MTTimer::MTTimer()` for this platform!
#else
#error Need to implement `MTTimer::MTTimer()` for this platform!
#endif
}

MTTimer::~MTTimer()
{
#if defined(_WIN32)
    timeKillEvent(id);
    timeEndPeriod(res);
#elif defined(__linux__)
    timer_delete((timer_t) id);
#elif defined(__APPLE__)
#warning Need to implement `MTTimer::~MTTimer()` for this platform!
#else
#error Need to implement `MTTimer::~MTTimer()` for this platform!
#endif
}

#ifdef _WIN32
void CALLBACK MTTimer::WinTimerProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
    MTTimer &ctimer = *(MTTimer*)dwUser;
    ctimer.mproc(&ctimer,ctimer.mparam);
}
#else

void MTTimer::UnixTimerProc(sigval timer)
{
    MTTimer& ctimer = *(MTTimer*) timer.sival_ptr;
    if (ctimer.event)
    {
        ctimer.event->pulse();
    }
    else
    {
        ctimer.mproc(&ctimer, ctimer.mparam);
    }
}

#endif

//---------------------------------------------------------------------------
MTCPUMonitor::MTCPUMonitor(int ncounters):
    flushid(0)
{
    int x;

    n = ((ncounters + 3) >> 2) << 2;
    state = (MTCPUState*) mtmemalloc(n * sizeof(MTCPUState), MTM_ZERO);
    for(x = 0; x < ncounters; x++)
    {
        state[x].used = true;
    }
}

MTCPUMonitor::~MTCPUMonitor()
{
    mtmemfree(state);
}

void MTCPUMonitor::startslice(int id)
{
    register int x;
    double start;

    mtsyscounterex(&start);
    if (id >= 0)
    {
        register MTCPUState& cstate = state[id];
        cstate.lasttime = cstate.starttime;
        cstate.starttime = start;
        if (cstate.divider == 0.0)
        {
            cstate.count = 0.0;
        }
    }
    else
    {
        for(x = 0; x < n; x++)
        {
            register MTCPUState& cstate = state[x];
            if (!cstate.used)
            {
                continue;
            }
            cstate.lasttime = cstate.starttime;
            cstate.starttime = start;
            if (cstate.divider == 0.0)
            {
                cstate.count = 0.0;
            }
        };
    };
}

void MTCPUMonitor::endslice(int id)
{
    register int x, s, e;
    double end;

    mtsyscounterex(&end);
    if (id < 0)
    {
        s = 0;
        e = n;
    }
    else
    {
        s = id;
        e = id + 1;
    };
    for(x = s; x < e; x++)
    {
        register MTCPUState& cstate = state[x];
        if (!cstate.used)
        {
            continue;
        }
        if (cstate.counting)
        {
            cstate.counting = false;
            cstate.count += end - cstate.start;
        };
        if (cstate.lasttime > 0.0)
        {
            cstate.divider += (cstate.starttime - cstate.lasttime);
        };
    };
}

void MTCPUMonitor::flushcpu(int id)
{
    register int x, s, e;
    double end;

    mtsyscounterex(&end);
    if (id < 0)
    {
        s = 0;
        e = n;
    }
    else
    {
        s = id;
        e = id + 1;
    };
    for(x = s; x < e; x++)
    {
        register MTCPUState& cstate = state[x];
        if (!cstate.used)
        {
            continue;
        }
        if (cstate.counting)
        {
            cstate.counting = false;
            cstate.count += end - cstate.start;
        };
        if (cstate.lasttime > 0.0)
        {
            cstate.divider += (cstate.starttime - cstate.lasttime);
            cstate.cpu = cstate.count / cstate.divider;
            cstate.divider = 0.0;
            cstate.lasttime = 0.0;
        };
    };
    flushid++;
}

void MTCPUMonitor::startadd(int id)
{
    register MTCPUState& cstate = state[id];

    if (cstate.counting)
    {
        return;
    }
    cstate.counting = true;
    mtsyscounterex(&cstate.start);
}

void MTCPUMonitor::endadd(int id)
{
    double end;
    register MTCPUState& cstate = state[id];

    if (!cstate.counting)
    {
        return;
    }
    cstate.counting = false;
    mtsyscounterex(&end);
    cstate.count += end - cstate.start;
}

double MTCPUMonitor::getcpu(int id)
{
    return state[id].cpu;
}

void* MTCPUMonitor::getcpustate(int id)
{
    if (id >= n)
    {
        return 0;
    }
    return &state[id];
}

int MTCPUMonitor::getcpustateid(void* s)
{
    int x;

    for(x = 0; x < n; x++)
    {
        if (&state[x] == state)
        {
            return x;
        }
    };
    return -1;
}

int MTCPUMonitor::addcpustate()
{
    int x;

    for(x = 0; x < n; x++)
    {
        if (!state[x].used)
        {
            mtmemzero(&state[x], sizeof(MTCPUState));
            state[x].used = true;
            return x;
        };
    };
    x = n;
    n += 4;
    state = (MTCPUState*) mtmemrealloc(state, n * sizeof(MTCPUState));
    return x;
}

void MTCPUMonitor::delcpustate(int id)
{
    int x, cn;

    state[id].used = false;
    for(x = n - 1; x >= 0; x--)
    {
        if (state[x].used)
        {
            cn = ((x + 4) >> 2) << 2;
            if (cn < n)
            {
                n = cn;
                state = (MTCPUState*) mtmemrealloc(state, n * sizeof(MTCPUState));
                return;
            };
        };
    };
}
//---------------------------------------------------------------------------
