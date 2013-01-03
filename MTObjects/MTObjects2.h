//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObjects2.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTOBJECTS2_INCLUDED
#define MTOBJECTS2_INCLUDED
//---------------------------------------------------------------------------
#ifdef MTVERSION_PROFESSIONAL
//---------------------------------------------------------------------------
#include <MTXAPI/MTXSystem.h>
//---------------------------------------------------------------------------
int MTCT engineproc(MTThread *thread,void *param);
//---------------------------------------------------------------------------
extern int nthreads;
extern MTThread *thread[128];
//---------------------------------------------------------------------------
#endif // MTVERSION_PROFESSIONAL
//---------------------------------------------------------------------------
#endif
