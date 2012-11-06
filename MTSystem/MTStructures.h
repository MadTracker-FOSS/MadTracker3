//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTStructures.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSTRUCTURES_INCLUDED
#define MTSTRUCTURES_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
class MTArray;
class MTHash;
typedef void (MTCT *ItemProc)(void *item,void *param);
typedef int (MTCT *SortProc)(void *item1,void *item2);
//---------------------------------------------------------------------------
class MTArray{
public:
	int _is;
	int nitems;
	union{
		void **a;
		void *d;
	};

	MTArray(int allocby,int itemsize = 0);
	virtual ~MTArray();

	inline void* operator[](unsigned int i){ return (a)?a[i]:((d)?(char*)d+_is*i:0); };
	virtual int MTCT additem(int at,void *item);
	virtual int MTCT additems(int at,int count);
	virtual void MTCT delitems(int from,int count);
	virtual int MTCT setitem(int at,void *item);
	virtual int MTCT push(void *item);
	virtual void* MTCT pop();
	virtual int MTCT getitemid(void *item);
	virtual void MTCT remove(void *item);
	virtual void MTCT clear(bool deldata = false,ItemProc proc = 0,void *param = 0);
	virtual void MTCT reset();
	virtual void* MTCT next();
	virtual void MTCT sort(SortProc proc);
private:
	int mallocby;
	int na;
	int countid;
	void quicksort(int lo,int hi,SortProc proc);
	void quicksortf(int lo,int hi,SortProc proc);
};

struct MTHashData{
	unsigned int key;
	char *ckey;
	void *data;
	int reserved;
};

class MTHash{
public:
	int nitems;

	MTHash(int allocby);
	virtual ~MTHash();

	virtual int MTCT additem(const char *key,void *data);
	virtual int MTCT additem(int key,void *data);
	virtual void MTCT delitem(const char *key,bool deldata = false,ItemProc proc = 0,void *param = 0);
	virtual void MTCT delitem(int key,bool deldata = false,ItemProc proc = 0,void *param = 0);
	virtual void MTCT delitemfromid(int id,bool deldata = false,ItemProc proc = 0,void *param = 0);
	virtual void* MTCT getitem(const char *key);
	virtual void* MTCT getitem(int key);
	virtual void* MTCT getitemfromid(int id);
	virtual int MTCT getitemid(void *item);
	virtual const char* MTCT getitemkey(void *item);
	virtual void MTCT clear(bool deldata = false,ItemProc proc = 0,void *param = 0);
	virtual void MTCT reset();
	virtual void* MTCT next();
private:
	MTHashData *hash;
	int mallocby;
	int na;
	int countid;
};
//---------------------------------------------------------------------------
extern "C"
{
MTArray* MTCT mtarraycreate(int allocby,int itemsize);
void MTCT mtarraydelete(MTArray *array);
MTHash* MTCT mthashcreate(int allocby);
void MTCT mthashdelete(MTHash *hash);
}
//---------------------------------------------------------------------------
#endif
