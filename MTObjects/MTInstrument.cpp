//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTInstrument.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <math.h>
#include "MTInstrument.h"
#include "MTObjects1.h"
#include "MTObjectsASM.h"
#include <MTXAPI/RES/MTObjectsRES.h>
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
InstrumentType *instrumenttype;
//---------------------------------------------------------------------------
InstrumentType::InstrumentType()
{
	type = MTO_MTINSTRUMENT;
	description = "Instrument";
}

MTObject* InstrumentType::create(MTObject *parent,mt_int32 id,void *param)
{
	return new MTInstrument(parent,id);
}
//---------------------------------------------------------------------------
// MTInstrument functions
//---------------------------------------------------------------------------
MTInstrument::MTInstrument(MTObject *parent,mt_int32 i):
Instrument(parent,MTO_MTINSTRUMENT,i),
gvol(1.0),
gpanx(0.0),
gpany(0.0),
gpanz(0.0),
vibtype(0),
vibsweep(0),
vibdepth(0),
vibrate(0),
fadeout(1.0),
nna(0),
filter(0),
cutoff(4000),
resonance(64),
attack(0.0f),
decay(0.0f)
{
	int x;

	mtmemzero(&range,sizeof(range));
	for (x=0;x<96;x++) range[1][x] = 128;
	mtmemzero(&env,sizeof(env));
	mtmemzero(&grp,sizeof(grp));
#	ifdef MTSYSTEM_RESOURCES
		res->loadstringf(MTT_instrument,name,255,id);
#	endif
}

MTInstrument::~MTInstrument()
{
	setfilter(false);
}

void MTInstrument::enumchildren(MTObjectEnum enumproc,void *data)
{
	int x;
	
	for (x=0;x<MAX_GRPS;x++){
		if (grp[x].spl){
			if (!enumproc(grp[x].spl,data)) return;
		};
	};
}

InstrumentInstance* MTInstrument::createinstance(Track *track,PatternInstance *caller,InstrumentInstance *previous)
{
	return new MTInstrumentInstance(this,track,caller,caller->layer,previous);
}

bool MTInstrument::acceptoscillator()
{
	int x;
	
	for (x=0;x<MAX_GRPS;x++){
		if (!grp[x].spl) return true;
	};
	return false;
}

int MTInstrument::addoscillator(Oscillator *o)
{
	int x;
	
	for (x=0;x<MAX_GRPS;x++){
		if (!grp[x].spl){
			grp[x].spl = o;
			grp[x].vol = 1.0;
			grp[x].pitch = 0;
			return x;
		};
	};
	return -1;
}

int MTInstrument::deloscillator(Oscillator *o)
{
	int x;
	
	for (x=0;x<MAX_GRPS;x++){
		if (grp[x].spl==o){
			grp[x].spl = 0;
			return x;
		};
	};
	return -1;
}

void MTInstrument::setfilter(bool active)
{
	if (active){
		if (filter) return;
		lock(MTOL_LOCK,true);
		filter = new MTFilter(parent,0);
		filter->setparam(0,0,(float)cutoff/44100.0);
		filter->setparam(0,1,(float)resonance/128.0);
		lock(MTOL_LOCK,false);
	}
	else{
		if (!filter) return;
		lock(MTOL_LOCK,true);
		delete filter;
		filter = 0;
		lock(MTOL_LOCK,false);
	};
}
//---------------------------------------------------------------------------
#ifndef MTFILTERASM_INCLUDED
const double f2pi = 6.283185307179586476925286766559;
#endif
//---------------------------------------------------------------------------
MTInstrumentInstance::MTInstrumentInstance(Instrument *p,Track *t,PatternInstance *c,int l,InstrumentInstance *previous):
InstrumentInstance(p,t,c,l,previous),
filter(0)
{
	int x;

	mtmemzero(osc,sizeof(osc));
	mtmemzero(envs,sizeof(envs));
	for (x=0;x<8;x++) envs[x].lenvp = 65535;
	MTInstrument &ins = *((MTInstrument*)parent);
	if (previous){
		MTInstrumentInstance &cp = *(MTInstrumentInstance*)previous;
		iflags = cp.flags;
		lastpos = cp.lastpos;
		fadepos = cp.fadepos;
		fadeout = cp.fadeout;
		nna = cp.nna;
		ccutoff = cp.ccutoff;
		gvol = cp.gvol;
		gpanx = cp.gpanx;
		gpany = cp.gpany;
		gpanz = cp.gpanz;
		tvol = cp.tvol;
		tpanx = cp.tpanx;
		tpany = cp.tpany;
		tpanz = cp.tpanz;
		cvol = cp.cvol;
		cpanx = cp.cpanx;
		cpany = cp.cpany;
		cpanz = cp.cpanz;
		mvol = cp.mvol;
		mpanx = cp.mpanx;
		mpany = cp.mpany;
		mpanz = cp.mpanz;
	}
	else{
		iflags = ins.flags;
		lastpos = fadepos = 0.0;
		fadeout = ins.fadeout;
		nna = ins.nna;
		gvol = ins.gvol;
		tvol = cvol = 1.0;
		ccutoff = 1.0;
		gpanx = ins.gpanx;
		gpany = ins.gpany;
		gpanz = ins.gpanz;
		tpanx = tpany = tpanz = cpanx = cpany = cpanz = 0.0;
	};
	if ((iflags & MTIF_FAE)==0) flags |= MTIIF_FADEOUT;
	if (ins.filter){
		buffer[0] = (sample*)si->getprivatedata(0);
		for (x=1;x<8;x++){
			buffer[x] = buffer[x-1]+(PRIVATE_BUFFER/8);
		};
		filter = (MTFilterInstance*)ins.filter->createinstance(t->noutputs,t->buffer,t->noutputs,buffer,this);
// Use built-in filter attack?
		if ((((ins.env[3].flags & EF_ENABLED)==0) || (ins.env[3].npoints==0)) && (ins.attack>0)){
			filter->setparam(0,0,0.0f);
			filter->setparam(0,0,ins.cutoff,module->beatstosamples((float)ins.attack/ins.tpb));
		};
	};
}

MTInstrumentInstance::~MTInstrumentInstance()
{
	if (osc[0]) delete osc[0];
	if (filter) delete filter;
}

void MTInstrumentInstance::processevents()
{
	int x,l,lp,e;
	double inc;
	MTInstrument &cins = *(MTInstrument*)parent;

	if (!osc[0]){
		nextevent = -1.0;
		return;
	};
	inc = cpos-lastpos;
	if (inc>0.0){
		for (x=0;x<8;x++){
			IEnvelope &cenv = ((MTInstrument*)parent)->env[x];
			EnvStatus &cs = envs[x];
			if ((cenv.flags & EF_ENABLED) && (cenv.npoints)){
				// If the envelope was active then increment the position
				if (!cs.process) cs.pos += inc;
				// If the envelope was not idle then decrement the counter until next point
				if (!cs.wait) cs.count -= inc;
			};
			// Mark the envelope as not active (it'll be active later if there is something to do with it)
			cs.process = false;
		};
	};
	lastpos = cpos;
	inc = 1000.0;
	if ((flags & MTIIF_NOTECUT)==0){
		for (x=0;x<8;x++){
			IEnvelope &cenv = cins.env[x];
			if ((cenv.flags & EF_ENABLED) && (cenv.npoints)){
				EnvStatus &cs = envs[x];
/*				if (x==0){
					FLOG4("%f\t%f\t%d\t%d\t%f",cs.pos,cs.count,cs.envp,cs.lenvp);
					FLOG1("%f"NL,cvol);
				};*/
				l = 0;
				if (cs.count<=0.0){ // We reached a new point
					lp = cs.lenvp;
					cs.lenvp = cs.envp;
					// Calculate the last point we can reach
					e = cenv.npoints-1; // ...either the last point
					if (cenv.flags & EF_LOOP) e = cenv.loope; // ...or the loop end
					if ((cenv.flags & EF_SUSTAIN) && ((flags & MTIIF_NOTEOFF)==0) && (cenv.suste<e)) e = cenv.suste; // ...or the sustain loop end
					if (e==0){ // First and only point
						switch (x){
						case 0:
							if ((e==cenv.npoints-1) && (iflags & MTIF_FAE) && ((flags & MTIIF_FADEOUT)==0)){
								if (((cenv.flags & EF_SUSTAIN)==0) || ((flags & MTIIF_NOTEOFF)!=0)){
									flags |= MTIIF_FADEOUT;
									fadepos = cpos;
								};
							};
							cvol = cenv.points[cs.envp].y;
							break;
						case 1:
							cpanx = cenv.points[cs.envp].y;
							break;
						case 3:
							ccutoff = cenv.points[cs.envp].y;
							break;
						};
						cs.wait = true;
						cs.count = 1000.0;
						l = -1;
					}
					else if (cs.envp<e){ // Not at the end
//						cs.process = true;
						if (lp==65535){ // First point
							switch (x){
							case 0:
								cvol = cenv.points[0].y;
								tvol = cvol*mvol;
								osc[0]->setvolume(gvol*tvol);
								break;
							case 1:
								cpanx = cenv.points[0].y;
								tpanx = pan_mul(mpanx,cpanx);
								osc[0]->setpanning(pan_mul(gpanx,tpanx));
								break;
							case 3:
								ccutoff = cenv.points[0].y;
								if (filter) filter->setparam(0,0,ccutoff*cins.cutoff);
								break;
							};
						};
						cs.count = cenv.points[cs.envp+1].x-cs.pos;
						if (cs.count<inc){
							inc = cs.count;
						};
						l = (int)module->beatstosamples(cs.count);
						switch (x){
						case 0:
							cvol = cenv.points[cs.envp+1].y;
							break;
						case 1:
							cpanx = cenv.points[cs.envp+1].y;
							break;
						case 3:
							ccutoff = cenv.points[cs.envp+1].y;
							break;
						};
						cs.envp++;
					}
					else{ // At the end -> check for loop / sustain
//						cs.process = true;
						if (((cenv.flags & EF_LOOP)==0) && (((cenv.flags & EF_SUSTAIN)==0) || (flags & MTIIF_NOTEOFF))){
							if (x==0){
								if (osc[0]->volume<VOLUME_THRESOLD) inc = -1.0;
								else{
									if ((iflags & MTIF_FAE) && ((flags & MTIIF_FADEOUT)==0)){
										if (ai->recording) ai->debugpoint(module->beatstosamples(module->playstatus.pos),"FO");
										flags |= MTIIF_FADEOUT;
										fadepos = cpos;
									};
								};
							};
							cs.wait = true;
							cs.count = 1000.0;
							l = -1;
						}
						else{
							lp = cs.envp;
							if ((cenv.flags & EF_SUSTAIN) && ((flags & MTIIF_NOTEOFF)==0) && (cenv.suste<cenv.loope)){
								cs.envp = cenv.susts;
								cs.pos = cs.pos-cenv.points[cenv.suste].x+cenv.points[cenv.susts].x;
							}
							else if (cenv.flags & EF_LOOP){
								cs.envp = cenv.loops;
								cs.pos = cs.pos-cenv.points[cenv.loope].x+cenv.points[cenv.loops].x;
							};
							if (lp!=cs.envp){
								l = FAST_RAMP;
								cs.count = module->samplestobeats(l);
								if (cs.count<inc) inc = cs.count;
								switch (x){
								case 0:
									cvol = cenv.points[cs.envp].y;
									break;
								case 1:
									cpanx = cenv.points[cs.envp].y;
									break;
								case 3:
									ccutoff = cenv.points[cs.envp].y;
									break;
								};
								cs.wait = false;
							}
							else{
								cs.wait = true;
								cs.count = 1000.0;
								l = -1;
							};
						};
					};
				}
				else{
					l = (int)module->beatstosamples(cs.count);
					if (cs.count<inc) inc = cs.count;
				};
				if (l>=0){
					switch (x){
					case 0:
						tvol = cvol*mvol;
						osc[0]->setvolume(gvol*tvol,l,0);
						break;
					case 1:
						tpanx = pan_mul(mpanx,cpanx);
						osc[0]->setpanning(pan_mul(gpanx,tpanx),pan_mul(gpany,tpany),pan_mul(gpanz,tpanz),l,0);
						break;
					case 3:
						if (filter) filter->setparam(0,0,(int)(ccutoff*cins.cutoff),l);
						break;
					};
				};
			};
			if ((x==0) && (flags & MTIIF_FADEOUT)){
				if (inc<=0.0) continue;
/*
				if (cfadeout==0.0){
					inc = -1.0;
					continue;
				};
				cvol = getvolume(0.0,false)*(1.0+fadeout*(cpos-fadepos+inc));
				cfadeout = cvol;
				if (cvol<0.0){
					cvol = 0.0;
					inc = -(cpos-fadepos+1.0/fadeout);
				};
				l = (int)module->beatstosamples(inc);
				tvol = cvol*mvol;
				osc[0]->setvolume(gvol*tvol,l,0);
*/
				if (fadepos==0.0){
					fadepos = cpos;
				};
				cvol = getvolume(inc);
				if (cvol<0.0){
					cvol = 0.0;
					inc = -(cpos-fadepos+1.0/fadeout);
				};
				l = (int)module->beatstosamples(inc);
				tvol = cvol*mvol;
				osc[0]->setvolume(gvol*tvol,l,0);
			};
		};
	};
// Filter
	if ((filter) && (((cins.env[3].flags & EF_ENABLED)==0) || (cins.env[3].npoints==0))){
		double bpt = 1/(double)cins.tpb;
		if ((cins.attack==0) && (cpos>=(double)cins.attack*bpt)){
			if ((inc==0) || (inc>bpt)) inc = bpt;
//			if ((inc==0) || (inc>bpt*0.5)) inc = bpt*0.5;
			l = (int)module->beatstosamples(inc);
			double c = sqrt(1.0f/((1.0f+(float)cins.decay*0.5f)*(1+(cpos+inc)*cins.tpb-cins.attack)));
			c = acos(1-c)/f2pi;
			filter->setparam(0,0,(float)cins.cutoff*c,l);
		};
	};
	if (inc>0.0){
		nextevent += inc;
		for (x=0;x<8;x++){
			IEnvelope &cenv = ((MTInstrument*)parent)->env[x];
			EnvStatus &cs = envs[x];
			cs.process = cs.wait;
		};
/*
		for (x=0;x<8;x++){
			IEnvelope &cenv = ((MTInstrument*)parent)->env[x];
			EnvStatus &cs = envs[x];
			if ((cenv.npoints) && (!cs.wait) && (cs.process)){
				cs.pos += inc;
			};
		};
*/
	}
	else{
		flags |= IIF_SLEEPING;
//		nextevent = -1.0;
	};
}

bool MTInstrumentInstance::seek(double offset,int origin,int units)
{
	if (!osc[0]) return false;
	return osc[0]->seek(offset,origin,units);
}

void MTInstrumentInstance::process(int count)
{
	int x,o,l;
	bool silence;
	MTInstrument &ins = *(MTInstrument*)parent;

	o = 0;
	if (filter) l = PRIVATE_BUFFER/8;
	else l = count;
	flags &= (~IIF_SLEEPING);
	while (count>0){
		if (!l) break;
		if (count<l) l = count;
		silence = false;
		if ((osc[0]) && (osc[0]->parent->lockread==0) && (osc[0]->parent->access.caccess & MTOA_CANPLAY)){
			if (!osc[0]->process(0,l,silence)){
				flags |= IIF_SLEEPING;
//				nextevent = -1.0;
			}
			else if (silence){
				flags |= IIF_SLEEPING;
				if (flags & IIF_BACKGROUND){
					if (flags & MTIIF_NOTECUT){
						nextevent = -1.0;
					};
				};
			};
		}
		else{
			nextevent = -1.0;
		};
		if (filter){
			silence = false;
			filter->process(o,0,l,silence);
			for (x=0;x<track->noutputs;x++){
				dspi->emptybuffer(buffer[x],l);
			};
		};
		o += l;
		count -= l;
	};
}

void MTInstrumentInstance::sendevents(int nevents,MTIEvent **events)
{
	int x,y;
	double next,tmp_vol,b,b1,b2;
	float tmp_panx,tmp_pany,tmp_panz;
	MTInstrument &cparent = *(MTInstrument*)parent;
	bool first = true;
	
	for (x=0;x<nevents;x++){
		MTIEvent *cevent = events[x];
		MTINoteEvent *nevent = (MTINoteEvent*)cevent;
		MTIParamEvent *pevent = (MTIParamEvent*)cevent;
		if (cevent){
			switch (cevent->type){
			case MTIE_NOTE:
				nextevent = cpos;
				if (nevent->note==0){
					flags |= MTIIF_NOTEOFF;
					if ((cparent.env[0].flags & EF_ENABLED)==0){
						flags |= MTIIF_NOTECUT;
						tvol = mvol = 0.0;
						if (osc[0]) osc[0]->setvolume(0.0,FAST_RAMP,0);
					}
					else{
						for (y=0;y<8;y++){
							if (envs[y].wait){
								envs[y].wait = false;
								envs[y].count = 0.0;
							};
						};
					};
					continue;
				};
				flags &= (~(MTIIF_NOTEOFF|MTIIF_NOTECUT|MTIIF_FADEOUT));
				// Global parameters
				if (nevent->flags & MTIEF_ISINS){ // Is there an instrument?
					gvol = cparent.gvol*nevent->gvolume;
					gpanx = pan_mul(nevent->gpanx,cparent.gpanx);
					gpany = pan_mul(nevent->gpany,cparent.gpany);
					gpanz = pan_mul(nevent->gpanz,cparent.gpanz);
					mtmemzero(envs,sizeof(envs));
					for (x=0;x<8;x++) envs[x].lenvp = 65535;
					cvol = getvolume();
					getpanning(&cpanx,&cpany,&cpanz);
					lastpos = cpos;
					fadepos = 0.0;
					MTInstrument &ins = *((MTInstrument*)parent);
					fadeout = ins.fadeout;
					nna = ins.nna;
					iflags = ins.flags;
				};
				note = nevent->note;
				if (osc[0]) first = false;
				if (nevent->flags & MTIEF_ISNOTE){ // Is there a note?
					if (osc[0]){
						first = true;
						delete osc[0];
						osc[0] = 0;
					};
					if ((note<12) || (note>106)) continue;
					grp[0] = cparent.range[0][(int)note-12];
				}
				else{
					if ((note<12) || (note>106)) continue;
					grp[0] = cparent.range[0][(int)note-12];
				};
				if (!osc[0]){
					if (cparent.grp[grp[0]].spl){
						if (filter){
							osc[0] = cparent.grp[grp[0]].spl->createinstance(track->noutputs,buffer,this);
						}
						else{
							osc[0] = cparent.grp[grp[0]].spl->createinstance(track->noutputs,track->buffer,this);
						};
					};
				};
				// Set the oscillator parameters
				if (osc[0]){
					if (first){
						osc[0]->setnote(note+(double)cparent.grp[grp[0]].pitch/128);
					};
					y = (int)note-12;
					gvol *= cparent.grp[grp[0]].vol;
					if (nevent->flags & MTIEF_ISINS){
						if (nevent->volume>-1.0) mvol = nevent->volume;
						else mvol = (float)cparent.range[1][y]/128;
						if (nevent->panx>-2.0) mpanx = pan_mul(cparent.grp[grp[0]].panx,nevent->panx);
						else mpanx = pan_mul(cparent.grp[grp[0]].panx,(float)((signed char)cparent.range[2][y])/127);
						if (nevent->pany>-2.0) mpany = pan_mul(cparent.grp[grp[0]].pany,nevent->pany);
						else mpany = pan_mul(cparent.grp[grp[0]].pany,(float)((signed char)cparent.range[3][y])/127);
						if (nevent->panz>-2.0) mpanz = pan_mul(cparent.grp[grp[0]].panz,nevent->panz);
						else mpanz = pan_mul(cparent.grp[grp[0]].panz,(float)((signed char)cparent.range[4][y])/127);
					};
					tvol = cvol*mvol;
					tpanx = pan_mul(mpanx,cpanx);
					tpany = pan_mul(mpany,cpany);
					tpanz = pan_mul(mpanz,cpanz);
					osc[0]->setvolume(gvol*tvol);
					osc[0]->setpanning(pan_mul(gpanx,tpanx),pan_mul(gpany,tpany),pan_mul(gpanz,tpanz));
				};
				break;
			case MTIE_KILL:
				if (!osc[0]) return;
				flags |= IIF_BACKGROUND;
				switch (nna & 0xFF){
				case NNA_NOTECUT:
					flags |= MTIIF_NOTECUT;
					tvol = mvol = 0.0;
					osc[0]->setvolume(0.0,FAST_RAMP,0);
					break;
				case NNA_NOTEOFF:
					flags |= MTIIF_NOTEOFF;
					if ((cparent.env[0].flags & EF_ENABLED)==0){
						flags |= MTIIF_NOTECUT;
						tvol = mvol = 0.0;
						osc[0]->setvolume(0.0,FAST_RAMP,0);
					}
					else{
						for (y=0;y<8;y++){
							if (envs[y].wait){
								envs[y].wait = false;
								envs[y].count = 0.0;
							};
						};
						nextevent = cpos;
					};
					break;
				case NNA_FADEOUT:
					flags |= MTIIF_FADEOUT;
					fadepos = cpos;
					nextevent = cpos;
/*
					tvol = mvol = 0.0;
					osc[0]->setvolume(0.0,FAST_RAMP,0);
*/
					break;
				};
				break;
			case MTIE_RESET:
				break;
			case MTIE_PARAM:
				if ((!osc[0]) || (flags & MTIIF_NOTECUT)) return;
				y = 0;
				if ((pevent->param==MTIP_VOLUME) || (pevent->param==MTIP_PANNING)){
					b1 = module->samplestobeats(MIN_RAMP);
					b2 = module->samplestobeats(MAX_RAMP);
					b = nextevent-cpos;
					if (b<b1){
						y = 0;
						b = 0.0;
					}
					else if (b>b2){
						y = FAST_RAMP;
						b = module->samplestobeats(FAST_RAMP);
					}
					else{
						y = (int)module->beatstosamples(b);
					};
					next = cpos+b;
					if (next<nextevent) nextevent = next;
				};
				switch (pevent->param){
				case MTIP_VOLUME:
					tmp_vol = getvolume(b);
					if (pevent->flags & MTIEF_MUL) mvol *= pevent->dvalue1;
					else if (pevent->flags & MTIEF_ADD) mvol += pevent->dvalue1;
					else mvol = pevent->dvalue1;
					if (mvol<0.0) mvol = 0.0;
					else if ((mvol>1.0) && ((pevent->flags & MTIEF_DONTSATURATE)==0)) mvol = 1.0;
					tvol = tmp_vol*mvol;
					osc[0]->setvolume(gvol*tvol,y);
					break;
				case MTIP_PANNING:
					getpanning(&tmp_panx,&tmp_pany,&tmp_panz,b);
					if (pevent->flags & MTIEF_MUL){
						pan_self_mul(mpanx,pevent->fvalue1);
						pan_self_mul(mpany,pevent->fvalue2);
						pan_self_mul(mpanz,pevent->fvalue3);
					}
					else if (pevent->flags & MTIEF_ADD){
						mpanx += pevent->fvalue1;
						mpany += pevent->fvalue2;
						mpanz += pevent->fvalue3;
						if (mpanx<-1.0) mpanx = -1.0;
						else if (mpanx>1.0) mpanx = 1.0;
						if (mpany<-1.0) mpany = -1.0;
						else if (mpany>1.0) mpany = 1.0;
						if (mpanz<-1.0) mpanz = -1.0;
						else if (mpanz>1.0) mpanz = 1.0;
					}
					else{
						mpanx = pevent->fvalue1;
						mpany = pevent->fvalue2;
						mpanz = pevent->fvalue3;
					};
					pan_self_mul(mpanx,cparent.grp[grp[0]].panx);
					pan_self_mul(mpany,cparent.grp[grp[0]].pany);
					pan_self_mul(mpanz,cparent.grp[grp[0]].panz);
					tpanx = pan_mul(mpanx,tmp_panx);
					tpany = pan_mul(mpany,tmp_pany);
					tpanz = pan_mul(mpanz,tmp_panz);
					osc[0]->setpanning(pan_mul(gpanx,tpanx),pan_mul(gpany,tpany),pan_mul(gpanz,tpanz),y);
					break;
				case MTIP_NOTE:
					if (pevent->flags & MTIEF_MUL) note *= pevent->dvalue1;
					else if (pevent->flags & MTIEF_ADD) note += pevent->dvalue1;
					else note = pevent->dvalue1;
					osc[0]->setnote(note);
					break;
				};
				break;
			};
		};
	};
}

float MTInstrumentInstance::getimportance()
{
	float i = 0.75f+(float)((MTInstrument*)parent)->gvol/4.0f;

	if (osc[0]) i *= osc[0]->getimportance();
	else return 0;
	if (flags & IIF_BACKGROUND) i /= 4;
	if (flags & IIF_SLEEPING) i /= 2;
	return i;
}

double MTInstrumentInstance::getvolume(double delay,bool needfadeout)
{
	double p,l,fo;
	EnvStatus &cs = envs[0];
	IEnvelope &cenv = ((MTInstrument*)parent)->env[0];

	fo = 1.0;
	if ((needfadeout) && (flags & MTIIF_FADEOUT)){
		fo = 1.0+fadeout*(cpos-fadepos+delay);
//		if (fo<0.0) fo = 0.0;
	};
	if (((cenv.flags & EF_ENABLED)==0) || (cenv.npoints==0))
		return fo;
	if ((cs.lenvp==cs.envp) || (cs.lenvp==65535))
		return cenv.points[cs.envp].y*fo;
	if (cs.lenvp<cs.envp){
		l = cenv.points[cs.envp].x-cenv.points[cs.lenvp].x;
		if (l<=0.0) return cenv.points[cs.envp].y*fo;
	}
	else{
		l = module->samplestobeats(FAST_RAMP);
	};
	p = cs.count+lastpos-cpos-delay;
	if (p>l) p = l;
	else if (p<0.0) p = 0.0;
	return ((cenv.points[cs.envp].y*(l-p)+cenv.points[cs.lenvp].y*p)/l)*fo;
}

void MTInstrumentInstance::getpanning(float *x,float *y,float *z,double delay)
{
	double p,l;
	EnvStatus &cs = envs[1];
	IEnvelope &cenv = ((MTInstrument*)parent)->env[1];

	if (y) *y = 0.0;
	if (z) *z = 0.0;
	if (((cenv.flags & EF_ENABLED)==0) || (cenv.npoints==0)){
		if (x) *x = 0.0;
		return;
	};
	if ((cs.lenvp==cs.envp) || (cs.lenvp==65535)){
		if (x) *x = cenv.points[cs.envp].y;
		return;
	};
	if (cs.lenvp<cs.envp){
		l = cenv.points[cs.envp].x-cenv.points[cs.lenvp].x;
		if (l<=0.0){
			if (x) *x = cenv.points[cs.envp].y;
			return;
		};
	}
	else{
		l = module->samplestobeats(FAST_RAMP);
	};
	p = cs.count+lastpos-cpos-delay;
	if (p>l) p = l;
	else if (p<0.0) p = 0.0;
	if (x) *x = (float)((double)(cenv.points[cs.envp].y*(l-p)+cenv.points[cs.lenvp].y*p)/l);
}
//---------------------------------------------------------------------------
