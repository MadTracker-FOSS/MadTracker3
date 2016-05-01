//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTFile.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTFILE_INCLUDED
#define MTFILE_INCLUDED
//---------------------------------------------------------------------------
#define MTF_READ       0x01
#define MTF_WRITE      0x02
#define MTF_CREATE     0x04
#define MTF_SHAREREAD  0x08
#define MTF_SHAREWRITE 0x10
#define MTF_SHARE      0x18
#define MTF_TEMP       0x80

#define MTF_BEGIN      0
#define MTF_CURRENT    1
#define MTF_END        2
#define MTF_LINE       3

#define MTFA_TYPEMASK 7
#define MTFA_ROOT     1
#define MTFA_DISK     2
#define MTFA_FOLDER   3
#define MTFA_FILE     4
#define MTFA_HIDDEN   8
#define MTFA_READONLY 16
//---------------------------------------------------------------------------
class MTFile;
class MTFolder;
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
#include "MTKernel.h"
#include <MTXAPI/MTXSystem.h>
//---------------------------------------------------------------------------
extern "C"
{
void initFiles();
void uninitFiles();
//---------------------------------------------------------------------------
MTFile* MTCT mtfileopen(const char *url,int flags);
void MTCT mtfileclose(MTFile *file);
MTFolder* MTCT mtfolderopen(char *url);
void MTCT mtfolderclose(MTFolder *folder);
bool MTCT mtfileexists(char *url);
bool MTCT mtfilecopy(char *source,char *destination);
bool MTCT mtfiledelete(char *filename);
bool MTCT mtfilerename(char *filename,char *newname);
void MTCT mtfiletype(const char *filename,char *filetype,int length);
void MTCT mtfilemaketemp(char *filename,int length);
MTFile* MTCT mttempfile(int access);
}
//---------------------------------------------------------------------------
extern MTHash *hooks;
//---------------------------------------------------------------------------
#endif
