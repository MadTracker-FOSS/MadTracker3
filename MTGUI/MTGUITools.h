//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	Win32,Linux (GTK+)
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
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
bool rectinrgn(MTRect &r,void *rgn);
void *recttorgn(MTRect &r);
void rgntorect(void *rgn,MTRect &r);
void offsetrgn(void *rgn,int ox,int oy);
void *copyrgn(void *rgn);
bool isemptyrgn(void *rgn) ;
void deletergn(void *rgn);
void intersectrgn(void *rgn,void *operand);
void addrgn(void *rgn,void *operand);
void subtractrgn(void *rgn,void *operand);
int rgngetnrects(void *rgn);
void rgngetrect(void *rgn,int id,MTRect *r);
void dumprgn(void *rgn);
void *createfont(const char *face,int size,bool bold);
void deletefont(void *font);
inline int calcpitch(int width,int bitcount,int granbits) {return (width*((bitcount+7)>>3)>>granbits)<<granbits;};
int calccolor(int source,int dest,float f);
//---------------------------------------------------------------------------
#endif
