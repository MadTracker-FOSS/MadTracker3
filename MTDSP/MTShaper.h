//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTShaper.h 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSHAPER_INCLUDED
#define MTSHAPER_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
//---------------------------------------------------------------------------
#define SHAPER_LAYERS 16
//---------------------------------------------------------------------------
#define MTSHAPE_EMPTY  0
#define MTSHAPE_FLAT   0x00000001
#define MTSHAPE_LINEAR 0x00000002
#define MTSHAPE_SECOND 0x00000004
#define MTSHAPE_THIRD  0x00000008
#define MTSHAPE_SPLINE 0x00000010
#define MTSHAPE_BUFFER 0x80000000
//---------------------------------------------------------------------------
struct MTShape
{
	int type;
	sample *data;
	int x1;
	sample y1;
	int x2;
	sample y2;
	int x3;
	sample y3;
	int x4;
	sample y4;
};

class MTShaper{
public:
	MTShaper();
	virtual ~MTShaper();

	virtual void MTCT add(int layer,int x1,int x2,sample y);
	virtual void MTCT add(int layer,int x1,sample y1,int x2,sample y2);
	virtual void MTCT add(int layer,int x1,sample y1,int x2,sample y2,int x3,sample y3,int x4,sample y4);
	virtual void MTCT add(int layer,int x1,int x2,sample *data);
	virtual MTShape* MTCT get(int from,int to,int accept);
	virtual void MTCT del(MTShape *s);
	virtual void MTCT flush(int to);
private:
	MTShape l[SHAPER_LAYERS];
};
//---------------------------------------------------------------------------
#endif
