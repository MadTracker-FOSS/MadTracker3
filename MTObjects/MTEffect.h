//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTEffect.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTEFFECT_INCLUDED
#define MTEFFECT_INCLUDED
//---------------------------------------------------------------------------
class Effect;
class EffectInstance;
//---------------------------------------------------------------------------
#include "MTNode.h"
#include <memory.h>
//---------------------------------------------------------------------------
class Effect : public Node{
public:
	float quality;
	float dry,wet;
	EffectInstance *ei;

	Effect(MTObject *parent,mt_uint32 type,mt_int32 i);
	virtual ~Effect();

	int MTCT process(int ooffset,int ioffset,int count,bool &silence);
	virtual void MTCT setquality(float quality){ this->quality = quality; };
	virtual EffectInstance* MTCT createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller = 0) = 0;
	virtual void MTCT deleteinstance(EffectInstance *i) = 0;
	virtual int MTCT getnumparams(int cat){ return 0; };
	virtual const char* MTCT getparamname(int cat,int id){ return ""; };
	virtual double MTCT getparam(int cat,int id,char *display){ return 0.0; };
	virtual void MTCT setparam(int cat,int id,double value,int steps = 0){ };
};

class EffectInstance{
public:
	int id;
	MTModule *module;
	Effect *parent;
	int ninputs,noutputs;
	sample *inputs[8],*outputs[8];
	float quality;

	EffectInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller){
		module = p->module; parent = p; ninputs = ni; noutputs = no; memcpy(inputs,i,sizeof(sample*)*ninputs); memcpy(outputs,o,sizeof(sample*)*noutputs); quality = p->quality;
	};
	virtual ~EffectInstance(){	};

	virtual int MTCT process(int ooffset,int ioffset,int count,bool &silence) = 0;
	virtual void MTCT setparam(int cat,int id,double value,int steps = 0) { };
};
//---------------------------------------------------------------------------
#endif
