//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTMiniConfig.h 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTMINICONFIG_INCLUDED
#define MTMINICONFIG_INCLUDED
//---------------------------------------------------------------------------
#define MTMC_STRUCTURE 1
#define MTMC_DATA      2
#define MTMC_ALL       3
#define MTMC_MD5KEYS   4
#define MTMC_HEADER    8
//---------------------------------------------------------------------------
class MTMiniConfig;
//---------------------------------------------------------------------------
#include "MTConfig.h"
//---------------------------------------------------------------------------
class MTMiniConfig{
public:
	MTMiniConfig();
	virtual ~MTMiniConfig();
	virtual bool MTCT getparameter(const char *paramname,void *value,int desiredtype,int size);
	virtual bool MTCT setparameter(const char *paramname,const void *value,int type,int size);
	virtual int MTCT loadfromstream(MTFile *f,int flags = (MTMC_ALL|MTMC_HEADER));
	virtual int MTCT savetostream(MTFile *f,int flags = (MTMC_ALL|MTMC_HEADER));
private:
	int np;
	MTHash *mp;
};
//---------------------------------------------------------------------------
extern "C"
{
MTMiniConfig* MTCT mtminiconfigcreate();
void MTCT mtminiconfigdelete(MTMiniConfig *cfg);
}
//---------------------------------------------------------------------------
#endif
