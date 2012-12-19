//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTStructures.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTXSystem.h"
#include "MTXSystem2.h"
#include "MTStructures.h"
#include "MTSystem1.h"
#include "MTMD5.h"
//---------------------------------------------------------------------------
MTArray* mtarraycreate(int allocby,int itemsize)
{
	return new MTArray(allocby,itemsize);
}

void mtarraydelete(MTArray *array)
{
	delete array;
}

MTHash* mthashcreate(int allocby)
{
	return new MTHash(allocby);
}

void mthashdelete(MTHash *hash)
{
	delete hash;
}
//---------------------------------------------------------------------------
MTArray::MTArray(int allocby,int itemsize):
nitems(0),
a(0),
mallocby(allocby),
na(0),
_is(itemsize)
{
	if (mallocby<=0) mallocby = 4;
	countid = mtlocalalloc();
}

MTArray::~MTArray()
{
	mtlocalfree(countid);
	if (a) mtmemfree(a);
}

int MTArray::additem(int at,void *item)
{
	int cat;
	int x;

	cat = nitems;
	nitems++;
	if (_is==0){
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (a) a = (void**)mtmemrealloc(a,4*na);
			else a = (void**)mtmemalloc(4*na,MTM_ZERO);
		};
		if ((at>=0) && (at<nitems-1)){
			cat = at;
			for (x=nitems-1;x>=cat+1;x--) a[x] = a[x-1];
			for (x=cat;x<cat+1;x++) a[x] = 0;
		};
		a[cat] = item;
	}
	else{
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (d) d = mtmemrealloc(d,_is*na);
			else d = mtmemalloc(_is*na,MTM_ZERO);
		};
		if ((at>=0) && (at<nitems-1)){
			char *sp,*dp;
			cat = at;
			dp = (char*)d+(nitems-1)*_is;
			sp = dp-_is;
			for (x=nitems-1;x>=cat+1;x--){
				memcpy(dp,sp,_is);
				sp -= _is;
				dp -= _is;
			};
			memcpy((char*)d+cat*_is,item,_is);
		};
	};
	return cat;
}

int MTArray::additems(int at,int count)
{
	int cat;
	int x;

	cat = nitems;
	nitems += count;
	if (_is==0){
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (a) a = (void**)mtmemrealloc(a,4*na);
			else a = (void**)mtmemalloc(4*na,MTM_ZERO);
		};
		if ((at>=0) && (at<nitems-count)){
			cat = at;
			for (x=nitems-1;x>=cat+count;x--) a[x] = a[x-count];
			for (x=cat;x<cat+count;x++) a[x] = 0;
		};
	}
	else{
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (d) d = mtmemrealloc(d,_is*na);
			else d = mtmemalloc(_is*na,MTM_ZERO);
		};
		if ((at>=0) && (at<nitems-count)){
			char *sp,*dp;
			cat = at;
			dp = (char*)d+(nitems-1)*_is;
			sp = dp-count*_is;
			for (x=nitems-1;x>=cat+count;x--){
				memcpy(dp,sp,_is);
				sp -= _is;
				dp -= _is;
			};
			mtmemzero((char*)d+cat*_is,count*_is);
		};
	};
	return cat;
}

void MTArray::delitems(int from,int count)
{
	int x;

	nitems -= count;
	if (_is==0){
		for (x=from;x<nitems;x++) a[x] = a[x+count];
		for (x=nitems;x<nitems+count;x++) a[x] = 0;
		if (nitems<na-mallocby){
			while (nitems<na-mallocby) na -= mallocby;
			if (na) a = (void**)mtmemrealloc(a,4*na);
			else{
				mtmemfree(a);
				a = 0;
			};
		};
	}
	else{
		char *sp,*dp;
		dp = (char*)d+from*_is;
		sp = dp+count*_is;
		for (x=from;x<nitems;x++){
			memcpy(dp,sp,_is);
			sp += _is;
			dp += _is;
		};
		mtmemzero((char*)d+nitems*_is,count*_is);
		if (nitems<na-mallocby){
			while (nitems<na-mallocby) na -= mallocby;
			if (na) d = mtmemrealloc(d,_is*na);
			else{
				mtmemfree(d);
				d = 0;
			};
		};
	};
}

int MTArray::setitem(int at,void *item)
{
	if ((_is==0) || (at>=nitems)) return -1;
	memcpy((char*)d+at*_is,item,_is);
	return at;
}

int MTArray::push(void *item)
{
	int at;
	
	at = nitems++;
	if (_is==0){
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (a) a = (void**)mtmemrealloc(a,4*na);
			else a = (void**)mtmemalloc(4*na,MTM_ZERO);
		};
		a[at] = item;
	}
	else{
		if (nitems>na){
			while (nitems>na) na += mallocby;
			if (d) d = mtmemrealloc(d,_is*na);
			else d = mtmemalloc(_is*na,MTM_ZERO);
		};
		memcpy((char*)d+at*_is,item,_is);
	};
	return at;
}

void* MTArray::pop()
{
	void *item;

	if (nitems<1) return 0;
	nitems--;
	if (_is==0){
		item = a[nitems];
		a[nitems] = 0;
		if (nitems<na-mallocby){
			while (nitems<na-mallocby) na -= mallocby;
			if (na) a = (void**)mtmemrealloc(a,4*na);
			else{
				mtmemfree(a);
				a = 0;
			};
		};
	}
	else{
		item = (void*)((char*)d+nitems*_is);
		mtmemzero((char*)d+nitems*_is,_is);
		if (nitems<na-mallocby){
			while (nitems<na-mallocby) na -= mallocby;
			if (na) d = (void**)mtmemrealloc(d,_is*na);
			else{
				mtmemfree(d);
				d = 0;
			};
		};
	};
	return item;
}

int MTArray::getitemid(void *item)
{
	int x;

	if ((_is>0) || (nitems<1)) return -1;
	for (x=0;x<nitems;x++){
		if (a[x]==item) return x;
	};
	return -1;
}

void MTArray::remove(void *item)
{
	int x;

	if ((_is>0) || (nitems<1)) return;
	for (x=0;x<nitems;x++){
		if (a[x]==item){
			for (;x<nitems-1;x++) a[x] = a[x+1];
			nitems--;
			a[nitems] = 0;
			if (nitems<na-mallocby){
				while (nitems<na-mallocby) na -= mallocby;
				if (na) a = (void**)mtmemrealloc(a,4*na);
				else{
					mtmemfree(a);
					a = 0;
				};
			};
			return;
		};
	};
}

void MTArray::clear(bool deldata,ItemProc proc,void *param)
{
	int x;

	if (a){
		if (proc){
			for (x=0;x<nitems;x++) proc(a[x],param);
		};
		if ((!_is) && (deldata)){
			for (x=0;x<nitems;x++) mtmemfree(a[x]);
		};
		mtmemfree(a);
		a = 0;
	};
	na = 0;
	nitems = 0;
}

void MTArray::reset()
{
	mtlocalset(countid,0);
}

void* MTArray::next()
{
	int count = (int)mtlocalget(countid);
	if (count>=nitems) return 0;
	mtlocalset(countid,(void*)(count+1));
	if (_is==0) return a[count];
	else return (char*)d+(count)*_is;
}

void MTArray::quicksort(int lo,int hi,SortProc proc)
{
	int pi,lo2,hi2;
	void *t,*p;

	if (lo<hi){
		if (hi-lo==1){
			if (proc(a[lo],a[hi])>0){
				t = a[lo];
				a[lo] = a[hi];
				a[hi] = t;
			};
		}
		else{
			pi = (lo+hi)>>1;
			p = a[pi];
			a[pi] = a[lo];
			a[lo] = p;
			lo2 = lo+1;
			hi2= hi;
			do{
				while ((lo2<hi2) && (proc(a[lo2],p)<=0)) lo2++;
				while (proc(a[hi2],p)>0) hi2--;
				if (lo2<hi2){
					t = a[lo2];
					a[lo2] = a[hi2];
					a[hi2] = t;
				};
			} while (lo2<hi2);
			a[lo] = a[hi2];
			a[hi2] = p;
			if (lo<hi2-1) quicksort(lo,hi2-1,proc);
			if (hi2+1<hi) quicksort(hi2+1,hi,proc);
		};
	};
}

void MTArray::quicksortf(int lo,int hi,SortProc proc)
{
	int lo2,hi2;
	char *clo,*chi,*cpi,*clo2,*chi2;
	static char t[4096];
	static char t2[4096];

	if (_is>4096) return;
	if (lo<hi){
		clo = (char*)d+lo*_is;
		chi = (char*)d+hi*_is;
		if (hi-lo==1){
			if (proc(clo,chi)>0){
				memcpy(t,clo,_is);
				memcpy(clo,chi,_is);
				memcpy(chi,t,_is);
			};
		}
		else{
			cpi = (char*)d+((lo+hi)>>1)*_is;
			memcpy(t,cpi,_is);
			memcpy(cpi,clo,_is);
			memcpy(clo,t,_is);
			lo2 = lo+1;
			hi2= hi;
			do{
				while ((lo2<hi2) && (proc((char*)d+lo2*_is,t)<=0)) lo2++;
				while (proc((char*)d+hi2*_is,t)>0) hi2--;
				clo2 = (char*)d+lo2*_is;
				chi2 = (char*)d+hi2*_is;
				if (lo2<hi2){
					memcpy(t2,clo2,_is);
					memcpy(clo2,chi2,_is);
					memcpy(chi2,t2,_is);
				};
			} while (lo2<hi2);
			memcpy(clo,chi2,_is);
			memcpy(chi2,t,_is);
			if (lo<hi2-1) quicksortf(lo,hi2-1,proc);
			if (hi2+1<hi) quicksortf(hi2+1,hi,proc);
		};
	};
}

void MTArray::sort(SortProc proc)
{
	if (nitems<2) return;
	if (_is==0) quicksort(0,nitems-1,proc);
	else quicksortf(0,nitems-1,proc);
}
//---------------------------------------------------------------------------
MTHash::MTHash(int allocby):
nitems(0),
hash(0),
mallocby(allocby),
na(0)
{
	if (mallocby<=0) mallocby = 4;
	countid = mtlocalalloc();
}

MTHash::~MTHash()
{
	int x;

	mtlocalfree(countid);
	if (hash){
		for (x=0;x<nitems;x++) mtmemfree(hash[x].ckey);
		mtmemfree(hash);
	};
}

int MTHash::additem(const char *key,void *data)
{
	int chash = nitems;
	unsigned int hkey[4];

	nitems++;
	if (nitems>na){
		na += mallocby;
		if (hash) hash = (MTHashData*)mtmemrealloc(hash,sizeof(MTHashData)*na);
		else hash = (MTHashData*)mtmemalloc(sizeof(MTHashData)*na,MTM_ZERO);
	};
	if (!hash) return -1;
	md5b((unsigned char*)hkey,key);
	hkey[0] = ((hkey[0]^hkey[1])^hkey[2])^hkey[3];
	if (getitem(hkey[0])){
		LOGD("%s - [System] Warning: Key already in hash!"NL);
		return -1;
	};
	hash[chash].key = hkey[0];
	hash[chash].ckey = (char*)mtmemalloc(strlen(key)+1);
	strcpy(hash[chash].ckey,key);
	hash[chash].data = data;
	return chash;
}

int MTHash::additem(int key,void *data)
{
	int chash = nitems;

	nitems++;
	if (nitems>na){
		na += mallocby;
		if (hash) hash = (MTHashData*)mtmemrealloc(hash,sizeof(MTHashData)*na);
		else hash = (MTHashData*)mtmemalloc(sizeof(MTHashData)*na,MTM_ZERO);
	};
	if (!hash) return -1;
	if (getitem(key)){
		LOGD("%s - [System] Warning: Key already in hash!"NL);
		return -1;
	};
	hash[chash].key = key;
	hash[chash].data = data;
	return chash;
}

void MTHash::delitem(const char *key,bool deldata,ItemProc proc,void *param)
{
	int x;
	unsigned int hkey[4];

	md5b((unsigned char*)hkey,key);
	hkey[0] = ((hkey[0]^hkey[1])^hkey[2])^hkey[3];
	for (x=0;x<nitems;x++){
		if (hash[x].key==hkey[0]){
			delitemfromid(x,deldata,proc,param);
			return;
		};
	};
}

void MTHash::delitem(int key,bool deldata,ItemProc proc,void *param)
{
	int x;

	if (!hash) return;
	for (x=0;x<nitems;x++){
		if (hash[x].key==key){
			delitemfromid(x,deldata,proc,param);
			return;
		};
	};
}

void MTHash::delitemfromid(int id,bool deldata,ItemProc proc,void *param)
{
	int x = id;

	if (!hash) return;
	mtmemfree(hash[id].ckey);
	if (deldata){
		if (proc) proc(hash[id].data,param);
		mtmemfree(hash[id].data);
	};
	while (x<nitems-1){
		hash[x] = hash[x+1];
		x++;
	};
	nitems--;
	hash[nitems].key = 0;
	hash[nitems].ckey = 0;
	hash[nitems].data  = 0;
	if (nitems<na-mallocby){
		na -= mallocby;
		if (na) hash = (MTHashData*)mtmemrealloc(hash,sizeof(MTHashData)*na);
		else{
			mtmemfree(hash);
			hash = 0;
		};
	};
}

void* MTHash::getitem(const char *key)
{
	int x;
	unsigned int hkey[4];
	MTHashData *c;

	if (!hash) return 0;
	md5b((unsigned char*)hkey,key);
	hkey[0] = ((hkey[0]^hkey[1])^hkey[2])^hkey[3];
	c = hash;
	x = nitems;
	while (x-->0){
		if (c->key==hkey[0]) return c->data;
		c++;
	};
	return 0;
}

void* MTHash::getitem(int key)
{
	int x;
	MTHashData *c;

	if (!hash) return 0;
	c = hash;
	x = nitems;
	while (x-->0){
		if (c->key==key) return c->data;
		c++;
	};
	return 0;
}

void* MTHash::getitemfromid(int id)
{
	if (!hash) return 0;
	return hash[id].data;
}

int MTHash::getitemid(void *item)
{
	int x;
	MTHashData *c;

	if (!hash) return -1;
	c = hash;
	x = nitems;
	while (x-->0){
		if (c->data==item) return nitems-x+1;
		c++;
	};
	return -1;
}

const char* MTHash::getitemkey(void *item)
{
	int x;
	MTHashData *c;

	if (!hash) return 0;
	c = hash;
	x = nitems;
	while (x-->0){
		if (c->data==item) return c->ckey;
		c++;
	};
	return 0;
}

void MTHash::clear(bool deldata,ItemProc proc,void *param)
{
	int x;

	if (hash){
		for (x=0;x<nitems;x++) mtmemfree(hash[x].ckey);
		if (proc){
			for (x=0;x<nitems;x++) proc(hash[x].data,param);
		};
		if (deldata){
			for (x=0;x<nitems;x++) mtmemfree(hash[x].data);
		};
		mtmemfree(hash);
		hash = 0;
	};
	na = 0;
	nitems = 0;
}

void MTHash::reset()
{
	mtlocalset(countid,0);
}

void* MTHash::next()
{
	int count = (int)mtlocalget(countid);
	if ((count>=nitems) || (!hash)) return 0;
	mtlocalset(countid,(void*)(count+1));
	return hash[count].data;
}
//---------------------------------------------------------------------------
