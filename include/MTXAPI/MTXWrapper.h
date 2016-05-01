//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXWrapper.h 67 2005-08-26 22:18:51Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXWRAPPER_INCLUDED
#define MTXWRAPPER_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
class MTWrapper;
//---------------------------------------------------------------------------
#include "MTXControls.h"
//---------------------------------------------------------------------------
class MTWrapper{
public:
	MTWindow *wthis;
	MTWrapper(MTWindow *w){ wthis = w; };
	virtual ~MTWrapper(){ };
	virtual bool MTCT onmessage(MTCMessage &msg){ return false; };
};
//---------------------------------------------------------------------------
#endif
