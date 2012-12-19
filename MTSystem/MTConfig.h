//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTConfig.h 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTCONFIG_INCLUDED
#define MTCONFIG_INCLUDED
//---------------------------------------------------------------------------
class MTConfigFile;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTFile.h"
#include "MTStructures.h"
#include "MTXSystem.h"
//---------------------------------------------------------------------------
extern "C"
{
MTConfigFile* MTCT mtconfigfind(const char *filename);
MTConfigFile* MTCT mtconfigopen(const char *filename);
void MTCT mtconfigclose(MTConfigFile *file);
}
//---------------------------------------------------------------------------
#endif
