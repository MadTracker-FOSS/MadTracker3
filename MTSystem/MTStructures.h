//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTStructures.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSTRUCTURES_INCLUDED
#define MTSTRUCTURES_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
//---------------------------------------------------------------------------
typedef void (MTCT *ItemProc)(void *item,void *param);
typedef int (MTCT *SortProc)(void *item1,void *item2);
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
