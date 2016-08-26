//---------------------------------------------------------------------------
//
//	MadTracker 3
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MT3.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------

/**
 * Extremely important reading for this piece of candy:
 * https://en.wikipedia.org/wiki/C_POSIX_library
 *
 * and C standard lib, obviously.
 */

// There are no comments. Nowhere. None. TODO Add them. Everywhere.
// TODO Fix/refactor all errors caused by precision loss, until mt3 compiles without -fpermissive.
// TODO Fix all, yes all, warnings thrown by -Wall, -Wextra and -pedantic under C++11 compilation.
// Example: The NL macro that's used in lots of const char* literal strings is interpreted as a
// user-defined literal suffix under C++11. Using std::string and plain old \n instead and then
// using an output stream to get the proper linebreak character would fix these warnings.


/**
 * ih3 says:
 * My best guess so far is that this program is only a small core template and all
 * additional modules are supposed to be loaded at runtime. The reason it creates an I/O console
 * is that the GUI module may be unloaded and if that happens you can still test functionality
 * by inputting commands.
 */

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
void* instance; // This one is going to be really problematic.
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

// Let's see if anything at all breaks from this being no longer a global variable.
// Update: Yup. Linker errors in MTData.cpp. They have been marked with FIXMEs.
const char* argv0;

char* cmdline;

bool running = true;

//NOTE: There are multiple main functions throughout the code. This might be because all submodules were originally builds of their own.
int main(int argc, const char* argv[])
{
    // Moved variable declaration into scope

    char input[4096];       // standard-fare input buffer
    MTConsole* console;     // User I/O, actually on console

    instance = (void*) getpid(); // this is a unistd.h function. huh. does it exist on windows? No it doesn't.

    argv0 = argv[0];        // program name; actually read from some other places. IDE has lots of trouble tracking this.

    if (argc > 1)
    {            // parse additional commandline arguments
        int l = 0;

        for(int x = 1; x < argc; x++)
        {
            l += strlen(argv[x]) + 1; // this counts the terminating \0 too, so it becomes the actual array size.
        }

        cmdline = (char*) calloc(1, l); // Allocation for 1 object of size l, where l is the total string length +1 for \0 at the end.
        // (this, of course, can be replaced with std::string)

        for(int x = 1; x < argc - 1; x++)
        { // now processing all arguments up until the last one.
            // so we read out an argument (without the \0) and append it to cmdline, then add a " " at the back
            // where the \0 was previously.
            strcat(cmdline, argv[x]);
            strcat(cmdline, " ");
        };
        // And then we put the final argument at the very end, with no space after it.
        strcat(cmdline, argv[argc - 1]);
    };

    init(); // This thing processes the command line arguments.
    // TODO: Use a std::vector and pass it to this init function.

    if (mi)
    { // mi, too, is one of these global pointers. It's of the type MT3Interface,
        // which is the primary interface for all important program functions.
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
        console = (MTConsole*) mi->getconsole(); // getconsole returns void*. Should be a polymorphic/composite class instead.
        if (console)
        {
            fputs("Entering console..."NL, stdout);
            while(running)
            {
                if (console->readln(input, sizeof(input)))
                {
                    console->userinput(input);
                    // input is the global char buffer, so if this is the main loop,
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
    if (argc > 1)
    {
        free(cmdline);
    }


    return 0;
}

#endif
//---------------------------------------------------------------------------
