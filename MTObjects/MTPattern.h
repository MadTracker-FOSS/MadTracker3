//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattern.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTPATTERN_INCLUDED
#define MTPATTERN_INCLUDED

#define MAX_PATT_TRACKS 64
#define MAX_PATT_COLS   32

class Pattern;
class PatternInstance;

#include "MTObjects1.h"
#include "MTInstrument.h"
#include "MTModule.h"
//---------------------------------------------------------------------------
// Pattern classes
//---------------------------------------------------------------------------
class Pattern : public MTObject{
public:
	double nbeats;
	
	Pattern(MTObject *parent,mt_uint32 type,mt_int32 i):MTObject(parent,type,i){
		nbeats = 8.0;
	};
	virtual ~Pattern();
	
	virtual PatternInstance* MTCT createinstance(int layer,Sequence *sequ,PatternInstance *previous) = 0;
};

class PatternInstance{
public:
	int id;
	MTModule *module;
	Pattern *parent;
	Sequence *sequ;
	int layer;
	double cpos;
	double nextevent;
	
	PatternInstance(Pattern *p,Sequence *s,int l,PatternInstance *previous){
		module = p->module; parent = p; sequ = s; layer = l; cpos = nextevent = 0.0;
	};
	virtual ~PatternInstance(){	};

	virtual void MTCT processevents() = 0;
	virtual bool MTCT seek(double offset,bool start) = 0;
	virtual void MTCT delinstance(InstrumentInstance *i) = 0;
};
//---------------------------------------------------------------------------
// MadTracker Pattern
//---------------------------------------------------------------------------
#include "MTPattManager.h"
//---------------------------------------------------------------------------
class MTPatternInstance;

enum{
	CT_NOTE = 0,
	CT_EFFECT,
	CT_OTHER
};

#define MTFP_ISNOTE    1
#define MTFP_ISINS     2
#define MTFP_PROCESSED 16

struct FirstPass{
	double delay;
	double gvolume;
	double volume;
	double pitch;
	float gpanx,gpany,gpanz;
	float panx,pany,panz;
	int flags;
};

struct ColumnStatus{
	double cpos;
	double nextevent;
	int datasize;
	char data[1];
};

#define CDS_SELECTED  0x01
#define CDS_STYLE1    0x00
#define CDS_STYLE2    0x10
#define CDS_STYLE3    0x20
#define CDS_STYLE4    0x30
#define CDS_STYLEMASK 0x70

struct ColumnDrawState{
	int flags;
	int line;
	int cursor;
	int lpb;
};

class Column{
public:
	int id;
	char *description;
	unsigned char type,nbytes,ndata,ncpos;
	virtual void MTCT init(MTPatternInstance*,ColumnStatus &status) = 0;
	virtual void MTCT firstpass(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks) = 0;
	virtual void MTCT columnhandle(MTPatternInstance*,unsigned char *celldata,FirstPass &pass,ColumnStatus &status,int tick,int nticks) = 0;
	virtual int MTCT getwidth(int charwidth) = 0;
	virtual void MTCT drawcolumn(MTBitmap*,MTRect&,unsigned char *celldata,ColumnDrawState &state) = 0;
	virtual void MTCT onmessage(MTPattManager *pm,MTCMessage&,int cursor,unsigned char *celldata) = 0;
};

class PatternType : public ObjectType{
public:
	MTArray *columns;

	PatternType();
	~PatternType();
	MTObject* MTCT create(MTObject *parent,mt_int32 id,void *param);
	virtual bool MTCT registercolumn(Column *column);
	virtual void MTCT unregistercolumn(Column *column);
	virtual Column* MTCT gethandler(const char *desc);
	virtual int MTCT getksgroup();
};

enum{
	PP_MT3 = 0,
	PP_MT2RAW,
	PP_MT2COMP,
	PP_MT2DRUMS,
	PP_XM,
	PP_IT
};

struct ColInfo{
	int celloffset;
	Column *handler;
};

struct TrackInfo{
	unsigned char id;
	bool on;
	bool solo;
	bool rec;
	unsigned short ncolumns;
	unsigned short colsize;
	ColInfo cols[MAX_PATT_COLS];
};

#define MTPF_INHERIT_TICKS 1

class MTPattern : public Pattern{
public:
	MTPattern(MTObject *parent,mt_int32 i);
	~MTPattern();
	
	unsigned char lpb,ticks;
	unsigned char ntracks,res1;
	unsigned short nlines,linesize;
	unsigned char *data;
	TrackInfo tracks[MAX_PATT_TRACKS];
	int MTCT loadfromstream(MTFile *f,int size,void *params);
	int MTCT savetostream(MTFile *f,void *params);
	PatternInstance* MTCT createinstance(int layer,Sequence *sequ,PatternInstance *previous);
	void MTCT change(int nlines,int ntracks,int ncols,char **columns,bool preserve);
	void MTCT setcolumns(int track,char **columns,bool preserve);
};

class MTPatternInstance : public PatternInstance{
public:
	int nticks;
	int cline,ctick,ctrack;
	ColumnStatus **cols[MAX_PATT_TRACKS];
	
	MTPatternInstance(Pattern *p,Sequence *s,int l,PatternInstance *previous);
	~MTPatternInstance();	

	void MTCT processevents();
	bool MTCT seek(double offset,bool start);
	void MTCT delinstance(InstrumentInstance *i);
	virtual ColumnStatus* MTCT getnotestatus();
	virtual unsigned char* MTCT getnotedata();
	virtual bool MTCT sendevents(int nevents,MTIEvent **events);
private:
	int lastline;
};
//---------------------------------------------------------------------------
extern PatternType *patterntype;
//---------------------------------------------------------------------------
#endif
