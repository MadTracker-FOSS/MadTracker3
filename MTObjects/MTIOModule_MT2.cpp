//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTIOModule_MT2.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTModule.h"
#include "MTAutomation.h"
#include "MTDelay.h"
#include "MTObjects1.h"
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
// MadTracker 2 Module
//---------------------------------------------------------------------------
#define MF_PACKEDPATTERNS 0x01
#define MF_AUTOMATION     0x02
#define MF_DRUMSAUTO      0x08
#define MF_MASTERAUTO     0x10

#if defined __BORLANDC__
#	define PACKED
#	pragma -a2
#elif defined(WIN32) || defined(__FLAT__)
#	define PACKED
#	pragma pack(push)
#	pragma pack(2)
#else
#	define PACKED  __attribute__((packed));
#endif

struct _MT2Header{
	char id[4];	// id = 'MT20'
	int userid;
	unsigned short version;
	char tracker[32];
	char title[64];
	unsigned short npos,restart,npatts,ntracks,spt;
	unsigned char ticks,lpb;
	int flags;
	unsigned short ninstr,nspl;
	unsigned char pl[256];
	unsigned short ddl;
}PACKED;

struct _MT2DrumsData{
	unsigned short ndpatts;
	unsigned short splmap[8];
	unsigned char dpl[256];
}PACKED;

struct _MT2TRKS{
	unsigned short volume;
	bool trackfx;
	char output;
	unsigned short trkfxid;
	unsigned short trkfxparam[64][8];
}PACKED;

struct _MT2Automation{
	unsigned short flags;
	unsigned short trkfxid;
}PACKED;

struct _MT2Automation203{
	int flags;
	int trkfxid;
}PACKED;

struct _MT2EnvPoint{
	unsigned short x;
	short y;
}PACKED;

struct _MT2Instrument{
	char name[32];
	int datalength;	// WARNING: if (datalength==32) datalength += 108+sizeof(_MT2IEnvelope)*4
	unsigned short nsamples;
	unsigned char splmap[96];
	unsigned char vibtype,vibsweep,vibdepth,vibrate;
	unsigned short fadeout,nna;
// if (ver>=0x0201)
	unsigned short flags;
// if (ver>=0x0202) else volume and panning only
	int envmask;
}PACKED;

struct _MT2IEnvelope{
	char flags;
	unsigned char npoints;
	unsigned char sust;
	unsigned char loops;
	unsigned char loope;
	char reserved[3];
	_MT2EnvPoint points[16];
}PACKED;

struct _MT2InstrSynth{	// if (_MT2Instrument.flags!=0)
	unsigned char synthid;
	unsigned char effectid;
	unsigned short cutoff;
	unsigned char resonance,attack,decay;
	unsigned char channel;
	signed char device;
	unsigned char volume;
	signed char pitch;
	signed char transpose;
	unsigned char midich;
	unsigned char midipr;
	unsigned char prog;
	char reserved[17];
}PACKED;

struct _MT2Sample{
	char name[32];
	int length;
	int datalength;
	int frequency;
	char depth;
	char nchannels;
	char flags;
	char loop;
	int loops;
	int loope;
	unsigned short volume;
	char panning;
	char note;
	short spb;
}PACKED;

struct _MT2Group{
	unsigned char splid;
	unsigned char vol;
	short pitch;
	char reserved[4];
}PACKED;

struct _MT2VST{
	char dll[64];
	char programname[28];
	mt_uint32 fxid;
	mt_uint32 fxversion;
	mt_int32 programnr;
	bool usechunks;
	unsigned char track;
	signed char pan;
	char res[17];
	mt_uint32 n;
}PACKED;

#if defined(WIN32) || defined(__FLAT__)
#	pragma pack(pop)
#elif defined CBUILDER
#	pragma -a-
#endif

void loadMT2pattern(MTPattern *p,void *buffer,int size,bool compressed)
{
	register unsigned char *start = (unsigned char*)buffer;
	register unsigned char *repeat,*dest,*cdest;
	unsigned char *end = start+size;
	register int x,track,line;
	register unsigned char cb,m;
	register unsigned short r;
	bool ok = true;
	int o = 0;
	static const char *columns[] = {"Note","Volume","Panning","Effect"};
	
	p->change(p->nlines,p->ntracks,4,columns,false);
	if ((p->data==0) || (p->tracks[0].colsize!=7)) return;
	dest = (unsigned char*)p->data;
	if (compressed){
		track = 0;
		line = 0;
		while (start<end){
			cb = *start++;
			r = 1;
			if (cb==0xFF){
				r += *start++;
				cb = *start++;
			};
			if (!(cb & 0x80)){
				--start;
				m = 0x7F;
			}
			else m = cb & 0x7F;
			repeat = start;
			if (m!=0){
				cdest = dest+line*p->linesize;
				for (x=0;x<r;x++){
					start = repeat;
					if (m & 1) cdest[0] = *start++;
					if (m & 2) cdest[1] = *start++;
					if (m & 4) cdest[2] = *start++;
					if (m & 8) cdest[3] = *start++;
					if (m & 16) cdest[4] = *start++;
					if (m & 32) cdest[5] = *start++;
					if (m & 64) cdest[6] = *start++;
					cdest += p->linesize;
					line++;
				};
			}
			else line += r;
			if (line==p->nlines){
				track++;
				line = 0;
				dest = (unsigned char*)p->data+track*7;
			};
		};
	}	
	else{
		for (x=0;x<p->nlines;x++){
			memcpy(dest,start,p->ntracks*7);
			start += p->ntracks*7;
			dest += p->linesize;
		};
	};
}

void loadMT2drums(MTPattern *p,void *buffer,int size)
{
	register unsigned char *start = (unsigned char*)buffer;
	register unsigned char *dest;
	unsigned char *end = start+size;
	register int track,line;
	bool ok = true;
	int o = 0;
	static const char *columns[] = {"Drums","Volume","Panning"};
	
	p->change(p->nlines,p->ntracks,3,columns,false);
	if ((p->data==0) || (p->tracks[0].colsize!=4)) return;
	dest = (unsigned char*)p->data;
	track = 0;
	line = 0;
	while (start<end){
		dest[0] = *start++;
		if (dest[0]) dest[1] = track+1;
		dest[2] = *start++;
		dest[3] = *start++;
		start++;
		dest += 4;
		if (++track==p->ntracks){
			track = 0;
		};
	};
}

bool loadMT2(MTObject *object,char *filename,void *process)
{
	MTModule &module = *(MTModule*)object;
	MTFile *f;
	MTProcess *p = (MTProcess*)process;
	int x,y,z,incl,size,csize,pc,ac,dc,max,nvst,eox;
	mt_uint32 tmpl;
	_MT2Header header;
	_MT2DrumsData drums;
	char tmpb,*tmpc,*tmpc2;
	unsigned short tmpw,ndtracks;
	double pcl,acl,dcl,incd,tmpd;
	bool ok;
	unsigned short nispl[256];
	unsigned short tmp_vol[256];
	char tmp_panx[256];

	if ((f = si->fileopen(filename,MTF_READ|MTF_SHAREREAD))==0) return false;
	mtmemzero(&nispl,sizeof(nispl));
	mtmemzero(&tmp_vol,sizeof(tmp_vol));
	mtmemzero(&tmp_panx,sizeof(tmp_panx));
	module.access.caccess &= ~MTOA_CANCOPY;
	max = f->length();
// Header
	f->read(&header,sizeof(header));
	if (strncmp(header.id,"MT20",4)) goto error;
//TODO
//	module.access.creatorid = header.userid;
	memcpy(module.name,header.title,64);
	FLOG3("Loading \"%s\" Author: %08X Software: %s"NL,header.title,header.userid,header.tracker);
	if (header.ticks<1) header.ticks = 6;
	if (header.lpb<1) header.lpb = 4;
	D(module.tempo,Tempo)[0].bpm = 44100.0*60.0/(header.ticks*header.lpb*header.spt);
	if (header.ddl){
		ndtracks = 8;
		f->read(&drums,sizeof(drums));
	}
	else{
		ndtracks = 0;
		mtmemzero(&drums,sizeof(drums));
	};
	if (!module.settracks(header.ntracks+ndtracks,module.nmtracks)) goto error;
	module.enabletracks();
	if (p){
		p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
	nvst = 0;
// Advanced Data
	f->read(&size,4);
	incl = 0;
	while (incl<size){
		f->read(&tmpl,4);
		f->read(&csize,4);
		z = f->pos();
		incl += csize+8;
		switch (tmpl){
		case FOURCC('B','P','M','+'):
			f->read(&tmpd,8);
			D(module.tempo,Tempo)[0].bpm = 44100.0*60.0/(header.ticks*header.lpb*tmpd);
			break;
		case FOURCC('T','F','X','M'):
			break;
		case FOURCC('T','R','K','S'):
			f->read(&tmpw,2);
			A(module.master,Track)[0]->vol = (float)tmpw/131072;
			for (x=0;x<header.ntracks+ndtracks;x++){
				Track &ctrk = *A(module.trk,Track)[x];
				_MT2TRKS trk;
				f->read(&trk,sizeof(trk));
				if (header.version<0x0205){
					trk.trkfxparam[1][0] = (int)(trk.trkfxparam[1][0]/header.spt);
					trk.trkfxparam[2][0] = (int)(trk.trkfxparam[2][0]/header.spt);
				};
				Effect *e = 0;
				if (trk.trackfx){
					switch (trk.trkfxid){
					case 1:		// Delay
						e = new MTDelay(&module,x);
						{
							MTDelay &cd = *(MTDelay*)e;
							cd.setparam(0,0,1.0);
							cd.setparam(1,0,DF_BEATS);
							cd.setparam(1,1,(float)trk.trkfxparam[1][0]/(header.lpb*header.ticks));
							cd.setparam(1,2,(float)trk.trkfxparam[1][1]/128.0);
							cd.setparam(1,3,(float)trk.trkfxparam[1][2]/128.0);
						};
						break;
					case 2:		// Stereo Delay
						e = new MTDelay(&module,x);
						{
							MTDelay &cd = *(MTDelay*)e;
							cd.setparam(1,0,DF_BEATS);
							cd.setparam(1,1,(float)trk.trkfxparam[1][0]/(header.lpb*header.ticks));
							cd.setparam(1,2,(float)trk.trkfxparam[1][1]/128.0);
							cd.setparam(1,3,(float)trk.trkfxparam[1][2]/128.0);
						}
						break;
					case 3:		// Filter
						break;
					case 4:		// Flange
						break;
					case 5:		// Analog Filter
						e = new MTFilter(&module,x);
						e->setparam(0,0,(double)trk.trkfxparam[5][1]/44100.0);
						e->setparam(0,1,(double)trk.trkfxparam[5][2]/128.0);
						e->setparam(0,3,trk.trkfxparam[5][4]);
						e->setparam(0,4,(double)trk.trkfxparam[5][5]/128.0);
						break;
					case 6:		// Distortion
						break;
					case 102:	// mpReverb
						break;
					};
				};
				if ((x>=header.ntracks) && (trk.output>header.ntracks)){
					if (trk.output>64) trk.output = trk.output-64+header.ntracks;
					else trk.output = trk.output-32+header.ntracks;
				};
				if (trk.output>=module.trk->nitems) trk.output = 0;
				if (e){
					module.fx->a[x] = e;
					ctrk.outputs[0].n = e;
					ctrk.outputs[1].n = e;
					if (trk.output){
						e->outputs[0].n = A(module.trk,Node)[trk.output];
						e->outputs[1].n = A(module.trk,Node)[trk.output];
					}
					else{
						e->outputs[0].n = A(module.master,Track)[0];
						e->outputs[1].n = A(module.master,Track)[0];
					};
				}
				else if (trk.output){
					ctrk.outputs[0].n = A(module.trk,Node)[trk.output];
					ctrk.outputs[1].n = A(module.trk,Node)[trk.output];
				};
				ctrk.vol = (float)trk.volume/32768;
			};
			module.needupdaterouting();
			break;
		case FOURCC('T','R','K','L'):
			tmpc = (char*)si->memalloc(csize,0);
			for (x=0;x<header.ntracks+ndtracks;x++){
				Track &ctrk = *A(module.trk,Track)[x];
				f->readln(tmpc,csize);
				if (tmpc[0]) ctrk.setname(tmpc);
			};
			si->memfree(tmpc);
			break;
		case FOURCC('P','A','T','N'):
			break;
		case FOURCC('M','S','G','\0'):
			f->read(&module.showmessage,1);
			module.message = (char*)si->memalloc(csize,0);
			f->read(module.message,csize-1);
			break;
		case FOURCC('P','I','C','T'):
			break;
		case FOURCC('S','U','M','\0'):
			for (x=0;x<6;x++){
				f->read(&tmpb,1);
				if (tmpb) module.summarymask |= (1<<x);
			};
			tmpc = (char*)si->memalloc(csize-6,0);
			tmpc2 = tmpc;
			f->read(tmpc,csize-6);
			for (x=0;x<6;x++){
				if ((tmpl = strlen(tmpc))!=0){
					module.summary[x] = (char*)si->memalloc(tmpl+1,0);
					strcpy(module.summary[x],tmpc);
				};
				tmpc = strchr(tmpc,'\0')+1;
			};
			si->memfree(tmpc2);
			break;
		case FOURCC('T','M','A','P'):
			break;
		case FOURCC('M','I','D','I'):
			break;
		case FOURCC('T','R','E','Q'):
			break;
		case FOURCC('V','S','T','2'):
			f->read(&nvst,4);
			break;
		};
		f->seek(z+csize,MTF_BEGIN);
	};
	FLOG1("  BPM: %0.4f"NL,D(module.tempo,Tempo)[0].bpm);
	if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
// Patterns
	for (x=0;x<header.npatts;x++){
		MTPattern &cpatt = *(MTPattern*)oi->newobject(MTO_MTPATTERN,&module,x,0,true);
		module.patt->a[x] = &cpatt;
		cpatt.lpb = header.lpb;
		cpatt.ticks = header.ticks;
		cpatt.flags |= MTPF_INHERIT_TICKS;
		tmpl = 0;
		f->read(&tmpl,2);
		f->read(&size,4);
		if (size){
			cpatt.nlines = tmpl;
			cpatt.nbeats = (double)tmpl/header.lpb;
			cpatt.ntracks = header.ntracks;
			void *buf = f->getpointer(-1,size);
			loadMT2pattern(&cpatt,buf,size,((header.flags & MF_PACKEDPATTERNS)!=0));
			f->releasepointer(buf);
			if (size & 1) size++;
			f->seek(size,MTF_CURRENT);
		};
		cpatt.lock(MTOL_LOCK,false);
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
// Drums Patterns
	for (x=0;x<drums.ndpatts;x++){
		MTPattern &cdpatt = *(MTPattern*)oi->newobject(MTO_MTPATTERN,&module,header.npatts+x,0,true);
		module.patt->a[header.npatts+x] = &cdpatt;
		cdpatt.lpb = header.lpb;
		cdpatt.ticks = header.ticks;
//		memcpy(cdpatt.splid,drums.splmap,sizeof(drums.splmap));
		tmpl = 0;
		f->read(&tmpl,2);
		cdpatt.nlines = tmpl;
		cdpatt.nbeats = (double)tmpl/header.lpb;
		cdpatt.ntracks = ndtracks;
		size = (int)(cdpatt.nbeats*header.lpb)*8*4;
		void *buf = f->getpointer(-1,size);
		loadMT2drums(&cdpatt,buf,size);
		f->releasepointer(buf);
		f->seek(size,MTF_CURRENT);
		cdpatt.lock(MTOL_LOCK,false);
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
// Automation
	if (header.flags & MF_AUTOMATION){
		for (x=0;x<header.npatts;x++){
			Automation *pauto = new Automation(A(module.patt,Pattern)[x],0);
//			A(module.apatt,Automation)[x] = pauto;
			y = 0;
			ok = false;
			while (true){
				if (y>MAX_TRACKS+1){
					_MT2Automation203 cauto;
					f->read(&cauto,sizeof(cauto));
					if (cauto.flags){
						tmpl = cauto.flags;
						while (tmpl!=0){
							_MT2EnvPoint tmpp[64];
							if (tmpl & 1){
								f->seek(4+sizeof(_MT2EnvPoint)*64,MTF_CURRENT);
							};
							tmpl >>= 1;
						};
					};
					goto autoskip;
				};
				if (header.version>=0x203){
					_MT2Automation203 cauto;
					f->read(&cauto,sizeof(cauto));
					tmpl = cauto.flags;
				}
				else{
					_MT2Automation cauto;
					f->read(&cauto,sizeof(cauto));
					tmpl = cauto.flags;
				};
				while (tmpl!=0){
					_MT2EnvPoint tmpp[64];
					if (tmpl & 1){
						TrackAuto ctrkauto;
						mtmemzero(&ctrkauto,sizeof(ctrkauto));
						f->read(&ctrkauto.env.npoints,4);
						f->read(tmpp,sizeof(tmpp));
						for (incl=0;incl<64;incl++){
							ctrkauto.env.points[incl].flags = MTEP_LINEAR;
							ctrkauto.env.points[incl].x = (float)tmpp[incl].x/(header.ticks*header.lpb);
							ctrkauto.env.points[incl].y = (float)tmpp[incl].y;
						};
						pauto->envelopes->push(&ctrkauto);
					};
					tmpl >>= 1;
					ok = true;
				};
autoskip:
				y++;
				if (y==header.ntracks+ndtracks){
					y = MAX_TRACKS;
				}
				else if (y==MAX_TRACKS+1){
					if (header.version<0x250) break;
				};
				if (y>=MAX_TRACKS+1+nvst) break;
			};
			if (!ok){
				delete pauto;
//				delete A(module.apatt,Automation)[x];
//				module.apatt->a[x] = 0;
			};
		};
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
//TODO Implement the sequence translation into a function
// Sequences
	tmpd = 0.0;
	pc = 0;
	ac = 0;
	dc = 0;
	pcl = 0;
	acl = 0;
	dcl = 0;
	for (x=0;x<256;x++){
		if (x==header.npos){
			module.loope = tmpd;
			break;
		};
		incd = A(module.patt,Pattern)[header.pl[x]]->nbeats;
		if ((pc) && (header.pl[x]==module.sequ[1][pc-1].patt) && (pcl==tmpd)){
			module.sequ[1][pc-1].length += incd;
		}
		else{
			module.sequ[1][pc].patt = header.pl[x];
			module.sequ[1][pc].pos = tmpd;
			module.sequ[1][pc].length = incd;
			pc++;
		};
		pcl = tmpd+incd;
		if (A(module.apatt,Automation)[header.pl[x]]){
			if ((ac) && (header.pl[x]==(module.sequ[0][ac-1].patt & 0xFFF))  && (acl==tmpd)){
				module.sequ[0][ac-1].length += incd;
			}
			else{
				module.sequ[0][ac].patt = header.pl[x] | 0x1000;
				module.sequ[0][ac].pos = tmpd;
				module.sequ[0][ac].length = incd;
				ac++;
			};
			acl = tmpd+incd;
		};
		if (drums.ndpatts){
			if ((dc) && (drums.dpl[x]==module.sequ[2][dc-1].patt-header.npatts) && (dcl==tmpd)){
				module.sequ[2][dc-1].length += incd;
			}
			else{
				module.sequ[2][dc].patt = drums.dpl[x]+header.npatts;
				module.sequ[2][dc].pos = tmpd;
				module.sequ[2][dc].length = incd;
				dc++;
			};
			dcl = tmpd+incd;
		};
		if (x==header.restart) module.loops = tmpd;
		tmpd += incd;
	};
	module.nsequ[0] = ac;
	module.nsequ[1] = pc;
	module.nsequ[2] = dc;

	module.setstatus();
	module.playstatus.length = module.loope;
// Instruments
	for (x=1;x<256;x++){
		_MT2Instrument idata;
		idata.flags = 0;
		idata.envmask = 3;
		tmpl = sizeof(idata);
		if (header.version<0x0202) tmpl -= 4;
		if (header.version<0x0201) tmpl -= 2;
		f->read(&idata,36);
		if (idata.datalength==32) idata.datalength += 108+sizeof(_MT2IEnvelope)*4;
		if (idata.datalength){
			f->read(&idata.nsamples,tmpl-36);
			MTInstrument &cinstr = *(new MTInstrument(&module,x));
			module.instr->a[x] = &cinstr;
			memcpy(cinstr.name,idata.name,32);
			nispl[x] = idata.nsamples;
			cinstr.flags = MTIF_FADEOUT|MTIF_FAE;
			memcpy(cinstr.range[0],idata.splmap,96);
			cinstr.vibtype = idata.vibtype;
			cinstr.vibsweep = idata.vibsweep;
			cinstr.vibdepth = idata.vibdepth;
			cinstr.vibrate = idata.vibrate;
			cinstr.fadeout = -(double)idata.fadeout*(header.ticks*header.lpb)/32768;
			cinstr.nna = idata.nna;
			tmpl = idata.envmask;
			cinstr.tpb = header.ticks*header.lpb;
			for (y=0;y<4;y++){
				_MT2IEnvelope iedata;
				if (tmpl & 1){
					IEnvelope &cenv = cinstr.env[y];
					f->read(&iedata,sizeof(iedata));
					cenv.flags = iedata.flags;
					cenv.npoints = iedata.npoints;
					cenv.loops = iedata.loops;
					cenv.loope = iedata.loope;
					cenv.susts = cenv.suste = iedata.sust;
					incl = 0;
					eox = 0;
//					if ((y<2) && (((cenv.flags & EF_SUSTAIN)==0) || (cenv.susts>0))){
					if (y<2){
						cenv.points[0].x = (float)(iedata.points[0].x+eox)/cinstr.tpb;
						cenv.points[0].y = (float)iedata.points[0].y/64;
						incl = 1;
						eox = 1;
						cenv.npoints++;
						cenv.loops++;
						cenv.loope++;
						cenv.susts++;
						cenv.suste++;
					};
					for (z=0;z<16;z++,incl++){
						cenv.points[incl].x = (float)(iedata.points[z].x+eox)/cinstr.tpb;
						cenv.points[incl].y = (float)iedata.points[z].y/64;
					};
				};
				tmpl >>= 1;
			};
			IEnvelope &cenv = cinstr.env[1];
			if (cenv.npoints){
				for (incl=0;incl<cenv.npoints;incl++){
					cenv.points[incl].y = cenv.points[incl].y*2-1;
				};
			};
			if (idata.flags){
				_MT2InstrSynth isdata;
				f->read(&isdata,sizeof(isdata));
				cinstr.cutoff = isdata.cutoff;
				cinstr.resonance = isdata.resonance;
				cinstr.attack = isdata.attack;
				cinstr.decay = isdata.decay;
				cinstr.setfilter(true);
			};
		};
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
// Samples
	for (x=0;x<256;x++){
		_MT2Sample sdata;
		f->read(&sdata,36);
		if (sdata.length){
			f->read(&sdata.datalength,sizeof(sdata)-36);
			MTSample &cspl = *(new MTSample(&module,x));
			module.spl->a[x] = &cspl;
			memcpy(cspl.name,sdata.name,32);
			cspl.length = sdata.datalength;
			cspl.frequency = sdata.frequency;
			cspl.depth = sdata.depth;
			cspl.nchannels = sdata.nchannels;
			cspl.flags = sdata.flags;
			cspl.loop = sdata.loop;
			cspl.loops = sdata.loops;
			cspl.loope = sdata.loope;
			tmp_vol[x] = sdata.volume;
			tmp_panx[x] = sdata.panning;
			cspl.note = sdata.note+11;
			if (sdata.spb>0) cspl.bpm = 60.0*cspl.frequency/sdata.spb;
			cspl.sl = cspl.depth*cspl.nchannels;
			cspl.ns = cspl.length/cspl.sl;
		};
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
// Groups
	tmpc = (char*)si->memalloc(8,0);
	for (x=1;x<=header.ninstr;x++){
		MTInstrument &cinstr = *A(module.instr,MTInstrument)[x];
		if (&cinstr){
			for (y=0;y<nispl[x];y++){
				_MT2Group gdata;
				f->read(&gdata,sizeof(gdata));
				if (y<MAX_GRPS){
					cinstr.grp[y].spl = A(module.spl,Oscillator)[gdata.splid];
					cinstr.grp[y].vol = (float)gdata.vol/128;
					cinstr.grp[y].pitch = gdata.pitch;
				};
			};
		}
		else f->seek(nispl[x]*sizeof(_MT2Group),MTF_CURRENT);
	};
	si->memfree(tmpc);
// Instruments <-> Samples
	for (x=2;x<256;x++){
		MTInstrument &cinstr = *A(module.instr,MTInstrument)[x];
		if ((&cinstr) && (cinstr.name[0]==0)){
			if ((nispl[x]==0) || ((nispl[x]==1) && (cinstr.grp[0].spl==0))){
				delete &cinstr;
				module.instr->a[x] = 0;
			};
		};
	};
	for (x=1;x<256;x++){
		MTInstrument &cinstr = *A(module.instr,MTInstrument)[x];
		if (!&cinstr) continue;
#		ifdef _DEBUG
//<TEST>
			char *e = strstr(cinstr.name,"VST ");
			if (e){
				oi->newobject(MTO_INSTRUMENT+1,&module,x,e+4,false,true);
				delete &cinstr;
				continue;
			};
//</TEST>
#		endif
		for (y=0;y<96;y++){
			Oscillator &cspl = *cinstr.grp[cinstr.range[0][y]].spl;
			if (!&cspl) continue;
			cinstr.range[1][y] = tmp_vol[cspl.id]/256;
			cinstr.range[2][y] = tmp_panx[cspl.id];
		};
	};
	if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
// Samples data
	tmpc2 = strrchr(filename,'/');
	if (!tmpc2)	tmpc2 = strrchr(filename,'\\');
	if (tmpc2) *(tmpc2+1) = 0;
	for (x=0;x<header.nspl;x++){
		if (module.spl->a[x]){
			MTSample &cspl = *A(module.spl,MTSample)[x];
			switch (cspl.flags & 0x5){
			case 0:
				if (cspl.length){
					y = cspl.length;
					cspl.loadfromstream(f,y,0);
					f->seek(y,MTF_CURRENT);
				};
				break;
			case 1:
			case 4:
				tmpc = (char*)si->memalloc(512,MTM_ZERO);
				f->read(&size,4);
				f->seek(12,MTF_CURRENT);
				f->read(tmpc,size);
				if (tmpc[1]!=':'){
					tmpl = strlen(filename)+strlen(tmpc)+1;
					cspl.filename = (char*)si->memalloc(tmpl,0);
					strcpy(cspl.filename,filename);
					strcat(cspl.filename,tmpc);
				}
				else{
					cspl.filename = (char*)si->memalloc(strlen(tmpc)+1,0);
					strcpy(cspl.filename,tmpc);
				};
				si->memfree(tmpc);
/*
				while (!si->fileexists(cspl.filename)){
					// Searching for file...
				};
*/
				if (si->fileexists(cspl.filename)){
				/*      MTFile *sf = si->fileopen(cspl.filename,MTF_READ|MTF_SHAREREAD);
				if (sf){
				if (sf->length()>1048576){
				if (longsamples>0) y = mrYes;
				else if (longsamples<0) y = mrNo;
				else y = mtfmessage(0,(char*)S_LONGSAMPLE,smtv,MB_ICONQUESTION|MB_YESNO,strrchr(cspl.sdname,'\\')+1);
				if (y==mrYes){
				cspl.flags &= ~SF_KEEPONDISK;
				cspl.flags |= SF_DIRECT2DISK;
				};
				};
				si->fileclose(sf);
				};
					cspl.load(0);*/
				};
			};
		};
		if (p) p->setprogress((float)f->seek(0,MTF_CURRENT)/max);
	};
	si->fileclose(f);
	return true;
abort:
error:
	si->fileclose(f);
	return false;
}

bool infoMT2(MTMiniConfig *data,char *filename,void *process)
{
	MTFile *f;
	_MT2Header header;
	_MT2DrumsData drums;
	char *e;
	int x,y,ndtracks;
	mt_uint32 incl,orig,tmpl,size,csize;
	mt_uint32 nvst = 0;
	double tmpd;
	double pattbeats[256];

	if ((f = si->fileopen(filename,MTF_READ|MTF_SHAREREAD))==0) return false;
	mtmemzero(pattbeats,sizeof(pattbeats));
	f->read(&header,sizeof(header));
	if (strncmp(header.id,"MT20",4)) goto error;
	if (header.title[0]!=0) e = header.title;
	else{
		e = strrchr(filename,'/');
		if (!e) e = strrchr(filename,'\\');
		if (e) e++;
		else e = filename;
	};
	data->setparameter("title",e,MTCT_STRING,-1);
	tmpd = 44100.0*60.0/(header.ticks*header.lpb*header.spt);
	data->setparameter("bpm",&tmpd,MTCT_FLOAT,sizeof(tmpd));
	if (header.ddl){
		ndtracks = 8;
		f->read(&drums,sizeof(drums));
	}
	else{
		ndtracks = 0;
		mtmemzero(&drums,sizeof(drums));
	};
	f->read(&size,4);
	incl = 0;
	while (incl<size){
		f->read(&tmpl,4);
		f->read(&csize,4);
		orig = f->pos();
		incl += csize+8;
		switch (tmpl){
		case FOURCC('B','P','M','+'):
			f->read(&tmpd,8);
			tmpd = 44100.0*60.0/(header.ticks*header.lpb*tmpd);
			data->setparameter("bpm",&tmpd,MTCT_FLOAT,sizeof(tmpd));
			break;
		case FOURCC('V','S','T','2'):
			f->read(&nvst,4);
			break;
		};
		f->seek(orig+csize,MTF_BEGIN);
	};	
	for (x=0;x<header.npatts;x++){
		tmpl = 0;
		f->read(&tmpl,2);
		pattbeats[x] = (double)tmpl/header.lpb;
		f->read(&size,4);
		if (size & 1) size++;
		f->seek(size,MTF_CURRENT);
	};
	for (x=0;x<drums.ndpatts;x++){
		tmpl = 0;
		f->read(&tmpl,2);
		f->seek(tmpl*8*4,MTF_CURRENT);
	};
	if (header.flags & MF_AUTOMATION){
		for (x=0;x<header.npatts;x++){
			y = 0;
			while (true){
				if (y>MAX_TRACKS+1){
					_MT2Automation203 cauto;
					f->read(&cauto,sizeof(cauto));
					if (cauto.flags){
						tmpl = cauto.flags;
						while (tmpl!=0){
							_MT2EnvPoint tmpp[64];
							if (tmpl & 1){
								f->seek(4+sizeof(_MT2EnvPoint)*64,MTF_CURRENT);
							};
							tmpl >>= 1;
						};
					};
					goto autoskip;
				};
				if (header.version>=0x203){
					_MT2Automation203 cauto;
					f->read(&cauto,sizeof(cauto));
					tmpl = cauto.flags;
				}
				else{
					_MT2Automation cauto;
					f->read(&cauto,sizeof(cauto));
					tmpl = cauto.flags;
				};
				while (tmpl!=0){
					_MT2EnvPoint tmpp[64];
					if (tmpl & 1){
						f->seek(4+sizeof(_MT2EnvPoint)*64,MTF_CURRENT);
					};
					tmpl >>= 1;
				};
autoskip:
				y++;
				if (y==header.ntracks+ndtracks){
					y = MAX_TRACKS;
				}
				else if (y==MAX_TRACKS+1){
					if (header.version<0x250) break;
				};
				if (y>=MAX_TRACKS+1+nvst) break;
			};
		};
	};
	tmpd = 0.0;
	for (x=0;x<header.npos;x++){
		tmpd += pattbeats[header.pl[x]];
	};
	data->setparameter("beats",&tmpd,MTCT_FLOAT,sizeof(tmpd));
	si->fileclose(f);
	return true;
error:
	si->fileclose(f);
	return false;
}
//---------------------------------------------------------------------------
