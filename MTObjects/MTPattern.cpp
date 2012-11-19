//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattern.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "MTPattern.h"
#include "MTPattEditor.h"
#include "MTColumns.h"
#include "MTObjectsASM.h"
#include "../Interface/MTObjectsRES.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
PatternType *patterntype;
//---------------------------------------------------------------------------
// Pattern functions
//---------------------------------------------------------------------------
Pattern::~Pattern()
{
	int x,y;
	
	if (parent){
		MTModule &cmodule = *module;
		cmodule.patt->a[id] = 0;
		for (y=0;y<MAX_LAYERS;y++){
			for (x=0;x<cmodule.nsequ[y];x++){
				if (cmodule.sequ[y][x].patt==id){
					cmodule.sequ[y][x].patt = -1;
					if (x==cmodule.nsequ[y]-1) cmodule.nsequ[y]--;
				};
			};
		};
	};
}
//---------------------------------------------------------------------------
PatternType::PatternType()
{
	type = MTO_MTPATTERN;
	description = "Pattern";
	columns = si->arraycreate(4,0);
}

PatternType::~PatternType()
{
	si->arraydelete(columns);
}

MTObject* PatternType::create(MTObject *parent,mt_int32 id,void *param)
{
	return new MTPattern(parent,id);
}

bool PatternType::registercolumn(Column *column)
{
	column->id = columns->push(column);
	return true;
}

void PatternType::unregistercolumn(Column *column)
{
	int x;

	columns->remove(column);
	for (x=0;x<columns->nitems;x++){
		((Column*)columns->a[x])->id = x;
	};
}

Column* PatternType::gethandler(const char *desc)
{
	int x;

	for (x=0;x<columns->nitems;x++){
		Column *c = (Column*)columns->a[x];
		if (strcmp(c->description,desc)==0) return c;
	};
	return 0;
}

int PatternType::getksgroup()
{
#	ifdef MTOBJECTS_EDITORS
		return peksgroup;
#	else
		return 0;
#	endif
}
//---------------------------------------------------------------------------
MTPattern::MTPattern(MTObject *parent,mt_int32 i):
Pattern(parent,MTO_MTPATTERN,i),
lpb(4),
ticks(6),
ntracks(4),
nlines(64),
linesize(0)
{
	int x,y;
	MTConfigFile *conf;
	bool config = false;
	char *id;
	Column *c;
	char parameter[16];
	char value[256];
	static char *columns[] = {"Note","Volume","Panning","Effect"};

	mtmemzero(tracks,sizeof(tracks));
#	ifdef MTSYSTEM_CONFIG
		if ((conf = (MTConfigFile*)mtinterface->getconf("Global",true))){
			if (conf->setsection("MTObjects")){
				conf->getparameter("PatternTracks",&ntracks,MTCT_UINTEGER,sizeof(ntracks));
				conf->getparameter("PatternLines",&nlines,MTCT_UINTEGER,sizeof(nlines));
				conf->getparameter("PatternLPB",&lpb,MTCT_UINTEGER,sizeof(lpb));
				conf->getparameter("PatternTicks",&ticks,MTCT_UINTEGER,sizeof(ticks));
				strcpy(parameter,"PatternColumn");
				id = strchr(parameter,0);
				for (x=0;x<MAX_PATT_COLS;x++){
					sprintf(id,"%02X",x);
					if (!conf->getparameter(parameter,value,MTCT_STRING,sizeof(value))) break;
					c = patterntype->gethandler(value);
					if (c){
						config = true;
						for (y=0;y<ntracks;y++){
							tracks[y].cols[x].celloffset = tracks[y].colsize;
							tracks[y].cols[x].handler = c;
							tracks[y].ncolumns++;
							tracks[y].colsize += c->nbytes;
							linesize += c->nbytes;
						};
					};
				};
			};
			mtinterface->releaseconf(conf);
		};
#	endif
	if (!config){
		for (x=0;x<sizeof(columns)/sizeof(char*);x++){
			c = patterntype->gethandler(columns[x]);
			if (c){
				for (y=0;y<ntracks;y++){
					tracks[y].cols[x].celloffset = tracks[y].colsize;
					tracks[y].cols[x].handler = c;
					tracks[y].ncolumns++;
					tracks[y].colsize += c->nbytes;
					linesize += c->nbytes;
				};
			};
		};
	};
	data = (unsigned char*)si->memalloc(linesize*nlines,MTM_ZERO);
#	ifdef MTSYSTEM_RESOURCES
		res->loadstringf(MTT_pattern,name,255,i);
#	endif
	for (x=0;x<MAX_PATT_TRACKS;x++){
		tracks[x].id = x;
		tracks[x].on = true;
	};
}

MTPattern::~MTPattern()
{
	if (data) si->memfree(data);
}

int MTPattern::loadfromstream(MTFile *f,int size,void *params)
{
	return 0;
}

int MTPattern::savetostream(MTFile *f,void *params)
{
	return 0;
}

PatternInstance* MTPattern::createinstance(int layer,Sequence *sequ,PatternInstance *previous)
{
	return new MTPatternInstance(this,sequ,layer,previous);
}

void MTPattern::change(int nlines,int ntracks,int ncols,char **columns,bool preserve)
{
	int x,y;
	Column *c;

	if (preserve){

	};
	this->ntracks = ntracks;
	this->nlines = nlines;
	linesize = 0;
	for (x=0;x<ntracks;x++){
		TrackInfo &ti = tracks[x];
		ti.ncolumns = 0;
		ti.colsize = 0;
		for (y=0;y<ncols;y++){
			ColInfo &ci = ti.cols[ti.ncolumns];
			c = patterntype->gethandler(columns[y]);
			if (c){
				ci.celloffset = ti.colsize;
				ci.handler = c;
				ti.colsize += c->nbytes;
				ti.ncolumns++;
			};
		};
		linesize += ti.colsize;
	};
	if (data) si->memfree(data);
	data = (unsigned char*)si->memalloc(linesize*nlines,MTM_ZERO);
}

void MTPattern::setcolumns(int track,char **columns,bool preserve)
{
	if (preserve){

	};
}
//---------------------------------------------------------------------------
MTPatternInstance::MTPatternInstance(Pattern *p,Sequence *s,int l,PatternInstance *previous):
PatternInstance(p,s,l,previous),
nticks(((MTPattern*)p)->ticks),
cline(0),
ctick(0),
lastline(-1)
{
	int t,x,y,n;
	MTPattern *mtp = (MTPattern*)p;
	char colmap[MAX_PATT_TRACKS][MAX_PATT_COLS];

	mtmemzero(cols,sizeof(cols));
	for (x=0;x<mtp->ntracks;x++){
		cols[x] = (ColumnStatus**)si->memalloc(sizeof(Column*)*mtp->tracks[x].ncolumns,0);
		for (y=0;y<mtp->tracks[x].ncolumns;y++){
			cols[x][y] = (ColumnStatus*)si->memalloc(sizeof(ColumnStatus)-1+mtp->tracks[x].cols[y].handler->ndata,MTM_ZERO);
		};
	};
	if ((previous) && (previous->parent->objecttype==parent->objecttype)){
		MTPatternInstance &cprevious = *(MTPatternInstance*)previous;
		MTPattern &cparent = *(MTPattern*)cprevious.parent;
		if (p->flags & MTPF_INHERIT_TICKS) nticks = cprevious.nticks;
		n = cparent.ntracks;
		if (mtp->ntracks<n) n = mtp->ntracks;
		memset(colmap,-1,sizeof(colmap));
		for (t=0;t<n;t++){
			for (x=0;x<cparent.tracks[t].ncolumns;x++){
				int id = cparent.tracks[t].cols[x].handler->id;
				for (y=0;y<mtp->tracks[t].ncolumns;y++){
					if (mtp->tracks[t].cols[y].handler->id==id){
						colmap[t][x] = (unsigned char)y;
						break;
					};
				};
			};
		};
		for (x=0;x<n;x++){
			NoteData *nd = (NoteData*)cprevious.cols[x][0]->data;
			for (y=0;y<nd->ninstances;y++){
				if (nd->lastinstance[y]) nd->lastinstance[y]->changecaller(this);
			};
			for (y=0;y<cparent.tracks[x].ncolumns;y++){
				if (colmap[x][y]<0) continue;
				memcpy(cols[x][colmap[x][y]]->data,cprevious.cols[x][y]->data,cparent.tracks[x].cols[y].handler->ndata);
				mtp->tracks[x].cols[colmap[x][y]].handler->init(this,*cols[x][colmap[x][y]]);
			};
		};
		for (;x<cparent.ntracks;x++){
			NoteData *nd = (NoteData*)cprevious.cols[x][0]->data;
			for (y=0;y<nd->ninstances;y++){
				if (nd->lastinstance[y]) nd->lastinstance[y]->changecaller(0);
			};
		};
	};
}

MTPatternInstance::~MTPatternInstance()
{
	int x,y;
	MTPattern *mtp = (MTPattern*)parent;

	for (x=0;x<mtp->ntracks;x++){
		for (y=0;y<mtp->tracks[x].ncolumns;y++){
			si->memfree(cols[x][y]);
		};
		si->memfree(cols[x]);
	};
}

void MTPatternInstance::processevents()
{
	int x,y;
	double inc,tmp;
	unsigned char *celldata;
	MTPattern &cparent = *(MTPattern*)parent;
	FirstPass pass;

	ctick = (int)floor(cpos*(double)(cparent.lpb*nticks)+0.0001);
	cline = ctick/nticks;
	ctick %= nticks;
	inc = 1.0;
	celldata = cparent.data+cline*cparent.linesize;
	if (lastline!=cline){
		for (x=0;x<cparent.ntracks;x++){
			for (y=0;y<cparent.tracks[x].ncolumns;y++){
				cols[x][y]->cpos = 0.0;
				cols[x][y]->nextevent = 0.0;
			};
		};
/*	Precision check
	}
	else if (ctick==0){
		FLOG2("oops %d %d"NL,cline,ctick);
*/
	};
	lastline = cline;
	for (ctrack=0;ctrack<cparent.ntracks;ctrack++){
		TrackInfo &ti = cparent.tracks[ctrack];
		if (ti.on){
			Track &ctrk = *A(module->trk,Track)[ti.id];
			mtmemzero(&pass,sizeof(pass));
			pass.pitch = 1.0;
			pass.gvolume = A(module->master,Track)[0]->vol*ctrk.vol;
			pass.gpanx = ctrk.panx;
			pass.gpany = ctrk.pany;
			pass.gpanz = ctrk.panz;
			pass.volume = -1.0;
			pass.panx = pass.pany = pass.panz = -2.0;
			for (y=0;y<ti.ncolumns;y++){
				ColumnStatus &cstatus = *cols[ctrack][y];
				if (cstatus.nextevent<=cstatus.cpos){
					ti.cols[y].handler->firstpass(this,celldata+ti.cols[y].celloffset,pass,cstatus,ctick,nticks);
					if (cstatus.nextevent==-2.0) nextevent = -2.0;
				};
			};
			if (nextevent==-2.0) return;
			if (pass.delay>0.0){
				for (y=0;y<ti.ncolumns;y++){
					ColumnStatus &cstatus = *cols[ctrack][y];
					cstatus.nextevent += pass.delay;
				};
			};
			for (y=0;y<ti.ncolumns;y++){
				ColumnStatus &cstatus = *cols[ctrack][y];
				if (cstatus.nextevent<=cstatus.cpos){
					ti.cols[y].handler->columnhandle(this,celldata+ti.cols[y].celloffset,pass,cstatus,ctick,nticks);
				};
				tmp = cstatus.nextevent-cstatus.cpos;
				if ((tmp>0.0) && (tmp<inc)) inc = tmp;
			};
		};
		celldata += ti.colsize;
	};
	for (x=0;x<cparent.ntracks;x++){
		for (y=0;y<cparent.tracks[x].ncolumns;y++){
			cols[x][y]->cpos += inc;
		};
	};
	nextevent += inc/(double)cparent.lpb;
}

bool MTPatternInstance::seek(double offset,bool start)
{
	if (start){
		cpos = offset;
	}
	else{
		cpos += offset;
	};
	nextevent = cpos;
	return true;
}

void MTPatternInstance::delinstance(InstrumentInstance *i)
{
	int x,y;
	
	for (x=0;x<((MTPattern*)parent)->ntracks;x++){
		NoteData *nd = (NoteData*)cols[x][0]->data;
		for (y=0;y<nd->ninstances;y++){
			if (nd->lastinstance[y]==i){
				for (;y<nd->ninstances-1;y++) nd->lastinstance[y] = nd->lastinstance[y+1];
				nd->lastinstance[--nd->ninstances] = 0;
				return;
			};
		};
	};
}

ColumnStatus* MTPatternInstance::getnotestatus()
{
	if (cols[ctrack]) return cols[ctrack][0];
	else return 0;
}

unsigned char* MTPatternInstance::getnotedata()
{
	MTPattern &cparent = *(MTPattern*)parent;
	return cparent.data+cline*cparent.linesize+ctrack*cparent.tracks[ctrack].colsize;
}

bool MTPatternInstance::sendevents(int nevents,MTIEvent **events)
{
	int x;
	bool res = false;

	NoteData *nd = (NoteData*)&cols[ctrack][0]->data;
	for (x=0;x<nd->ninstances;x++){
		InstrumentInstance &ci = *nd->lastinstance[x];
		if (ci.flags & IIF_BACKGROUND) continue;
		ci.sendevents(nevents,events);
		res = true;
	};
	return res;
}
//---------------------------------------------------------------------------
