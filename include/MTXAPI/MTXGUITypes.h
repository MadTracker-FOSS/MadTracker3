//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXGUITypes.h 67 2005-08-26 22:18:51Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXGUITYPES_INCLUDED
#define MTXGUITYPES_INCLUDED
//---------------------------------------------------------------------------
#ifndef MTGUITypes
#define MTGUITypes
struct MTPoint
{
    int x, y;
};

struct MTRect
{
    int left, top, right, bottom;
};

struct MTBounds
{
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
};
#endif

#ifndef MTSkinTypes
#define MTSkinTypes
struct MTSkinPart
{
    unsigned char mode, flags, bmpid, reserved;
    union
    {
        MTBounds b;
        int color;
    };
};
#endif
//---------------------------------------------------------------------------
#endif
