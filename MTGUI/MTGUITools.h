//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	Win32,Linux (GTK+)
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGUITools.h 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTGUITOOLS_INCLUDED
#define MTGUITOOLS_INCLUDED

#ifndef MTGUITypes
#define MTGUITypes
struct MTPoint{
	int x,y;
};

struct MTRect{
	int left,top,right,bottom;
};

struct MTBounds{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
};
#endif

#ifdef _DEBUG
	#define DUMPRGN(R) dumprgn(R)
#else
	#define DUMPRGN
#endif

#ifdef _WIN32
	#include <windows.h>
#else
	#include <gdk/gdk.h>
#endif
//---------------------------------------------------------------------------
#ifdef _WIN32
	extern RGNDATA *rgndata;
#endif
 extern void *lastrgn;
//---------------------------------------------------------------------------
void minmax(int &min,int &max);
void pminmax(MTPoint &min,MTPoint &max);
inline bool pointinrect(MTPoint &p,MTRect &r)
{
	return ((p.x>=r.left) && (p.x<=r.right) && (p.y>=r.top) && (p.y<=r.bottom));
};
inline bool cliprect(MTRect &sel,MTRect &clip)
{
	if (sel.left<clip.left) sel.left = clip.left;
	if (sel.top<clip.top) sel.top = clip.top;
	if (sel.right>clip.right) sel.right = clip.right;
	if (sel.right<=sel.left) return false;
	if (sel.bottom>clip.bottom) sel.bottom = clip.bottom;
	if (sel.bottom<=sel.top) return false;
	return true;
};
inline bool intersectrect(MTRect &sel,MTRect &clip)
{
	if ((sel.right<clip.left) || (sel.bottom<clip.top) || (sel.left>clip.right) || (sel.top>clip.bottom)) return false;
	return true;
};
#ifdef _WIN32
	inline bool rectinrgn(MTRect &r,void *rgn) {return (RectInRegion((HRGN)rgn,(RECT*)&r)!=0);};
	inline void *recttorgn(MTRect &r) {return (void*)CreateRectRgnIndirect((RECT*)&r);};
	inline void rgntorect(void *rgn,MTRect &r) {GetRgnBox((HRGN)rgn,(RECT*)&r);};
	inline void offsetrgn(void *rgn,int ox,int oy) {OffsetRgn((HRGN)rgn,ox,oy);lastrgn=0;};
	inline void *copyrgn(void *rgn) {HRGN tmp = CreateRectRgn(0,0,1,1);CombineRgn((HRGN)tmp,(HRGN)rgn,(HRGN)rgn,RGN_COPY);return tmp;};
	inline bool isemptyrgn(void *rgn) {return (GetRegionData((HRGN)rgn,0,0)==sizeof(RGNDATAHEADER));};
	inline void deletergn(void *rgn) {DeleteObject((HRGN)rgn);lastrgn=0;};
	inline void intersectrgn(void *rgn,void *operand) {CombineRgn((HRGN)rgn,(HRGN)rgn,(HRGN)operand,RGN_AND);lastrgn=0;};
	inline void addrgn(void *rgn,void *operand) {CombineRgn((HRGN)rgn,(HRGN)rgn,(HRGN)operand,RGN_OR);lastrgn=0;};
	inline void subtractrgn(void *rgn,void *operand) {CombineRgn((HRGN)rgn,(HRGN)rgn,(HRGN)operand,RGN_DIFF);lastrgn=0;};
#else
	inline bool rectinrgn(MTRect &r,void *rgn) {return (gdk_region_rect_in((GdkRegion*)rgn,(GdkRectangle*)&r)==GDK_OVERLAP_RECTANGLE_IN);};
	inline void *recttorgn(MTRect &r) {return (void*)gdk_region_rectangle((GdkRectangle*)&r);};
	inline void rgntorect(void *rgn,MTRect &r) {gdk_region_get_clipbox((GdkRegion*)rgn,(GdkRectangle*)&r);};
	inline void offsetrgn(void *rgn,int ox,int oy) {gdk_region_offset((GdkRegion*)rgn,ox,oy);lastrgn=0;};
	inline void *copyrgn(void *rgn) {return (void*)gdk_region_copy((GdkRegion*)rgn);};
	inline bool isemptyrgn(void *rgn) {return (gdk_region_empty((GdkRegion*)rgn)!=0);};
	inline void deletergn(void *rgn) {gdk_region_destroy((GdkRegion*)rgn);lastrgn=0;};
	inline void intersectrgn(void *rgn,void *operand) {gdk_region_intersect((GdkRegion*)rgn,(GdkRegion*)operand);lastrgn=0;};
	inline void addrgn(void *rgn,void *operand) {gdk_region_union((GdkRegion*)rgn,(GdkRegion*)operand);lastrgn=0;};
	inline void subtractrgn(void *rgn,void *operand) {gdk_region_subtract((GdkRegion*)rgn,(GdkRegion*)operand);lastrgn=0;};
#endif
int rgngetnrects(void *rgn);
void rgngetrect(void *rgn,int id,MTRect *r);
void dumprgn(void *rgn);
void *createfont(const char *face,int size,bool bold);
void deletefont(void *font);
inline int calcpitch(int width,int bitcount,int granbits) {return (width*((bitcount+7)>>3)>>granbits)<<granbits;};
int calccolor(int source,int dest,float f);
//---------------------------------------------------------------------------
#endif
