//---------------------------------------------------------------------------
#ifndef MTGDIDEVICE_INCLUDED
#define MTGDIDEVICE_INCLUDED
//---------------------------------------------------------------------------
class MTGDIDevice;
class MTGDIDeviceManager;
//---------------------------------------------------------------------------
#include "MTDisplayDevice.h"
#include "MTGDIBitmap.h"
//---------------------------------------------------------------------------
class MTGDIDevice : public MTDisplayDevice{
public:
	bool MTCT init(bool &fullscreen);
	void MTCT uninit();
	MTBitmap* MTCT newbitmap(int flags,int width,int height);
	MTBitmap* MTCT newresbitmap(int flags,MTResources *res,int id,int colorkey);
	MTBitmap* MTCT newfilebitmap(int flags,const char *filename,int colorkey);
	MTBitmap* MTCT newbmpbitmap(int flags,MTBitmap &orig,int colorkey);
	void MTCT delbitmap(MTBitmap *bitmap);
	MTMask* MTCT newmask(int w,int h);
	void MTCT delmask(MTMask *mask);
	bool MTCT setfullscreen(bool &fullscreen);
};

class MTGDIDeviceManager : public MTDisplayDeviceManager{
public:
	MTGDIDeviceManager();
	~MTGDIDeviceManager();
	MTDisplayDevice* MTCT newdevice(int id);
	void MTCT deldevice(MTDisplayDevice *device);
};
//---------------------------------------------------------------------------
#endif
