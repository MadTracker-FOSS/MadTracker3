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
#ifdef _WIN32
	#include <windows.h>
#else
	#include <gdk/gdk.h>
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
bool rectinrgn(MTRect &r,void *rgn)
{
	return (RectInRegion((HRGN) rgn, (RECT*) &r) != 0);
}

void *recttorgn(MTRect &r)
{
	return (void*) CreateRectRgnIndirect((RECT*) &r);
}

void rgntorect(void *rgn,MTRect &r)
{
	GetRgnBox((HRGN) rgn, (RECT*) &r);
}

void offsetrgn(void *rgn,int ox,int oy)
{
	OffsetRgn((HRGN) rgn, ox, oy);
	lastrgn = 0;
}

void *copyrgn(void *rgn)
{
	HRGN tmp = CreateRectRgn(0, 0, 1, 1);
	CombineRgn((HRGN) tmp, (HRGN) rgn, (HRGN) rgn, RGN_COPY);
	return tmp;
}

bool isemptyrgn(void *rgn)
{
	return (GetRegionData((HRGN) rgn, 0, 0) == sizeof(RGNDATAHEADER));
}

void deletergn(void *rgn)
{
	DeleteObject((HRGN) rgn);
	lastrgn = 0;
}

void intersectrgn(void *rgn,void *operand)
{
	CombineRgn((HRGN) rgn, (HRGN) rgn, (HRGN) operand, RGN_AND);
	lastrgn = 0;
}

void addrgn(void *rgn,void *operand)
{
	CombineRgn((HRGN) rgn, (HRGN) rgn, (HRGN) operand, RGN_OR);
	lastrgn = 0;
}

void subtractrgn(void *rgn,void *operand)
{
	CombineRgn((HRGN)rgn,(HRGN)rgn,(HRGN)operand,RGN_DIFF);
	lastrgn = 0;
}

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
bool rectinrgn(MTRect &r,void *rgn)
{
	return (gdk_region_rect_in((GdkRegion*) rgn, (GdkRectangle*) &r) == GDK_OVERLAP_RECTANGLE_IN);
}

void *recttorgn(MTRect &r)
{
	return (void*) gdk_region_rectangle((GdkRectangle*)&r);
}

void rgntorect(void *rgn,MTRect &r)
{
	gdk_region_get_clipbox((GdkRegion*) rgn, (GdkRectangle*) &r);
}

void offsetrgn(void *rgn,int ox,int oy)
{
	gdk_region_offset((GdkRegion*)rgn, ox, oy);
	lastrgn = 0;
}

void *copyrgn(void *rgn)
{
	return (void*) gdk_region_copy((GdkRegion*) rgn);
}

bool isemptyrgn(void *rgn)
{
	return (gdk_region_empty((GdkRegion*) rgn) != 0);
}

void deletergn(void *rgn)
{
	gdk_region_destroy((GdkRegion*) rgn);
	lastrgn = 0;
}

void intersectrgn(void *rgn,void *operand)
{
	gdk_region_intersect((GdkRegion*) rgn, (GdkRegion*) operand);
	lastrgn = 0;
}

void addrgn(void *rgn,void *operand)
{
	gdk_region_union((GdkRegion*) rgn, (GdkRegion*) operand);
	lastrgn = 0;
}

void subtractrgn(void *rgn,void *operand)
{
	gdk_region_subtract((GdkRegion*) rgn, (GdkRegion*) operand);
	lastrgn = 0;
}

// TODO: GTK PORT
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
