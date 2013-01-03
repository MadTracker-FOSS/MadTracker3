//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGenerator.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include <math.h>
#include "MTGenerator.h"
#include "MTObjects1.h"
#include "../Interface/MTObjectsRES.h"
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
GeneratorType *generatortype;
//---------------------------------------------------------------------------
GeneratorType::GeneratorType()
{
	type = MTO_MTGENERATOR;
	description = "Generator";
}

MTObject* GeneratorType::create(MTObject *parent,mt_int32 id,void *param)
{
	return new MTGenerator(parent,id);
}
//---------------------------------------------------------------------------
// MTGenerator functions
//---------------------------------------------------------------------------
MTGenerator::MTGenerator(MTObject *parent,mt_int32 i):
Oscillator(parent,MTO_MTSAMPLE,i)
{

}

MTGenerator::~MTGenerator()
{

}

int MTGenerator::loadfromstream(MTFile *f,int size,void *params)
{
	return 0;
}

int MTGenerator::savetostream(MTFile *f,void *params)
{
	return 0;
}

OscillatorInstance* MTGenerator::createinstance(int noutputs,sample **outputs,InstrumentInstance *caller)
{
	return new MTGeneratorInstance(this,noutputs,outputs,caller);
}
//---------------------------------------------------------------------------
MTGeneratorInstance::MTGeneratorInstance(Oscillator *p,int no,sample **o,InstrumentInstance *caller):
OscillatorInstance(p,no,o,caller),
cphase(0.0)
{
}

bool MTGeneratorInstance::seek(double offset,int origin,int units)
{
	return true;
}

bool MTGeneratorInstance::process(int offset,int count,bool &silence)
{
	int x,i;
	double _phase;
	sample *cp[8];

	if (silence){
		cphase += pitch*count;
		return true;
	};
	_phase = cphase;
	for (x=0;x<noutputs;x++){
		cp[x] = outputs[x]+offset;
	};
	for (x=0;x<noutputs;x++){
		cphase = _phase;
		for (i=0;i<count;i++){
			*cp[x]++ = (float)(volume*sin(cphase));
			cphase += pitch;
		};
	};
	if (volume==0.0) silence = true;
	return true;
}

void MTGeneratorInstance::setnote(double n)
{
	note = n;
	pitch = ((double)(440.0*2.0*PI)/(double)output->frequency)*pow(1.0594630943592952645618252949463,note-69);
}

void MTGeneratorInstance::setvolume(double vs,int steps,int curve)
{
	volume = vs;
}

void MTGeneratorInstance::setpanning(float xs,float ys,float zs,int steps,int curve)
{

}

double MTGeneratorInstance::getnote()
{
	return note;
}

double MTGeneratorInstance::getvolume(int steps)
{
	return 0.0;
}

void MTGeneratorInstance::getpanning(float *x,float *y,float *z,int steps)
{

}

float MTGeneratorInstance::getimportance()
{
	return 0.5;
}
//---------------------------------------------------------------------------
