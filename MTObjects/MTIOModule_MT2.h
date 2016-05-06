//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTIOModule_MT2.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTIOMODULE_MT2_INCLUDED
#define MTIOMODULE_MT2_INCLUDED
//---------------------------------------------------------------------------
#include "MTObject.h"

//---------------------------------------------------------------------------
bool loadMT2(MTObject *object, char *filename, void *process);

bool infoMT2(MTMiniConfig *data, char *filename, void *process);
//---------------------------------------------------------------------------
#endif
