//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattEditor.h 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTPATTEDITOR_INCLUDED
#define MTPATTEDITOR_INCLUDED
//---------------------------------------------------------------------------
#include "MTPattern.h"
//---------------------------------------------------------------------------
void initPattEditor();
void uninitPattEditor();
bool MTCT PatternEdit(MTObject *object,MTWindow *window,int flags,MTUser *user);
//---------------------------------------------------------------------------
extern int peksgroup;
//---------------------------------------------------------------------------
#endif
