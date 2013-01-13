//---------------------------------------------------------------------------
//
//	MadTracker 3
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MT3.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/types.h>
#	include <stdlib.h>
#	include <stdio.h>
#	include <unistd.h>
#endif
#include "MTData.h"
//---------------------------------------------------------------------------
void *instance;
//---------------------------------------------------------------------------
#ifdef _WIN32
#include "MTInterface.h"
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
//
//	Main call
//
{
	instance = (void*)hInstance;
	if (init()){
retry:
		MTTRY
			while (gi->processmessages(true)>0){
				if (wantreset){
					wantreset = false;
					LOGD("%s - Reseting extensions..."NL);
					stopExtensions();
					uninitExtensions();
					if (initExtensions()) startExtensions();
				};
			};
		MTCATCH
			SetWindowPos((HWND)wnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
			if (MessageBox(0,"An unhandled exception occured!"NL"\
A log file has been generated with details about the error."NL"\
You should send this file to Yannick so that he can fix this problem."NL"\
Click on \"Retry\" to try to continue to work with MadTracker."NL"\
Otherwise, click on \"Cancel\" to exit gracefully.","MadTracker - Fatal Error",MB_RETRYCANCEL|MB_ICONERROR|MB_TASKMODAL)==IDRETRY) goto retry;
		MTEND
	};
	uninit();
	return 0;
}
#else
#include "MTConsole.h"
const char *argv0;
char *cmdline;
bool running = true;

int main(int argc,const char* argv[])
{
	int x,l;
	char input[4096];
	MTConsole *console;

	instance = (void*)getpid();
	argv0 = argv[0];
	if (argc>1){
		l = 0;
		for (x=1;x<argc;x++) l += strlen(argv[x])+1;
		cmdline = (char*)calloc(1,l);
		for (x=1;x<argc-1;x++){
			strcat(cmdline,argv[x]);
			strcat(cmdline," ");
		};
		strcat(cmdline,argv[argc-1]);
	};
	init();
	if (mi){
/*
		LOGD("%s - Start"NL);
		MTEvent *e[2];
		e[0] = si->eventcreate(true,5000,0,false,true);
		e[1] = si->eventcreate(true,7000,0,false,true);
		si->syswaitmultiple(2,e,true,10000);
		si->eventdelete(e[0]);
		si->eventdelete(e[1]);
		LOGD("%s - End"NL);
*/
		console = (MTConsole*)mi->getconsole();
		if (console){
			fputs("Entering console..."NL,stdout);
			while (running){
				if (console->readln(input,sizeof(input))){
					console->userinput(input);
				};
			};
		};
	};
	uninit();
	if (argc>1) free(cmdline);
	return 0;
}
#endif
//---------------------------------------------------------------------------
