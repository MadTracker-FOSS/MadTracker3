//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTGenerator.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTGENERATOR_INCLUDED
#define MTGENERATOR_INCLUDED

class MTGenerator;
class MTGeneratorInstance;

#include "MTObject.h"
#include "MTTrack.h"

#define MTO_MTGENERATOR (MTO_MTSAMPLE|1)

#define PI 3.1415926535897932384626433832795
//---------------------------------------------------------------------------
class GeneratorType : public ObjectType{
public:
	GeneratorType();

	MTObject* MTCT create(MTObject *parent,mt_int32 id,void *param);
};

class MTGenerator : public Oscillator{
public:
	MTGenerator(MTObject *parent,mt_int32 i);
	~MTGenerator();

	int MTCT loadfromstream(MTFile *f,int size,void *params);
	int MTCT savetostream(MTFile *f,void *params);
	OscillatorInstance* MTCT createinstance(int noutputs,sample **outputs,InstrumentInstance *caller);
};

class MTGeneratorInstance : public OscillatorInstance{
public:
	MTGeneratorInstance(Oscillator *p,int no,sample **o,InstrumentInstance *caller);
	
	bool MTCT seek(double offset,int origin,int units);
	bool MTCT process(int offset,int count,bool &silence);
	void MTCT setnote(double n);
	void MTCT setvolume(double vs,int steps = 0,int curve = 0);
	void MTCT setpanning(float xs,float ys = 0.0,float zs = 0.0,int steps = 0,int curve = 0);
	double MTCT getnote();
	double MTCT getvolume(int steps = 0);
	void MTCT getpanning(float *x,float *y,float *z,int steps = 0);
	float MTCT getimportance();
private:
	double cphase;
};
//---------------------------------------------------------------------------
extern GeneratorType *generatortype;
//---------------------------------------------------------------------------
#endif
