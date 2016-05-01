//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXSystem2.h 77 2005-08-28 20:51:37Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXSYSTEM2_INCLUDED
#define MTXSYSTEM2_INCLUDED
//---------------------------------------------------------------------------
#include <string.h>

inline void mtmemzero(void *mem, int count) // Pretty pointless function, but okay.
{
	memset(mem,0,count);
}

inline void mtmemzero64(void *mem, int count)
{
	memset(mem, 0, count * 8);
}
//---------------------------------------------------------------------------
#endif // MTXSYSTEM2_INCLUDED
