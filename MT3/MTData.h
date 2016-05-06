//---------------------------------------------------------------------------
//	$Id: MTData.h 100 2005-11-30 20:19:39Z Yannick $
//---------------------------------------------------------------------------
#ifndef MTDATA_INCLUDED
#define MTDATA_INCLUDED
//---------------------------------------------------------------------------
#include "MTExtensions.h"
#include <MTXAPI/MTXModule.h>
#include <MTXAPI/MTXAudio.h>
#include <MTXAPI/MTXSystem.h>

//---------------------------------------------------------------------------
struct _MTConf
{
    int lastuse;
    int refcount;
    MTConfigFile *conf;
};

//---------------------------------------------------------------------------
int newmodule();

//---------------------------------------------------------------------------
bool init();

void uninit();

//---------------------------------------------------------------------------
extern MTPreferences prefs;

extern MTModule *module[16];

extern WaveOutput *output;

extern MTUser cuser;

extern MTFile *outmsg;

extern MTHash *confs;

extern bool wantreset;

extern bool exitasap;
//---------------------------------------------------------------------------
#endif
