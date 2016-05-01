//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTMemoryFile.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "MTMemoryFile.h"
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
MTMemoryHook memoryhook;
//---------------------------------------------------------------------------
MTMemoryHook::MTMemoryHook()
{
}

MTFile* MTMemoryHook::fileopen(const char *url,int flags)
{
	// FIXME: This function deals with strings like crap. -flibit

	const char *e,*a;
	void *ptr;
	int length;

	e = strstr(url,"//");
	if (!e){
		e = strchr(url,':');
		if (!e) return 0;
		e++;
	}
	else{
		e += 2;
	};
	a = e;
	e = strchr(a,':');
	if (e){
		// *e++ - 0; <- WTF was this? -flibit
		length = atol(e);
	}
	else{
		length = 4096;
	};
	if (a[0]) ptr = (void*)strtol(a,0,16);
	else ptr = 0;
	return new MTMemoryFile(ptr,length,flags);
}

MTFolder* MTMemoryHook::folderopen(char *url)
{
	return 0;
}

bool MTMemoryHook::filecopy(char *source,char *dest)
{
	return false;
}

bool MTMemoryHook::filerename(char *source,char *dest)
{
	return false;
}

bool MTMemoryHook::filedelete(char *url)
{
	return false;
}

void MTMemoryHook::filetype(const char *url,char *type,int length)
{
	return;
}
//---------------------------------------------------------------------------
MTMemoryFile::MTMemoryFile(void *mem,int length,int access):
maccess(access),
cpos(0),
al(length),
l(0),
subclassed(false)
{
	if (mem){
		m = mem;
		autosize = false;
		autofree = false;
	}
	else{
		m = mtmemalloc(al,MTM_ZERO);
		autosize = true;
		autofree = true;
	};
	c = (char*)m;
}

MTMemoryFile::MTMemoryFile(MTFile *parent,int start,int end,int access):
maccess(access),
c((char*)((MTMemoryFile*)parent)->m),
cpos(0),
al(end-start),
l(end-start),
subclassed(true),
autosize(false)
{
	c += start;
	m = c;
}

MTMemoryFile::~MTMemoryFile()
{
	if ((!subclassed) && (autofree) && (m)) mtmemfree(m);
}

int MTMemoryFile::read(void *buffer,int size)
{
	int read;

	if ((cpos>=l) || ((maccess & MTF_READ)==0)) return 0;
	if (cpos+size>l) read = l-cpos;
	else read = size;
	if (read<=0) return 0;
	memcpy(buffer,c,read);
	cpos += read;
	c += read;
	return read;
}

int MTMemoryFile::readln(char *buffer,int maxsize)
{
	int read = 0;
	int x;
	char *e;
	
	if ((cpos>=l) || ((maccess & MTF_READ)==0)) return 0;
	maxsize--;
	if (cpos+maxsize>l) maxsize = l-cpos;
	if (maxsize<=0) return 0;
	for (x=0,e=c;x<maxsize;x++,e++){
		if ((*e==0) || (*e=='\r') || (*e=='\n')){
			if (x>0) memcpy(buffer,c,x);
			buffer[x] = 0;
			read = x+1;
			if (*e++=='\r'){
				if (*e=='\n'){
					read++;
				};
			};
			goto done;
		};
	};
	read = maxsize;
	memcpy(buffer,c,read);
	buffer[read] = 0;
done:
	cpos += read;
	c += read;
	return read;
}
/*
int MTMemoryFile::reads(char *buffer,int maxsize)
{
	int read = 0;
	int x;
	char *e;
	
	if ((cpos>=l) || ((maccess & MTF_READ)==0)) return 0;
	maxsize--;
	if (cpos+maxsize>l) maxsize = l-cpos;
	if (maxsize<=0) return 0;
	for (x=0,e=c;x<maxsize;x++,e++){
		if (*e==0){
			if (x>0) memcpy(buffer,c,x);
			buffer[x] = 0;
			read = x+1;
			goto done;
		};
	};
	read = maxsize;
	memcpy(buffer,c,read);
	buffer[read] = 0;
done:
	cpos += read;
	c += read;
	return read;
}
*/
int MTMemoryFile::write(const void *buffer,int size)
{
	if ((maccess & MTF_WRITE)==0) return 0;
	if ((!autosize) && (cpos+size>l)) size = l-cpos;
	if (size<=0) return 0;
	if ((autosize) && (cpos+size>al)){
		al = ((al+size+16383)>>14)<<14;
		m = mtmemrealloc(m,al);
		c = (char*)m+cpos;
	};
	memcpy(c,buffer,size);
	if (cpos+size>l) l = cpos+size;
	cpos += size;
	c += size;
	return size;
}

int MTMemoryFile::seek(int pos,int origin)
{
	switch (origin){
	case MTF_BEGIN:
		cpos = pos;
		break;
	case MTF_CURRENT:
		cpos += pos;
		break;
	case MTF_END:
		cpos = l-pos;
		break;
	};
	if (cpos<0) cpos = 0;
	else if (cpos>l){
		if (autosize){
			al = ((al+32767)>>14)<<14;
			m = mtmemrealloc(m,al);
			c = (char*)m+cpos;
		}
		else{
			cpos = l;
		};
	};
	c = (char*)m+cpos;
	return cpos;
}

int MTMemoryFile::length()
{
	return l;
}

void* MTMemoryFile::getpointer(int offset,int size)
{
	if (offset<0) return c;
	if (offset>=l) return 0;
	return (char*)m+offset;
}

void MTMemoryFile::releasepointer(void *mem)
{
}

int MTMemoryFile::pos()
{
	return cpos;
}

bool MTMemoryFile::eof()
{
	return (cpos>=l);
}

bool MTMemoryFile::seteof()
{
	if (autosize){
		l = cpos;
		m = mtmemrealloc(m,l);
		c = (char*)m+cpos;
		return true;
	};
	return false;
}

bool MTMemoryFile::gettime(int *modified,int *accessed)
{
	return false;
}

bool MTMemoryFile::settime(int *modified,int *accessed)
{
	return false;
}

MTFile* MTMemoryFile::subclass(int start,int length,int access)
{
	return new MTMemoryFile(this,start,length,access);
}
//---------------------------------------------------------------------------
