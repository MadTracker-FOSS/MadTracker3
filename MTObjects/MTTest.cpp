//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTTest.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTModule.h"
#include "MTDelay.h"
#include "MTGenerator.h"
//---------------------------------------------------------------------------
#ifdef _DEBUG
MTModule *m;
//---------------------------------------------------------------------------
void _test_start()
{
	MTPattern *p;
	MTDelay *d;
	MTInstrument *i1,*i2;
	MTSample *s;
	MTGenerator *g;
	unsigned char *patt;

	return;
	m = (MTModule*)oi->newobject(MTO_MODULE,0,0,0);
	p = (MTPattern*)oi->newobject(MTO_MTPATTERN,m,0,0);
	d = (MTDelay*)oi->newobject(MTO_MTDELAY,m,0,0);
	i1 = (MTInstrument*)oi->newobject(MTO_MTINSTRUMENT,m,1,0);
	i2 = (MTInstrument*)oi->newobject(MTO_MTINSTRUMENT,m,2,0);
	s = (MTSample*)oi->newobject(MTO_MTSAMPLE,m,0,0);
	g = (MTGenerator*)oi->newobject(MTO_MTGENERATOR,m,1,0);
// Module settings
	m->sequ[1][0].patt = 0;
	m->sequ[1][0].pos = 0.0;
	m->sequ[1][0].length = p->nbeats;
	m->nsequ[1] = 1;
	m->loope = p->nbeats;
	m->setstatus();
	m->playstatus.length = m->loope;
// Pattern settings
	patt = p->data;
	patt[0] = 58;
	patt[1] = 1;
	patt += p->linesize*4;
	patt[0] = 97;
	patt += p->linesize*4;
	patt[0] = 52;
	patt[1] = 1;
	patt += p->linesize*4;
	patt[0] = 97;
	patt += p->linesize*4;
	patt[0] = 61;
	patt[1] = 1;
	patt += p->linesize*4;
	patt[0] = 97;
// Delay settings
	A(m->trk,Node)[0]->setoutput(d,1.0);
	d->setparam(0,0,1.0);
	d->setparam(1,0,DF_BEATS);
	d->setparam(1,1,0.25);
	d->setparam(1,2,0.75);
	d->setparam(1,3,0.25);
// Generator settings
//	oi->loadobject(s,"Pulse_440.wav");
	oi->loadobject(s,"Square_440.wav");
//	oi->loadobject(s,"Debug.wav");
// Instrument settings
	i1->addoscillator(s);
	i2->addoscillator(g);

	mtinterface->addmodule(m);
	mtinterface->editobject(m,false);
}

void _test_stop()
{
	if (!m) return;
	mtinterface->delmodule(m);
	oi->deleteobject(m);
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
