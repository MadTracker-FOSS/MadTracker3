//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAutomation.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTModule.h"
#include "MTObjects1.h"
#include "MTAutomation.h"
#include <MTXAPI/RES/MTObjectsRES.h>
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
// Automation functions
//---------------------------------------------------------------------------
Automation::Automation(MTObject *parent,mt_int32 i):
MTObject(parent,MTO_AUTOMATION,i)
{
	envelopes = si->arraycreate(8,sizeof(TrackAuto));
#	ifdef MTSYSTEM_RESOURCES
		res->loadstringf(MTT_automation,name,255,id);
#	endif
}

Automation::~Automation()
{
	int x,y;
	
	if (module){
		A(module->apatt,Automation)[id] = 0;
		for (y=0;y<MAX_LAYERS;y++){
			for (x=0;x<MAX_SEQUENCES;x++)
				if (module->sequ[y][x].patt==(id | 0x1000)) module->sequ[y][x].patt = 0xFFFF;
		};
	};
	si->arraydelete(envelopes);
}
//---------------------------------------------------------------------------
