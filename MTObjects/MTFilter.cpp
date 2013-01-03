//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTFilter.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTObject.h"
#include "MTFilter.h"
#include <MTXAPI/MTXSystem2.h>
#include "../Interface/MTObjectsRES.h"
//---------------------------------------------------------------------------
MTFilter::MTFilter(MTObject *parent,mt_int32 i):
Effect(parent,MTO_MTFILTER,i),
frequency(4000.0),
resonance(0.5)
{
#	ifdef MTSYSTEM_RESOURCES
		res->loadstringf(MTT_effect,name,255,i+1);
#	endif
}

EffectInstance* MTFilter::createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller)
{
	if (ninputs!=noutputs) return 0;
	if (caller){
		return new MTFilterInstance(this,noutputs,outputs,ninputs,inputs,caller);
	};
	ei = new MTFilterInstance(this,noutputs,outputs,ninputs,inputs,0);
	return ei;
}

void MTFilter::deleteinstance(EffectInstance *i)
{
	if (!i) i = ei;
	if (i){
		delete (MTFilterInstance*)i;
		if (i==ei) ei = 0;
	};
}

int MTFilter::getnumparams(int cat)
{
	if (cat==0) return 5;
	return 0;
}

const char* MTFilter::getparamname(int cat,int id)
{
	if (cat!=0) return "";
	switch (id){
	case 0:
		return "Cutoff";
	case 1:
		return "Resonance";
	case 2:
		return "LFO Waveform";
	case 3:
		return "LFO Period";
	case 4:
		return "LFO Amplitude";
	default:
		return "";
	};
}

double MTFilter::getparam(int cat,int id,char *display)
{
	if (cat!=0) return 0.0;
	switch (id){
	case 0:
		if (display) sprintf(display,"%.0f Hz",frequency);
		return frequency/44100.0;
	case 1:
		if (display) sprintf(display,"%.2f%",resonance*100);
		return resonance;
	case 2:
		return 0.0;
	case 3:
		return 0.0;
	case 4:
		return 0.0;
	default:
		return 0.0;
	};
}

void MTFilter::setparam(int cat,int id,double value,int steps)
{
	if (cat!=0) return;
	switch (id){
	case 0:
		frequency = (float)(value*44100.0);
		break;
	case 1:
		resonance = (float)value;
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	};
	if (ei) ei->setparam(cat,id,value,steps);
}
//---------------------------------------------------------------------------
MTFilterInstance::MTFilterInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller):
EffectInstance(p,no,o,ni,i,caller)
{
	int x;

	MTFilter &cp = *(MTFilter*)p;
	mtmemzero(&status,sizeof(status));
	for (x=0;x<ni;x++){
		status[x].frequency = cp.frequency;
		status[x].resonance = cp.resonance;
	};
}

int MTFilterInstance::process(int ooffset,int ioffset,int count,bool &silence)
{
	int x;

	for (x=0;x<noutputs;x++){
		dspi->filter[0](outputs[x]+ooffset,inputs[x]+ioffset,status[x],count,output->frequency);
	};
	return count;
}

void MTFilterInstance::setparam(int cat,int id,double value,int length)
{
	int x;

	if (cat!=0) return;
	switch (id){
	case 0:
		if (length){
			for (x=0;x<noutputs;x++){
				status[x].fvarlng = length;
				status[x].frequency2 = (float)value;
				status[x].flags |= FILTER_RAMP;
			};
		}
		else{
			for (x=0;x<noutputs;x++){
				status[x].fvarlng = 0;
				status[x].frequency = (float)value;
				status[x].flags = 0;
			};
		};
		break;
	case 1:
		if (length){
			for (x=0;x<noutputs;x++){
				status[x].rvarlng = length;
				status[x].resonance2 = (float)value;
				status[x].flags |= FILTER_RAMP;
			};
		}
		else{
			for (x=0;x<noutputs;x++){
				status[x].rvarlng = 0;
				status[x].resonance = (float)value;
				status[x].flags = 0;
			};
		};
		break;
	};
}
//---------------------------------------------------------------------------
