//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObjects2.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTObjects2.h"
#include "MTModule.h"
//---------------------------------------------------------------------------
#ifdef MTVERSION_PROFESSIONAL
//---------------------------------------------------------------------------
int nthreads;
MTThread *thread[128];
//---------------------------------------------------------------------------
int MTCT engineproc(MTThread *thread,void *param)
{
	int msg,lng;
	MTModule *module;

	while (!thread->terminated){
		if (thread->getmessage(msg,*(int*)&module,lng,true)){
			if (msg==-1) break;
			else if (msg==4096){
				if ((!module) || (module->lockread)) continue;
				module->subprocess(0,(int)param,lng);
			};
		}
		else{
			si->syswait(1);
		};
	};
	return 0;
}
//---------------------------------------------------------------------------
#endif // MTVERSION_PROFESSIONAL
//---------------------------------------------------------------------------
