//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTResources.h 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTRESOURCES_INCLUDED
#define MTRESOURCES_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
#include <MTXAPI/MTXSystem.h>
//---------------------------------------------------------------------------
#define MTR_WINDOW   FOURCC('M','T','W','N')
#define MTR_SKIN     FOURCC('M','T','S','K')
#define MTR_TEXT     FOURCC('M','T','T','X')
#define MTR_SHORTCUT FOURCC('M','T','S','C')
#define MTR_BITMAP   FOURCC('M','T','B','M')
#define MTR_HTML     FOURCC('M','T','H','T')
#define MTR_SAMPLE   FOURCC('M','T','S','P')

//---------------------------------------------------------------------------
class MTResources;
//---------------------------------------------------------------------------
#include "MTFile.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern "C" {
MTResources* MTCT mtresfind(const char* filename, bool write);
MTResources* MTCT mtresopen(MTFile* f, bool ownfile);
void MTCT mtresclose(MTResources* res);
}
//---------------------------------------------------------------------------
#endif
