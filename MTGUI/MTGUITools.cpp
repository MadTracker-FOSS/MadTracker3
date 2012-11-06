//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	Win32,Linux (GTK+)
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGUITools.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTGUITools.h"
#ifdef _DEBUG
	#include <assert.h>
#endif
#include "MTXSystem.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
static char buffer[4096];
#ifdef _WIN32
	RGNDATA *rgndata = (RGNDATA*)buffer;
#endif
void *lastrgn;
//---------------------------------------------------------------------------
extern MTSystemInterface *si;
//---------------------------------------------------------------------------
void minmax(int &min,int &max)
{
	int x;
	
	if (min>max){
		x = min;
		min = max;
		max = x;
	};
}

void pminmax(MTPoint &min,MTPoint &max)
{
	int x;
	
	if (min.x>max.x){
		x = min.x;
		min.x = max.x;
		max.x = x;
	};
	if (min.y>max.y){
		x = min.y;
		min.y = max.y;
		max.y = x;
	};
}

#ifdef _WIN32
int rgngetnrects(void *rgn)
{
	if (lastrgn!=rgn){
		int l = GetRegionData((HRGN)rgn,0,0);
#ifdef _DEBUG
		assert(l<=4096);
#endif
		GetRegionData((HRGN)rgn,l,rgndata);
		lastrgn = rgn;
	};
	return rgndata->rdh.nCount;
}

void rgngetrect(void *rgn,int id,MTRect *r)
{
	if (lastrgn!=rgn){
		int l = GetRegionData((HRGN)rgn,0,0);
#ifdef _DEBUG
		assert(l<=4096);
#endif
		GetRegionData((HRGN)rgn,l,rgndata);
		lastrgn = rgn;
	};
	*r = *(MTRect*)((char*)&rgndata->Buffer+sizeof(RECT)*id);
}

void dumprgn(void *rgn)
{
	int x,n;

	if (lastrgn!=rgn){
		int l = GetRegionData((HRGN)rgn,0,0);
#ifdef _DEBUG
		assert(l<=4096);
#endif
		GetRegionData((HRGN)rgn,l,rgndata);
		lastrgn = rgn;
	};
	n = rgndata->rdh.nCount;
	FLOG1("%d rectangles:"NL,n);
	MTRect *r = (MTRect*)((char*)&rgndata->Buffer);
	for (x=0;x<n;x++,r++){
		FLOG4("l: %d t: %d r: %d b: %d"NL,r->left,r->top,r->right,r->bottom);
	};
}

void *createfont(const char *face,int size,bool bold)
{
	LOGFONT font;
	
	mtmemzero(&font,sizeof(font));
	strcpy(font.lfFaceName,face);
	font.lfHeight = -size;
	font.lfWeight = 400;
	if (bold & 1) font.lfWeight += 300;
	return (void*)CreateFontIndirect(&font);
}

void deletefont(void *font)
{
	DeleteObject((HFONT)font);
}
#else
//TODO
#endif

int calccolor(int source,int dest,float f)
{
	if (f==0.0) return source;
	if (f==1.0) return dest;
	if (source==dest) return source;

	register unsigned long ca1,cb1,ca2,cb2;
	register unsigned long uf = (unsigned long)(f*256.0);

	ca1 = source & 0xFF00FF;
	cb1 = (source & 0xFF00FF00)>>8;
	ca2 = dest & 0xFF00FF;
	cb2 = (dest & 0xFF00FF00)>>8;
	ca2 *= uf;
	cb2 *= uf;
	uf = 256-uf;
	ca1 *= uf;
	cb1 *= uf;
	ca1 &= 0xFF00FF00;
	ca2 &= 0xFF00FF00;
	cb1 &= 0xFF00FF00;
	cb2 &= 0xFF00FF00;
	return ((ca1+ca2)>>8)|(cb1+cb2);
}
//---------------------------------------------------------------------------
