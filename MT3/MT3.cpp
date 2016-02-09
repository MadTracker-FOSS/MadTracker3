//---------------------------------------------------------------------------
//
//	MadTracker 3
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
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
void *instance; // This one is going to be really problematic.
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
	int x,l;                // x is an incremental counter towards argc, l is a string length temporary.
	char input[4096];       // standard-fare input buffer
	MTConsole *console;     // User I/O, actually on console? (it's windows so probably not)

	instance = (void*)getpid(); // this is a unistd.h function. huh. does it exist on windows? No it doesn't.

	argv0 = argv[0];        // program name

	if (argc>1){            // parse additional commandline arguments
		l = 0;              // there's that temp again. Its declaration should be moved here; this is not time-expensive

        for (x=1;x<argc;x++)
        {
            l += strlen(argv[x]) + 1; // this counts the terminating \0 too, so it becomes the actual array size.
        }

        cmdline = (char*)calloc(1,l); // i hate you. Anyway, this allocates space for the global command line string.
		for (x=1;x<argc-1;x++){ // now processing all arguments up until the last one.
            // so we read out an argument (without the \0) and append it to cmdline, then add a " " at the back
            // where toe \0 was previously.
			strcat(cmdline,argv[x]);
			strcat(cmdline," ");
		};
        // And then we put the final argument at the very end, with no space after it.
		strcat(cmdline,argv[argc-1]);
	};

	init(); // I'm gonna guess this thing processes the command line arguments.
    // TODO: Use. std::. Motherfucking. vector. And pass it to this function.

	if (mi){ // mi, too, is one of these global pointers. It's of the type MT3Interface.
            //TODO look up what that's responsible for.
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
		console = (MTConsole*)mi->getconsole(); // getconsole returns void*. Should be a polymorphic/composite class instead.
		if (console){
			fputs("Entering console..."NL,stdout);
			while (running){
				if (console->readln(input,sizeof(input))){
					console->userinput(input);  // input is the global char buffer, so if this is the main loop,
                    // I would assume this buffer is used quite a lot throughout the program.
                    // If we refactor this, a lot of the program would have to be remodeled.

                    // Interestingly enough, neither readln nor userinput set running to false.
                    // Most other instances of "running" that the fulltext search produces are member
                    // variables of various classes that, so I assume, were part of Yannicks initial
                    // attempts to port this program to C++.
				};
			};
		};
	};

	uninit();

    // This becomes pointless if we use std::vector.
	if (argc>1)
        free(cmdline);


	return 0;
}
#endif
//---------------------------------------------------------------------------
