//---------------------------------------------------------------------------
#ifndef MTDXDEVICE_INCLUDED
#define MTDXDEVICE_INCLUDED
//---------------------------------------------------------------------------
class MTDXDevice;
class MTDXDevice7;
class MTDXDeviceManager;
//---------------------------------------------------------------------------
#define DIRECT3D_VERSION 0x0700
#include <ddraw.h>
#include <d3d.h>
#include "MTDisplayDevice.h"
#include "MTDXBitmap.h"
//---------------------------------------------------------------------------
struct DDDevice{
	GUID *guid;
	char *desc;
	char *name;
	DDCAPS caps;
};

class MTDXDevice : public MTDisplayDevice{
public:
	MTDXDevice(DDDevice *dev);
	~MTDXDevice();
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
	void MTCT checkbitmaps();
	void MTCT reloadbitmaps();
	void MTCT setfocus(bool focused);
	void MTCT sync();
private:
	friend class MTDXBitmap;
	friend class MTDXMask;
	bool started;
	IDirectDraw *iddraw;
	IDirect3D2 *id3d;
	IDirect3DDevice2 *id3ddev;
	IDirect3DViewport2 *view;
	MTDXBitmap *lastb;
	DDDevice *cdev;
	D3DDEVICEDESC desc3d;
	D3DVIEWPORT viewport;
	void MTCT setbitmap(MTDXBitmap *b);
	bool MTCT start(MTDXBitmap *b);
	void MTCT end(bool reset = false);
};

class MTDXDevice7 : public MTDisplayDevice{
public:
	MTDXDevice7(DDDevice *dev);
	~MTDXDevice7();
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
	void MTCT checkbitmaps();
	void MTCT reloadbitmaps();
	void MTCT setfocus(bool focused);
	void MTCT sync();
private:
	friend class MTDXBitmap7;
	friend class MTDXMask7;
	bool started;
	IDirectDraw7 *iddraw;
	IDirect3D7 *id3d;
	IDirect3DDevice7 *id3ddev;
	MTDXBitmap7 *lastb;
	DDDevice *cdev;
	D3DDEVICEDESC7 desc3d;
	D3DVIEWPORT7 viewport;
	void MTCT setbitmap(MTDXBitmap7 *b);
	bool MTCT start(MTDXBitmap7 *b);
	void MTCT end(bool reset = false);
};

class MTDXDeviceManager : public MTDisplayDeviceManager{
public:
	MTDXDeviceManager();
	~MTDXDeviceManager();
	MTDisplayDevice* MTCT newdevice(int id);
	void MTCT deldevice(MTDisplayDevice *device);
};
//---------------------------------------------------------------------------
#ifdef _DEBUG
int dxerror(unsigned int code,const char *file,const int line);
#else
#define dxerror(C,F,L) (C)
#endif
//---------------------------------------------------------------------------
extern int ddver;
//---------------------------------------------------------------------------
#endif
