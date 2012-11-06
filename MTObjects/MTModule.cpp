//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTModule.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MTModule.h"
#include "MTObjects1.h"
#include "MTObjects2.h"
#include "MTObjectsASM.h"
#include "../../debug/Interface/MTObjectsRES.h"
#include "MTXSystem2.h"
#include "MTGraphs.h"
//---------------------------------------------------------------------------
// Routing instruction structure
//---------------------------------------------------------------------------
enum{
	RI_NOOP = 0,
	RI_CLEARBUFFER,
	RI_PREPROCESS,
	RI_PROCESS,
	RI_POSTPROCESS,
	RI_PROCESSNODE,
	RI_AMPLIFYBUFFER,
	RI_MIXBUFFER
};

struct RI{
	unsigned char i;
	unsigned char cpu;
	unsigned short level;
	union{
		Node *node;
		sample *buffer;
		Track *track;
	};
	union{
		sample *dest;
		double factor;
	};
	MTLock *lock;
	float cpuh[4];
};

struct ThreadStatus{
	unsigned short level;
	unsigned char cpu;
	unsigned char res;
	MTEvent *ready;
	MTEvent *go;
};
//---------------------------------------------------------------------------
// MTModule functions
//---------------------------------------------------------------------------
MTModule::MTModule(mt_int32 i):
MTObject(0,MTO_MODULE,i),
ntracks(8),
nmtracks(1),
loops(0.0),
loope(0.0),
message(0),
showmessage(false),
summarymask(0),
crashcount(3),
needupdate(true),
mts(0)
{
	int x;
	
	module = this;
	mlock = si->lockcreate();
	flags = -1;
#	ifdef MTSYSTEM_RESOURCES
		res->loadstring(MTT_module,name,255);
#	endif
	filename = (char*)si->memalloc(512,MTM_ZERO);
	mtmemzero(&nsequ,sizeof(nsequ));
	mtmemzero(&sequ,sizeof(sequ));
	patt = si->arraycreate(4,0);
	apatt = si->arraycreate(4,0);
	instr = si->arraycreate(8,0);
	spl = si->arraycreate(8,0);
	master = si->arraycreate(1,0);
	trk = si->arraycreate(4,0);
	fx = si->arraycreate(4,0);
	tempo = si->arraycreate(4,sizeof(Tempo));
	patt->additems(0,MAX_PATT);
	apatt->additems(0,MAX_PATT);
	instr->additems(0,MAX_INSTRS);
	spl->additems(0,MAX_SAMPLES);
	master->additems(0,MAX_MTRACKS);
	trk->additems(0,MAX_TRACKS);
	fx->additems(0,MAX_TRACKS);
	tempo->additems(0,1);
	D(tempo,Tempo)[0].bpm = 125.0;
	mtmemzero(&summary,sizeof(summary));
	mtmemzero(&playstatus,sizeof(playstatus));
	cpu = si->cpumonitorcreate(8);
	buffers = si->arraycreate(32,0);
	ris = si->arraycreate(32,sizeof(RI));
	setstatus();
	for (x=0;x<nmtracks;x++) master->a[x] = new Track(this,x+MAX_TRACKS);
	for (x=0;x<ntracks;x++) trk->a[x] = new Track(this,x);
#	ifdef MTVERSION_PROFESSIONAL
		mts = si->memalloc(sizeof(ThreadStatus)*nthreads,MTM_ZERO);
		for (x=1;x<nthreads;x++){
			ThreadStatus &cts = ((ThreadStatus*)mts)[x];
			cts.ready = si->eventcreate(true,0,0,true,false);
			cts.go = si->eventcreate(true,0,0,true,false);
		};
#	endif
}

void clearobject(void *o,void*)
{
	oi->deleteobject((MTObject*)o);
}

MTModule::~MTModule()
{
	int x;

	play(PLAY_STOP);
	if (output){
		output->lock->lock();
		lock(MTOL_LOCK,true);
		output->lock->unlock();
	}
	else{
		lock(MTOL_LOCK,true);
	};
	patt->clear(false,clearobject);
	apatt->clear(false,clearobject);
	instr->clear(false,clearobject);
	spl->clear(false,clearobject);
	master->clear(false,clearobject);
	trk->clear(false,clearobject);
	fx->clear(false,clearobject);
	for (x=0;x<playstatus.nchannels;x++){
		if (playstatus.chan[x]) delete playstatus.chan[x];
	};
	for (x=0;x<MAX_LAYERS;x++){
		if (playstatus.patti[x]) delete playstatus.patti[x];
	};
	if (playstatus.chan) si->memfree(playstatus.chan);
	if (message) si->memfree(message);
	for (x=0;x<6;x++){
		if (summary[x]!=0) si->memfree(summary[x]);
	};
	si->arraydelete(patt);
	si->arraydelete(apatt);
	si->arraydelete(instr);
	si->arraydelete(spl);
	si->arraydelete(master);
	si->arraydelete(trk);
	si->arraydelete(fx);
	si->arraydelete(tempo);
	buffers->clear(true);
	si->arraydelete(buffers);
	si->arraydelete(ris);
	si->lockdelete(mlock);
	si->memfree(filename);
	delete cpu;
#	ifdef MTVERSION_PROFESSIONAL
		if (mts){
			for (x=1;x<nthreads;x++){
				ThreadStatus &cts = ((ThreadStatus*)mts)[x];
				si->eventdelete(cts.ready);
				si->eventdelete(cts.go);
			};
			si->memfree(mts);
		};
#	endif
}

void MTModule::setmodified(int value,int flags)
{
	int x;
	bool wasmodified = (modified!=0);
	
	if (value) modified += value;
	else{
		for (x=0;x<patt->nitems;x++){
			if (patt->a[x]){
				A(patt,MTObject)[x]->setmodified(0,flags);
			};
		};
		for (x=0;x<apatt->nitems;x++){
			if (apatt->a[x])
				A(apatt,MTObject)[x]->setmodified(0,flags);
		};
		for (x=0;x<instr->nitems;x++){
			if (instr->a[x]){
				A(instr,MTObject)[x]->setmodified(0,flags);
			};
		};
		for (x=0;x<spl->nitems;x++){
			if (spl->a[x]){
				A(spl,MTObject)[x]->setmodified(0,flags);
			};
		};
		for (x=0;x<master->nitems;x++){
			if (master->a[x])
				A(master,MTObject)[x]->setmodified(0,flags);
		};
		for (x=0;x<trk->nitems;x++){
			if (trk->a[x]){
				A(trk,MTObject)[x]->setmodified(0,flags);
			};
		};
		for (x=0;x<fx->nitems;x++){
			if (fx->a[x]){
				A(fx,MTObject)[x]->setmodified(0,flags);
			};
		};
		modified = 0;
	};
	if (value) modifying++;
	mtinterface->notify(this,MTN_MODIFY,flags);
	if (value) modifying--;
}

void MTModule::notify(MTObject *source,int message,int param1,void *param2)
{
	int x;

	for (x=0;x<patt->nitems;x++){
		if (patt->a[x]){
			A(patt,MTObject)[x]->notify(source,message,param1,param2);
		};
	};
	for (x=0;x<apatt->nitems;x++){
		if (apatt->a[x])
			A(apatt,MTObject)[x]->notify(source,message,param1,param2);
	};
	for (x=0;x<instr->nitems;x++){
		if (instr->a[x]){
			A(instr,MTObject)[x]->notify(source,message,param1,param2);
		};
	};
	for (x=0;x<spl->nitems;x++){
		if (spl->a[x]){
			A(spl,MTObject)[x]->notify(source,message,param1,param2);
		};
	};
	for (x=0;x<master->nitems;x++){
		if (master->a[x])
			A(master,MTObject)[x]->notify(source,message,param1,param2);
	};
	for (x=0;x<trk->nitems;x++){
		if (trk->a[x]){
			A(trk,MTObject)[x]->notify(source,message,param1,param2);
		};
	};
	for (x=0;x<fx->nitems;x++){
		if (fx->a[x]){
			A(fx,MTObject)[x]->notify(source,message,param1,param2);
		};
	};
}

void MTModule::enumchildren(MTObjectEnum enumproc,void *data)
{
	int x;
	
	for (x=1;x<instr->nitems;x++){
		if (instr->a[x]){
			if (!enumproc(A(instr,MTObject)[x],data)) return;
		};
	};
	for (x=0;x<spl->nitems;x++){
		if (spl->a[x]){
			if (!enumproc(A(spl,MTObject)[x],data)) return;
		};
	};
	for (x=1;x<patt->nitems;x++){
		if (patt->a[x]){
			if (!enumproc(A(patt,MTObject)[x],data)) return;
		};
	};
	for (x=1;x<apatt->nitems;x++){
		if (apatt->a[x]){
			if (!enumproc(A(apatt,MTObject)[x],data)) return;
		};
	};
	for (x=0;x<master->nitems;x++){
		if (master->a[x]){
			if (!enumproc(A(master,MTObject)[x],data)) return;
		};
	};
	for (x=0;x<trk->nitems;x++){
		if (trk->a[x]){
			if (!enumproc(A(trk,MTObject)[x],data)) return;
		};
	};
	for (x=0;x<fx->nitems;x++){
		if (fx->a[x]){
			if (!enumproc(A(fx,MTObject)[x],data)) return;
		};
	};
}

void MTModule::getdisplayname(char *buffer,int cb)
{
	int x,y;
	bool twins;
	char *tmps,*tmps2;
	
	buffer[0] = '\0';
	if (strlen(name))
		strcpy(buffer,name);
	else{
		tmps = strrchr(filename,'/');
		if (!tmps) tmps = strrchr(filename,'\\');
		if (tmps) strcpy(buffer,tmps+1);
		else strcpy(buffer,filename);
	};
	y = 1;
	twins = false;
	x = mtinterface->getnummodules();
	while (x>0){
		MTModule *module = (MTModule*)mtinterface->getmodule(--x);
		if (module==this) continue;
		if (strlen(module->name))
			tmps2 = module->name;
		else{
			tmps = strrchr(module->filename,'/');
			if (!tmps) tmps = strrchr(module->filename,'\\');
			if (tmps) tmps2 = tmps;
			else tmps2 = module->filename;
		};
		if (strcmp(buffer,tmps2)==0){
			twins = true;
			if (x<id) y++;
		};
	};
#	ifdef MTSYSTEM_RESOURCES
		if (!strlen(buffer)) res->loadresource(MTR_TEXT,0,buffer,cb);
#	endif
	if (twins) sprintf(strrchr(buffer,'\0')," (%d)",y);
}

void MTModule::setstatus()
{
	resetchannels();
	resetpatterns();
	playstatus.pos = playstatus.nextevent = playstatus.length = 0.0;
	playstatus.bpm = D(tempo,Tempo)[0].bpm;
	playstatus.loopfrom = loops;
	playstatus.loopto = loope;
	lastbeat = -1;
}

void MTModule::updatelength()
{
	int x,l;
	double p;
	bool critical = (output->ndevices>0);

	MTTRY
		mlock->lock();
		playstatus.length = 0;
		for (x=0;x<MAX_LAYERS;x++){
			l = nsequ[x];
			if (l--){
				p = sequ[x][l].pos+sequ[x][l].length;
				if (p>playstatus.length) playstatus.length = p;
			};
		};
	MTCATCH
	MTEND
	mlock->unlock();
}

bool MTModule::settracks(int tracks,int mtracks)
{
	int x;
	
	lock(MTOL_LOCK,true);
	for (x=0;x<MAX_TRACKS;x++){
		if (x<tracks){
			if (!trk->a[x]){
				trk->a[x] = new Track(this,x);
			};
		}
		else if (trk->a[x]) delete A(trk,Track)[x];
	};
	for (x=0;x<MAX_MTRACKS;x++){
		if (x<mtracks){
			if (!master->a[x]){
				master->a[x] = new Track(this,x+MAX_TRACKS);
			};
		}
		else if (master->a[x]) delete A(master,Track)[x];
	};
	ntracks = tracks;
	nmtracks = mtracks;
	setmodified(1,MTO_TRACK);
	lock(MTOL_LOCK,false);
	return true;
}

void MTModule::enabletracks()
{
	int x;
	
	if (!output->ndevices) return;
	for (x=0;x<ntracks;x++)
		if (trk->a[x]) A(trk,Track)[x]->alloc();
}

void MTModule::disabletracks()
{
	int x;
	
	for (x=0;x<ntracks;x++)
		if (trk->a[x]) A(trk,Track)[x]->free();
}

int MTModule::getpattpos(int layer,double pos,double *offset,int *cseq,int from)
{
	register int x;
	register double cpos,clength;
	register unsigned short p,p2;
	
	if ((layer<0) || (layer>=MAX_LAYERS) || (nsequ[layer]==0)) return -1;
	if (from<0) from = 0;
	for (x=from;x<nsequ[layer];x++){
		cpos = sequ[layer][x].pos;
		clength = sequ[layer][x].length;
		if ((pos>=cpos) && (pos<cpos+clength)){
			pos -= cpos;
			p = sequ[layer][x].patt;
			p2 = p & 0xFFF;
			if (p & 0x1000){
				if ((p2>=apatt->nitems) || (!apatt->a[p2])) return -1;
			}
			else{
				if ((p>=patt->nitems) || (!patt->a[p])) return -1;
			};
			if (cseq) *cseq = x;
			if (offset) *offset = fmod(pos,clength);
			return p;
		};
	};
	return -1;
}

int MTModule::getsequence(int layer,double pos,int last)
{
	register int x;
	register double cpos,cpos2;
	
	if ((layer<0) || (layer>=MAX_LAYERS)) return -1;
	switch (last){
	case 0: // Exact sequence
		for (x=0;x<nsequ[layer];x++){
			cpos = sequ[layer][x].pos;
			cpos2 = cpos+sequ[layer][x].length;
			if (pos<cpos) return -1;
			if (pos<cpos2) return x;
		};
		return -1;
	case 1: // First sequence found after
		for (x=0;x<nsequ[layer]-1;x++){
			cpos = sequ[layer][x].pos;
			cpos2 = cpos+sequ[layer][x].length;
			if ((pos<=cpos) || (pos<cpos2)) return x;
		};
		return -1;
	case 2: // First sequence found after or just before
		if (!nsequ[layer]) return -1;
		if (pos>=(int)sequ[layer][nsequ[layer]-1].pos) return nsequ[layer]-1;
		for (x=0;x<nsequ[layer]-1;x++){
			cpos = sequ[layer][x].pos;
			cpos2 = sequ[layer][x+1].pos;
			if ((pos<=cpos) || (pos<cpos2)) return x;
		};
		return -1;
	};
	return -1;
}

void MTModule::play(int mode,bool fromengine)
{
	MTTRY
		mlock->lock();
		playstatus.flags = mode;
		if (mode==PLAY_STOP){
			if (!fromengine){
				resetchannels();
				resetpatterns();
			};
		}
		else{
			playstatus.nextevent = 0.0;
			notify(this,MTN_TEMPO,0,&playstatus.bpm);
		};
	MTCATCH
	MTEND
	mlock->unlock();
}

void MTModule::setpos(double pos,bool fromengine)
{
	int x;

	MTTRY
		mlock->lock();
		if (!fromengine){
			resetchannels();
			resetpatterns();
		};
		playstatus.pos = playstatus.nextevent = pos;
		lastbeat = -1;
		for (x=0;x<MAX_LAYERS;x++) playstatus.cseq[x] = -1;
	MTCATCH
	MTEND
	mlock->unlock();
}

void MTModule::settempo(int ctempo,int param,void *value,bool fromengine)
{
	bool change = false;

	MTTRY
		mlock->lock();
		if (D(tempo,Tempo)[ctempo].pos<=playstatus.pos){
			if (ctempo==tempo->nitems-1) change = true;
			else if (D(tempo,Tempo)[ctempo+1].pos>=playstatus.pos) change = true;
		}
		else if (D(tempo,Tempo)[ctempo-1].pos<=playstatus.pos){
			if (D(tempo,Tempo)[ctempo].flags & TF_SLIDE) change = true;
			switch (param){
			case 0:
				D(tempo,Tempo)[ctempo].pos = *(int*)value;
				break;
			case 1:
				D(tempo,Tempo)[ctempo].bpm = *(double*)value;
				if (change) playstatus.bpm = *(double*)value;
				break;
			case 2:
				if (*(bool*)value) D(tempo,Tempo)[ctempo].flags |= TF_SLIDE;
				else D(tempo,Tempo)[ctempo].flags &= (~TF_SLIDE);
			};
		};
	MTCATCH
	MTEND
	mlock->unlock();
}
//---------------------------------------------------------------------------
// Module Player functions
//---------------------------------------------------------------------------
bool MTModule::process(WaveOutput *output)
{
	register int x,y,n,i;
	register double inc,tmp;
	int remain = output->playlng;
	bool locked = false;
	bool silence = false;
	bool needposchange = false;
	int poschanges = 0;

	playstatus.coutput = output;
	if (!objectlock) return false;
	MTTRY
		objectlock->lock();
		mlock->lock();
		if ((!playstatus.flags) || (lockread) || ((access.caccess & MTOA_CANPLAY)==0)){
			mlock->unlock();
			objectlock->unlock();
			return false;
		};
		if (crashcount<=0){
			play(PLAY_STOP);
			crashcount = 3;
			mlock->unlock();
			objectlock->unlock();
			return false;
		};
		cpu->startslice(-1);
		cpu->startadd(0);	// Total
		locked = true;
		if (playstatus.bpm<MIN_BPM) playstatus.bpm = MIN_BPM;
		else if (playstatus.bpm>MAX_BPM) playstatus.bpm = MAX_BPM;
		playstatus.spb = ((double)output->frequency*60.0)/playstatus.bpm;
		for (x=0;x<nmtracks;x++) A(master,Track)[x]->offset = 0;
		for (x=0;x<output->ndevices;x++) output->device[0]->master->offset = 0;
		if (needupdate) updaterouting();
		mlock->unlock();
		locked = false;
		x = 1;
prebuffer:
		MTTRY
			while (x<instr->nitems){
				Instrument &ci = *A(instr,Instrument)[x];
				if ((&ci) && (ci.flags & IF_NEEDPREBUFFER)) ci.prebuffer(output->playlng);
				x++;
			};
		MTCATCH
			LOGD("%s - [Objects] ERROR: Exception while pre-buffering instrument!"NL);
			x++;
			goto prebuffer;
		MTEND
		while (remain>0){
			mlock->lock();
			locked = true;
			cpu->startadd(1);	// Events
poschange:
// Anything to do in the sequencer?
			if (playstatus.nextevent<=playstatus.pos){
				if ((playstatus.flags==PLAY_LOOP) && (playstatus.pos>=playstatus.loopto)){
					setpos(playstatus.loopfrom,true);
				};
				if (playstatus.pos>=loope){
					playstatus.flags = 0;
					resetchannels();
					resetpatterns();
				};
				playstatus.nextevent = playstatus.loopto;
				if (playstatus.loopto<=playstatus.loopfrom){
					playstatus.flags = 0;
					resetchannels();
					resetpatterns();
				};
				for (x=0;x<MAX_LAYERS;x++){
					y = -1;
					i = getpattpos(x,playstatus.pos,&inc,&y,playstatus.cseq[x]);
// Is there a pattern to play?
					if (i>=0){
// Yes, so instanciate it.
						playstatus.cseq[x] = y;
						if (i & 0x1000){
						}
						else{
// When will the next even occur?
							tmp = A(patt,Pattern)[i]->nbeats;
							n = (int)(inc/tmp)+1;
							tmp = sequ[x][y].pos+A(patt,Pattern)[i]->nbeats*n;
							if ((tmp>playstatus.pos) && (tmp<playstatus.nextevent)) playstatus.nextevent = tmp;
							PatternInstance *lpatti = playstatus.patti[x];
							PatternInstance &cpatti = *A(patt,Pattern)[i]->createinstance(x,&sequ[x][y],lpatti);
							if (lpatti){
								for (i=0;i<playstatus.nchannels;i++){
									InstrumentInstance &cchan = *playstatus.chan[i];
									if ((&cchan) && (cchan.layer==lpatti->layer) && (cchan.caller==lpatti)){
										cchan.changecaller(0);
									};
								};
								delete lpatti;
							};
							playstatus.patti[x] = &cpatti;
							if (&cpatti){
								cpatti.id = x;
								if (cpatti.parent->nbeats>0.0){
									inc = fmod(inc,cpatti.parent->nbeats);
									cpatti.seek(inc,true);
								};
							};
						};
					}
					else{
						i = getsequence(x,playstatus.pos,1);
						if (i>=0){
							tmp = sequ[x][i].pos;
							if ((tmp>playstatus.pos) && (tmp<playstatus.nextevent)) playstatus.nextevent = tmp;
						};
					};
				};
			};

// Process events
			inc = playstatus.nextevent-playstatus.pos;
			for (x=0;x<MAX_LAYERS;x++){
				PatternInstance &cpatti = *playstatus.patti[x];
				if ((&cpatti) && (cpatti.parent->lockread==0) && (cpatti.parent->access.caccess & MTOA_CANPLAY)){
					if (cpatti.nextevent<=cpatti.cpos) cpatti.processevents();
					if ((cpatti.nextevent>0) && (cpatti.nextevent-cpatti.cpos<inc)) inc = cpatti.nextevent-cpatti.cpos;
					else if (cpatti.nextevent==-2.0) needposchange = true;
				};
			};
			for (x=0;x<playstatus.nchannels;x++){
				InstrumentInstance &cchan = *playstatus.chan[x];
				if ((&cchan) && (cchan.parent->lockread==0) && (cchan.parent->access.caccess & MTOA_CANPLAY)){
					if (cchan.nextevent<=cchan.cpos) cchan.processevents();
					if ((cchan.nextevent>0) && (cchan.nextevent-cchan.cpos<inc)) inc = cchan.nextevent-cchan.cpos;
					else if (cchan.nextevent==-2.0) needposchange = true;
				};
			};
			if (needposchange){
				needposchange = false;
				if (++poschanges<1000) goto poschange;
			};
			cpu->endadd(1);	// Events
			i = (int)ceil(inc*playstatus.spb);
			if (i>remain) i = remain;
			for (x=0;x<ntracks;x++){
				if (i>A(trk,Track)[x]->nsamples) i = A(trk,Track)[x]->nsamples;
			};
			if (i<=0){
				i = 1;
			};
			inc = i/playstatus.spb;
			remain -= i;

// Empty the tracks
			cpu->startadd(2);	// Mix
			for (y=0;y<A(master,Track)[0]->noutputs;y++){
				dspi->emptybuffer(A(master,Track)[0]->buffer[y],i);
			};
			cpu->endadd(2);	// Mix

// Process output signal
			cpu->startadd(3);	// Output
			x = 1;
preprocess:
			MTTRY
				while (x<instr->nitems){
					Instrument &ci = *A(instr,Instrument)[x];
					if ((&ci) && (ci.flags & IF_NEEDPREPROCESS)) ci.preprocess(i);
					x++;
				};
			MTCATCH
				LOGD("%s - [Objects] ERROR: Exception while pre-processing instrument!"NL);
				x++;
				goto preprocess;
			MTEND

// Compiled engine instructions
			subprocess(output,0,inc,i,silence);

			x = 1;
postprocess:
			MTTRY
				while (x<instr->nitems){
					Instrument &ci = *A(instr,Instrument)[x];
					if ((&ci) && (ci.flags & IF_NEEDPOSTPROCESS)) ci.postprocess(i);
					x++;
				};
			MTCATCH
				LOGD("%s - [Objects] ERROR: Exception while post-processing instrument!"NL);
				x++;
				goto postprocess;
			MTEND
			cpu->endadd(3);	// Output
			cpu->startadd(2);	// Mix
// Out to the soundcard tracks
			for (x=0;x<nmtracks;x++){
				Track &ctrk = *A(master,Track)[x];
				for (y=0;y<ctrk.noutputs;y++){
					Track &dtrk = *output->device[x]->master;
					if (!&dtrk) continue;
					dspi->addbuffer(dtrk.buffer[y]+dtrk.offset,ctrk.buffer[y],i);
				};
			};
			cpu->endadd(2);	// Mix
			for (x=0;x<output->ndevices;x++) output->device[0]->master->offset += i;
// Advance position
			for (x=0;x<MAX_LAYERS;x++){
				PatternInstance &cpatti = *playstatus.patti[x];
				if (&cpatti) cpatti.cpos += inc;
			};
			for (x=playstatus.nchannels-1;x>=0;x--){
				InstrumentInstance &cchan = *playstatus.chan[x];
				if (&cchan){
					if (cchan.nextevent<=0) delchannel(&cchan);
					else cchan.cpos += inc;
				};
			};
			playstatus.pos += inc;
			mlock->unlock();
			locked = false;
		};
		cpu->endadd(0);	// Total
		cpu->endslice(-1);
		if (playstatus.pos*8>=lastbeat+1){
			lastbeat = (int)(playstatus.pos*8);
			cpu->flushcpu(-1);
#			ifdef _DEBUG
				static int count;
				if (++count>=128){
					int nbackground = 0,nasleep = 0;
					count = 0;
					for (x=0;x<playstatus.nchannels;x++){
						if (playstatus.chan[x]->flags & IIF_BACKGROUND) nbackground++;
						if (playstatus.chan[x]->flags & IIF_SLEEPING) nasleep++;
					};
					FLOGD4("%s - [Objects] Position: %9.4f - Channels: % 3d foreground % 3d background (% 3d asleep)"NL,playstatus.pos,playstatus.nchannels-nbackground,nbackground,nasleep);
					FLOG5("  CPU: Total: %7.4f%% Events: %7.4f%% Mix: %7.4f%% Channels: %7.4f%% Effects: %7.4f%%"NL,cpu->getcpu(0)*100,cpu->getcpu(1)*100,cpu->getcpu(2)*100,cpu->getcpu(3)*100,cpu->getcpu(4)*100);
				};
#			endif
		};
		x = 1;
postbuffer:
		MTTRY
			while (x<instr->nitems){
				Instrument &ci = *A(instr,Instrument)[x];
				if ((&ci) && (ci.flags & IF_NEEDPOSTBUFFER)) ci.postbuffer(output->playlng);
				x++;
			};
		MTCATCH
			LOGD("%s - [Objects] ERROR: Exception while post-buffering instrument!"NL);
			x++;
			goto postbuffer;
		MTEND
	MTCATCH
		LOGD("%s - [Objects] ERROR: Exception while processing module!"NL);
		crashcount--;
	MTEND
	objectlock->unlock();
	if (locked) mlock->unlock();
	return true;
}

bool MTModule::subprocess(WaveOutput *output,int cpuid,double inc,int lng,bool silence)
{
	int x;
	RI *ri;

#	ifdef MTVERSION_PROFESSIONAL
		ThreadStatus *cts = (ThreadStatus*)mts;
		MTEvent *events[32];

		if ((smpsupport) && (cpuid==0)){
			for (x=0;x<nthreads;x++){
				cts[x].level = 65534;
				events[x] = cts[x].ready;
				if (x>0) thread[x]->postmessage(4096,(int)this,lng);
			};
		};
#	endif
	ris->reset();
routing:
	MTTRY
		while ((ri = (RI*)ris->next())){
#			ifdef MTVERSION_PROFESSIONAL
				if ((ri->cpu!=255) && (ri->cpu!=cpuid)) continue;
				if ((smpsupport) && (ri->level<cts[cpuid].level)){
					if (cpuid==0){
						x = si->syswaitmultiple(nthreads-1,&events[1],true,1000);
						if (x==-1) return false;
						cts[0].level = ri->level;
						for (x=1;x<nthreads;x++) cts[x].go->set();
					}
					else{
						cts[cpuid].level = ri->level;
						if (!cts[cpuid].ready->set()) return false;
						if (!cts[cpuid].go->wait(1000)) return false;
					};
				};
				if ((smpsupport) && (ri->lock) && (!ri->lock->lock())) continue;
#			endif
			switch (ri->i){
			case RI_CLEARBUFFER:
				dspi->emptybuffer(ri->buffer,lng);
				break;
			case RI_PREPROCESS:
				break;
			case RI_PROCESS:
				for (x=0;x<playstatus.nchannels;x++){
					InstrumentInstance &cchan = *playstatus.chan[x];
					if ((&cchan) && (cchan.track==ri->track) && (cchan.cpu==cpuid) && (cchan.parent->lockread==0) && (cchan.parent->access.caccess & MTOA_CANPLAY)){
						MTTRY
							if (cchan.flags & IIF_SLEEPING){
								cchan.sleepingtime += inc;
								if (cchan.sleepingtime>=objectsprefs.maxsleeptime) cchan.nextevent = -1.0;
							};
							cchan.process(lng);
						MTCATCH
							LOGD("%s - [Objects] ERROR: Exception while processing channel!"NL);
							cchan.nextevent = -1.0;
						MTEND
					};
				};
				break;
			case RI_POSTPROCESS:
				break;
			case RI_PROCESSNODE:
				cpu->endadd(2);	// Mix
				cpu->startadd(4);	// Effects
				ri->node->process(0,0,lng,silence);
				cpu->endadd(4);	// Effects
				cpu->startadd(2);	// Mix
				break;
			case RI_AMPLIFYBUFFER:
				dspi->ampbuffer(ri->buffer,ri->factor,lng);
				break;
			case RI_MIXBUFFER:
				dspi->addbuffer(ri->dest,ri->buffer,lng);
				break;
			default:
				break;
			};
#			ifdef MTVERSION_PROFESSIONAL
				if ((smpsupport) && (ri->lock)) ri->lock->unlock();
#			endif
		};
#		ifdef MTVERSION_PROFESSIONAL
			if (smpsupport){
				if (cpuid==0){
					x = si->syswaitmultiple(nthreads-1,&events[1],true,1000);
					if (x==-1) return false;
					for (x=1;x<nthreads;x++) cts[x].go->set();
				}
				else{
					if (!cts[cpuid].ready->set()) return false;
					if (!cts[cpuid].go->wait(1000)) return false;
				};
			};
#		endif
	MTCATCH
		LOGD("%s - [Objects] ERROR: Exception while sub-processing!"NL);
		goto routing;
	MTEND
	return true;
}
//---------------------------------------------------------------------------
bool MTModule::addchannel(InstrumentInstance *c)
{
	int x;

	if (!mtinterface->addchannel()){
		LOGD("%s - [Objects] WARNING: Cannnot add channel!"NL);
		return false;
	};
	if (playstatus.nchannels==playstatus.nachannels){
		playstatus.nachannels += 8;
		playstatus.chan = (InstrumentInstance**)si->memrealloc(playstatus.chan,4*playstatus.nachannels);
	};
	for (x=playstatus.nchannels;x>0;x--){
		playstatus.chan[x] = playstatus.chan[x-1];
		playstatus.chan[x]->id = x;
	};
	playstatus.chan[0] = c;
	c->id = 0;
	playstatus.nchannels++;
	return true;
}

void MTModule::delchannel(InstrumentInstance *c)
{
	int x;
	
	if ((!c) || (playstatus.chan[c->id]!=c)){
		LOGD("%s - [Objects] ERROR: Invalid channel!"NL);
		return;
	};
	if (c->caller) c->caller->delinstance(c);
	for (x=c->id;x<playstatus.nchannels-1;x++){
		playstatus.chan[x] = playstatus.chan[x+1];
		playstatus.chan[x]->id = x;
	};
	playstatus.chan[--playstatus.nchannels] = 0;
	if (playstatus.nchannels<playstatus.nachannels-8){
		playstatus.nachannels -= 8;
		playstatus.chan = (InstrumentInstance**)si->memrealloc(playstatus.chan,4*playstatus.nachannels);
	};
	delete c;
}

void MTModule::resetchannels()
{
	int x;

	ENTER("MTModule::resetchannels");
	if (playstatus.chan){
		for (x=0;x<playstatus.nchannels;x++){
			if (playstatus.chan[x])	delete playstatus.chan[x];
		};
		si->memfree(playstatus.chan);
	};
	playstatus.nchannels = 0;
	playstatus.nachannels = 8;
	playstatus.chan = (InstrumentInstance**)si->memalloc(4*playstatus.nachannels,MTM_ZERO);
	LEAVE();
}

void MTModule::resetpatterns()
{
	int x;

	ENTER("MTModule::resetpatterns");
	for (x=0;x<MAX_LAYERS;x++){
		if (playstatus.patti[x]){
			delete playstatus.patti[x];
			playstatus.patti[x] = 0;
		};
		playstatus.cseq[x] = -1;
	};
	LEAVE();
}

InstrumentInstance* MTModule::getlessimportantchannel(int *importance)
{
	int x,less;
	float cimp;
	float max = 1000.0;
	
	if (playstatus.nchannels<=0) return 0;
	less = -1;
	for (x=playstatus.nchannels-1;x>=0;x--){
		InstrumentInstance &ci = *playstatus.chan[x];
		cimp = ci.getimportance();
		if (ci.cpos<0.5) cimp *= 2;
		if (ci.cpos<0.25) cimp *= 2;
		if (ci.cpos<0.125) cimp *= 2;
		if (cimp<max){
			max = cimp;
			less = x;
			if (max==0.0) break;
		};
	};
	if (less<0){
		if (playstatus.nchannels) less = 0;
		else return 0;
	};
	return playstatus.chan[less];
}

double MTModule::beatstosamples(double nbeats)
{
	return (nbeats*playstatus.coutput->frequency*60)/playstatus.bpm;
}

double MTModule::samplestobeats(double nsamples)
{
	return (nsamples*playstatus.bpm)/(playstatus.coutput->frequency*60);
}
//---------------------------------------------------------------------------
//
// Routing functions
//
//---------------------------------------------------------------------------
struct RN{
	int level;
	Node *node;
	MTArray *ins;
	MTArray *outs;
};

struct TmpBuf{
	sample *buffer[8];
};

#ifdef _DEBUG
void printnodes(MTArray *nodes)
{
	RN *n;
	int x;

	nodes->reset();
	while ((n = (RN*)nodes->next())){
		FLOG2("%-16s Level: %d",n->node->name,n->level);
		if (n->ins->nitems>0){
			LOG("  Inputs:");
			for (x=0;x<n->ins->nitems;x++){
				Pin *p = A(n->ins,Pin)[x];
				FLOG3(" %d<-%d:%s ",p->s,p->d,p->n->name);
			};
		};
		LOG(NL);
	};
	LOG(NL);
}

void printinstructions(MTArray *ris)
{
	RI *i;

	FLOG1("%d instructions:"NL,ris->nitems);
	ris->reset();
	while ((i = (RI*)ris->next())){
		if (i->lock) LOG("* ");
		else LOG("  ");
		switch (i->i){
		case RI_NOOP:
			FLOG2("%5d %3d NOOP"NL,i->level,i->cpu);
			break;
		case RI_CLEARBUFFER:
			FLOG3("%5d %3d CLEAR       %.8X"NL,i->level,i->cpu,i->buffer);
			break;
		case RI_PREPROCESS:
			FLOG3("%5d %3d PREPROCESS  %s"NL,i->level,i->cpu,i->track->name);
			break;
		case RI_PROCESS:
			FLOG3("%5d %3d PROCESS     %s"NL,i->level,i->cpu,i->track->name);
			break;
		case RI_POSTPROCESS:
			FLOG3("%5d %3d POSTPROCESS %s"NL,i->level,i->cpu,i->track->name);
			break;
		case RI_PROCESSNODE:
			FLOG3("%5d %3d PROCESSNODE %s"NL,i->level,i->cpu,i->node->name);
			break;
		case RI_AMPLIFYBUFFER:
			FLOG4("%5d %3d AMPLIFY     %.8X %f"NL,i->level,i->cpu,i->buffer,i->factor);
			break;
		case RI_MIXBUFFER:
			FLOG4("%5d %3d MIX         %.8X %.8X"NL,i->level,i->cpu,i->buffer,i->dest);
			break;
		default:
			break;
		};
	};
}
#endif

void checknode(RN *rn,Node *n,int level)
{
	int x;
	Node *o;
	MTArray *outputs;

	if ((n->noutputs==0) || (!n->outputs[0].n)) return;
	if (level+1>rn->level) rn->level = level+1;
	outputs = si->arraycreate(4,0);
	for (x=0;x<n->noutputs;x++){
		o = n->outputs[x].n;
		if (o==n){
			rn->level = -1;
			break;
		};
		if (!o) continue;
		if (((o->objecttype & MTO_TYPEMASK)==MTO_TRACK) && (o->id>=MAX_TRACKS)) continue;
		if (outputs->getitemid(o)>=0) continue;
		outputs->push(o);
		if (rn->outs->getitemid(o)!=-1){
// There is a loop, so we deactivate the node
			rn->level = -1;
			break;
		}
		else{
			checknode(rn,o,level+1);
			if (rn->level==-1) break;
		};
	};
	outputs->reset();
	while ((o = (Node*)outputs->next())){
		rn->outs->push(o);
	};
	si->arraydelete(outputs);
}

void MTCT delitem(void *item,void*)
{
	if (!item) return;
	((RN*)item)->ins->clear(true);
	si->arraydelete(((RN*)item)->ins);
	si->arraydelete(((RN*)item)->outs);
}

int MTCT sortnodes(void *item1,void *item2)
{
	int d = ((RN*)item2)->level-((RN*)item1)->level;

	if (d!=0) return d;
	return ((RN*)item1)->node->id-((RN*)item2)->node->id;
}

RN* getrn(MTArray *nodes,Node *n)
{
	int x;

	for (x=0;x<nodes->nitems;x++){
		if (A(nodes,RN)[x]->node==n) return A(nodes,RN)[x];
	};
	return 0;
}

void MTModule::updaterouting()
{
	int x,y,z,l;
#ifdef _DEBUG
	int total = 0;
	bool log = false;
#endif
	RN *b,*o;
	MTArray *nodes;
	MTArray *tmpbuf;
	RI ri,iri;
	RI *i;
#ifdef MTVERSION_PROFESSIONAL
	unsigned char ccpu = 0;
	bool inlevel[256];
#endif

#	ifdef _DEBUG
#		ifdef _WIN32
			if (GetKeyState(VK_SHIFT)<0) log = true;
#		else
			//TODO
			log = true;
#		endif
#	endif
	ENTER("MTModule::updaterouting");
	needupdate = false;
	mtmemzero(&ri,sizeof(ri));
	mtmemzero(&iri,sizeof(iri));
	MTTRY
		lock(MTOL_LOCK,true);
		LOGD("%s - [Objects] Compiling route..."NL);
		l = (output->buffersamples*2)*sizeof(sample);
		buffers->clear(true);
		ris->clear();
		nodes = si->arraycreate(8,0);
		for (x=0;x<master->nitems;x++){
			if (master->a[x]){
				b = mtnew(RN);
				b->level = 0;
				b->node = A(master,Node)[x];
				b->ins = si->arraycreate(4,0);
				b->outs = si->arraycreate(4,0);
				nodes->push(b);
			};
		};
// Check the output level of each track
		for (x=0;x<trk->nitems;x++){
			if (trk->a[x]){
				b = mtnew(RN);
				b->level = 0;
				b->node = A(trk,Node)[x];
				b->ins = si->arraycreate(4,0);
				b->outs = si->arraycreate(4,0);
				nodes->push(b);
				checknode(b,A(trk,Node)[x],0);
			};
		};
// Check the output level of each effect
		for (x=0;x<fx->nitems;x++){
			if (fx->a[x]){
				A(fx,Effect)[x]->deleteinstance(0);
				b = mtnew(RN);
				b->level = 0;
				b->node = A(fx,Node)[x];
				b->ins = si->arraycreate(4,0);
				b->outs = si->arraycreate(4,0);
				nodes->push(b);
				checknode(b,A(fx,Node)[x],0);
			};
		};
// Check for input dependencies
		for (x=0;x<nodes->nitems;x++){
			b = A(nodes,RN)[x];
			Node *d = b->node;
			nodes->reset();
			while ((o = (RN*)nodes->next())){
				Node *s =o->node;
				if (s==d) continue;
				for (y=0;y<s->noutputs;y++){
					Pin &cpin = s->outputs[y];
					if (cpin.n==d){
						Pin *pin = mtnew(Pin);
						pin->s = cpin.d;
						pin->d = cpin.s;
						pin->m = cpin.m;
						pin->n = s;
						b->ins->push(pin);
					};
				};
			};
		};
// Sort nodes (track+effects) by output level importance
		nodes->sort(sortnodes);
#		ifdef _DEBUG
			if (log) printnodes(nodes);
#		endif
		nodes->reset();
// Empty the buffer of each track
		ri.i = RI_CLEARBUFFER;
		ri.level = 65534;
		for (x=0;x<ntracks;x++){
#			ifdef MTVERSION_PROFESSIONAL
				ri.cpu = ccpu;
				if ((smpsupport) && (++ccpu==nthreads)) ccpu = 0;
#			else
				ri.cpu = 0;
#			endif
			for (y=0;y<A(trk,Track)[x]->noutputs;y++){
				ri.buffer = A(trk,Track)[x]->buffer[y];
				ris->push(&ri);
			};
		};
// Prepare the input buffers of each effect
		tmpbuf = si->arraycreate(fx->nitems,0);
		tmpbuf->additems(0,fx->nitems);
		while ((b = (RN*)nodes->next())){
			if ((b->node->objecttype & MTO_TYPEMASK)!=MTO_TRACK){
				Pin *pin;
				Effect &e = *(Effect*)b->node;
				b->ins->reset();
				while ((pin = (Pin*)b->ins->next())){
					if (!tmpbuf->a[e.id]){
						tmpbuf->a[e.id] = mtnew(TmpBuf);
					};
					if (A(tmpbuf,TmpBuf)[e.id]->buffer[pin->s]==0){
						sample* buffer = (sample*)si->memalloc(l,MTM_ZERO);
#						ifdef _DEBUG
							total += l;
#						endif
						buffers->push(buffer);
						A(tmpbuf,TmpBuf)[e.id]->buffer[pin->s] = buffer;
						ri.i = RI_CLEARBUFFER;
						ri.level = 65533;
#						ifdef MTVERSION_PROFESSIONAL
							ri.cpu = ccpu;
							if ((smpsupport) && (++ccpu==nthreads)) ccpu = 0;
#						else
							ri.cpu = 0;
#						endif
						ri.buffer = buffer;
						ris->push(&ri);
					};
				};
			};
		};
// Create the routing instructions
		for (x=0;x<nodes->nitems;x++){
			int use[8] = {0,0,0,0,0,0,0,0};
			sample *outputs[8] = {0,0,0,0,0,0,0,0};
			b = A(nodes,RN)[x];
			ri.level = b->level;
#			ifdef MTVERSION_PROFESSIONAL
				ri.cpu = ccpu;
				if ((smpsupport) && (++ccpu==nthreads)) ccpu = 0;
#			else
				ri.cpu = 0;
#			endif
			Node &cn = *(b->node);
// Skip master tracks
			if (((cn.objecttype & MTO_TYPEMASK)==MTO_TRACK) && (cn.id>=MAX_TRACKS)) continue;
			if (b->level==0){
				FLOG1("  ERROR: Level-0 track or effect (%s)! Bogus routing!"NL,b->node->name);
				continue;
			};
			for (y=0;y<MAX_CONNECTIONS;y++){
				Node *n = cn.outputs[y].n;
				if (!n) continue;
				use[cn.outputs[y].s]++;
				if ((n->objecttype & MTO_TYPEMASK)==MTO_TRACK) outputs[cn.outputs[y].s] = ((Track*)n)->buffer[cn.outputs[y].d];
				else outputs[cn.outputs[y].s] = A(tmpbuf,TmpBuf)[n->id]->buffer[cn.outputs[y].d];
			};
// Is the output used more than one time?
			for (y=0;y<8;y++){
				if (use[y]>1){
					sample* buffer = (sample*)si->memalloc(l,MTM_ZERO);
#					ifdef _DEBUG
						total += l;
#					endif
					buffers->push(buffer);
					outputs[y] = buffer;
				};
			};
			if ((cn.objecttype & MTO_TYPEMASK)==MTO_TRACK){
				Track &t = *(Track*)&cn;
// Process instruments before
				iri.i = RI_PREPROCESS;
				iri.level = ri.level+3;
				iri.track = &t;
				iri.cpu = 255;
				iri.lock = cn._lock;
				y = 0;
				ris->reset();
				while ((i = (RI*)ris->next())){
					if ((i->level<32768) && (i->level<iri.level)){
						ris->additem(y,&iri);
						y = -1;
						break;
					};
					y++;
				};
				if (y>=0) ris->push(&iri);
				iri.i++;
				iri.level--;
				y = 0;
				ris->reset();
				while ((i = (RI*)ris->next())){
					if ((i->level<32768) && (i->level<iri.level)){
						ris->additem(y,&iri);
						y = -1;
						break;
					};
					y++;
				};
				if (y>=0) ris->push(&iri);
				iri.i++;
				iri.level--;
				y = 0;
				ris->reset();
				while ((i = (RI*)ris->next())){
					if ((i->level<32768) && (i->level<iri.level)){
						ris->additem(y,&iri);
						y = -1;
						break;
					};
					y++;
				};
				if (y>=0) ris->push(&iri);
// Process the tracks
				ri.i = RI_MIXBUFFER;
#				ifdef _DEBUG
					if (log){
						FLOG1("Outputs of %s:"NL,t.name);
						LOG("   Inputs    Outputs"NL);
						LOG("---------------------"NL);
					};
#				endif
				for (y=0;y<8;y++){
					if (!outputs[y]) continue;
#					ifdef _DEBUG
						if (log) FLOG3("%d  %.8X  %.8X"NL,y,t.buffer[y],outputs[y]);
#					endif
					ri.buffer = t.buffer[y];
					ri.dest = outputs[y];
					ri.lock = cn.outputs[y].n->_lock;
					ris->push(&ri);
				};
#				ifdef _DEBUG
					if (log) LOG(NL);
#				endif
			}
			else{
// Create the instance of each effect
				Effect &e = *(Effect*)&cn;
				e.createinstance(e.noutputs,outputs,e.ninputs,A(tmpbuf,TmpBuf)[e.id]->buffer);
#				ifdef _DEBUG
					if (log){
						FLOG1("Instanciating %s:"NL,e.name);
						LOG("   Inputs    Outputs"NL);
						LOG("---------------------"NL);
					};
#				endif
				for (y=0;y<8;y++){
					if (!outputs[y]) continue;
#					ifdef _DEBUG
						if (log) FLOG3("%d  %.8X  %.8X"NL,y,A(tmpbuf,TmpBuf)[e.id]->buffer[y],outputs[y]);
#					endif
				};
#				ifdef _DEBUG
					if (log) LOG(NL);
#				endif
				ri.i = RI_PROCESSNODE;
				ri.node = &cn;
				ris->push(&ri);
			};
			for (y=0;y<MAX_CONNECTIONS;y++){
				Node *n = cn.outputs[y].n;
				if (!n) continue;
// This output is used more than one time
				if (use[cn.outputs[y].s]>1){
// We need to mix it first, because it is a temporary buffer
					ri.i = RI_MIXBUFFER;
					ri.lock = cn.outputs[y].n->_lock;
					ri.buffer = outputs[cn.outputs[y].s];
					if ((n->objecttype & MTO_TYPEMASK)==MTO_TRACK) ri.dest = ((Track*)n)->buffer[cn.outputs[y].d];
					else ri.dest = A(tmpbuf,TmpBuf)[n->id]->buffer[cn.outputs[y].d];
					ris->push(&ri);
					ri.lock = 0;
				};
				o = getrn(nodes,n);
				ri.i = RI_AMPLIFYBUFFER;
				if ((n->objecttype & MTO_TYPEMASK)==MTO_TRACK) ri.buffer = ((Track*)n)->buffer[cn.outputs[y].d];
				else ri.buffer = A(tmpbuf,TmpBuf)[n->id]->buffer[cn.outputs[y].d];
				ri.factor = cn.outputs[y].m;
				for (z=0;z<o->ins->nitems;z++){
					Pin *pin = A(o->ins,Pin)[z];
					if (pin->s==cn.outputs[y].d){
						if (pin->n==b->node){
							si->memfree(pin);
							o->ins->delitems(z--,1);
							continue;
						};
						ri.factor /= pin->m;
					};
				};
				if (ri.factor!=1.0) ris->push(&ri);
			};
		};
#		ifdef MTVERSION_PROFESSIONAL
// Add NOOP instructions to empty levels for synchronization
			if (smpsupport){
				ri.i = RI_NOOP;
				l = 65534;
				mtmemzero(inlevel,sizeof(inlevel));
				ris->reset();
				x = 0;
				while ((i = (RI*)ris->next())){
					unsigned char ccpu = i->cpu;
					if (i->level<l){
						ri.level = l;
						l = i->level;
						for (y=0;y<nthreads;y++){
							if (!inlevel[y]){
								ri.cpu = y;
								ris->additem(x,&ri);
								ris->next();
								x++;
							};
						};
						mtmemzero(inlevel,sizeof(inlevel));
					};
					inlevel[ccpu] = true;
					x++;
				};
				ri.level = l;
				for (y=0;y<nthreads;y++){
					if (!inlevel[y]){
						ri.cpu = y;
						ris->push(&ri);
					};
				};
			};
#		endif
#		ifdef _DEBUG
			if (log) printinstructions(ris);
#		endif
		tmpbuf->clear(true);
		nodes->clear(true,delitem);
		si->arraydelete(nodes);
		FLOG1("%d bytes allocated for buffers."NL,total);
		LOGD("%s - [Objects] Route compiled."NL);
	MTCATCH
	MTEND
	lock(MTOL_LOCK,false);
	LEAVE();
}

void MTModule::needupdaterouting()
{
	needupdate = true;
}
//---------------------------------------------------------------------------
void MTModule::remove(MTObject *o)
{
	switch (o->objecttype & MTO_TYPEMASK){
	case MTO_PATTERN:
		patt->a[o->id] = 0;
		break;
	case MTO_AUTOMATION:
		apatt->a[o->id] = 0;
		break;
	case MTO_INSTRUMENT:
		instr->a[o->id] = 0;
		break;
	case MTO_OSCILLATOR:
		spl->a[o->id] = 0;
		break;
	case MTO_TRACK:
		if (o->id>=MAX_TRACKS) master->a[o->id-MAX_TRACKS] = 0;
		else trk->a[o->id] = 0;
		break;
	case MTO_EFFECT:
	case MTO_TRACKEFFECT:
		fx->a[o->id] = 0;
		break;
	};
}
//---------------------------------------------------------------------------
