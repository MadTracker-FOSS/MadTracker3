//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTFilter.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTFILTER_INCLUDED
#define MTFILTER_INCLUDED
//---------------------------------------------------------------------------
class MTFilter;
class MTFilterInstance;
//---------------------------------------------------------------------------
#include "MTEffect.h"
#include "MTXDSP.h"
//---------------------------------------------------------------------------
class MTFilter : public Effect{
public:
	float frequency;
	float resonance;

	MTFilter(MTObject *parent,mt_int32 i);
	EffectInstance* MTCT createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller = 0);
	void MTCT deleteinstance(EffectInstance *i);
	int MTCT getnumparams(int cat);
	const char* MTCT getparamname(int cat,int id);
	double MTCT getparam(int cat,int id,char *display);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
};

class MTFilterInstance : public EffectInstance{
public:
	MTFilterInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller);
	int MTCT process(int ooffset,int ioffset,int count,bool &silence);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
private:
	FilterStatus status[8];
};
//---------------------------------------------------------------------------
#endif
