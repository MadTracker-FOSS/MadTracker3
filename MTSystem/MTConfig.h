//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTConfig.h 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTCONFIG_INCLUDED
#define MTCONFIG_INCLUDED
//---------------------------------------------------------------------------
enum MTConfigType{
	MTCT_CONFIG = 0,
	MTCT_SINTEGER,
	MTCT_UINTEGER,
	MTCT_FLOAT,
	MTCT_BOOLEAN,
	MTCT_STRING,
	MTCT_BINARY
};
//---------------------------------------------------------------------------
class MTConfigFile;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTFile.h"
#include "MTStructures.h"
//---------------------------------------------------------------------------
class MTConfigFile{
public:
	MTConfigFile(const char *filename);
	virtual ~MTConfigFile();
	virtual void MTCT clear();
	virtual bool MTCT setsection(const char *name);
	virtual bool MTCT getparameter(const char *paramname,void *value,int desiredtype,int size);
	virtual bool MTCT createsection(const char *name);
	virtual bool MTCT setparameter(const char *paramname,void *value,int type,int size);
	virtual const char* MTCT getfilename();
	bool loaded();
private:
	MTFile *f;
	int sectionpos,sectionline,sectionnp,cpos,cline,cnp;
	MTArray *np;
};
//---------------------------------------------------------------------------
extern "C"
{
MTConfigFile* MTCT mtconfigfind(const char *filename);
MTConfigFile* MTCT mtconfigopen(const char *filename);
void MTCT mtconfigclose(MTConfigFile *file);
}
//---------------------------------------------------------------------------
#endif
