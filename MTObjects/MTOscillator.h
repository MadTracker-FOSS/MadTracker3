//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTOscillator.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTOSCILLATOR_INCLUDED
#define MTOSCILLATOR_INCLUDED

class Oscillator;
class OscillatorInstance;

#define VOLUME_THRESOLD 4.656612873077392578125e-10

#include "MTObject.h"
#include "MTTrack.h"
#include "MTInstrument.h"
#include "MTObjectsASM.h"
#include "MTObjects1.h"
#include <MTXAPI/MTXDSP.h>
//---------------------------------------------------------------------------
class Oscillator : public MTObject{
public:
	float quality;

	Oscillator(MTObject *parent,mt_uint32 type,mt_int32 i):MTObject(parent,type,i){
		quality = 0.125;
	};
	virtual ~Oscillator(){	};
	
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
	
	OscillatorInstance(Oscillator *p,int no,sample **o,InstrumentInstance *caller){
		module = p->module; parent = p; noutputs = no; outputs = o; volume = 1.0 ;pitch = 1.0; panx = pany = panz = 0.0; quality = p->quality;
	};
	virtual ~OscillatorInstance(){	};
	
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
#define SAFE_BUFFER  32

#define PEAK_RES     128
#define PEAK_LEVELS  1

#define SF_DIRECT2DISK  0x1
#define SF_SYNCHRONIZED 0x2
#define SF_KEEPONDISK   0x4
//---------------------------------------------------------------------------

// Peaks structure
struct Peak{
	union{
		struct{
			char bmin;
			char bmax;
		};
		struct{
			short wmin;
			short wmax;
		};
	};
};

struct Peaks{
	int count;
	Peak peak[1];
};

class SampleType : public ObjectType{
public:
	SampleType();

	MTObject* MTCT create(MTObject *parent,mt_int32 id,void *param);
};

// Sample class
class MTSample : public Oscillator{
public:
	MTSample(MTObject *parent,mt_int32 i);
	~MTSample();
	
	char *filename;
	void *data[8];
	MTFile *file;
	int fileoffset;
	Peaks *peaks;
	int time;
	int length;
	int frequency;
	char depth;
	char nchannels;
	char loop;
	char reserved;
	int loops;
	int loope;
	char note;
	double bpm;
	int ns;
	int sl;
	
	int MTCT loadfromstream(MTFile *f,int size,void *params);
	int MTCT savetostream(MTFile *f,void *params);
	OscillatorInstance* MTCT createinstance(int noutputs,sample **outputs,InstrumentInstance *caller);
	virtual bool MTCT changesign();
	virtual bool MTCT splalloc(int nsamples);
	virtual bool MTCT splrealloc(int nsamples);
	virtual bool MTCT splfree();
	virtual bool MTCT buildpeaks(void *process);
};

class MTSampleInstance : public OscillatorInstance{
public:
	int flags;
	ChannelStatus status[8];
	
	MTSampleInstance(Oscillator *p,int no,sample **o,InstrumentInstance *caller);
	
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
	int nstatus;
	int divider;
//	int pitchi;
//	unsigned int pitchd;
	int volvarlng;
	double volvar;
	int panvarlng;
	float panxvar,panyvar,panzvar;
};
//---------------------------------------------------------------------------
bool loadWAV(MTObject *object,char *filename,void *process);
//---------------------------------------------------------------------------
extern SampleType *sampletype;
//---------------------------------------------------------------------------
#endif
