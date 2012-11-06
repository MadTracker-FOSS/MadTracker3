//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTDSP1.cpp 106 2005-11-30 21:08:42Z Yannick $
//
//---------------------------------------------------------------------------
#include <math.h>
#include "MTDSP1.h"
#include "MTShaper.h"
#include "MTBufferASM.h"
#include "MTFilterASM.h"
#include "MTResamplingASM.h"
#include "MTCatmullASM.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
static const char *dspname = {"MadTracker DSP"};
static const int dspversion = 0x30000;
static const MTXKey dspkey = {0,0,0,0};
#ifndef MTBUILTIN
	MTXInterfaces i;
	MTDSPInterface *dspi;
	MTInterface *mtinterface;
	MTSystemInterface *si;
#endif
MTResampleProc proc[255];
//---------------------------------------------------------------------------
MTDSPInterface::MTDSPInterface()
{
	type = dsptype;
	key = &dspkey;
	name = dspname;
	version = dspversion;
	status = 0;
	mtmemzero(proc,sizeof(proc));
}

bool MTDSPInterface::init()
{
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	if (!si) return false;
	ENTER("MTDSPInterface::init");
	LOGD("%s - [DSP] Initializing..."NL);
	emptybuffer = a_emptybuffer;
	replacebuffer = a_replacebuffer;
	replacebuffermul = a_replacebuffermul;
	addbuffer = a_addbuffer;
	addbuffermul = a_addbuffermul;
	addbuffermul2 = a_addbuffermul2;
	addbufferslide = a_addbufferslide;
	addbufferslide2 = a_addbufferslide2;
	ampbuffer = a_ampbuffer;
	modulatebuffer = a_modulatebuffer;
	filter[0x0] = a_filter_;
	filter[0x1] = a_filter_;
	filter[0x2] = a_filter_;
	filter[0x3] = a_filter_;
	filter[0x4] = a_filter_;
	filter[0x5] = a_filter_;
	filter[0x6] = a_filter_;
	filter[0x7] = a_filter_;
	filter[0x8] = a_filter_;
	filter[0x9] = a_filter_;
	filter[0xA] = a_filter_;
	filter[0xB] = a_filter_;
	filter[0xC] = a_filter_;
	filter[0xD] = a_filter_;
	filter[0xE] = a_filter_;
	filter[0xF] = a_filter_;
	resample[0x00] = (MTResampleProc)a_resample_linear_8;
	resample[0x01] = (MTResampleProc)a_resample_linear_16;
	resample[0x02] = (MTResampleProc)a_resample_linear_32;
	resample[0x03] = (MTResampleProc)a_resample_linear_32;
	resample[0x04] = (MTResampleProc)a_resample_linear_8;
	resample[0x05] = (MTResampleProc)a_resample_linear_16;
	resample[0x06] = (MTResampleProc)a_resample_linear_32;
	resample[0x07] = (MTResampleProc)a_resample_linear_32;
	resample[0x08] = (MTResampleProc)a_resample_linear_8;
	resample[0x09] = (MTResampleProc)a_resample_linear_16;
	resample[0x0A] = (MTResampleProc)a_resample_linear_32;
	resample[0x0B] = (MTResampleProc)a_resample_linear_32;
	resample[0x0C] = (MTResampleProc)a_resample_linear_8;
	resample[0x0D] = (MTResampleProc)a_resample_linear_16;
	resample[0x0E] = (MTResampleProc)a_resample_linear_32;
	resample[0x0F] = (MTResampleProc)a_resample_linear_32;
	resample[0x10] = (MTResampleProc)a_resample_linear_8;
	resample[0x11] = (MTResampleProc)a_resample_linear_16;
	resample[0x12] = (MTResampleProc)a_resample_linear_32;
	resample[0x13] = (MTResampleProc)a_resample_linear_32;
	resample[0x14] = (MTResampleProc)a_resample_linear_8;
	resample[0x15] = (MTResampleProc)a_resample_linear_16;
	resample[0x16] = (MTResampleProc)a_resample_linear_32;
	resample[0x17] = (MTResampleProc)a_resample_linear_32;
	resample[0x18] = (MTResampleProc)a_resample_linear_8;
	resample[0x19] = (MTResampleProc)a_resample_linear_16;
	resample[0x1A] = (MTResampleProc)a_resample_linear_32;
	resample[0x1B] = (MTResampleProc)a_resample_linear_32;
	resample[0x1C] = (MTResampleProc)a_resample_linear_8;
	resample[0x1D] = (MTResampleProc)a_resample_linear_16;
	resample[0x1E] = (MTResampleProc)a_resample_linear_32;
	resample[0x1F] = (MTResampleProc)a_resample_linear_32;
	splinereplace = a_splinereplace;
	splinemodulate = a_splinemodulate;
#	ifdef _DEBUG
		char file[256];
		MTFile *f;
		MTShaper s;
		MTShape *sh;
		sample *p;
		int x;
		short v;
		s.add(0,64,0.5,192,0.75);
		s.add(1,0,0.0,64,1.0,128,0.0,256,1.0);
		sh = s.get(0,256,MTSHAPE_BUFFER);
		strcpy(file,mtinterface->getprefs()->syspath[SP_ROOT]);
		strcat(file,"Spline.raw");
		f = si->fileopen(file,MTF_WRITE|MTF_SHAREREAD|MTF_CREATE);
		if (f){
			p = sh->data;
			for (x=sh->x1;x<sh->x2;x++){
				v = (short)(32767*(*p++));
				f->write(&v,sizeof(v));
			};
			si->fileclose(f);
		};
		s.del(sh);
#	endif
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTDSPInterface::uninit()
{
	ENTER("MTDSPInterface::uninit");
	LOGD("%s - [DSP] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	mtmemzero(proc,sizeof(proc));
	LEAVE();
}

void MTDSPInterface::start()
{
}

void MTDSPInterface::stop()
{
}

void MTDSPInterface::processcmdline(void *params)
{

}

void MTDSPInterface::showusage(void *out)
{

}

int MTDSPInterface::config(int command,int param)
{
	return 0;
}
//---------------------------------------------------------------------------
#ifndef MTBUILTIN
extern "C"
{

MTEXPORT MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!dspi) dspi = new MTDSPInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)dspi;
	return &i;
}

}
#endif
//---------------------------------------------------------------------------
