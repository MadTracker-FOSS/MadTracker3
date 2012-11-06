//---------------------------------------------------------------------------
#ifndef MTDISPLAYDEVICE_INCLUDED
#define MTDISPLAYDEVICE_INCLUDED
//---------------------------------------------------------------------------
class MTDisplayDevice;
class MTDisplayDeviceManager;
//---------------------------------------------------------------------------
#include "MTBitmap.h"
#include "../Headers/MTXSystem.h"
#include "MTDisplay1.h"
//---------------------------------------------------------------------------
class MTDisplayDevice{
public:
	MTDisplayDevice();
	virtual bool MTCT init(bool &fullscreen) = 0;
	virtual void MTCT uninit() = 0;
	virtual MTBitmap* MTCT newbitmap(int flags,int width,int height) = 0;
	virtual MTBitmap* MTCT newresbitmap(int flags,MTResources *res,int id,int colorkey) = 0;
	virtual MTBitmap* MTCT newfilebitmap(int flags,const char *filename,int colorkey) = 0;
	virtual MTBitmap* MTCT newbmpbitmap(int flags,MTBitmap &orig,int colorkey) = 0;
	virtual void MTCT delbitmap(MTBitmap *bitmap) = 0;
	virtual MTMask* MTCT newmask(int w,int h) = 0;
	virtual void MTCT delmask(MTMask *mask) = 0;
	virtual bool MTCT setfullscreen(bool &fullscreen) = 0;
	virtual bool MTCT switchto(MTDisplayDevice *newdevice);
	virtual void MTCT add(MTBitmap *bmp);
	virtual void MTCT remove(MTBitmap *bmp);
	virtual void MTCT add(MTMask *bmp);
	virtual void MTCT remove(MTMask *bmp);
	virtual void MTCT checkbitmaps();
	virtual void MTCT setfocus(bool focused);
	virtual void MTCT sync();
protected:
	MTBitmap *bitmaps[32];
	int nbitmaps;
	MTMask *masks[32];
	int nmasks;
};

class MTDisplayDeviceManager{
public:
	char *devicename[MAX_DISPLAYDEVICES];

	MTDisplayDeviceManager();
	virtual MTDisplayDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTDisplayDevice *device) = 0;
};
//---------------------------------------------------------------------------
#endif
