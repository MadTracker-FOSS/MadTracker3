//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTLocalFile.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTLOCALFILE_INCLUDED
#define MTLOCALFILE_INCLUDED

//---------------------------------------------------------------------------
class MTLocalHook;

class MTLocalFile;

class MTLocalFolder;
//---------------------------------------------------------------------------
#include "MTFile.h"

#ifdef _WIN32
#	include <windows.h>
#	include <shlobj.h>
#	include <shellapi.h>
#else

#	include <sys/types.h>
#	include <sys/stat.h>
#	include <fcntl.h>
#	include <stdio.h>
#	include <dirent.h>

#endif

//---------------------------------------------------------------------------
class MTLocalHook: public MTFileHook
{
public:
    MTLocalHook();

    MTFile *MTCT fileopen(const char *url, int flags);

    MTFolder *MTCT folderopen(char *url);

    bool MTCT filecopy(char *source, char *dest);

    bool MTCT filerename(char *source, char *dest);

    bool MTCT filedelete(char *url);

    void MTCT filetype(const char *url, char *type, int length);
};

class MTLocalFile: public MTFile
{
public:
    MTLocalFile(const char *path, int access);

    MTLocalFile(MTFile *parent, int start, int end, int access);

    ~MTLocalFile();

    int MTCT read(void *buffer, int size);

    int MTCT readln(char *buffer, int maxsize);

//	int MTCT reads(char *buffer,int maxsize);
    int MTCT write(const void *buffer, int size);

    int MTCT seek(int pos, int origin);

    void *MTCT getpointer(int offset, int size);

    void MTCT releasepointer(void *mem);

    int MTCT length();

    int MTCT pos();

    bool MTCT eof();

    bool MTCT seteof();

    bool MTCT gettime(int *modified, int *accessed);

    bool MTCT settime(int *modified, int *accessed);

    MTFile *MTCT subclass(int start, int length, int access);

private:
#ifdef _WIN32
    HANDLE h;
    HANDLE maph;
#else
    FILE *fs;
    int maplength;
#endif
    bool stdhandle;
    int maccess;
    int cpos;
    int from, to;
    void *mmap;
    int mapoffset;
};

class MTLocalFolder: public MTFolder
{
public:
    MTLocalFolder(char *url);

    ~MTLocalFolder();

    bool MTCT getfile(const char **name, int *attrib, double *size);

    bool MTCT next();

private:
    char mpath[512];
    int id;
#ifdef _WIN32
    HANDLE search;
    WIN32_FIND_DATA data;
#else
    DIR *d;
#endif
};

//---------------------------------------------------------------------------
extern MTLocalHook localhook;
//---------------------------------------------------------------------------
#endif
