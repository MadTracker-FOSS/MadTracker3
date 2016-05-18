//---------------------------------------------------------------------------
//
//	MadTracker Console
//
//		Platforms:	All
//		Processors: All
//
//	Copyright 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTConsole.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTConsole.h"
#include "MTExtensions.h"
#include <stdio.h>

//---------------------------------------------------------------------------
MTConsole::MTConsole(MTInterface* mti)
{
}

MTConsole::MTConsole(MTFile* parent, int start, int end, int access)
{
}

MTConsole::~MTConsole()
{
}

int MTConsole::read(void* buffer, int size)
{
    int read = 0;

#	ifndef _WIN32
    read = fread(buffer, 1, size, stdin);
#	endif
    return read;
}

int MTConsole::readln(char* buffer, int maxsize)
{
    int read = 0;

#	ifndef _WIN32
    char* e;
    fwrite("> ", 1, 2, stdout);
    if (fgets(buffer, maxsize, stdin))
    {
        e = strchr(buffer, 0);
        if (*--e == '\n')
        {
            while((*e == '\r') || (*e == '\n'))
            {
                *e-- = 0;
            }
        };
        read = strlen(buffer);
    }
    else
    {
        read = 0;
    }
#	endif
    return read;
}

/*
int MTConsole::reads(char *buffer,int maxsize)
{
	int read = 0;

	return read;
}
*/
int MTConsole::write(const void* buffer, int size)
{
#	ifdef _WIN32
    size = 0;
#	else
    size = fwrite(buffer, 1, size, stdout);
#	endif
    return size;
}

int MTConsole::seek(int pos, int origin)
{
    return -1;
}

int MTConsole::length()
{
    return -1;
}

void* MTConsole::getpointer(int offset, int size)
{
    return 0;
}

void MTConsole::releasepointer(void* mem)
{
}

int MTConsole::pos()
{
    return -1;
}

bool MTConsole::eof()
{
    return false;
}

bool MTConsole::seteof()
{
    return false;
}

bool MTConsole::gettime(int* modified, int* accessed)
{
    return false;
}

bool MTConsole::settime(int* modified, int* accessed)
{
    return false;
}

MTFile* MTConsole::subclass(int start, int length, int access)
{
    return 0;
}

int MTConsole::userinput(const char* input)
{
    int x, y;
    bool ok = false;
    char cmd[64];

    if (mi->processinput(input) != 0)
    {
        ok = true;
    }
    for(x = 0; x < next; x++)
    {
        MTExtension& cext = *ext[x];
        if (!cext.system)
        {
            for(y = 0; y < cext.i->ninterfaces; y++)
            {
                MTTRY
                    if (cext.i->interfaces[y]->status & MTX_INITIALIZED)
                    {
                        if (cext.i->interfaces[y]->processinput(input) != 0)
                        {
                            ok = true;
                        }
                    };
                MTCATCH
                    LOGD("%s - ERROR: Exception while processing input in '");
                    LOG(cext.i->interfaces[y]->name);
                    LOG("'!"
                            NL);
                MTEND
            };
        };
    };
    if (!ok)
    {
        char buf[256];
        const char* e = strchr(input, ' ');
        if (e)
        {
            // pointer arithmetiXX. bah.
            x = e - input;
            if (x > sizeof(cmd) - 1)
            {
                x = sizeof(cmd) - 1;
            }
            strncpy(cmd, input, x);
            cmd[x] = 0;
        }
        else
        {
            cmd[sizeof(cmd) - 1] = 0;
            strncpy(cmd, input, sizeof(cmd) - 1);
        };
        sprintf(buf, "Unknown command %s!"NL, cmd);
        write(buf, strlen(buf));
    };
    return 0;
}
//---------------------------------------------------------------------------
