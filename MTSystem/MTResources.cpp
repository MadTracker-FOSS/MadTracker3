//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTResources.cpp 68 2005-08-26 22:19:12Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include "MTResources.h"
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
MTResources* mtresfind(const char *filename,bool write)
{
	MTFile *f = 0;
	int x,flags;
	char path[512];
	
	flags = MTF_READ|MTF_SHAREREAD;
	if (write) flags |= MTF_WRITE;
	if (mtfileexists((char*)filename)) f = mtfileopen((char*)filename,flags);
	if (!f){
		MTPreferences *prefs;
		prefs = (MTPreferences*)mtinterface->getprefs();
		if (!prefs) return 0;
		for (x=0;x<SP_MAX;x++){
			strcpy(path,prefs->syspath[x]);
			strcat(path,filename);
			if (mtfileexists(path)) f = mtfileopen(path,flags);
			else f = 0;
			if (f) goto found;
		};
		return 0;
	};
found:
	return new MTResources(f,true);
}

MTResources* mtresopen(MTFile *f,bool ownfile)
{
	return new MTResources(f,ownfile);
}

void mtresclose(MTResources *res)
{
	delete res;
}
//---------------------------------------------------------------------------
static const char mtsr[28] = {"MadTracker System Resources"};

MTResources::MTResources(MTFile *f,bool ownfile):
mf(f),
table(0),
nres(0),
onres(0),
nares(0),
modified(false),
mownfile(ownfile)
{
	int n,ver;
	char buf[32];
	
	nres = 0;
	nares = 0;
	if (!mf->length()) return;
	mtmemzero(buf,sizeof(buf));
	mf->seek(0,MTF_BEGIN);
	mf->read(buf,28);
	if (strcmp(buf,mtsr)==0){
		mf->read(&ver,4);
		if (ver==0x0300){
			mf->read(&n,4);
			onres = n;
			nres = n;
			nares = ((n+7)>>3)<<3;
			table = (MTResTable*)mtmemalloc(sizeof(MTResTable)*nares);
			mf->read(table,16*nares);
		};
	}
	else{
		mf->seek(0,MTF_BEGIN);
		nres = 1;
		nares = 1;
		table = (MTResTable*)mtmemalloc(sizeof(MTResTable));
		mf->read(&table->type,4);
		mf->read(&table->size,4);
		table->offset = 8;
		table->uid = 0;
	};
}

MTResources::~MTResources()
{
	int x,n,o,s;
	
	if (modified){
		if (onres==0){
			x = 0x0300;
			mf->seek(0,MTF_BEGIN);
			mf->write(mtsr,28);
			mf->write(&x,4);
			mf->write(&nres,4);
			mf->seek(1024,MTF_BEGIN);
			mf->seteof();
		}
		else{
			mf->seek(32,MTF_BEGIN);
			mf->write(&nres,4);
		};
		mf->seek(0,MTF_END);
		n = onres;
		for (x=0;x<nres;x++){
			if (table[x].size<0){
				s = -table[x].size;
				o = mf->pos();
				mf->write((void*)table[x].offset,s);
				mf->seek(36+n*16,MTF_BEGIN);
				mf->write(&table[x],8);
				mf->write(&o,4);
				mf->write(&s,4);
				mf->seek(o+s,MTF_BEGIN);
				n++;
			};
		};
	};
	for (x=0;x<nres;x++){
		if (table[x].size<0) mtmemfree((void*)table[x].offset);
	};
	mtmemfree(table);
	if (mownfile) mtfileclose(mf);
}

int MTResources::getnumresources()
{
	return nres;
}

bool MTResources::getresourceinfo(int id,int *type,int *uid,int *size)
{
	if ((id<0) || (id>=nres)) return false;
	if (type) *type = table[id].type;
	if (uid) *uid = table[id].uid;
	if (size) *size = table[id].size;
	return true;
}

int MTResources::loadresource(int type,int uid,void *buffer,int size)
{
	int x;
	
	for (x=0;x<nres;x++){
		if ((table[x].type==type) && (table[x].uid==uid)){
			if (table[x].size>=0){
				if (table[x].size<size) size = table[x].size;
				mf->seek(table[x].offset,MTF_BEGIN);
				mf->read(buffer,size);
			}
			else{
				if (-table[x].size<size) size = -table[x].size;
				memcpy(buffer,(void*)table[x].offset,size);
			};
			return size;
		};
	};
	return 0;
}

int MTResources::loadstring(int uid,char *buffer,int size)
{
	int x;
	
	for (x=0;x<nres;x++){
		if ((table[x].type==MTR_TEXT) && (table[x].uid==uid)){
			if (table[x].size>=0){
				if (table[x].size<size) size = table[x].size;
				mf->seek(table[x].offset,MTF_BEGIN);
				mf->read(buffer,size);
			}
			else{
				if (-table[x].size<size) size = -table[x].size;
				memcpy(buffer,(char*)table[x].offset,size);
			};
			return size;
		};
	};
	return 0;
}

int MTResources::loadstringf(int uid,char *buffer,int size,...)
{
	int x;
	char *buf;
	va_list l;
	
	for (x=0;x<nres;x++){
		if ((table[x].type==MTR_TEXT) && (table[x].uid==uid)){
			if (table[x].size>=0){
				if (table[x].size<size) size = table[x].size;
				buf = (char*)mtmemalloc(size+1);
				mf->seek(table[x].offset,MTF_BEGIN);
				mf->read(buf,size);
				va_start(l,size);
				vsprintf(buffer,buf,l);
				va_end(l);
				mtmemfree(buf);
			}
			else{
				if (-table[x].size<size) size = -table[x].size;
				va_start(l,size);
				vsprintf(buffer,(char*)table[x].offset,l);
				va_end(l);
			};
			return size;
		};
	};
	return 0;
}

void* MTResources::getresource(int type,int uid,int *size)
{
	int x;
	void *buf;
	
	for (x=0;x<nres;x++){
		if ((table[x].type==type) && (table[x].uid==uid)){
			if (table[x].size>=0){
				buf = mtmemalloc(table[x].size);
				mf->seek(table[x].offset,MTF_BEGIN);
				mf->read(buf,table[x].size);
				if (size) *size = table[x].size;
			}
			else{
				buf = (void*)((char*)table[x].offset);
				if (size) *size = -table[x].size;
			};
			return buf;
		};
	};
	return 0;
}

void MTResources::releaseresource(void *res)
{
	int x;
	
	for (x=0;x<nres;x++){
		if ((table[x].size<0) && (table[x].offset==(int)res)) return;
	};
	mtmemfree(res);
}

MTFile* MTResources::getresourcefile(int type,int uid,int *size)
{
	int x;
	char buf[64];
	
	for (x=0;x<nres;x++){
		if ((table[x].type==type) && (table[x].uid==uid)){
			if (table[x].size>=0){
				mf->seek(table[x].offset,MTF_BEGIN);
				if (size) *size = table[x].size;
				return mf->subclass(-1,table[x].size,-1);
			}
			else{
				if (size) *size = -table[x].size;
				sprintf(buf,"mem://%.8X:%.8X",table[x].offset,-table[x].size);
				return mtfileopen(buf,MTF_READ|MTF_SHARE);
			};
		};
	};
	return 0;
}

void MTResources::releaseresourcefile(MTFile *f)
{
	mtfileclose(f);
}

bool MTResources::addresource(int type,int uid,void *res,int size)
{
	int x,id;
	void *buf;
	
	setmodified();
	id = -1;
	for (x=0;x<nres;x++){
		if ((table[x].type==type) && (table[x].uid==uid)){
			id = x;
			if (table[x].size<0) mtmemfree((void*)table[x].offset);
		};
	};
	if (id<0){
		id = nres++;
		if (nres>nares){
			nares += 8;
			table = (MTResTable*)mtmemrealloc(table,sizeof(MTResTable)*nares);
		};
	};
	table[id].type = type;
	table[id].uid = uid;
	table[id].size = -size;
	buf = mtmemalloc(size);
	table[id].offset = (int)buf;
	memcpy(buf,res,size);
	return true;
}

bool MTResources::addfile(int type,int uid,MTFile *f)
{
	int size;
	void *buf;
	int x,id;

	if (!f) return false;
	size = f->length();
	if (size<=0) return false;
	setmodified();
	id = -1;
	for (x=0;x<nres;x++){
		if ((table[x].type==type) && (table[x].uid==uid)){
			id = x;
			if (table[x].size<0) mtmemfree((void*)table[x].offset);
			break;
		};
	};
	if (id<0){
		id = nres++;
		if (nres>nares){
			nares += 8;
			table = (MTResTable*)mtmemrealloc(table,sizeof(MTResTable)*nares);
		};
	};
	table[id].type = type;
	table[id].uid = uid;
	table[id].size = -size;
	buf = mtmemalloc(size);
	table[id].offset = (int)buf;
	f->seek(0,MTF_BEGIN);
	f->read(buf,size);
	return true;
}

const char* MTResources::getresourceurl()
{
	if (!mf) return 0;
	return mf->url;
}

void MTResources::setmodified()
{
	int x;
	void *buf;

	modified = true;
	onres = 0;
	for (x=0;x<nres;x++){
		if (table[x].size>=0){
			mf->seek(table[x].offset,MTF_BEGIN);
			buf = mtmemalloc(table[x].size);
			mf->read(buf,table[x].size);
			table[x].size = -table[x].size;
			table[x].offset = (int)buf;
		};
	};
}
//---------------------------------------------------------------------------
