//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDelay.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTObject.h"
#include "MTObjects1.h"
#include "MTDelay.h"
#include <MTXAPI/MTXSystem2.h>
#include <MTXAPI/RES/MTObjectsRES.h>
//---------------------------------------------------------------------------
DelayType *delaytype;
//---------------------------------------------------------------------------
DelayType::DelayType()
{
	type = MTO_MTDELAY;
	description = "Delay";
}


MTObject* DelayType::create(MTObject *parent,mt_int32 id,void *param)
{
	return new MTDelay(parent,id);
}
//---------------------------------------------------------------------------
MTDelay::MTDelay(MTObject *parent,mt_int32 i):
Effect(parent,MTO_MTDELAY,i),
ntaps(0),
monomerge(false)
{
#	ifdef MTSYSTEM_RESOURCES
		res->loadstringf(MTT_effect,name,255,i+1);
#	endif
	mtmemzero(buffer,sizeof(buffer));
	mtmemzero(taps,sizeof(taps));
	_frequency = ai->getoutput()->frequency;
	_bpm = module->playstatus.bpm;
	setbuffer(_frequency);
}

MTDelay::~MTDelay()
{
	int x;

	for (x=0;x<8;x++){
		si->memfree(buffer[x]);
	};
}

void MTDelay::notify(MTObject *source,int message,int param1,void *param2)
{
	int csize = nsamples;

	if (message==MTN_FREQUENCY){
		_frequency = param1;
		setbuffer(_frequency);
	}
	else if (message==MTN_TEMPO){
		_bpm = *(double*)param2;
	};
}

EffectInstance* MTDelay::createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller)
{
	if (ninputs!=noutputs) return 0;
	if (caller){
		return new MTDelayInstance(this,noutputs,outputs,ninputs,inputs,caller);
	};
	ei = new MTDelayInstance(this,noutputs,outputs,ninputs,inputs,0);
	return ei;
}

void MTDelay::deleteinstance(EffectInstance *i)
{
	if (!i) i = ei;
	if (i){
		delete (MTDelayInstance*)i;
		if (i==ei) ei = 0;
	};
}

int MTDelay::getnumparams(int cat)
{
	if (cat==-1) return 16;
	if (cat==0) return 2;
	if (cat>16) return 0;
	return 16;
}

const char* MTDelay::getparamname(int cat,int id)
{
	static const char *pname[16] = {"Flags","Delay","Volume","Feedback","X Panning","X Amplitude","X Frequency","X Phase","Y Panning","Y Amplitude","Y Frequency","Y Phase","Z Panning","Z Amplitude","Z Frequency","Z Phase"};
	if (cat==0){
		if (id==0) return "Number of taps";
		if (id==1) return "Merge taps to mono";
		return "";
	};
	if ((cat<0) || (cat>16)) return "";
	if ((id<0) || (id>=16)) return "";
	return pname[id];
}

double MTDelay::getparam(int cat,int id,char *display)
{
	if (cat==0){
		if (id==0) return ntaps;
		if (id==1) return monomerge;
		return 0.0;
	};
	if ((cat<1) || (cat>16)) return 0.0;
	cat--;
	switch (id){
	case 0: return taps[cat].flags;
	case 1: return taps[cat].delay;
	case 2: return taps[cat].volume;
	case 3: return taps[cat].feedback;
	case 4: return taps[cat].pan[0].pan;
	case 5: return taps[cat].pan[0].amp;
	case 6: return taps[cat].pan[0].period;
	case 7: return taps[cat].pan[0].phase;
	case 8: return taps[cat].pan[1].pan;
	case 9: return taps[cat].pan[1].amp;
	case 10: return taps[cat].pan[1].period;
	case 11: return taps[cat].pan[1].phase;
	case 12: return taps[cat].pan[2].pan;
	case 13: return taps[cat].pan[2].amp;
	case 14: return taps[cat].pan[2].period;
	case 15: return taps[cat].pan[2].phase;
	default: return 0.0;
	};
}

void MTDelay::setparam(int cat,int id,double value,int steps)
{
	if (cat==0){
		if (id==0) ntaps = (int)value;
		else if (id==1){
			monomerge = (value!=0.0);
			setbuffer(nsamples);
		};
		return;
	};
	if ((cat<1) || (cat>16)) return;
	cat--;
	switch (id){
	case 0: taps[cat].flags = (int)value; break;
	case 1: taps[cat].delay = (float)value; break;
	case 2: taps[cat].volume = (float)value; break;
	case 3: taps[cat].feedback = (float)value; break;
	case 4: taps[cat].pan[0].pan = (float)value; break;
	case 5: taps[cat].pan[0].amp = (float)value; break;
	case 6: taps[cat].pan[0].period = (float)value; break;
	case 7: taps[cat].pan[0].phase = (float)value; break;
	case 8: taps[cat].pan[1].pan = (float)value; break;
	case 9: taps[cat].pan[1].amp = (float)value; break;
	case 10: taps[cat].pan[1].period = (float)value; break;
	case 11: taps[cat].pan[1].phase = (float)value; break;
	case 12: taps[cat].pan[2].pan = (float)value; break;
	case 13: taps[cat].pan[2].amp = (float)value; break;
	case 14: taps[cat].pan[2].period = (float)value; break;
	case 15: taps[cat].pan[2].phase = (float)value; break;
	default: break;
	};
	cat++;
	if (ei) ei->setparam(cat,id,value,steps);
}

void MTDelay::settap(int tap,int flags,float delay,float volume,float feedback,Effect *effect)
{
	if ((tap<0) || (tap>=DELAY_MAX_TAPS)) return;
	taps[tap].flags = flags;
	taps[tap].delay = delay;
	taps[tap].volume = volume;
	taps[tap].feedback = feedback;
	if (taps[tap].effect!=effect){
		if (taps[tap].effect) oi->deleteobject(taps[tap].effect);
		taps[tap].effect = effect;
	};
	if (tap>=ntaps) ntaps = tap+1;
}

void MTDelay::setbuffer(int nsamples)
{
	int x;

	this->nsamples = nsamples;
	cpos = 0;
	if (monomerge){
		buffer[0] = (sample*)si->memrealloc(buffer[0],nsamples*sizeof(sample));
		x = 1;
	}
	else{
		for (x=0;x<ninputs;x++){
			buffer[x] = (sample*)si->memrealloc(buffer[x],nsamples*sizeof(sample));
		};
	};
	for (;x<8;x++){
		if (buffer[x]){
			si->memfree(buffer[x]);
			buffer[x] = 0;
		};
	};
}
//---------------------------------------------------------------------------
MTDelayInstance::MTDelayInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller):
EffectInstance(p,no,o,ni,i,caller)
{
	int x;
	MTDelay &cp = *(MTDelay*)p;

	for (x=0;x<DELAY_MAX_TAPS;x++){
		taps[x].cphase[0] = cp.taps[x].pan[0].phase;
		taps[x].cphase[1] = cp.taps[x].pan[1].phase;
		taps[x].cphase[2] = cp.taps[x].pan[2].phase;
	};
}

int MTDelayInstance::process(int ooffset,int ioffset,int count,bool &silence)
{
	int x,y,t,d;
	MTDelay &cp = *(MTDelay*)parent;

	if ((noutputs==0) || (ninputs!=noutputs)) return 0;
// Delay buffer
	if (cp.monomerge){
		double factor = 1.0/noutputs;
		if (cp.cpos+count<=cp.nsamples){
			dspi->replacebuffermul(cp.buffer[0]+cp.cpos,inputs[0]+ioffset,factor,count);
			for (x=1;x<noutputs;x++){
				dspi->addbuffermul(cp.buffer[0]+cp.cpos,inputs[x]+ioffset,factor,count);
			};
		}
		else{
			t = cp.nsamples-cp.cpos;
			d = count-t;
			dspi->replacebuffermul(cp.buffer[0]+cp.cpos,inputs[0]+ioffset,factor,t);
			dspi->replacebuffermul(cp.buffer[0],inputs[0]+ioffset+t,factor,d);
			for (x=1;x<noutputs;x++){
				dspi->addbuffermul(cp.buffer[0]+cp.cpos,inputs[x]+ioffset,factor,t);
				dspi->addbuffermul(cp.buffer[0],inputs[x]+ioffset+t,factor,d);
			};
		};
	}
	else{
		if (cp.cpos+count<=cp.nsamples){
			for (x=0;x<noutputs;x++){
				dspi->replacebuffer(cp.buffer[x]+cp.cpos,inputs[x]+ioffset,count);
			};
		}
		else{
			t = cp.nsamples-cp.cpos;
			d = count-t;
			for (x=0;x<noutputs;x++){
				dspi->replacebuffer(cp.buffer[x]+cp.cpos,inputs[x]+ioffset,t);
				dspi->replacebuffer(cp.buffer[x],inputs[x]+ioffset+t,d);
			};
		};
	};

// Process the dry part
	if (parent->dry==1.0){
		for (x=0;x<noutputs;x++){
			dspi->addbuffer(outputs[x]+ooffset,inputs[x]+ioffset,count);
		};
	}
	else if (parent->dry>0.0){
		for (x=0;x<noutputs;x++){
			dspi->addbuffermul(outputs[x]+ooffset,inputs[x]+ioffset,parent->dry,count);
		};
	};

// Processing wet + feedback
	for (t=0;t<cp.ntaps;t++)
	{
		Tap ct = cp.taps[t];
		switch (ct.flags & DF_DELAYF){
		case DF_SAMPLES:
			d = (int)ct.delay;
			break;
		case DF_MS:
			d = (int)((ct.delay*cp._frequency)/1000.0);
			break;
		case DF_BEATS:
			d = (int)(((double)cp._frequency*60.0*ct.delay)/cp._bpm);
			break;
		default:
			continue;
		};
		if (d<=0) continue;
		register float cf = ct.feedback;
		if (cp.monomerge) cf /= noutputs;
		y = 0;
		for (x=0;x<noutputs;x++){
			register sample *i,*f,*o;
			register int l = count;
			register int c;
			sample *is,*ie;
			i = cp.buffer[y]+(cp.cpos-d);
			f = cp.buffer[y]+cp.cpos;
			is = cp.buffer[y];
			ie = cp.buffer[y]+cp.nsamples;
			if (i<is) i = ie-(is-i);
			o = outputs[x]+ooffset;
			while (l>0){
				c = l;
				if (c>ie-i) c = ie-i;
				if (c>ie-f) c = ie-f;
				dspi->addbuffermul2(o,f,i,ct.volume,cf,c);
//				dspi->addbufferslide2(o,f,i,ct.volume,ct.feedback,-ct.volume/c,-ct.feedback/c,c);
				o += c;
				i += c;
				while (i>=ie) i -= cp.nsamples;
				while (f>=ie) f -= cp.nsamples;
				l -= c;
			};
			if (!cp.monomerge) y++;
		};
	};

	cp.cpos += count;
	while (cp.cpos>=cp.nsamples) cp.cpos -= cp.nsamples;

	return 0;
}

void MTDelayInstance::setparam(int cat,int id,double value,int length)
{

}
//---------------------------------------------------------------------------
