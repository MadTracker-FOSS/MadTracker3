//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: x86
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSystem1.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "MTSystem1.h"
#include "MTXExtension.h"
#include "MTXSystem2.h"
#include "MTXDisplay.h"
#include "MTXGUI.h"
#include "MTXControls.h"
#ifdef MTSYSTEM_INTERNET
#	include "MTInternet.h"
#endif
#include "MTSystemRES.h"
#ifdef _WIN32
#	include <shellapi.h>
#	include <mmsystem.h>
#	include <imagehlp.h>
#	include <tlhelp32.h>
#else
#	include <setjmp.h>
#	include <signal.h>
#	include <stdlib.h>
#	include <dlfcn.h>
#	include <termios.h>
#	include <execinfo.h>
#	include <sys/utsname.h>
#	include <sys/sysinfo.h>
#endif
//---------------------------------------------------------------------------
static const char *sysname = {"MadTracker System Core"};
static const int sysversion = 0x30000;
static const MTXKey systemkey = {0,0,0,0};
#ifndef MTBUILTIN
	#ifdef MTSYSTEM_EXPORTS
		MTXInterfaces i;
		MTSystemInterface *si;
	#endif
	MTInterface *mtinterface;
	MTGUIInterface *gi;
	MTDisplayInterface *di;
#endif
#ifdef MTSYSTEM_RESOURCES
	MTResources *sysres;
#endif
#ifdef _WIN32
	OSVERSIONINFO osinfo;
	SYSTEM_INFO sysinfo;
	TIMECAPS timecaps;
	DWORD mainthreadid;
	HANDLE mainthreadh;
	BOOL (WINAPI *MTSymCleanup)(HANDLE);
	BOOL (WINAPI *MTSymGetSymFromAddr)(HANDLE,DWORD,LPDWORD,PIMAGEHLP_SYMBOL);
	BOOL (WINAPI *MTStackWalk)(DWORD,HANDLE,HANDLE,LPSTACKFRAME,LPVOID,void*,void*,void*,void*);
	BOOL (WINAPI *MTSymFunctionTableAccess)(HANDLE,DWORD);
	BOOL (WINAPI *MTSymGetModuleBase)(HANDLE,DWORD);
	BOOL (WINAPI *MTSymSetOptions)(DWORD);
	BOOL (WINAPI *MTSymInitialize)(HANDLE,LPSTR,BOOL);
	BOOL (WINAPI *MTSymLoadModule)(HANDLE,HANDLE,LPSTR,LPSTR,DWORD,DWORD);
	HANDLE (WINAPI *MTCreateToolhelp32Snapshot)(DWORD,DWORD);
	BOOL (WINAPI *MTModule32First)(HANDLE,LPMODULEENTRY32);
	BOOL (WINAPI *MTModule32Next)(HANDLE,LPMODULEENTRY32);
#else
	struct timespec timerres;
#endif
MTWindow *auth;
bool debugged = false;
extern char rootn[64];
char dialog[256];
char *d_ok,*d_cancel,*d_yes,*d_no;
#ifdef MTSYSTEM_EXPORTS
//---------------------------------------------------------------------------
//  Linux specific
//---------------------------------------------------------------------------
#ifndef _WIN32
bool badtest = false;
sigjmp_buf gpbuf;

void on_segfault(int n)
{
	if (badtest) siglongjmp(gpbuf,n);
}

bool IsBadReadPtr(const void* ptr,unsigned long size)
{
	void (*prev)(int);

	badtest = true;
	prev = signal(SIGSEGV,on_segfault);
	if (sigsetjmp(gpbuf,1)==0){
		asm ( "rep lodsb"
			:
			:"S"(ptr),"c"(size)
			:"eax"
			);
		badtest = false;
		signal(SIGSEGV,prev);
		return false;
	}
	else{
		badtest = false;
		signal(SIGSEGV,prev);
		return true;
	};
}
#endif
//---------------------------------------------------------------------------
//  Logging
//---------------------------------------------------------------------------
MTFile *logfile;
char logpath[256],logbuf[1024],logdate[128],lastlog[1024];
int lastcount;
int lasttime;
bool logging = false;
bool waserror = false;
bool checklast = true;

void startlog()
{
	char buf[64];

	logfile = mtfileopen(logpath,MTF_READ|MTF_WRITE|MTF_SHAREREAD|MTF_CREATE);
	if (logfile){
		logfile->seek(0,MTF_END);
		logging = true;
		mtlog("== Logging started on %s =="NL,2);
		if (mtinterface){
			mtlog("Application:   ");
			mtlog(mtinterface->name);
			mtflog(" %d.%d.%d",false,mtinterface->version>>16,(mtinterface->version>>8) & 0xFF,mtinterface->version & 0xFF);
		};
		mtlog(NL"Hostname:      ");
		mtlog(si->hostname);
		mtlog(NL"Platform:      ");
		mtlog(si->platform);
		mtlog(" ");
		mtlog(si->build);
		mtlog(NL"Processor(s):  ");
		mtlog(si->processor);
#		ifdef _WIN32
			MEMORYSTATUS mem;
			GlobalMemoryStatus(&mem);
			mtlog(NL"Memory:        Total: ");
			sprintf(buf,"%d MB (%d MB)",mem.dwTotalPhys/1048576,mem.dwTotalPageFile/1048576);
			mtlog(buf);
			mtlog(NL"               Free:  ");
			sprintf(buf,"%d MB (%d MB)"NL,mem.dwAvailPhys/1048576,mem.dwAvailPageFile/1048576);
			mtlog(buf);
			mtlog("Capabilities:  Timer resolution: ");
			sprintf(buf,"%d msec"NL""NL,timecaps.wPeriodMin);
			mtlog(buf);
#		else
			int fd,len;
			sysinfo si;
			if (sysinfo(&si)==0){
				mtlog(NL"Memory:        Total: ");
				sprintf(buf,"%d MB (%d MB)",si.totalram/1048576,mem.totalswap/1048576);
				mtlog(buf);
				mtlog(NL"               Free:  ");
				sprintf(buf,"%d MB (%d MB)"NL,mem.freeram/1048576,mem.freeswap/1048576);
				mtlog(buf);
			};
			mtlog("Capabilities:  Timer resolution: ");
			sprintf(buf,"%f msec"NL""NL,(double)timerres.tv_nsec/1000000);
			mtlog(buf);
#		endif
	};
}

void stoplog()
{
	if (logfile){
		mtlog(NL"== Logging stopped on %s =="NL""NL""NL,2);
		logging = false;
		delete logfile;
		logfile = 0;
	};
}

void mtlog(const char *log,char date)
{
#	ifdef _DEBUG
		bool iserror = false;
#	endif
	bool debugbreak = false;

	if ((!logging) || (!log) || (log[0]==0)) return;
	if (!logfile){
		logging = false;
		startlog();
		if (!logging) return;
	};
	if ((checklast) && (log[0]!='\r') && (log[0]!='\n') && (log[0]!=' ') && (mtsyscounter()-lasttime>3000) && (strcmp(lastlog,log)==0)) lastcount++;
	else{
		if ((checklast) && (lastcount>1)){
			sprintf(logdate,"  WARNING: ^ This message has been repeated %d times."NL""NL,lastcount);
			logfile->write(logdate,strlen(logdate));
		};
		if (strstr(log,"ERROR")){
#			ifdef _DEBUG
				iserror = true;
#			endif
			waserror = true;
			if (debugged) debugbreak = true;
		};
		if (checklast){
			lastcount = 1;
			lasttime = mtsyscounter();
			strcpy(lastlog,log);
		};
		if (date){
			time_t lt;
			tm *lts;
			time(&lt);
			lts = localtime(&lt);
			if (date>1){
				sprintf(logdate,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",lts->tm_year+1900,lts->tm_mon+1,lts->tm_mday,lts->tm_hour,lts->tm_min,lts->tm_sec);
			}
			else{
				sprintf(logdate,"%.2d:%.2d:%.2d (%.8d)",lts->tm_hour,lts->tm_min,lts->tm_sec,mtsyscounter());
			};
			sprintf(logbuf,log,logdate);
#			ifdef _WIN32
				if (debugged) OutputDebugString(logbuf);
#			else
				if (debugged) fputs(logbuf,stdout);
#			endif
			logfile->write(logbuf,strlen(logbuf));
		}
		else{
#			ifdef _WIN32
				if (debugged) OutputDebugString(log);
#			else
				if (debugged) fputs(log,stdout);
#			endif
			logfile->write(log,strlen(log));
		};
#		ifdef _DEBUG
			if (iserror){
				sprintf(logbuf,"  %s"NL,mtgetcallstack());
				logfile->write(logbuf,strlen(logbuf));
			};
#		endif
#		ifdef _WIN32
			if (debugbreak) DebugBreak();
#		endif
	};
}

void mtflog(const char *log,char date,...)
{
	va_list l;
	int dl;
	const char *e;
	static char logdate[32];
	
	e = log;
	if (date){
		while (true){
			e = strchr(e,'%');
			if (!e) return;
			if (*(++e)!='s') continue;
			dl = ((++e)-log);
			memcpy(logdate,log,dl);
			logdate[dl] = 0;
			checklast = false;
			mtlog(logdate,date);
			checklast = true;
			break;
		};
	};
	va_start(l,date);
	vsprintf(logbuf,e,l);
	va_end(l);
	mtlog(logbuf);
}

void mtdump(unsigned char *address,int length,int offset)
{
	unsigned int *cl = (unsigned int*)address;
	unsigned char *e = address+length;
	int x;
	char val[9];

	val[8] = 0;
	offset = 0;
	try{
		while (address<e){
			if (IsBadReadPtr(address,8)==0){
				memcpy(val,address,8);
				for (x=0;x<8;x++){
					if (val[x]<' ') val[x] = '.';
				};
				mtflog("  %.8X (+%.4X): %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X - %.8X %.8X ; %s"NL,false,address,offset,address[0],address[1],address[2],address[3],address[4],address[5],address[6],address[7],cl[0],cl[1],val);
				address += 8;
				cl += 2;
			}
			else{
				mtflog("  %.8X (+%.4X): ?? ?? ?? ?? ?? ?? ?? ?? - ???????? ???????? ; ????????"NL,false,address,offset);
				address += 8;
				cl += 2;
			};
			offset += 8;
		};
	}
	catch(...){
	};
}
//---------------------------------------------------------------------------
//  Call Stack
//---------------------------------------------------------------------------
void mtenter(const char *func)
{
#	ifdef _DEBUG
		int l;
		bool first;
		char *callstack = (char*)mtgetprivatedata(-5);
		char *csp = (char*)mtgetprivatedata(-4);
		int lostcount = (int)mtgetprivatedata(-3);

		if (!callstack) return;
		l = strlen(func);
		first = (csp==&callstack[MAX_STACK-1]);
		if (!first){
			if (csp-3-l<callstack){
				mtsetprivatedata(-3,(void*)(lostcount+1));
				return;
			};
			csp -= 3;
			memcpy(csp," < ",3);
		}
		else if (csp-l<callstack){
			mtsetprivatedata(-3,(void*)(lostcount+1));
			return;
		};
		csp -= l;
		memcpy(csp,func,l);
		mtsetprivatedata(-4,csp);
#	endif
}

void mtfenter(const char *func,...)
{
#	ifdef _DEBUG
		int x;
		bool first;
		char *callstack = (char*)mtgetprivatedata(-5);
		char *csp = (char*)mtgetprivatedata(-4);
		int lostcount = (int)mtgetprivatedata(-3);
		va_list l;
		static char buf[256];

		if (!callstack) return;
		va_start(l,func);
		vsprintf(buf,func,l);
		va_end(l);
		x = strlen(buf);
		first = (csp==&callstack[MAX_STACK-1]);
		if (!first){
			if (csp-3-x<callstack){
				mtsetprivatedata(-3,(void*)(lostcount+1));
				return;
			};
			csp -= 3;
			memcpy(csp," < ",3);
		}
		else if (csp-x<callstack){
				mtsetprivatedata(-3,(void*)(lostcount+1));
			return;
		};
		csp -= x;
		memcpy(csp,buf,x);
		mtsetprivatedata(-4,csp);
#	endif
}

void mtleave()
{
#	ifdef _DEBUG
		char *e;
		char *callstack = (char*)mtgetprivatedata(-5);
		char *csp = (char*)mtgetprivatedata(-4);
		int lostcount = (int)mtgetprivatedata(-3);

		if (!callstack) return;
		if (lostcount){
			mtsetprivatedata(-3,(void*)(lostcount-1));
			return;
		};
		e = strchr(csp,'<');
		if (e){
			csp = e+2;
		}
		else{
			csp = &callstack[MAX_STACK-1];
		};
		mtsetprivatedata(-4,csp);
#	endif
}

const char* mtgetcallstack()
{
#	ifdef _DEBUG
		return (const char*)mtgetprivatedata(-4);
#	else
		return "N/A (Use debug build instead.)";
#	endif
}

void mtgetlibmemoryrange(void *lib,int flags,void **start,int *length)
{
	if ((!start) || (!length)) return;
	*start = lib;
	*length = 0;
#	ifdef _WIN32
		MEMORY_BASIC_INFORMATION meminfo;
		void *end;
		end = lib;
		while (VirtualQuery(end,&meminfo,sizeof(meminfo))){
			if ((meminfo.AllocationBase==0) || (meminfo.State!=MEM_COMMIT)) break;
			end = (char*)end+meminfo.RegionSize;
		};
		*length = (char*)end-(char*)lib;
#	endif
}
//---------------------------------------------------------------------------
//  Exception Handling
//---------------------------------------------------------------------------
#ifdef _WIN32
static struct{
	unsigned int ecode;
	char *emessage;
} _e[20] = {
	{EXCEPTION_ACCESS_VIOLATION,"Access violation"},
	{EXCEPTION_ARRAY_BOUNDS_EXCEEDED,"Array out of bounds"},
	{EXCEPTION_BREAKPOINT,"Breakpoint"},
	{EXCEPTION_DATATYPE_MISALIGNMENT,"Data misalignment"},
	{EXCEPTION_FLT_DENORMAL_OPERAND,"FP Denormal operand"},
	{EXCEPTION_FLT_DIVIDE_BY_ZERO,"FP Divide by zero"},
	{EXCEPTION_FLT_INEXACT_RESULT,"FP Inexact result"},
	{EXCEPTION_FLT_INVALID_OPERATION,"FP Invalid operation"},
	{EXCEPTION_FLT_OVERFLOW,"FP Overflow"},
	{EXCEPTION_FLT_STACK_CHECK,"FP Stack check"},
	{EXCEPTION_FLT_UNDERFLOW,"FP Underflow"},
	{EXCEPTION_ILLEGAL_INSTRUCTION,"Illegal instruction"},
	{EXCEPTION_IN_PAGE_ERROR,"In page error"},
	{EXCEPTION_INT_DIVIDE_BY_ZERO,"Divide by zero"},
	{EXCEPTION_INT_OVERFLOW,"Overflow"},
	{EXCEPTION_INVALID_DISPOSITION,"Invalid disposition"},
	{EXCEPTION_NONCONTINUABLE_EXCEPTION,"Noncontinuable exception"},
	{EXCEPTION_PRIV_INSTRUCTION,"Privilege instruction"},
	{EXCEPTION_STACK_OVERFLOW,"Stack overflow"},
	{0,"Unknown exception"}
};
char errorbuf[1024];

void getmodule(void *addr,char *buf)
{
	MEMORY_BASIC_INFORMATION mbi;
	char tmp[MAX_PATH+1];
	char *e;

	VirtualQuery(addr,&mbi,sizeof(mbi));
	if ((&MTSymLoadModule) && (GetModuleFileName(HINSTANCE(mbi.AllocationBase),tmp,MAX_PATH)>0)){
		e = strrchr(tmp,'\\');
		if (!e) e = tmp;
		else e++;
		strcpy(buf,e);
		MTSymLoadModule(GetCurrentProcess(),0,tmp,0,DWORD(mbi.AllocationBase),mbi.RegionSize);
	}
	else strcpy(buf,"<Unknown>");
}

void stackwalk(HANDLE hthread,CONTEXT *ctx)
{
	STACKFRAME sf;
	int count = 0;
	unsigned long so = 0;
	IMAGEHLP_SYMBOL *sym;
	char cmod[256];

	mtmemzero(&sf,sizeof(sf));
	sf.AddrPC.Offset = (long)ctx->Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = (long)ctx->Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = (long)ctx->Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sym = (IMAGEHLP_SYMBOL*)mtmemalloc(sizeof(IMAGEHLP_SYMBOL)+1024);
	while (MTStackWalk(IMAGE_FILE_MACHINE_I386,GetCurrentProcess(),hthread,&sf,ctx,0,MTSymFunctionTableAccess,MTSymGetModuleBase,0)){
		mtmemzero(sym,sizeof(IMAGEHLP_SYMBOL));
		sym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		sym->MaxNameLength = 1024;
		if (sf.AddrPC.Offset){
			getmodule((void*)sf.AddrPC.Offset,cmod);
			if (MTSymGetSymFromAddr(GetCurrentProcess(),sf.AddrPC.Offset,&so,sym)){
				mtflog("  %08X: %16s %s + %d"NL,false,sf.AddrPC.Offset,cmod,sym->Name,so);
			}
			else{
				mtflog("  %08X: %16s (No symbolic information)"NL,false,sf.AddrPC.Offset,cmod);
			};
		}
		else{
			mtflog("  (Error %d)"NL,false,GetLastError());
		};
		if (++count>=32) break;
	};
	mtmemfree(sym);
}

int WINAPI onexception(EXCEPTION_POINTERS *ep)
{
	static char *readwrite[2] = {"read","write"};

	if (GetAsyncKeyState(VK_SHIFT)<0) return EXCEPTION_EXECUTE_HANDLER;
	if (ep){
		CONTEXT ctx,&c = *ep->ContextRecord;
		int x;
		unsigned char *cip = (unsigned char*)c.Eip;
		unsigned char *cbp = (unsigned char*)c.Ebp;
		char *cecode;
		char cext[256];
		bool waslogging = logging;

		if (!logging) startlog();
		cecode = _e[19].emessage;
		for (x=0;x<19;x++){
			if (ep->ExceptionRecord->ExceptionCode==_e[x].ecode){
				cecode = _e[x].emessage;
				break;
			};
		};
		getmodule(ep->ExceptionRecord->ExceptionAddress,cext);
		mtflog("%s - [EXCEPTION] %s at address %.8X in %s"NL,true,cecode,(char*)ep->ExceptionRecord->ExceptionAddress,cext);
		if ((x==0) && (ep->ExceptionRecord->NumberParameters>=2)){
			mtflog("Cannot %s memory at address %.8X"NL""NL,false,readwrite[ep->ExceptionRecord->ExceptionInformation[0]],ep->ExceptionRecord->ExceptionInformation[1]);
		};
		mtflog("Context:"NL"  EAX: %.8X  ESI: %.8X"NL"\
  EBX: %.8X  EDI: %.8X"NL"\
  ECX: %.8X  ESP: %.8X"NL"\
  EDX: %.8X  EBP: %.8X"NL"\
  EIP: %.8X"NL""NL,false,c.Eax,c.Esi,c.Ebx,c.Edi,c.Ecx,c.Esp,c.Edx,c.Ebp,c.Eip);
		if (IsBadReadPtr(cip,16)==0){
			mtlog("Bytes at EIP:"NL);
			mtflog("  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X ",false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
			cip += 8;
			mtflog("  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X"NL""NL,false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
		};
		if (&MTSymInitialize){
			mtlog("Stack trace:"NL);
			MTSymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES|SYMOPT_CASE_INSENSITIVE|SYMOPT_UNDNAME);
			if (MTSymInitialize(GetCurrentProcess(),0,true)){
				MTSymSetOptions(SYMOPT_LOAD_LINES|SYMOPT_CASE_INSENSITIVE|SYMOPT_UNDNAME);
				stackwalk(GetCurrentThread(),&c);
				if (GetCurrentThreadId()!=mainthreadid){
					SuspendThread(mainthreadh);
					mtmemzero(&ctx,sizeof(ctx));
					ctx.ContextFlags = CONTEXT_FULL;
					if (GetThreadContext(mainthreadh,&ctx)){
						ResumeThread(mainthreadh);
						mtlog(NL"Main thread stack trace:"NL);
						stackwalk(mainthreadh,&ctx);
					}
					else ResumeThread(mainthreadh);
				};
				MTSymCleanup(GetCurrentProcess());
			}
			else mtflog("  ERRO: SymInitialize failed with error %d!",false,GetLastError());
			mtlog(NL);
		};
/*
		if (&MTCreateToolhelp32Snapshot){
			mtlog("Modules:"NL);
			HANDLE hsnap = MTCreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
			if (hsnap!=INVALID_HANDLE_VALUE){
				MODULEENTRY32 minfo;
				minfo.dwSize = sizeof(MODULEENTRY32);
				if (MTModule32First(hsnap,&minfo)){
					while (true){
						if (minfo.dwSize==sizeof(MODULEENTRY32)){
							mtflog("  %08x: %s"NL,false,minfo.modBaseAddr,minfo.szModule);
						};
						minfo.dwSize = sizeof(MODULEENTRY32);
						if (!MTModule32Next(hsnap,&minfo)) break;
					};
				};
				CloseHandle(hsnap);
			}
			else mtflog("  ERRO: CreateToolhelp32Snapshot failed with error %d!",false,GetLastError());
			mtlog(NL);
		};
*/
		if (!waslogging) stoplog();
	};
	return (debugged)?EXCEPTION_CONTINUE_SEARCH:EXCEPTION_EXECUTE_HANDLER;
}
#else
#define SIG_MAX_FRAMES 64

void onsignal(int sig,siginfo_t *info,void *context)
{
	static char *_sigs[32] = {0,"SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM","SIGSTKFLT","SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP","SIGTTIN","SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH","SIGIO","SIGPWR","SIGSYS"};
	static void *_rets[SIG_MAX_FRAMES];
	static int nsignals;

	int x,n;
	char **symbols;
	char *e;
	mcontext_t &ctx = ((ucontext_t*)context)->uc_mcontext;
	unsigned char *cip = (unsigned char*)ctx.gregs[REG_EIP];
	bool btok = false;

	if (++nsignals>10){
		mtlog("ERROR: More than 10 signals have been sent. Exiting."NL);
		sigaction(SIGSEGV,0,0);
		sigaction(SIGBUS,0,0);
		sigaction(SIGILL,0,0);
		sigaction(SIGABRT,0,0);
		sigaction(SIGFPE,0,0);
		abort();
		return;
	};
	n = backtrace(_rets,SIG_MAX_FRAMES);
	mtflog("%s - [SIGNAL] Received signal %s with code %d at address %08X"NL"Callstack:"NL"  %s"NL""NL,true,_sigs[sig & 31],info->si_code,cip,mtgetcallstack());
	mtflog("Context:"NL"  EAX: %.8X  ESI: %.8X"NL"\
  EBX: %.8X  EDI: %.8X"NL"\
  ECX: %.8X  ESP: %.8X"NL"\
  EDX: %.8X  EBP: %.8X"NL"\
  EIP: %.8X"NL""NL,false,ctx.gregs[REG_EAX],ctx.gregs[REG_ESI],ctx.gregs[REG_EBX],ctx.gregs[REG_EDI],ctx.gregs[REG_ECX],ctx.gregs[REG_ESP],ctx.gregs[REG_EDX],ctx.gregs[REG_EBP],ctx.gregs[REG_EIP]);
/*
	if (IsBadReadPtr(cip,16)==0){
		mtlog("Bytes at EIP:"NL);
		mtflog("  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X ",false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
		cip += 8;
		mtflog("  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X"NL""NL,false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
	};
*/
	mtflog("Stack trace:"NL);
	symbols = backtrace_symbols(_rets,n);
	if (symbols){
		for (x=0;x<n;x++){
			if (_rets[x]==cip) btok = true;
			if (!btok) continue;
			e = strrchr(symbols[x],'/');
			if (e) e++;
			else e = symbols[x];
			mtflog("  %08X: %s"NL,false,_rets[x],e);
		};
		free(symbols);
	}
	else{
		for (x=0;x<n;x++){
			if (_rets[x]==cip) btok = true;
			if (!btok) continue;
			mtflog("  %08X"NL,false,_rets[x]);
		};
	};
	mtlog(NL,false);
	mtsigreturn(sig);
	mtlog("ERROR: Signal cannot be handled. Exiting."NL);
	signal(sig,SIG_DFL);
	if (sig!=SIGABRT) signal(SIGABRT,SIG_DFL);
	abort();
}
#endif
#endif
//---------------------------------------------------------------------------
//  Memory Debugging
//---------------------------------------------------------------------------
#ifdef _DEBUG
unsigned int nallocs,nmallocs,totalmemory,peakmemory;
MTLock memlock;
struct MTAlloc{
	void *address;
	int size;
	void *caller;
	char *callstack;
} *mtalloc;

MTAlloc *addalloc()
{
	if ((!mtalloc) || (nmallocs<=0)){
		nmallocs = 1024;
#		ifdef _WIN32
			mtalloc = (MTAlloc*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nmallocs*sizeof(MTAlloc));
#		else
			mtalloc = (MTAlloc*)calloc(1,nmallocs*sizeof(MTAlloc));
#		endif
	}
	else if (nallocs>=nmallocs){
		nmallocs += 64;
#		ifdef _WIN32
			mtalloc = (MTAlloc*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,mtalloc,nmallocs*sizeof(MTAlloc));
#		else
			mtalloc = (MTAlloc*)realloc(mtalloc,nmallocs*sizeof(MTAlloc));
#		endif
	};
	return &mtalloc[nallocs++];
}

void delalloc(unsigned int id)
{
	if (mtalloc[id].callstack){
#		ifdef _WIN32
			HeapFree(GetProcessHeap(),0,mtalloc[id].callstack);
#		else
			free(mtalloc[id].callstack);
#		endif
	};
	if (id!=nallocs-1) memcpy(&mtalloc[id],&mtalloc[nallocs-1],sizeof(MTAlloc));
	mtmemzero(&mtalloc[nallocs-1],sizeof(MTAlloc));
	nallocs--;
	if ((nallocs<nmallocs-64) && (nmallocs>1024)){
		nmallocs -= 64;
		if (nmallocs>0){
#			ifdef _WIN32
				mtalloc = (MTAlloc*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,mtalloc,nmallocs*sizeof(MTAlloc));
#			else
				mtalloc = (MTAlloc*)realloc(mtalloc,nmallocs*sizeof(MTAlloc));
#			endif
		}
		else{
#			ifdef _WIN32
				HeapFree(GetProcessHeap(),0,mtalloc);
#			else
				free(mtalloc);
#			endif
			mtalloc = 0;
		};
	};
}
#endif
//---------------------------------------------------------------------------
void* mtmemalloc(int size,int flags)
{
	void *mem;
	
#	ifdef _DEBUG
		if (size>0x2000000){
			FLOGD1("%s - [System] ERROR: Too big memory allocation! (%d)"NL,size);
			return 0;
		};
#	endif
#	ifdef _WIN32
		mem = HeapAlloc(GetProcessHeap(),(flags & MTM_ZERO)?HEAP_ZERO_MEMORY:0,size);
#	else
		if (flags & MTM_ZERO) mem = calloc(1,size);
		else mem = malloc(size);
#	endif
#	ifdef _DEBUG
		if (mem){
			const char *cs;
			memlock.lock();
			totalmemory += size;
			if (totalmemory>peakmemory) peakmemory = totalmemory;
			MTAlloc &calloc = *addalloc();
			calloc.address = mem;
			calloc.size = size;
			calloc.caller = *((void**)(((char*)(&size))-4));
			calloc.callstack = 0;
#			ifdef MTSYSTEM_EXPORTS
				cs = mtgetcallstack();
				if ((cs) && (strlen(cs)>0)){
#					ifdef _WIN32
						calloc.callstack = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,strlen(cs)+1);
#					else
						calloc.callstack = (char*)malloc(strlen(cs)+1);
#					endif
					strcpy(calloc.callstack,cs);
				};
#			endif
			memlock.unlock();
		}
		else{
			FLOGD1("%s - [System] ERROR: Cannot allocate memory! (Requested size: %d)"NL,size);
		};
#	endif
	return mem;
}

bool mtmemfree(void *mem)
{
	if (mem){
#		ifdef _DEBUG
			bool ok = false;
			unsigned int x;
			memlock.lock();
			for (x=0;x<nallocs;x++){
				if (mtalloc[x].address==mem){
					totalmemory -= mtalloc[x].size;
					ok = true;
					delalloc(x);
					break;
				};
			};
			memlock.unlock();
			if (!ok){
				LOGD("%s - [System] ERROR: Invalid memory pointer!"NL);
				return false;
			};
#		endif
#		ifdef _WIN32
			return (HeapFree(GetProcessHeap(),0,mem)==0);
#		else
			free(mem);
#		endif
	};
	return true;
}

void* mtmemrealloc(void *mem,int size)
{
	void* newmem;
	
	if (!mem) return mtmemalloc(size,MTM_ZERO);
#	ifdef _WIN32
		HANDLE ph = GetProcessHeap();
		int csize = HeapSize(ph,0,mem);
		if (size<=csize) return mem;
		newmem = HeapReAlloc(ph,HEAP_ZERO_MEMORY,mem,size);
#	else
		newmem = realloc(mem,size);
#	endif
	if (!newmem){
		mtshowlastoserror();
#		ifdef _WIN32
			newmem = HeapAlloc(ph,HEAP_ZERO_MEMORY,size);
			if (!newmem) mtshowlastoserror();
			else{
				memcpy(newmem,mem,HeapSize(ph,0,mem));
				HeapFree(ph,0,mem);
			};
#		endif
	};
#	ifdef _DEBUG
		unsigned int x;
		memlock.lock();
		for (x=0;x<nallocs;x++){
			if (mtalloc[x].address==mem){
				totalmemory -= mtalloc[x].size;
				mtalloc[x].address = newmem;
				mtalloc[x].size = size;
				totalmemory += size;
				if (totalmemory>peakmemory) peakmemory = totalmemory;
				break;
			};
		};
		memlock.unlock();
#	endif
	return (void*)newmem;
}
//---------------------------------------------------------------------------
bool MTCT DialogProc(MTWinControl *window,MTCMessage &msg)
{
	if (msg.msg==MTCM_TIMER){
		window->modalresult = MTDR_TIMEOUT;
		MTCMessage msg = {MTCM_CLOSE,0,window};
		window->message(msg);
		return true;
	}
	else if (msg.msg==MTCM_CLOSE){
		gi->deltimer(window,window->tag);
	};
	return false;
}

#ifndef _WIN32
char mtgetchar()
{
	struct termios ot,t;
	int c;

	tcgetattr(fileno(stdin),&ot);
	t = ot;
	t.c_lflag &= (~ICANON);
	tcsetattr(fileno(stdin),TCSANOW,&t);
	c = fgetc(stdin);
	tcsetattr(fileno(stdin),TCSANOW,&ot);
	return (char)c;
}
#endif

int mtdialog(char *message,char *caption,char *buttons,int flags,int timeout)
{
#ifdef MTSYSTEM_RESOURCES
	char *e;
	MTDesktop *dsk;
	int x,y,l,t,h,r,size,cbutton;
	char buf[128];
	if ((sysres) && ((dsk = (MTDesktop*)di->getdefaultdesktop())) && ((dsk->flags & MTCF_HIDDEN)==0)){
		MTFile *wf = sysres->getresourcefile(MTR_WINDOW,MTW_dialog,&size);
		if (wf){
			MTLabel *c;
			MTIcon *i;
			MTButton *ob;
			MTButton *b[8];
			MTWindow *alert = gi->loadwindowfromfile(wf,size,dsk);
			sysres->releaseresourcefile(wf);
			alert->messageproc = DialogProc;
			alert->flags |= MTCF_FREEONCLOSE;
			c = (MTLabel*)alert->getcontrolfromuid(MTC_ltext);
			i = (MTIcon*)alert->getcontrolfromuid(MTC_icon);
			mtmemzero(b,sizeof(b));
			ob = (MTButton*)alert->getcontrolfromuid(MTC_b1);
			if ((!c) || (!ob)){
				gi->delcontrol(alert);
				return MTDR_NULL;
			};
			alert->setcaption(caption);
			if (i) i->icon = flags & MTD_ICONMASK;
			if (((int)buttons<256) && (dialog[0]==0)){
				if (sysres->loadresource(MTR_TEXT,MTT_dialog,dialog,sizeof(dialog))){
					d_ok = dialog;
					d_cancel = strchr(d_ok,'|');
					if (d_cancel){
						*d_cancel++ = 0;
						d_yes = strchr(d_cancel,'|');
						if (d_yes){
							*d_yes++ = 0;
							d_no = strchr(d_yes,'|');
							if (d_no) *d_no++ = 0;
							else dialog[0] = 1;
						}
						else dialog[0] = 1;
					}
					else dialog[0] = 1;
				}
				else{
					dialog[0] = 1;
				};
			};
			if (dialog[0]==1){
				gi->delcontrol(alert);
				return MTDR_NULL;
			};
			cbutton = (flags & MTD_BUTTONMASK)>>4;
			l = ob->left;
			t = ob->top;
			h = ob->height;
			r = l+ob->width;
			ob->switchflags(MTCF_HIDDEN,true);
			switch ((int)buttons){
			case (int)MTD_OK:
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7]->setcaption(d_ok);
				b[7]->modalresult = 0;
				alert->focus(b[7]);
				break;
			case (int)MTD_OKCANCEL:
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6]->setcaption(d_ok);
				b[7]->setcaption(d_cancel);
				b[6]->modalresult = 0;
				b[7]->modalresult = 1;
				if (cbutton>1) cbutton = 1;
				alert->focus(b[6+cbutton]);
				break;
			case (int)MTD_YESNO:
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6]->setcaption(d_yes);
				b[7]->setcaption(d_no);
				b[6]->modalresult = 0;
				b[7]->modalresult = 1;
				if (cbutton>1) cbutton = 1;
				alert->focus(b[6+cbutton]);
				break;
			case (int)MTD_YESNOCANCEL:
				b[5] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[5]->setcaption(d_yes);
				b[6]->setcaption(d_no);
				b[7]->setcaption(d_cancel);
				b[5]->modalresult = 0;
				b[6]->modalresult = 1;
				b[7]->modalresult = 2;
				if (cbutton>2) cbutton = 2;
				alert->focus(b[5+cbutton]);
				break;
			default:
				strcpy(buf,buttons);
				for (x=7;x>=0;x--){
					e = strrchr(buf,'|');
					if (!e) e = buf;
					else *e++ = 0;
					b[x] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
					b[x]->modalresult = x;
					if (e==buf) break;
				};
				y = x;
				e = buf;
				if (cbutton>7-y) cbutton = 7-y;
				alert->focus(b[y+cbutton]);
				for (x=y;x<8;x++){
					b[x]->setcaption(e);
					b[x]->modalresult -= y;
					e = strchr(e,0)+1;
				};
				break;
			};
			r += 4;
			for (x=7;x>=0;x--){
				if (!b[x]) break;
				r -= b[x]->width+4;
				b[x]->switchflags(MTCF_DONTSAVE,true);
				b[x]->setbounds(r,t,0,0);
				b[x]->align = MTCA_BOTTOMRIGHT;
			};
			if (r<l) r = alert->width+l-r;
			else r = 0;
			c->setcaption(message);
			if (alert->width>r) r = 0;
			h = alert->height;
			if (h<96) h = 96;
			alert->setbounds((dsk->width-alert->width)/2,(dsk->height-alert->height)/2,r,h);
			alert->modalresult = MTDR_CANCEL;
			if (timeout>0) alert->tag = gi->ctrltimer(alert,0,timeout,false);
			return dsk->show(alert,((flags & MTD_MODAL)!=0)?1:0);
		};
		return MTDR_NULL;
	}
	else if ((int)buttons>=256) return MTDR_NULL;
	else
#endif
	{
#		ifdef _WIN32
			int wflags = 0;
			int res;
			switch ((int)buttons){
			case (int)MTD_OK:
				wflags = MB_OK;
				break;
			case (int)MTD_OKCANCEL:
				wflags = MB_OKCANCEL;
				break;
			case (int)MTD_YESNO:
				wflags = MB_YESNO;
				break;
			case (int)MTD_YESNOCANCEL:
				wflags = MB_YESNOCANCEL;
				break;
			};
			switch (flags & MTD_ICONMASK){
			case MTD_INFORMATION:
				wflags |= MB_ICONINFORMATION;
				break;
			case MTD_QUESTION:
				wflags |= MB_ICONQUESTION;
				break;
			case MTD_EXCLAMATION:
				wflags |= MB_ICONEXCLAMATION;
				break;
			case MTD_ERROR:
				wflags |= MB_ICONERROR;
				break;
			};
			if (flags & MTD_BUTTONMASK){
				wflags |= (flags & MTD_BUTTONMASK)<<4;
			};
			res = MessageBox(0,message,caption,wflags);
			if ((res==IDOK) || (res==IDYES) || (res==IDABORT)) return 0;
			if (res==IDNO) return 1;
			if (buttons==MTD_YESNOCANCEL) return 2;
			return 1;
#		else
//TODO More than console
			int c,ret;
			ret = MTDR_NULL;
			fprintf(stdout,"-- %s --"NL"%s"NL"--"NL,caption,message);
			switch ((int)buttons){
			case (int)MTD_OKCANCEL:
				fputs("[Ok][Cancel]?",stdout);
				while (true){
					c = mtgetchar();
					if ((c=='o') || (c=='O')){
						ret = 0;
						break;
					}
					else if ((c=='c') || (c=='C')){
						ret = 1;
						break;
					};
				};
				break;
			case (int)MTD_YESNO:
				fputs("[Yes][No]?",stdout);
				while (true){
					c = mtgetchar();
					if ((c=='y') || (c=='Y')){
						ret = 0;
						break;
					}
					else if ((c=='n') || (c=='N')){
						ret = 1;
						break;
					};
				};
				break;
			case (int)MTD_YESNOCANCEL:
				fputs("[Yes][No][Cancel]?",stdout);
				while (true){
					c = mtgetchar();
					if ((c=='y') || (c=='Y')){
						ret = 0;
						break;
					}
					else if ((c=='n') || (c=='N')){
						ret = 1;
						break;
					}
					else{
						ret = 2;
						break;
					};
				};
				break;
			default:
				fputs("Press any key to continue...",stdout);
				mtgetchar();
				ret = 0;
				break;
			};
			fprintf(stdout,NL);
			return ret;
#		endif
	};
}

#ifdef MTSYSTEM_RESOURCES
int mtresdialog(MTResources *res,int id,char *caption,char *buttons,int flags,int timeout,...)
{
	int ret;
	va_list l;
	char *buf = (char*)mtmemalloc(1024);
	char *buffer = (char*)mtmemalloc(512);
	
	res->loadstring(id,buffer,1024);
	va_start(l,timeout);
	vsprintf(buf,buffer,l);
	va_end(l);
	mtmemfree(buffer);
	ret = mtdialog(buf,caption,buttons,flags,timeout);
	mtmemfree(buf);
	return ret;
}

int mtauthdialog(char *message,char *login,char *password)
{
	MTDesktop *dsk;
	MTLabel *l;
	MTEdit *elogin,*epassword;
	int size;
	
	if (!auth){
		if ((sysres) && ((dsk = (MTDesktop*)di->getdefaultdesktop()))){
			MTFile *wf = sysres->getresourcefile(MTR_WINDOW,MTW_auth,&size);
			if (wf){
				auth = gi->loadwindowfromfile(wf,size,dsk);
				if ((int)auth<=0) return -1;
				auth->setbounds((dsk->width-auth->width)/2,(dsk->height-auth->height)/2,0,0);
				sysres->releaseresourcefile(wf);
			};
		};
	};
	dsk = auth->dsk;
	if ((auth) && ((dsk->flags & MTCF_HIDDEN)==0)){
		l = (MTLabel*)auth->getcontrolfromuid(MTC_lcaption);
		if (l) l->setcaption(message);
		elogin = (MTEdit*)auth->getcontrolfromuid(MTC_elogin);
		epassword = (MTEdit*)auth->getcontrolfromuid(MTC_epassword);
		if (!login) elogin->switchflags(MTCF_DISABLED,true);
		if (!password) epassword->switchflags(MTCF_DISABLED,true);
		auth->modalresult = MTDR_CANCEL;
		if (dsk->show(auth,1)==0){
			if (login) strcpy(login,elogin->text);
			if (password) strcpy(password,epassword->text);
			return 0;
		};
	};
	return -1;
}
#endif

void mtshowoserror(int error)
{
	char *message;

#	ifdef _WIN32
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,error,0,(char*)&message,16,0);
#	else
		message = strerror(error);
#	endif
	LOGD("%s - [System] ERROR: ");
	LOG(message);
	LOG(NL);
	mtdialog(message,"System Error",MTD_OK,MTD_EXCLAMATION,0);
}

void mtshowlastoserror()
{
#	ifdef _WIN32
		mtshowoserror(GetLastError());
#	else
		mtshowoserror(errno);
#	endif
}
//---------------------------------------------------------------------------
int mtsync_inc(int *value)
{
#	ifdef _WIN32
		return InterlockedIncrement((long*)value);
#	else
		int r;
		asm ("\
			movl %[value],%%ecx\n\
			movl $1,%%eax\n\
			lock xaddl %%eax,(%%ecx)\n\
			incl %%eax\n\
			"
			:"=a"(r)
			:[value]"m"(value)
			:"ecx"
			);
		return r;
#	endif
}

int mtsync_dec(int *value)
{
#	ifdef _WIN32
		return InterlockedDecrement((long*)value);
#	else
		int r;
		asm ("\
			movl %[value],%%ecx\n\
			mov $-1,%%eax\n\
			lock xaddl %%eax,(%%ecx)\n\
			dec %%eax\n\
			"
			:"=a"(r)
			:[value]"m"(value)
			:"ecx"
			);
		return r;
#	endif
}
//---------------------------------------------------------------------------
#ifdef MTSYSTEM_EXPORTS
MTSystemInterface::MTSystemInterface():
sysflags(0),
ncpu(1),
cpufrequ(0),
memalloc(mtmemalloc),
memfree(mtmemfree),
memrealloc(mtmemrealloc),
getsysthread(mtgetsysthread),
getcurrentthread(mtgetcurrentthread),
issysthread(mtissysthread),
setprivatedata(mtsetprivatedata),
getprivatedata(mtgetprivatedata),
threadcreate(mtthreadcreate),
processcreate(mtprocesscreate),
fileopen(mtfileopen),
fileclose(mtfileclose),
fileexists(mtfileexists),
filecopy(mtfilecopy),
filedelete(mtfiledelete),
filerename(mtfilerename),
filetype(mtfiletype),
filemaketemp(mtfilemaketemp),
folderopen(mtfolderopen),
folderclose(mtfolderclose),
arraycreate(mtarraycreate),
arraydelete(mtarraydelete),
hashcreate(mthashcreate),
hashdelete(mthashdelete),
#ifdef MTSYSTEM_RESOURCES
	resfind(mtresfind),
	resopen(mtresopen),
	resclose(mtresclose),
#endif
#ifdef MTSYSTEM_CONFIG
	configfind(mtconfigfind),
	configopen(mtconfigopen),
	configclose(mtconfigclose),
#endif
#ifdef MTSYSTEM_MINICONFIG
	miniconfigcreate(mtminiconfigcreate),
	miniconfigdelete(mtminiconfigdelete),
#endif
lockcreate(mtlockcreate),
lockdelete(mtlockdelete),
eventcreate(mteventcreate),
eventdelete(mteventdelete),
timercreate(mttimercreate),
timerevent(mttimerevent),
timerdelete(mttimerdelete),
cpumonitorcreate(mtcpumonitorcreate),
syscounter(mtsyscounter),
syscounterex(mtsyscounterex),
syswait(mtsyswait),
//syswaitmultiple(mtsyswaitmultiple),
dialog(mtdialog),
#ifdef MTSYSTEM_RESOURCES
	resdialog(mtresdialog),
	authdialog(mtauthdialog),
#endif
showoserror(mtshowoserror),
showlastoserror(mtshowlastoserror),
log(mtlog),
flog(mtflog),
dump(mtdump),
enter(mtenter),
fenter(mtfenter),
leave(mtleave),
getcallstack(mtgetcallstack),
getlibmemoryrange(mtgetlibmemoryrange),
sync_inc(mtsync_inc),
sync_dec(mtsync_dec)
{
	type = systemtype;
	key = &systemkey;
	name = sysname;
	version = sysversion;
	status = 0;
}

bool MTSystemInterface::init()
{
#ifdef MTSYSTEM_CONFIG
	MTFile *f;
	MTConfigFile *conf;
#endif
	char *e;
	tm *lts;
	time_t lt;
	char lorev,hirev;
	unsigned int cpuflags,cpuflags2,cpuver;
	bool savecpu = true;
	char buf[256],proctype[256];
	static int cpu_wait = 500;
	static double cpu_div = 500000.0;
#ifdef _WIN32
#else
	int fd,len;
	utsname un;
#endif

//fprintf(stderr,"ERROR CHECK: %d"NL,IsBadReadPtr((void*)0,128));
#	ifdef _WIN32
		onerror = onexception;
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)::onexception);
		mainthreadid = GetCurrentThreadId();
		DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&mainthreadh,0,false,DUPLICATE_SAME_ACCESS);
		HMODULE hkernel = GetModuleHandle("KERNEL32.DLL");
		HMODULE hdbg = LoadLibrary("DBGHELP.DLL");
		if (hkernel){
			*(int*)&MTCreateToolhelp32Snapshot = (int)GetProcAddress(hkernel,"CreateToolhelp32Snapshot");
			*(int*)&MTModule32First = (int)GetProcAddress(hkernel,"Module32First");
			*(int*)&MTModule32Next = (int)GetProcAddress(hkernel,"Module32Next");
			BOOL (*isdebugger)();
			*(int*)&isdebugger = (int)GetProcAddress(hkernel,"IsDebuggerPresent");
			if (isdebugger){
				debugged = (isdebugger()!=0);
				if (GetAsyncKeyState(VK_SHIFT)>=0){
					if (debugged) sysflags |= MTS_DEBUGGED;
				};
			};
		};
		if (hdbg){
			*(int*)&MTSymCleanup = (int)GetProcAddress(hdbg,"SymCleanup");
			*(int*)&MTSymGetSymFromAddr = (int)GetProcAddress(hdbg,"SymGetSymFromAddr");
			*(int*)&MTStackWalk = (int)GetProcAddress(hdbg,"StackWalk");
			*(int*)&MTSymFunctionTableAccess = (int)GetProcAddress(hdbg,"SymFunctionTableAccess");
			*(int*)&MTSymGetModuleBase = (int)GetProcAddress(hdbg,"SymGetModuleBase");
			*(int*)&MTSymSetOptions = (int)GetProcAddress(hdbg,"SymSetOptions");
			*(int*)&MTSymInitialize = (int)GetProcAddress(hdbg,"SymInitialize");
			*(int*)&MTSymLoadModule = (int)GetProcAddress(hdbg,"SymLoadModule");
		};
#	else
		struct sigaction sa;
		mtmemzero(&sa,sizeof(sa));
		sa.sa_sigaction = onsignal;
		sa.sa_flags = SA_SIGINFO;
		sigaction(SIGSEGV,&sa,0);
		sigaction(SIGBUS,&sa,0);
		sigaction(SIGILL,&sa,0);
		sigaction(SIGABRT,&sa,0);
		sigaction(SIGFPE,&sa,0);
//		signal(SIGSEGV,mtsigreturn);
//
		debugged = true;
#	endif
	initKernel();
	initFiles();
	if (mtinterface){
		platform = (char*)mtmemalloc(512);
		build = (char*)mtmemalloc(512);
		processor = (char*)mtmemalloc(512);
		hostname = (char*)mtmemalloc(512);
#		ifdef _WIN32
			DWORD hns = 512;
			osinfo.dwOSVersionInfoSize = sizeof(osinfo);
			GetVersionEx(&osinfo);
			GetComputerName(hostname,&hns);
			GetSystemInfo(&sysinfo);
			ncpu = sysinfo.dwNumberOfProcessors;
			strcpy(platform,"Windows ");
			if (osinfo.dwPlatformId==VER_PLATFORM_WIN32_NT){
				sysflags |= MTS_WINNT;
				if (osinfo.dwMajorVersion>=5){
					switch (osinfo.dwMinorVersion){
					case 0:
						strcat(platform,"2000 ");
						break;
					case 1:
						strcat(platform,"XP ");
						break;
					default:
						strcat(platform,"NT ");
						break;
					};
				}
				else{
					strcat(platform,"NT ");
				};
			}
			else{
				osinfo.dwBuildNumber &= 0xFFFF;
				if (osinfo.dwMinorVersion>=90){
					strcat(platform,"ME ");
				}
				else if (osinfo.dwMinorVersion>=10){
					strcat(platform,"98 ");
				}
				else{
					strcat(platform,"95 ");
				};
			};
			e = strchr(platform,0);
			sprintf(e,"Version %d.%d",osinfo.dwMajorVersion,osinfo.dwMinorVersion);
			if (osinfo.szCSDVersion[0]) sprintf(build,"Build %d (%s)",osinfo.dwBuildNumber,osinfo.szCSDVersion);
			else sprintf(build,"Build %d",osinfo.dwBuildNumber);
			sprintf(processor,"%d x ",sysinfo.dwNumberOfProcessors);
			switch (sysinfo.wProcessorArchitecture){
			case PROCESSOR_ARCHITECTURE_INTEL:
				strcpy(proctype,"x86 ");
				break;
			case PROCESSOR_ARCHITECTURE_MIPS:
				strcpy(proctype,"MIPS ");
				break;
			case PROCESSOR_ARCHITECTURE_ALPHA:
				strcpy(proctype,"Alpha ");
				break;
			case PROCESSOR_ARCHITECTURE_PPC:
				strcpy(proctype,"PPC ");
				break;
			default:
				strcpy(proctype,"Unknown ");
				break;
			};
			__asm{
				push	ebx
				mov		eax,0x80000000
				cpuid
				cmp		eax,0x80000004
				jb		_noname
				mov		eax,0x80000002
				cpuid
				mov		dword ptr buf[0],eax
				mov		dword ptr buf[4],ebx
				mov		dword ptr buf[8],ecx
				mov		dword ptr buf[12],edx
				mov		eax,0x80000003
				cpuid
				mov		dword ptr buf[16],eax
				mov		dword ptr buf[20],ebx
				mov		dword ptr buf[24],ecx
				mov		dword ptr buf[28],edx
				mov		eax,0x80000004
				cpuid
				mov		dword ptr buf[32],eax
				mov		dword ptr buf[36],ebx
				mov		dword ptr buf[40],ecx
				mov		dword ptr buf[44],edx
				mov		byte ptr buf[48],0
				xor		eax,eax
				cpuid
				jmp		_nameok
			_noname:
				xor		eax,eax
				cpuid
				mov		dword ptr buf[0],ebx
				mov		dword ptr buf[4],edx
				mov		dword ptr buf[8],ecx
				mov		byte ptr buf[12],0
			_nameok:
				test	eax,eax
				jz		_nommx
				mov		eax,1
				cpuid
				mov		cpuver,eax
				mov		cpuflags,edx
				mov		cpuflags2,ecx
			_nommx:
				pop		ebx
			};
			strcat(proctype,buf);
			e = strchr(proctype,0);
			if (sysflags & MTS_WINNT){
				sprintf(e," Level %d ",sysinfo.wProcessorLevel);
				e = strchr(proctype,0);
				lorev = (char)sysinfo.wProcessorRevision;
				hirev = sysinfo.wProcessorRevision>>8;
				switch (sysinfo.wProcessorArchitecture){
				case PROCESSOR_ARCHITECTURE_INTEL:
					sprintf(e,"Model %d Stepping %d",hirev,lorev);
					break;
				case PROCESSOR_ARCHITECTURE_ALPHA:
					sprintf(e,"Model %c Pass %d",hirev+'A',lorev);
					break;
				case PROCESSOR_ARCHITECTURE_PPC:
					sprintf(e,"Version %d.%d",hirev,lorev);
					break;
				default:
					sprintf(e,"Revision %d",sysinfo.wProcessorRevision);
					break;
				};
			}
			else{
				lorev = cpuver & 0xF;
				cpuver >>= 4;
				hirev = cpuver & 0xF;
				cpuver >>= 4;
				cpuver &= 0xF;
				sprintf(e," Level %d Model %d Stepping %d",cpuver,hirev,lorev);
			};
#		else
			strcpy(processor,"? x ");
			fd = open("/proc/stat",O_RDONLY);
			len = read(fd,buf,255);
			close(fd);
			if (len==-1) goto staterror;
			buf[len] = 0;
			e = strchr(buf,'\n');
			if (!e) goto staterror;
			len = 0;
			while (e){
				e++;
				if (strncmp(e,"cpu",3)) break;
				len++;
				e = strchr(e,'\n');
			};
			if (len==0) len = 1;
			sprintf(processor,"%d x ",len);
		staterror:
			if (uname(&un)==0){
				strcpy(hostname,un.node);
				strcpy(platform,un.sysname);
				strcat(platform," ");
				strcat(platform,un.machine);
				strcat(platform," Version ");
				strcat(platform,un.release);
				strcpy(build,"Build ");
				strcat(build,un.version);
			};
/*			
			e = platform;
			fd = open("/proc/sys/kernel/ostype",O_RDONLY);
			len = read(fd,e,128);
			close(fd);
			if (len==-1) goto oserror;
			e[len] = 0;
			e = strchr(platform,'\n');
			if (!e) e = strchr(platform,0);
			*e++ = ' ';
			fd = open("/proc/sys/kernel/osrelease",O_RDONLY);
			len = read(fd,e,128);
			close(fd);
			if (len==-1) goto oserror;
			e[len] = 0;
			e = strchr(platform,'\n');
			if (e) *e = 0;
			e = build;
			fd = open("/proc/sys/kernel/version",O_RDONLY);
			len = read(fd,e,128);
			close(fd);
			if (len==-1) goto oserror;
			e[len] = 0;
			e = strchr(build,'\n');
			if (e) *e = 0;
*/
		oserror:
			strcpy(proctype,"x86 ");
			char *_buf = (char*)calloc(1,256);
			asm (
				"\
				movl	$0x80000000,%%eax\n\
				cpuid\n\
				cmpl	$0x80000004,%%eax\n\
				jb		_noname\n\
				movl	$0x80000002,%%eax\n\
				cpuid\n\
				movl	%%eax,(%%esi)\n\
				movl	%%ebx,4(%%esi)\n\
				movl	%%ecx,8(%%esi)\n\
				movl	%%edx,12(%%esi)\n\
				movl	$0x80000003,%%eax\n\
				cpuid\n\
				movl	%%eax,16(%%esi)\n\
				movl	%%ebx,20(%%esi)\n\
				movl	%%ecx,24(%%esi)\n\
				movl	%%edx,28(%%esi)\n\
				movl	$0x80000004,%%eax\n\
				cpuid\n\
				movl	%%eax,32(%%esi)\n\
				movl	%%ebx,36(%%esi)\n\
				movl	%%ecx,40(%%esi)\n\
				movl	%%edx,44(%%esi)\n\
				movb	$0,48(%%esi)\n\
				xorl	%%eax,%%eax\n\
				cpuid\n\
				jmp		_nameok\n\
			_noname:\n\
				xorl	%%eax,%%eax\n\
				cpuid\n\
				movl	%%ebx,(%%esi)\n\
				movl	%%edx,4(%%esi)\n\
				movl	%%ecx,8(%%esi)\n\
				movb	$0,12(%%esi)\n\
			_nameok:\n\
				testl	%%eax,%%eax\n\
				jz		_nommx\n\
				movl	$1,%%eax\n\
				cpuid\n\
				movl	%%eax,%[cpuver]\n\
				movl	%%edx,%[cpuflags]\n\
				movl	%%ecx,%[cpuflags2]\n\
			_nommx:\n\
				"
				:[cpuver]"=m"(cpuver),[cpuflags]"=m"(cpuflags),[cpuflags2]"=m"(cpuflags2)
				:"S"(_buf)
				:"eax","ebx","ecx","edx"
			);
			strcat(proctype,_buf);
			free(_buf);
			e = strchr(proctype,0);
			lorev = cpuver & 0xF;
			cpuver >>= 4;
			hirev = cpuver & 0xF;
			cpuver >>= 4;
			cpuver &= 0xF;
			sprintf(e," Level %d Model %d Stepping %d",cpuver,hirev,lorev);
#		endif
#		ifdef MTSYSTEM_CONFIG
			if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
				if (conf->setsection("MTSystem")){
					if (conf->getparameter("CPUType",&buf,MTCT_STRING,sizeof(buf))){
						if (strcmp(buf,proctype)==0){
							if (conf->getparameter("CPUFrequency",&cpufrequ,MTCT_UINTEGER,sizeof(cpufrequ))){
								savecpu = false;
							};
						};
					};
				};
			};
#		endif
//		cpufrequ = 1000;
		if (cpufrequ==0){
#			ifndef __GNUC__
				__asm{
					rdtsc
					push	edx
					push	eax
					push	cpu_wait
					call	mtsyswait
					add		esp,4
					rdtsc
					sub		dword ptr [esp],eax
					sbb		dword ptr [esp+4],edx
					fild	qword ptr [esp]
					fchs
					fdiv	cpu_div
					mov		eax,this
#					ifdef __BORLANDC__
						fistp	dword ptr [eax].cpufrequ
#					else
						fistp	dword ptr [eax]this.cpufrequ
#					endif
					add		esp,8
				};
#			else
				asm (
					"\
					rdtsc\n\
					pushl	%%edx\n\
					pushl	%%eax\n\
					pushl	%[cpu_wait]\n\
					call	mtsyswait\n\
					addl	$4,%%esp\n\
					rdtsc\n\
					subl	%%eax,(%%esp)\n\
					sbbl	%%edx,4(%%esp)\n\
					fildq	(%%esp)\n\
					fchs\n\
					fdivl	%[cpu_div]\n\
					fistpl	%[cpufrequ]\n\
					addl	$8,%%esp\n\
					"
					:[cpufrequ]"=m"(cpufrequ)
					:[cpu_div]"m"(cpu_div),[cpu_wait]"m"(cpu_wait)
					:"eax","ebx","ecx","edx"
					);
#			endif
			cpufrequ = (cpufrequ/3)*3;
		};
#		ifdef MTSYSTEM_CONFIG
			if (conf){
				if (savecpu){
					if (conf->createsection("MTSystem")){
						conf->setparameter("CPUType",&proctype,MTCT_STRING,sizeof(proctype));
						conf->setparameter("CPUFrequency",&cpufrequ,MTCT_UINTEGER,sizeof(cpufrequ));
					};
				};
				mtinterface->releaseconf(conf);
			};
#		endif
		if (cpuflags & 0x00000100) sysflags |= MTS_CX8;
		if (cpuflags & 0x00008000) sysflags |= MTS_CMOV;
		if (cpuflags & 0x00800000) sysflags |= MTS_MMX;
		if (cpuflags & 0x02000000) sysflags |= MTS_SSE;
		if (cpuflags & 0x04000000) sysflags |= MTS_SSE2;
		if (cpuflags & 0x10000000) sysflags |= MTS_HT;
		if (cpuflags2 & 0x00000001) sysflags |= MTS_SSE3;
		e = strchr(processor,0);
		sprintf(e,"%d MHz (%s)",cpufrequ,proctype);
#		ifdef _WIN32
			timeGetDevCaps(&timecaps,sizeof(TIMECAPS));
#		else
			clock_getres(CLOCK_REALTIME,&timerres);
#		endif
		di = (MTDisplayInterface*)mtinterface->getinterface(displaytype);
		gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
#		ifdef MTSYSTEM_RESOURCES
			if ((gi) && (di) && (mtinterface->type!=FOURCC('G','U','I','E'))){
				strcpy(buf,mtinterface->getprefs()->syspath[SP_INTERFACE]);
				strcat(buf,"MTSystem.mtr");
				f = mtfileopen(buf,MTF_READ|MTF_SHAREREAD);
				if (f) sysres = new MTResources(f,true);
			};
#		endif
		strcpy(logpath,mtinterface->getprefs()->syspath[SP_USER]);
		strcat(logpath,"LOG_");
		e = strrchr(logpath,0);
		memcpy(e,&mtinterface->type,4);
		e += 4;
		*e++ = '_';
		time(&lt);
		lts = localtime(&lt);
		sprintf(e,"%.4d-%.2d-%.2d",lts->tm_year+1900,lts->tm_mon+1,lts->tm_mday);
		strcat(logpath,".txt");
#		ifdef _DEBUG
			startlog();
#		endif
#		ifdef MTSYSTEM_RESOURCES
			if (sysres) sysres->loadstring(MTT_storage,rootn,sizeof(rootn));
#		endif
	};
#	ifdef MTSYSTEM_INTERNET
		initInternet();
#	endif
	status |= MTX_INITIALIZED;
	return true;
}

void MTSystemInterface::uninit()
{
#	ifdef _DEBUG
		unsigned int na;
		char buf[256];
#	endif
	
	status &= (~MTX_INITIALIZED);
	mtmemfree(platform);
	mtmemfree(build);
	mtmemfree(processor);
#	ifdef MTSYSTEM_INTERNET
		uninitInternet();
		uninitSocket();
#	endif
#	ifdef MTSYSTEM_RESOURCES
		if (sysres){
			delete sysres;
			sysres = 0;
		};
#	endif
	uninitFiles();
	if (logfile){
		mtmemfree(logfile->url);
		logfile->url = 0;
	};
#	ifndef _WIN32
		sigaction(SIGSEGV,0,0);
		sigaction(SIGBUS,0,0);
		sigaction(SIGILL,0,0);
		sigaction(SIGABRT,0,0);
		sigaction(SIGFPE,0,0);
#	endif
	uninitKernel();
#	ifdef _DEBUG
		if (nallocs!=0){
			int x = 0;

			na = nallocs;
			LOG("ERROR: Remaining memory allocations:"NL);
			while (nallocs>0){
				FLOG4(NL"%.8x: %d bytes allocated by %.8X"NL"Callstack:"NL"  %s"NL,mtalloc[0].address,mtalloc[0].size,mtalloc[0].caller,mtalloc[0].callstack);
				if (x<16){
					mtdump((unsigned char*)mtalloc[0].address,64);
					x++;
				};
				mtmemfree(mtalloc[0].address);
			};
			sprintf(buf,"%d remaining memory allocation(s)!",na);
			mtdialog(buf,"Memory",MTD_OK,MTD_EXCLAMATION,5000);
		};
		FLOG1("Memory usage peak: %d bytes"NL,peakmemory);
#	endif
	stoplog();
	if (waserror){
#		ifdef _WIN32
			if (mtdialog("One or more errors occured!"NL"Do you want to see the log file?","Errors",MTD_YESNO,MTD_QUESTION,5000)==0){
				ShellExecute(0,"open",logpath,"","",SW_SHOWNORMAL);
			};
#		else
			if (mtdialog("One or more errors occured!"NL"Do you want to see the log file?","Errors",MTD_YESNO,MTD_INFORMATION,5000)==0){
				execlp("less","less",logpath,0);
			};
#		endif
	};
}

void MTSystemInterface::start()
{
#	ifdef _DEBUG
#		if defined(MTSYSTEM_CONFIG) && defined(MTSYSTEM_MEMORYFILE)
			MTMiniConfig *mc = new MTMiniConfig();
			MTFile *mf = mtfileopen("mem://",MTF_CREATE|MTF_READ|MTF_WRITE);
			int test = 43234;
			char buf[256];
			mc->setparameter("Test",&test,MTCT_UINTEGER,4);
			mc->setparameter("Bla.Test","Hello there",MTCT_STRING,-1);
			mc->getparameter("Bla",buf,MTCT_STRING,sizeof(buf));
			mc->savetostream(mf);
			mtfileclose(mf);
			delete mc;
#		endif
#	endif
}

void MTSystemInterface::stop()
{
	stopThreads(true);
	if (auth){
		gi->delcontrol(auth);
		auth = 0;
	};
}

void MTSystemInterface::processcmdline(void *params)
{

}

void MTSystemInterface::showusage(void *out)
{

}

int MTSystemInterface::config(int command,int param)
{
	return 0;
}

int MTSystemInterface::getlasterror()
{
	return (int)mtgetprivatedata(-7);
}

void MTSystemInterface::setlasterror(int error)
{
	mtsetprivatedata(-7,(void*)error);
}

void MTSystemInterface::addfilehook(char *type,MTFileHook *hook)
{
	hooks->additem(type,hook);
}

void MTSystemInterface::delfilehook(char *type,MTFileHook *hook)
{
	hooks->delitem(type);
}
//---------------------------------------------------------------------------
#ifndef MTBUILTIN
extern "C"
{

MTEXPORT MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!si) si = new MTSystemInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)si;
	return &i;
}

}
#endif
#endif
//---------------------------------------------------------------------------
