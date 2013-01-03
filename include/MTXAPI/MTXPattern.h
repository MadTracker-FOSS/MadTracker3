//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXPattern.h 98 2005-11-30 20:07:26Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXPATTERN_INCLUDED
#define MTXPATTERN_INCLUDED
//---------------------------------------------------------------------------
class Pattern;
class PatternInstance;
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXModule.h"
#include "MTXInstrument.h"
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
#endif
