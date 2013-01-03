//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXNode.h 107 2005-11-30 21:30:55Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXNODE_INCLUDED
#define MTXNODE_INCLUDED

#define MAX_CONNECTIONS 32
//---------------------------------------------------------------------------
struct Pin;
class Node;
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
extern MTSystemInterface *si;
//---------------------------------------------------------------------------
struct Pin{
	unsigned short s;	// Source pin
	unsigned short d;	// Destination pin
	float m;					// Volume (multiplier)
	Node *n;					// Connected node
};

class Node : public MTObject{
public:
	int ninputs,noutputs;
	Pin outputs[MAX_CONNECTIONS];
	MTLock *_lock;

	Node(MTObject *parent,mt_uint32 type,mt_int32 i):MTObject(parent,type,i){ ninputs = noutputs = 2; mtmemzero(outputs,sizeof(outputs)); _lock = si->lockcreate(); };
	virtual ~Node(){ si->lockdelete(_lock); };
	virtual int MTCT process(int ooffset,int ioffset,int count,bool &silence) = 0;
	virtual void MTCT preprocess(int count){  };
	virtual void MTCT postprocess(int count){  };
	virtual void MTCT prebuffer(int count){  };
	virtual void MTCT postbuffer(int count){  };
	void setoutput(Node *dest,float m){
		int x,n = noutputs;
		if (dest->ninputs<n) n = dest->ninputs;
		for (x=0;x<n;x++){
			outputs[x].s = x;
			outputs[x].d = x;
			outputs[x].m = m;
			outputs[x].n = dest;
		};
		for (;x<noutputs;x++){
			outputs[x].n = 0;
		};
	};
};
//---------------------------------------------------------------------------
#endif
