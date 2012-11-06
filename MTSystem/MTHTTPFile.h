//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTHTTPFile.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTHTTPFILE_INCLUDED
#define MTHTTPFILE_INCLUDED
//---------------------------------------------------------------------------
class MTHTTPHook;
class MTHTTPFile;
class MTHTTPFolder;
//---------------------------------------------------------------------------
#include "MTFile.h"
#include "MTLocalFile.h"
//---------------------------------------------------------------------------
class MTHTTPHook : public MTFileHook{
public:
	MTHTTPHook();
	MTFile* MTCT fileopen(char *url,int flags);
	MTFolder* MTCT folderopen(char *url);
	bool MTCT filecopy(char *source,char *dest);
	bool MTCT filerename(char *source,char *dest);
	bool MTCT filedelete(char *url);
	void MTCT filetype(const char *url,char *type,int length);
};

class MTHTTPFile : public MTFile{
public:
	MTHTTPFile(char *path,int access);
	MTHTTPFile(MTFile *parent,int start,int end,int access);
	~MTHTTPFile();
	int MTCT read(void *buffer,int size);
	int MTCT readln(char *buffer,int maxsize);
//	int MTCT reads(char *buffer,int maxsize);
	int MTCT write(const void *buffer,int size);
	int MTCT seek(int pos,int origin);
	void* MTCT getpointer(int offset,int size);
	void MTCT releasepointer(void *mem);
	int MTCT length();
	int MTCT pos();
	bool MTCT eof();
	bool MTCT seteof();
	bool MTCT gettime(int *modified,int *accessed);
	bool MTCT settime(int *modified,int *accessed);
	MTFile* MTCT subclass(int start,int length,int access);
private:
	int maccess;
	int cpos;
	int from,to;
	MTLocalFile *cache;
};

class MTHTTPFolder : public MTFolder{
public:
	MTHTTPFolder(char *url);
	~MTHTTPFolder();
	bool MTCT getfile(const char **name,int *attrib,double *size);
	bool MTCT next();
};
//---------------------------------------------------------------------------
bool initHTTP();
void uninitHTTP();
//---------------------------------------------------------------------------
extern MTHTTPHook httphook;
//---------------------------------------------------------------------------
#endif
