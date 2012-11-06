//---------------------------------------------------------------------------
#ifndef MTDXBITMAP_INCLUDED
#define MTDXBITMAP_INCLUDED
//---------------------------------------------------------------------------
class MTDXBitmap;
class MTDXBitmap7;
class MTDXMask;
class MTDXMask7;
//---------------------------------------------------------------------------
#include <windows.h>
#include "MTDXDevice.h"
#include "MTGDIBitmap.h"
//---------------------------------------------------------------------------
class MTDXBitmap : public MTGDIBitmap{
public:
	MTDXBitmap(MTDisplayDevice *d,int f,int w,int h);
	MTDXBitmap(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey);
	MTDXBitmap(MTDisplayDevice *d,int f,const char *filename,int colorkey);
	MTDXBitmap(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey);
	~MTDXBitmap();
	void MTCT unload();
	bool MTCT setsize(int w,int h);
	void* MTCT open(int wantedtype);
	void MTCT close(void *o);
	void MTCT clip(MTRect *rect);
	void MTCT cliprgn(void *rgn);
	void MTCT unclip();
	bool MTCT openbits(MTRect &rect,void **bits,int *pitch,void **maskbits = 0,int *maskpitch = 0);
	void MTCT closebits();
	bool MTCT blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	bool MTCT sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode = MTBM_COPY);
	bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o,int color);
	bool MTCT skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color);
	bool MTCT maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o);
	bool MTCT shade(int x,int y,int w,int h,MTMask *mask,int mx,int my);
	bool MTCT tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity);
	bool MTCT fill(int x,int y,int w,int h,int color,int opacity = 255);
	void MTCT setwindow(MTWinControl *window);
	inline static bool cdxblt(MTDXBitmap *bmp,RECT *dr,RECT *sr,int flags,DDBLTFX *fx);
	static bool dxbltex(MTDXBitmap *dest,int dx,int dy,int dw,int dh,MTDXBitmap *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4);
	static bool mdxbltex(MTDXBitmap *dest,int dx,int dy,int dw,int dh,MTDXMask *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4);
	inline static bool dxblt(MTDXBitmap *dest,RECT *dr,MTDXBitmap *src,RECT *sr,int flags,DDBLTFX *fx);
protected:
	friend class MTDXDevice;
	friend class MTDXMask;
	int bcount;
	IDirectDraw *iddraw;
	IDirectDrawSurface *ddsurf;
	IDirectDrawSurface3 *ddsurf3;
	IDirect3DTexture2 *texture;
	IDirectDrawClipper *iddclip;
	IDirectDrawClipper *clipper;
	D3DTEXTUREHANDLE htex;
	DDSURFACEDESC desc;
	DDSCAPS caps;
	double tfx,tfy;
	void MTCT initialize();
	void MTCT resettexture();
	int MTCT gettexture();
};

class MTDXBitmap7 : public MTGDIBitmap{
public:
	MTDXBitmap7(MTDisplayDevice *d,int f,int w,int h);
	MTDXBitmap7(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey);
	MTDXBitmap7(MTDisplayDevice *d,int f,const char *filename,int colorkey);
	MTDXBitmap7(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey);
	~MTDXBitmap7();
	void MTCT unload();
	bool MTCT setsize(int w,int h);
	void* MTCT open(int wantedtype);
	void MTCT close(void *o);
	void MTCT clip(MTRect *rect);
	void MTCT cliprgn(void *rgn);
	void MTCT unclip();
	bool MTCT openbits(MTRect &rect,void **bits,int *pitch,void **maskbits = 0,int *maskpitch = 0);
	void MTCT closebits();
	bool MTCT blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode = MTBM_COPY);
	bool MTCT sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode = MTBM_COPY);
	bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o,int color);
	bool MTCT skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color);
	bool MTCT maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o);
	bool MTCT shade(int x,int y,int w,int h,MTMask *mask,int mx,int my);
	bool MTCT tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh);
	bool MTCT blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity);
	bool MTCT fill(int x,int y,int w,int h,int color,int opacity = 255);
	void MTCT setwindow(MTWinControl *window);
	inline static bool cdxblt(MTDXBitmap7 *bmp,RECT *dr,RECT *sr,int flags,DDBLTFX *fx);
	static bool dxbltex(MTDXBitmap7 *dest,int dx,int dy,int dw,int dh,MTDXBitmap7 *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4);
	static bool mdxbltex(MTDXBitmap7 *dest,int dx,int dy,int dw,int dh,MTDXMask7 *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4);
	inline static bool dxblt(MTDXBitmap7 *dest,RECT *dr,MTDXBitmap7 *src,RECT *sr,int flags,DDBLTFX *fx);
protected:
	friend class MTDXDevice7;
	friend class MTDXMask7;
	int bcount;
	IDirectDraw7 *iddraw;
	IDirectDrawSurface7 *ddsurf;
	IDirectDrawClipper *iddclip;
	IDirectDrawClipper *clipper;
	RECT lockrect;
	DDSURFACEDESC2 desc;
	DDSCAPS2 caps;
	double tfx,tfy;
	void MTCT initialize();
};

class MTDXMask : public MTGDIMask{
public:
	MTDXMask(MTDisplayDevice *d,int w,int h);
	~MTDXMask();
	void MTCT blur(int amount);
	void MTCT opacity(int amount);
	void MTCT fill(int x,int y,int w,int h,int amount);
private:
	friend class MTDXBitmap;
	friend class MTDXDevice;
	IDirectDrawSurface *ddsurf;
	IDirectDrawSurface3 *ddsurf3;
	IDirect3DTexture2 *texture;
	D3DTEXTUREHANDLE htex;
	DDSURFACEDESC desc;
	D3DVALUE tfx,tfy;
	void MTCT getbits();
	int MTCT gettexture();
};

class MTDXMask7 : public MTGDIMask{
public:
	MTDXMask7(MTDisplayDevice *d,int w,int h);
	~MTDXMask7();
	void MTCT blur(int amount);
	void MTCT opacity(int amount);
	void MTCT fill(int x,int y,int w,int h,int amount);
private:
	friend class MTDXBitmap7;
	friend class MTDXDevice7;
	IDirectDrawSurface7 *ddsurf;
	DDSURFACEDESC2 desc;
	D3DVALUE tfx,tfy;
	void MTCT getbits();
};
//---------------------------------------------------------------------------
extern D3DTLVERTEX *vbuf;
extern int nv;
extern int nlow;
//---------------------------------------------------------------------------
#endif