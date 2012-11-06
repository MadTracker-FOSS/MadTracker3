//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTHTTPFile.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <time.h>
#include "MTHTTPFile.h"
#include "MTMD5.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
MTHTTPHook httphook;
char http_version[8] = "1.1";
char http_agent[256] = {"MTSystem/1.0"};
int http_threads = 8;
MTThread *http_thread[32];
//---------------------------------------------------------------------------
bool initHTTP()
{
#	ifdef MTSYSTEM_CONFIG
		MTConfigFile *conf;
		if ((mtinterface) && ((conf = (MTConfigFile*)mtinterface->getconf("Global",false)))){
			if (conf->setsection("MTSystem")){
				conf->getparameter("HTTPVersion",http_version,MTCT_STRING,sizeof(http_version));
				conf->getparameter("HTTPAgent",http_agent,MTCT_STRING,sizeof(http_agent));
				conf->getparameter("HTTPThreads",&http_threads,MTCT_UINTEGER,sizeof(http_threads));
			};
			mtinterface->releaseconf(conf);
		};
#	endif
#	ifdef _DEBUG
		MTHTTPFile *test = new MTHTTPFile("http://195.95.38.138",MTF_READ);
		if (test) delete test;
#	endif
	return true;
}

void uninitHTTP()
{
	int x;

	for (x=0;x<32;x++){
		if (http_thread[x]){
			http_thread[x]->terminate();
			http_thread[x] = 0;
		};
	};
}
//---------------------------------------------------------------------------
MTHTTPHook::MTHTTPHook()
{
}

MTFile* MTHTTPHook::fileopen(char *url,int flags)
{
	return new MTHTTPFile(url,flags);
}

MTFolder* MTHTTPHook::folderopen(char *url)
{
	return new MTHTTPFolder(url);
}

bool MTHTTPHook::filecopy(char *source,char *dest)
{
	return false;
}

bool MTHTTPHook::filerename(char *source,char *dest)
{
	return false;
}

bool MTHTTPHook::filedelete(char *url)
{
	return false;
}

void MTHTTPHook::filetype(const char *url,char *type,int length)
{
	char *e;
	
	e = (char*)strrchr(url,'.');
	if ((e) && (strchr(e,'/'))) e = 0;
	if (e){
		strncpy(type,e,length);
		strlwr(type);
	};
}
//---------------------------------------------------------------------------
int MTCT HTTPThread(MTThread *thread,void *param)
{
	while (!thread->terminated){
		mtsyswait(100);
	};
	return 0;
}
//---------------------------------------------------------------------------
MTHTTPFile::MTHTTPFile(char *path,int access):
maccess(access),
cpos(0),
from(0),
to(0x7FFFFFFF),
cache(0)
{
	char cacheurl[1024];

	mtsetlasterror(0);
	strcpy(cacheurl,mtinterface->getprefs()->path[UP_CACHE]);
	md5(strchr(cacheurl,0),path);
}

MTHTTPFile::MTHTTPFile(MTFile *parent,int start,int end,int access):
maccess(access),
cpos(start),
from(start),
to(start+end),
cache(0)
{
	mtsetlasterror(0);

}

MTHTTPFile::~MTHTTPFile()
{
}

int MTHTTPFile::read(void *buffer,int size)
{
	int read = 0;

	return read;
}

int MTHTTPFile::readln(char *buffer,int maxsize)
{
	int read = 0;

	return read;
}
/*
int MTHTTPFile::reads(char *buffer,int maxsize)
{
	int read = 0;

	return read;
}
*/
int MTHTTPFile::write(const void *buffer,int size)
{
	return 0;
}

int MTHTTPFile::seek(int pos,int origin)
{
	if (origin==MTF_BEGIN) pos += from;
	if ((to<0x7FFFFFFF) && (origin==MTF_END)){
		origin = MTF_BEGIN;
		pos = to-pos;
	};

	return cpos-from;
}

int MTHTTPFile::length()
{
	if (to<0x7FFFFFFF) return to-from;
	return 0;
}

void *MTHTTPFile::getpointer(int offset,int size)
{
	return 0;
}

void MTHTTPFile::releasepointer(void *mem)
{

}

int MTHTTPFile::pos()
{
	return cpos-from;
}

bool MTHTTPFile::eof()
{
	if (cpos>=to) return true;

	return false;
}

bool MTHTTPFile::seteof()
{
	return false;
}

bool MTHTTPFile::gettime(int *modified,int *accessed)
{

	return false;
}

bool MTHTTPFile::settime(int *modified,int *accessed)
{
	return false;
}

MTFile* MTHTTPFile::subclass(int start,int length,int access)
{

	return 0;
}
//---------------------------------------------------------------------------
MTHTTPFolder::MTHTTPFolder(char *path)
{

}

MTHTTPFolder::~MTHTTPFolder()
{

}

bool MTHTTPFolder::getfile(const char **name,int *attrib,double *size)
{

	return false;
}

bool MTHTTPFolder::next()
{
	return false;
}
//---------------------------------------------------------------------------
