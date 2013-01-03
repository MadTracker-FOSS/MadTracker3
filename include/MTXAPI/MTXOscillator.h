//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXOscillator.h 98 2005-11-30 20:07:26Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXOSCILLATOR_INCLUDED
#define MTXOSCILLATOR_INCLUDED
//---------------------------------------------------------------------------
class Oscillator;
class OscillatorInstance;
//---------------------------------------------------------------------------
#include "MTXObject.h"
//---------------------------------------------------------------------------
class Oscillator : public MTObject{
public:
	double volume;
	float panx,pany,panz;
	float quality;

	Oscillator(MTObject *parent,mt_uint32 type,mt_int32 i);
	virtual ~Oscillator() = 0;
	
	virtual OscillatorInstance* MTCT createinstance(int noutputs,sample **outputs,InstrumentInstance *caller) = 0;
};

class OscillatorInstance{
public:
	int id;
	MTModule *module;
	Oscillator *parent;
	int noutputs;
	sample **outputs;
	double note;
	double pitch;
	double volume;
	float panx,pany,panz;
	float quality;
	
	OscillatorInstance(Oscillator *p,int no,sample **o,InstrumentInstance *caller);
	virtual ~OscillatorInstance() = 0;
	
	virtual bool MTCT seek(double offset,int origin = MTIS_BEGIN,int units = MTIS_BEATS) = 0;
	virtual bool MTCT process(int offset,int count,bool &silence) = 0;
	virtual void MTCT setnote(double n) = 0;
	virtual void MTCT setvolume(double vs,int steps = 0,int curve = 0) = 0;
	virtual void MTCT setpanning(float xs,float ys = 0.0,float zs = 0.0,int steps = 0,int curve = 0) = 0;
	virtual double MTCT getnote() = 0;
	virtual double MTCT getvolume(int steps = 0) = 0;
	virtual void MTCT getpanning(float *x,float *y,float *z,int steps = 0) = 0;
	virtual float MTCT getimportance() = 0;
};
//---------------------------------------------------------------------------
#endif
