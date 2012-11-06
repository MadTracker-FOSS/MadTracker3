#ifndef NODIRECTX
//---------------------------------------------------------------------------
//
//	MadTracker Display Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTDisplay1.h"
#include "MTDXBitmap.h"
#include "../MTGUI/MTGUITools.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
#define MTB_DXBITMAP (0x80000000|MTB_KEEPMASK)
//---------------------------------------------------------------------------
D3DTLVERTEX *vbuf = 0;
int nv = 0;
int nlow = 0;
//---------------------------------------------------------------------------
// MTDXBitmap
//---------------------------------------------------------------------------
inline bool MTDXBitmap::cdxblt(MTDXBitmap *bmp,RECT *dr,RECT *sr,int flags,DDBLTFX *fx)
{
	int res;
	IDirectDrawSurface *tmp;
	RECT tr;
	DDSURFACEDESC desc;
	
	mtmemzero(&desc,sizeof(DDSURFACEDESC));
	desc.dwSize = sizeof(DDSURFACEDESC);
	if (bmp->ddsurf->GetSurfaceDesc(&desc)) return false;
	desc.dwWidth = sr->right-sr->left;
	desc.dwHeight = sr->bottom-sr->top;
	if (bmp->iddraw->CreateSurface(&desc,&tmp,0)) return false;
	tr.left = tr.top = 0;
	tr.right = desc.dwWidth;
	tr.bottom = desc.dwHeight;
	tmp->Blt(&tr,bmp->ddsurf,sr,flags,fx);
	switch (res = bmp->ddsurf->Blt(dr,tmp,&tr,flags,fx)){
	case 0:
		tmp->Release();
		return true;
	case DDERR_WASSTILLDRAWING:
		flags |= DDBLT_WAIT;
		if (bmp->ddsurf->Blt(dr,tmp,&tr,flags,fx)==0){
			tmp->Release();
			return true;
		};
	default:
		tmp->Release();
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
#endif
		return false;
	};
}

bool MTDXBitmap::dxbltex(MTDXBitmap *dest,int dx,int dy,int dw,int dh,MTDXBitmap *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4)
{
	int x,n,htex;
	int sx,sy;
	double fx,fy,cfx;
	double adx,ady,aox,aoy;
	void *drgn;
	MTRect dr,r;
	bool ok = true;

	MTDXDevice &cdevice = *(MTDXDevice*)dest->device;
	if ((!&cdevice) || (!cdevice.id3ddev)) return false;
	sx = dx-ox;
	sy = dy-oy;
	fx = (double)ow/dw;
	fy = (double)oh/dh;
	dr.left = dx;
	dr.top = dy;
	dr.right = dx+dw;
	dr.bottom = dy+dh;
	drgn = recttorgn(dr);
	intersectrgn(drgn,dest->getclip());
	offsetrgn(drgn,-dx,-dy);
	adx = (double)dx+displayprefs.texeladjustx;
	ady = (double)dy+displayprefs.texeladjusty;
	aox = (double)ox+displayprefs.skintexeladjustx;
	aoy = (double)oy+displayprefs.skintexeladjusty;
	n = rgngetnrects(drgn);
	if (n){
		if (!cdevice.start(dest)) return false;
		htex = (src)?src->gettexture():0;
		if ((src) && (!htex)) return false;
		if (n>nv){
			nv = ((n+7)>>3)<<3;
			if (vbuf) vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
			else vbuf = (D3DTLVERTEX*)si->memalloc(sizeof(D3DTLVERTEX)*nv*6,MTM_ZERO);
		}
		else if ((n<nv) && (++nlow==32)){
			nv = n;
			vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
		};
		color1 = swapcolor(color1 & 0xFFFFFF) | (color1 & 0xFF000000);
		color2 = swapcolor(color2 & 0xFFFFFF) | (color2 & 0xFF000000);
		color3 = swapcolor(color3 & 0xFFFFFF) | (color3 & 0xFF000000);
		color4 = swapcolor(color4 & 0xFFFFFF) | (color4 & 0xFF000000);
		D3DTLVERTEX *ct = vbuf;
		for (x=0;x<n;x++,ct+=6){
			rgngetrect(drgn,x,&r);
			cfx = r.left/dw;
			ct[0].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[2].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			cfx = r.right/dw;
			ct[1].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[5].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			ct[0].rhw = ct[1].rhw = ct[2].rhw = ct[5].rhw = 1.0;
			ct[0].sx = adx+r.left;
			ct[0].sy = ady+r.top;
			ct[5].sx = adx+r.right;
			ct[5].sy = ady+r.bottom;
			if (src){
				ct[0].tu = (D3DVALUE)((double)r.left*fx+aox)*src->tfx/src->width;
				ct[0].tv = (D3DVALUE)((double)r.top*fy+aoy)*src->tfy/src->height;
				ct[5].tu = (D3DVALUE)((double)r.right*fx+aox)*src->tfx/src->width;
				ct[5].tv = (D3DVALUE)((double)r.bottom*fy+aoy)*src->tfy/src->height;
			};
			ct[1].sx = ct[5].sx;
			ct[1].sy = ct[0].sy;
			ct[1].tu = ct[5].tu;
			ct[1].tv = ct[0].tv;
			ct[2].sx = ct[0].sx;
			ct[2].sy = ct[5].sy;
			ct[2].tu = ct[0].tu;
			ct[2].tv = ct[5].tv;
			ct[3] = ct[2];
			ct[4] = ct[1];
		};
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
		};*/
		cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,htex);
		ok &= (dxerror(cdevice.id3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,D3DVT_TLVERTEX,vbuf,n*6,0),__FILE__,__LINE__)==0);
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
		};*/
	};
	deletergn(drgn);
	return ok;
}

bool MTDXBitmap::mdxbltex(MTDXBitmap *dest,int dx,int dy,int dw,int dh,MTDXMask *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4)
{
	int x,n,htex;
	int sx,sy;
	double fx,fy,cfx;
	double adx,ady,aox,aoy;
	void *drgn;
	MTRect dr,r;
	bool ok = true;

	MTDXDevice &cdevice = *(MTDXDevice*)dest->device;
	if ((!&cdevice) || (!cdevice.id3ddev)) return false;
	sx = dx-ox;
	sy = dy-oy;
	fx = (double)ow/dw;
	fy = (double)oh/dh;
	dr.left = dx;
	dr.top = dy;
	dr.right = dx+dw;
	dr.bottom = dy+dh;
	drgn = recttorgn(dr);
	intersectrgn(drgn,dest->getclip());
	offsetrgn(drgn,-dx,-dy);
	adx = (double)dx+displayprefs.texeladjustx;
	ady = (double)dy+displayprefs.texeladjusty;
	aox = (double)ox+displayprefs.skintexeladjustx;
	aoy = (double)oy+displayprefs.skintexeladjusty;
	n = rgngetnrects(drgn);
	if (n){
		if (!cdevice.start(dest)) return false;
		htex = (src)?src->gettexture():0;
		if ((src) && (!htex)) return false;
		if (n>nv){
			nv = ((n+7)>>3)<<3;
			if (vbuf) vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
			else vbuf = (D3DTLVERTEX*)si->memalloc(sizeof(D3DTLVERTEX)*nv*6,MTM_ZERO);
		}
		else if ((n<nv) && (++nlow==32)){
			nv = n;
			vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
		};
		color1 = swapcolor(color1 & 0xFFFFFF) | (color1 & 0xFF000000);
		color2 = swapcolor(color2 & 0xFFFFFF) | (color2 & 0xFF000000);
		color3 = swapcolor(color3 & 0xFFFFFF) | (color3 & 0xFF000000);
		color4 = swapcolor(color4 & 0xFFFFFF) | (color4 & 0xFF000000);
		D3DTLVERTEX *ct = vbuf;
		for (x=0;x<n;x++,ct+=6){
			rgngetrect(drgn,x,&r);
			cfx = r.left/dw;
			ct[0].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[2].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			cfx = r.right/dw;
			ct[1].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[5].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			ct[0].rhw = ct[1].rhw = ct[2].rhw = ct[5].rhw = 1.0;
			ct[0].sx = adx+r.left;
			ct[0].sy = ady+r.top;
			ct[5].sx = adx+r.right;
			ct[5].sy = ady+r.bottom;
			if (src){
				ct[0].tu = (D3DVALUE)((double)r.left*fx+aox)*src->tfx/src->width;
				ct[0].tv = (D3DVALUE)((double)r.top*fy+aoy)*src->tfy/src->height;
				ct[5].tu = (D3DVALUE)((double)r.right*fx+aox)*src->tfx/src->width;
				ct[5].tv = (D3DVALUE)((double)r.bottom*fy+aoy)*src->tfy/src->height;
			};
			ct[1].sx = ct[5].sx;
			ct[1].sy = ct[0].sy;
			ct[1].tu = ct[5].tu;
			ct[1].tv = ct[0].tv;
			ct[2].sx = ct[0].sx;
			ct[2].sy = ct[5].sy;
			ct[2].tu = ct[0].tu;
			ct[2].tv = ct[5].tv;
			ct[3] = ct[2];
			ct[4] = ct[1];
		};
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
		};*/
		cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,htex);
		ok &= (dxerror(cdevice.id3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,D3DVT_TLVERTEX,vbuf,n*6,0),__FILE__,__LINE__)==0);
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
		};*/
	};
	deletergn(drgn);
	return ok;
}

inline bool MTDXBitmap::dxblt(MTDXBitmap *dest,RECT *dr,MTDXBitmap *src,RECT *sr,int flags,DDBLTFX *fx)
{
	int res;

//	if (dest->texture) return false;
	if ((src) && (src->texture)){
		return dxbltex(dest,dr->left,dr->top,dr->right-dr->left,dr->bottom-dr->top,src,sr->left,sr->top,sr->right-sr->left,sr->bottom-sr->top,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF);
	};
	if ((gi) && (!gi->visible) && (dest==screen) && (di->fullscreen)) return false;
	((MTDXDevice*)dest->device)->end();
//	if ((dest==src) && (dr->right-dr->left==sr->right-sr->left) && (dr->bottom-dr->top==sr->bottom-sr->top) && (intersectrect(*(MTRect*)sr,*(MTRect*)dr))) return cdxblt(dest,dr,sr,flags,fx);
	switch (res = dest->ddsurf->Blt(dr,(src)?src->ddsurf:0,sr,flags,fx)){
	case 0:
		return true;
	case DDERR_SURFACELOST:
		if (gi) gi->needbitmapcheck();
/*
		if (src){
			if (src->ddsurf->IsLost()==DDERR_SURFACELOST){
				src->ddsurf->Restore();
			};
		};
		if (dest->ddsurf->IsLost()==DDERR_SURFACELOST){
			dest->ddsurf->Restore();
		};
*/
		return false;
	case DDERR_WASSTILLDRAWING:
		flags |= DDBLT_WAIT;
		return (dest->ddsurf->Blt(dr,(src)?src->ddsurf:0,sr,flags,fx)==0);
	default:
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
#endif
		return false;
	};
}
//---------------------------------------------------------------------------
MTDXBitmap::MTDXBitmap(MTDisplayDevice *d,int f,int w,int h):
MTGDIBitmap(d,f|MTB_DXBITMAP,w,h),
iddraw(((MTDXDevice*)d)->iddraw),
iddclip(0),
ddsurf(0),
ddsurf3(0),
texture(0),
clipper(0),
htex(0),
bcount(0)
{
	FENTER4("MTDXBitmap::MTDXBitmap(%.8X,%d,%d,%d)",d,f,w,h);
	mtmemzero(&desc,sizeof(desc));
	if (flags & MTB_SCREEN){
		desc.dwSize = sizeof(desc);
		desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		iddraw->GetDisplayMode(&desc);
		LOG("Display mode:"NL);
		DUMP(&desc,sizeof(desc),0);
		bitcount = desc.ddpfPixelFormat.dwRGBBitCount;
		if (bitcount==16){
			if ((desc.ddpfPixelFormat.dwRBitMask & 8000)==0) bitcount = 15;
		};
		width = desc.dwWidth;
		height = desc.dwHeight;
		mpitch = desc.lPitch;
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE|DDSCAPS_PRIMARYSURFACE;
//		if (!(cdddev->caps.dwCaps & DDCAPS_NOHARDWARE)) desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		if (dxerror(iddraw->CreateSurface(&desc,&ddsurf,0),__FILE__,__LINE__)) return;
		ddsurf->QueryInterface(IID_IDirectDrawSurface3,(void**)&ddsurf3);
		if ((ddsurf3) && (di->fullscreen)){
			ddsurf3->PageLock(0);
		};
		dxerror(ddsurf->GetCaps(&caps),__FILE__,__LINE__);
		if (dxerror(iddraw->CreateClipper(0,&clipper,0),__FILE__,__LINE__)) return;
		dxerror(ddsurf->SetClipper(clipper),__FILE__,__LINE__);
	}
	else{
		initialize();
		loaded = load();
	};
	LEAVE();
}

MTDXBitmap::MTDXBitmap(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,res,resid,colorkey),
iddraw(((MTDXDevice*)d)->iddraw),
iddclip(0),
ddsurf(0),
ddsurf3(0),
texture(0),
clipper(0),
htex(0),
bcount(0)
{
	FENTER5("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%d,%.6X)",d,f,res,resid,colorkey);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap::MTDXBitmap(MTDisplayDevice *d,int f,const char *filename,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,filename,colorkey),
iddraw(((MTDXDevice*)d)->iddraw),
iddclip(0),
ddsurf(0),
ddsurf3(0),
texture(0),
clipper(0),
htex(0),
bcount(0)
{
	FENTER3("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%s)",d,f,filename);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap::MTDXBitmap(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,orig,colorkey),
iddraw(((MTDXDevice*)d)->iddraw),
iddclip(0),
ddsurf(0),
ddsurf3(0),
texture(0),
clipper(0),
htex(0),
bcount(0)
{
	FENTER4("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%.6X)",d,f,&orig,colorkey);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap::~MTDXBitmap()
{
	unload();
	if (clipper) clipper->Release();
	LEAVE();
}

void MTDXBitmap::initialize()
{
	ENTER("MTDXBitmap::initialize");
	mdc = 0;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
//		desc.ddsCaps.dwCaps = DDSCAPS_OWNDC;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
	if (!(((MTDXDevice*)device)->cdev->caps.dwCaps & DDCAPS_NOHARDWARE)) desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
	if (screen) bitcount = screen->bitcount;
	LEAVE();
}

void MTDXBitmap::unload()
{
	ENTER("MTDXBitmap::unload");
	loaded = false;
	if (mbits){
		dxerror(ddsurf->Unlock(mbits),__FILE__,__LINE__);
		mbits = 0;
	};
	((MTDXDevice*)device)->end(true);
	htex = 0;
	if (mask){
		DeleteDC(mmdc);
		DeleteObject(mask);
		si->memfree(mmbits);
		mmdc = 0;
		mask = 0;
		mmbits = 0;
	};
	if (ddsurf){
		if (iddclip){
			iddclip->Release();
			iddclip = 0;
		};
		if (texture){
			texture->Release();
			texture = 0;
		};
		if (ddsurf3){
			ddsurf3->Release();
			ddsurf3 = 0;
		};
		if (ddsurf){
			if (iddraw) ddsurf->Release();
			ddsurf = 0;
		};
	};
	LEAVE();
}

bool MTDXBitmap::setsize(int w,int h)
{
	int res;
	bool retried = false;
	DDBLTFX fx;
	
	if ((loaded) && (w==width) && (h==height)) return true;
	FENTER2("MTDXBitmap::setsize(%d,%d)",w,h);
	unload();
	if ((w>0) && (h>0)){
		width = w;
		height = h;
	};
	wr.left = wr.top = 0;
	wr.right = width;
	wr.bottom = height;
	if (flags & MTB_DRAW){
		if (mrgn) DeleteObject(mrgn);
		mrgn = CreateRectRgnIndirect((RECT*)&wr);
	};
	mdc = 0;
	mbits = 0;
	bcount = 0;
	if (!iddraw){
		LOGD("%s - [Display] ERROR: DirectDraw not initialized!"NL);
		LEAVE();
		return false;
	};
	desc.ddsCaps.dwCaps &= (~(DDSCAPS_VISIBLE|DDSCAPS_WRITEONLY|DDSCAPS_LOCALVIDMEM|DDSCAPS_NONLOCALVIDMEM));
	desc.ddsCaps.dwCaps &= (~(DDSCAPS_SYSTEMMEMORY));
	desc.ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM);
	MTDXDevice &cdevice = *(MTDXDevice*)device;
	if (desc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE){
		desc.dwFlags &= (~(DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT));
	}
	else{
		if (flags & MTB_DRAW) desc.ddsCaps.dwCaps |= (DDSCAPS_OFFSCREENPLAIN|DDSCAPS_3DDEVICE);
		tfx = tfy = 1.0;
		if ((flags & MTB_SKIN) && (cdevice.id3d)){
			desc.ddsCaps.dwCaps &= (~DDSCAPS_OFFSCREENPLAIN);
			desc.dwWidth = 16;
			desc.dwHeight = 16;
			while (desc.dwWidth<width) desc.dwWidth *= 2;
			while (desc.dwHeight<height) desc.dwHeight *= 2;
			if (!cdevice.desc3d.dwSize){
				desc.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;
				tfx = (D3DVALUE)width/desc.dwWidth;
				tfy = (D3DVALUE)height/desc.dwHeight;
			}
			else{
				if ((desc.dwWidth>cdevice.desc3d.dwMaxTextureWidth) || (desc.dwHeight>cdevice.desc3d.dwMaxTextureHeight)){
					desc.dwWidth = width;
					desc.dwHeight = height;
					flags &= (~MTB_SKIN);
				}
				else{
					if (desc.dwWidth<cdevice.desc3d.dwMinTextureWidth) desc.dwWidth = cdevice.desc3d.dwMinTextureWidth;
					if (desc.dwHeight<cdevice.desc3d.dwMinTextureHeight) desc.dwWidth = cdevice.desc3d.dwMinTextureHeight;
					desc.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;
					tfx = (D3DVALUE)width/desc.dwWidth;
					tfy = (D3DVALUE)height/desc.dwHeight;
				};
			};
		}
		else{
			desc.dwWidth = width;
			desc.dwHeight = height;
		};
	};
	goto go;
retry:
	if (retried){
		LEAVE();
		return false;
	};
	retried = true;
go:
	res = iddraw->CreateSurface(&desc,&ddsurf,0);
	switch (res){
	case 0:
		if (flags & MTB_DRAW){
			if ((cdevice.id3d) && (!cdevice.id3ddev)){
				D3DFINDDEVICESEARCH search;
				D3DFINDDEVICERESULT result;
				mtmemzero(&search,sizeof(search));
				mtmemzero(&result,sizeof(result));
				search.dwSize = sizeof(search);
				search.dwFlags = D3DFDS_HARDWARE;
				search.bHardware = 1;
				result.dwSize = sizeof(result);
				if (dxerror(cdevice.id3d->FindDevice(&search,&result),__FILE__,__LINE__)==0){
					cdevice.id3d->CreateDevice(result.guid,ddsurf,&cdevice.id3ddev);
				};
				if (!cdevice.id3ddev){
					if (cdevice.id3d->CreateDevice(IID_IDirect3DHALDevice,ddsurf,&cdevice.id3ddev)){
						if (cdevice.id3d->CreateDevice(IID_IDirect3DMMXDevice,ddsurf,&cdevice.id3ddev)){
							if (cdevice.id3d->CreateDevice(IID_IDirect3DRGBDevice,ddsurf,&cdevice.id3ddev)){
								LOGD("%s - [Display] Cannot create a Direct3D device!"NL);
							};
						};
					};
				};
				if (cdevice.id3ddev){
					D3DDEVICEDESC descsoft;
					mtmemzero(&cdevice.desc3d,sizeof(cdevice.desc3d));
					cdevice.desc3d.dwSize = sizeof(cdevice.desc3d);
					mtmemzero(&descsoft,sizeof(descsoft));
					descsoft.dwSize = sizeof(descsoft);
					cdevice.id3ddev->GetCaps(&cdevice.desc3d,&descsoft);
					cdevice.setbitmap(this);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,1);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,1);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
/*/
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
/*/
				};
			};
			dxerror(iddraw->CreateClipper(0,&iddclip,0),__FILE__,__LINE__);
		};
		ddsurf->QueryInterface(IID_IDirectDrawSurface3,(void**)&ddsurf3);
		if (ddsurf3){
			if ((flags & MTB_SKIN) && (cdevice.id3d)){
				ddsurf3->QueryInterface(IID_IDirect3DTexture2,(void**)&texture);
				if (cdevice.id3ddev) texture->GetHandle(cdevice.id3ddev,&htex);
			};
			if (di->fullscreen) ddsurf3->PageLock(0);
		};
		dxerror(ddsurf->GetSurfaceDesc(&desc),__FILE__,__LINE__);
		mpitch = desc.lPitch;
		if (mck>=0){
			DDCOLORKEY ddck;
			ddck.dwColorSpaceLowValue = translatecolor(swapcolor(mck));
			ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
			dxerror(ddsurf->SetColorKey(DDCKEY_SRCBLT,&ddck),__FILE__,__LINE__);
		};
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0;
		dxblt(this,0,0,0,DDBLT_COLORFILL,&fx);
		break;
	case DDERR_TOOBIGWIDTH:
		desc.dwWidth = width;
		desc.dwHeight = height;
		flags &= (~MTB_SKIN);
		desc.ddsCaps.dwCaps &= (~DDSCAPS_TEXTURE);
		tfx = tfy = 1.0;
		goto retry;
	case DDERR_OUTOFVIDEOMEMORY:
		desc.ddsCaps.dwCaps &= (~(DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM|DDSCAPS_NONLOCALVIDMEM));
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		goto retry;
	default:
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
		LOG("desc:"NL);
		DUMP(&desc,desc.dwSize,0);
#endif
		LEAVE();
		return false;
	};
	if ((bitcount==0) && (screen)) bitcount = screen->bitcount;
	LEAVE();
	return true;
}

void *MTDXBitmap::open(int wantedtype)
{
	int ret;

	mcount++;
	mtextinit = false;
	if (wantedtype==2) return (void*)ddsurf;
	if (!mdc){
		if ((!ddsurf) || (!iddraw)){
			LOGD("%s - [Display] ERROR: DirectDraw not initialized!"NL);
			return 0;
		};
		((MTDXDevice*)device)->end();
		ret = ddsurf->GetDC(&mdc);
		if (ret==DDERR_SURFACELOST){
			ret = ddsurf->Restore();
			if (ret==DDERR_WRONGMODE){
				((MTDXDevice*)device)->reloadbitmaps();
			}
			else if (ret!=0){
				dxerror(ret,__FILE__,__LINE__);
			};
			dxerror(ddsurf->GetDC(&mdc),__FILE__,__LINE__);
		};
		if (ret==0){
			if ((flags & MTB_DRAW) && (clipped)) SelectClipRgn(mdc,crgn[clipped-1]);
		};
	};
	return (void*)mdc;
}

void MTDXBitmap::close(void *o)
{
	if (!mcount) return;
	mcount--;
	if ((!mcount) && (mdc)){
		if (flags & MTB_DRAW){
			if (clipped) SelectClipRgn(mdc,0);
			DeleteObject(GetCurrentObject(mdc,OBJ_PEN));
			DeleteObject(GetCurrentObject(mdc,OBJ_BRUSH));
		};
		if ((ddsurf) && (iddraw)) dxerror(ddsurf->ReleaseDC((HDC)mdc),__FILE__,__LINE__);
		mdc = 0;
	};
}

void MTDXBitmap::clip(MTRect *rect)
{
	int cr = clipped++;
	int s;
	RGNDATA *rd;
	
	if ((!iddclip) || (clipped==MAX_CLIP)) return;
	crgn[cr] = CreateRectRgnIndirect((RECT*)rect);
	if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
	s = GetRegionData(crgn[cr],0,0);
	rd = (RGNDATA*)si->memalloc(s,0);
	GetRegionData(crgn[cr],s,rd);
	iddclip->SetClipList(rd,0);
	if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
	si->memfree(rd);
}

void MTDXBitmap::cliprgn(void *rgn)
{
	int cr = clipped++;
	int s;
	RGNDATA *rd;
	
	if ((!iddclip) || (clipped==MAX_CLIP)) return;
	crgn[cr] = CreateRectRgn(0,0,1,1);
	CombineRgn((HRGN)crgn[cr],(HRGN)rgn,(HRGN)rgn,RGN_COPY);
	if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
	s = GetRegionData(crgn[cr],0,0);
	rd = (RGNDATA*)si->memalloc(s,0);
	GetRegionData(crgn[cr],s,rd);
	iddclip->SetClipList(rd,0);
	if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
	si->memfree(rd);
}

void MTDXBitmap::unclip()
{
	int cr = --clipped;
	
	if (!iddclip) return;
	if (clipped){
		int s = GetRegionData(crgn[clipped-1],0,0);
		RGNDATA *rd = (RGNDATA*)si->memalloc(s,0);
		GetRegionData(crgn[clipped-1],s,rd);
		iddclip->SetClipList(rd,0);
		if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
		si->memfree(rd);
	}
	else{
		if (ddsurf) dxerror(ddsurf->SetClipper(0),__FILE__,__LINE__);
		iddclip->SetClipList(0,0);
	};
	DeleteObject(crgn[cr]);
	crgn[cr] = 0;
}

void MTDXBitmap::setwindow(MTWinControl *window)
{
	int l;
	void *oldwnd = mwnd;
	static int cyc = -1,cysc,cxf,cyf,cxdf,cydf;
	
	FENTER1("MTDXBitmap::setwindow(%.8X)",window);
	if (window){
		if (window->guiid==MTC_DESKTOP){
			mwnd = ((MTDesktop*)window)->mwnd;
		}
		else{
			mwnd = window->dsk->mwnd;
		};
		if (mwnd!=oldwnd) mox = -1;
		else if (!modified){
			LEAVE();
			return;
		}
		else mox = -1;
		modified = false;
	}
	else{
		if (!mwnd) mox = -1;
		mwnd = 0;
		wr.left = wr.top = 0;
		wr.right = width;
		wr.bottom = height;
		if (clipper) clipper->SetHWnd(0,0);
		LEAVE();
		return;
	};
	if (clipper) clipper->SetHWnd(0,(HWND)mwnd);
	if (di->fullscreen){
		wr.left = wr.top = 0;
		wr.right = desc.dwWidth;
		wr.bottom = desc.dwHeight;
	}
	else{
		GetWindowRect((HWND)mwnd,(RECT*)&wr);
		if (mox<0){
			mox = moy = 0;
			l = GetWindowLong((HWND)mwnd,GWL_STYLE);
			if (cyc<0){
				cyc = GetSystemMetrics(SM_CYCAPTION);
				cysc = GetSystemMetrics(SM_CYSMCAPTION);
				cxf = GetSystemMetrics(SM_CXFRAME);
				cyf = GetSystemMetrics(SM_CYFRAME);
				cxdf = GetSystemMetrics(SM_CXDLGFRAME);
				cydf = GetSystemMetrics(SM_CYDLGFRAME);
			};
			if ((l & WS_CAPTION)==WS_CAPTION){
				if (GetWindowLong((HWND)mwnd,GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
					moy += cysc;
				else
					moy += cyc;
			};
			if (l & WS_THICKFRAME){
				mox += cxf-1;
				moy += cyf-1;
			}
			else if (l & WS_BORDER){
				mox += cxdf;
				moy += cydf;
			};
		};
		wr.left += mox;
		wr.right += mox;
		wr.top += moy;
		wr.bottom += moy;
		mox = wr.left;
		moy = wr.top;
	};
	LEAVE();
}

bool MTDXBitmap::openbits(MTRect &rect,void **bits,int *pitch,void **maskbits,int *maskpitch)
{
	bool in = false;
	DDSURFACEDESC2 desc;
	MTRect r = rect;
	
	if ((!bits) || (!pitch)) return false;
	r.left = (r.left>>5)<<5;
	r.right = ((r.right+31)>>5)<<5;
	if (r.left<0) r.left = 0;
	if (r.top<0) r.top = 0;
	if (r.right>width) r.right = width;
	if (r.bottom>height) r.bottom = height;
	if ((r.right<=r.left) || (r.bottom<=r.top)) return false;
	if (bcount==0){
		ob = r;
	}
	else{
		if ((r.left<ob.left) || (r.top<ob.top) || (r.right>ob.right) || (r.bottom>ob.bottom)) return false;
		in = true;
	};
	bcount++;
	if ((!ddsurf) || (!iddraw)) return false;
	if (in){
		*bits = (char*)mbits+(rect.left-ob.left)*((bitcount+7)>>3)+(rect.top-ob.top)*mpitch;
		*pitch = mpitch;
		return true;
	};
	((MTDXDevice*)device)->end();
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	if (dxerror(ddsurf->Lock((RECT*)&r,(LPDDSURFACEDESC)&desc,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,0),__FILE__,__LINE__)==0){
		mbits = desc.lpSurface;
		mpitch = desc.lPitch;
		*bits = (char*)mbits+(rect.left-r.left)*((bitcount+7)>>3)+(rect.top-r.top)*mpitch;
		*pitch = mpitch;
		if (maskbits){
			*maskbits = 0;
			*maskpitch = screen->translatecolor(mck);
		};
		return true;
	};
	return false;
}

void MTDXBitmap::closebits()
{
	if (--bcount!=0) return;
	if (mbits){
		dxerror(ddsurf->Unlock(mbits),__FILE__,__LINE__);
		mbits = 0;
	};
}

bool MTDXBitmap::blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode)
{
	MTDXBitmap *dxdest = (MTDXBitmap*)dest;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	bool tr = (mck>=0);
	DDBLTFX fx;
	
	if ((!dxdest->ddsurf) || (!ddsurf)) return false;
//	((MTDXDevice*)device)->end();
	if ((w==0) && (h==0)){
		w = width;
		h = height;
	};
	RECT dr = {dx,dy,dx+w,dy+h};
	RECT or = {ox,oy,ox+w,oy+h};
	if (or.left<0){
		dr.left -= or.left;
		or.left = 0;
	};
	if (or.top<0){
		dr.top -= or.top;
		or.top = 0;
	};
	if (or.right>width){
		dr.right -= (or.right-width);
		or.right = width;
	};
	if (or.bottom>height){
		dr.bottom -= (or.bottom-height);
		or.bottom = height;
	};
	if (dr.left<0){
		or.left -= dr.left;
		dr.left = 0;
	};
	if (dr.top<0){
		or.top -= dr.top;
		dr.top = 0;
	};
	if (dr.right>dest->width){
		or.right -= (dr.right-dest->width);
		dr.right = dest->width;
	};
	if (dr.bottom>dest->height){
		or.bottom -= (dr.bottom-dest->height);
		dr.bottom = dest->height;
	};
	if ((or.bottom<=or.top) || (or.right<=or.left)) return false;
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fxflags = DDBLT_KEYSRC;
	};
	if (mode!=MTBM_COPY){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fx.dwROP = bltmode[mode];
		fxflags |= DDBLT_ROP;
	};
	return dxblt(dxdest,&dr,this,&or,fxflags,fxp);
}

bool MTDXBitmap::sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode)
{
	MTDXBitmap *dxdest = (MTDXBitmap*)dest;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	bool tr = (mck>=0);
	DDBLTFX fx;
	
	if ((!dxdest->ddsurf) || (!ddsurf)) return false;
//	((MTDXDevice*)device)->end();
	RECT dr = {dx,dy,dx+dw,dy+dh};
	RECT or = {ox,oy,ox+ow,oy+oh};
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fxflags = DDBLT_KEYSRC;
	};
	if (mode!=MTBM_COPY){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwROP = bltmode[mode];
		fxp = &fx;
		fxflags |= DDBLT_ROP;
	};
	return dxblt(dxdest,&dr,this,&or,fxflags,fxp);
}

bool MTDXBitmap::skinblt(int x,int y,int w,int h,MTSkinPart &o,int color)
{
	if (w==0) w = o.b.w;
	if (h==0) h = o.b.h;
	int ow,oh,cw,ch;
	int oldx = x;
	int oldw = w;
	bool tr;
	bool ok = true;
	MTDXBitmap *skinbmp = (MTDXBitmap*)::skinbmp[o.bmpid & 0xF];
	
	if (!ddsurf) return false;
	if ((w==0) || (h==0)) return true;
	DDBLTFX fx;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	RECT dr = {x,y,x+w,y+h};
	RECT or = {o.b.x,o.b.y,o.b.x+o.b.w,o.b.y+o.b.h};
	mtmemzero(&fx,sizeof(fx));
	fx.dwSize = sizeof(fx);
	if (o.mode==SKIN_COLOR){
		if (color!=0xFFFFFF) color = combinecolor(color,o.color);
		else color = o.color;
		fx.dwFillColor = screen->translatecolor(swapcolor(color));
		return dxblt(this,&dr,0,0,DDBLT_COLORFILL,&fx);
	};
	if ((!skinbmp) || (!skinbmp->ddsurf)) return false;
	tr = ((o.flags & SKIN_TRANSP) && (skinbmp->mck>=0));
	if (tr){
		fxflags = DDBLT_KEYSRC;
		fxp = &fx;
	};
	if ((o.b.w==w) && (o.b.h==h)) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
	else{
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<o.b.w))) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			ow = o.b.w;
			oh = o.b.h;
			if ((!ow) || (!oh)) return true;
			while (h>0){
				if (h>oh) ch = oh;
				else{
					or.bottom = or.top+h;
					ch = h;
				};
				while (w>0){
					if (w>ow) cw = ow;
					else{
						or.right = or.left+w;
						cw = w;
					};
					dr.right = dr.left+cw;
					dr.bottom = dr.top+ch;
					ok &= dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
					dr.left += cw;
					dr.right += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				or.right = or.left+o.b.w;
				dr.left = x;
				dr.right = x+w;
				dr.top += ch;
				dr.bottom += ch;
				h -= ch;
			};
			return ok;
		};
	};
}

bool MTDXBitmap::skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color)
{
	int ow,oh,cw,ch;
	int oldx;
	int oldw;
	int aw = o.b.w/nx;
	int ah = o.b.h/ny;
	int caw,cah;
	bool tr;
	bool ok = true;
	MTDXBitmap *skinbmp = (MTDXBitmap*)::skinbmp[o.bmpid & 0xF];
	
	if (!skinbmp) return false;
	if ((w|h)==0){
		w = aw;
		h = ah;
	}
	else if ((w==0) || (h==0)) return true;
	oldx = x;
	oldw = w;
	tr = ((o.flags & SKIN_TRANSP) && (skinbmp->mck>=0));
	caw = (step%nx)*aw;
	cah = (step/nx)*ah;
	if ((!ddsurf) || (!skinbmp->ddsurf)) return false;
	DDBLTFX fx;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	if (color!=0xFFFFFF){
		color |= 0xFF000000;
		if (dxbltex(this,x,y,w,h,skinbmp,o.b.x+caw,o.b.y+cah,w,h,color,color,color,color)) return true;
	};
	RECT dr = {x,y,x+w,y+h};
	RECT or = {o.b.x+caw,o.b.y+cah,o.b.x+caw+aw,o.b.y+cah+ah};
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxflags = DDBLT_KEYSRC;
		fxp = &fx;
	};
	if ((aw==w) && (ah==h)) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
	else{
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<aw))) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			ok = true;
			ow = aw;
			oh = ah;
			if ((!ow) || (!oh)) return true;
			while (h>0){
				if (h>oh) ch = oh;
				else{
					or.bottom = or.top+h;
					ch = h;
				};
				while (w>0){
					if (w>ow) cw = ow;
					else{
						or.right = or.left+w;
						cw = w;
					};
					dr.right = dr.left+cw;
					dr.bottom = dr.top+ch;
					ok &= dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
					dr.left += cw;
					dr.right += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				or.right = or.left+o.b.w;
				dr.left = x;
				dr.right = x+w;
				dr.top += ch;
				dr.bottom += ch;
				h -= ch;
			};
			return ok;
		};
	};
}

bool MTDXBitmap::maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o)
{
	bool ok = MTGDIBitmap::maskblt(mask,x,y,w,h,o);
	if ((ok) && (o.mode!=SKIN_COLOR)) ((MTDXMask*)mask)->getbits();
	return ok;
}

bool MTDXBitmap::shade(int x,int y,int w,int h,MTMask *mask,int mx,int my)
{
	MTDXMask *dm = (MTDXMask*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask*)mask,mx,my,w,h,0,0,0,0)) return false/*MTGDIBitmap::shade(x,y,w,h,mask,mx,my)*/;
	return true;
}

bool MTDXBitmap::tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
//FIXME
	MTDXMask *dm = (MTDXMask*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask*)mask,mx,my,mw,mh,0,0,0,0)) return false/*MTGDIBitmap::tshade(x,y,w,h,mask,mx,my,mw,mh)*/;
	return true;
}

bool MTDXBitmap::sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
	MTDXMask *dm = (MTDXMask*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask*)mask,mx,my,mw,mh,0,0,0,0)) return false/*MTGDIBitmap::sshade(x,y,w,h,mask,mx,my,mw,mh)*/;
	return true;
}

bool MTDXBitmap::blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity)
{
	int color = (opacity<<24)|0xFFFFFF;

	if (dxbltex((MTDXBitmap*)dest,dx,dy,w,h,(MTDXBitmap*)this,ox,oy,w,h,color,color,color,color)) return true;
	return MTGDIBitmap::blendblt(dest,dx,dy,w,h,ox,oy,opacity);
}

bool MTDXBitmap::fill(int x,int y,int w,int h,int color,int opacity)
{
	DDBLTFX fx;
	int fxflags = DDBLT_COLORFILL;
	
	if ((w==0) || (h==0)) return true;
	if (!ddsurf) return false;
	if (opacity<255){
		color = color|(opacity<<24);
		if (dxbltex(this,x,y,w,h,0,0,0,0,0,color,color,color,color)) return true;
		return MTGDIBitmap::fill(x,y,w,h,color,opacity);
	};
	RECT dr = {x,y,x+w,y+h};
	mtmemzero(&fx,sizeof(fx));
	fx.dwSize = sizeof(fx);
	fx.dwFillColor = screen->translatecolor(swapcolor(color));
	return dxblt(this,&dr,0,0,fxflags,&fx);
}

void MTDXBitmap::resettexture()
{
	MTDXDevice &cdevice = *(MTDXDevice*)device;

	if (texture){
		texture->Release();
		texture = 0;
	};
	htex = 0;
	if (ddsurf3){
		if ((flags & MTB_SKIN) && (cdevice.id3d)){
			ddsurf3->QueryInterface(IID_IDirect3DTexture2,(void**)&texture);
			if (cdevice.id3ddev) texture->GetHandle(cdevice.id3ddev,&htex);
		};
	};
}

int MTDXBitmap::gettexture()
{
	MTDXDevice &cdevice = *(MTDXDevice*)device;

	if (((flags & MTB_SKIN)==0) || (!texture) || (!cdevice.id3ddev)) return 0;
	if (htex) return htex;
	if (texture->GetHandle(cdevice.id3ddev,&htex)==0){
		if (!htex) htex = -1;
	}
	else htex = -1;
	return htex;
}
//---------------------------------------------------------------------------
// MTDXBitmap7 
//---------------------------------------------------------------------------
inline bool MTDXBitmap7::cdxblt(MTDXBitmap7 *bmp,RECT *dr,RECT *sr,int flags,DDBLTFX *fx)
{
	int res;
	IDirectDrawSurface7 *tmp;
	RECT tr;
	DDSURFACEDESC2 desc;
	
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	if (bmp->ddsurf->GetSurfaceDesc(&desc)) return false;
	desc.dwWidth = sr->right-sr->left;
	desc.dwHeight = sr->bottom-sr->top;
	if (bmp->iddraw->CreateSurface(&desc,&tmp,0)) return false;
	tr.left = tr.top = 0;
	tr.right = desc.dwWidth;
	tr.bottom = desc.dwHeight;
	tmp->Blt(&tr,bmp->ddsurf,sr,flags,fx);
	switch (res = bmp->ddsurf->Blt(dr,tmp,&tr,flags,fx)){
	case 0:
		tmp->Release();
		return true;
	case DDERR_WASSTILLDRAWING:
		flags |= DDBLT_WAIT;
		if (bmp->ddsurf->Blt(dr,tmp,&tr,flags,fx)==0){
			tmp->Release();
			return true;
		};
	default:
		tmp->Release();
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
#endif
		return false;
	};
}

bool MTDXBitmap7::dxbltex(MTDXBitmap7 *dest,int dx,int dy,int dw,int dh,MTDXBitmap7 *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4)
{
	int x,n;
	int sx,sy;
	double fx,fy,cfx;
	double adx,ady,aox,aoy;
	void *drgn;
	MTRect dr,r;
	bool ok = true;

	MTDXDevice7 &cdevice = *(MTDXDevice7*)dest->device;
	if ((!&cdevice) || (!cdevice.id3ddev)) return false;
	sx = dx-ox;
	sy = dy-oy;
	fx = (double)ow/dw;
	fy = (double)oh/dh;
	dr.left = dx;
	dr.top = dy;
	dr.right = dx+dw;
	dr.bottom = dy+dh;
	drgn = recttorgn(dr);
	intersectrgn(drgn,dest->getclip());
	offsetrgn(drgn,-dx,-dy);
	adx = (double)dx+displayprefs.texeladjustx;
	ady = (double)dy+displayprefs.texeladjusty;
	aox = (double)ox+displayprefs.skintexeladjustx;
	aoy = (double)oy+displayprefs.skintexeladjusty;
	n = rgngetnrects(drgn);
	if (n){
		if (!cdevice.start(dest)) return false;
		if (n>nv){
			nv = ((n+7)>>3)<<3;
			if (vbuf) vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
			else vbuf = (D3DTLVERTEX*)si->memalloc(sizeof(D3DTLVERTEX)*nv*6,MTM_ZERO);
		}
		else if ((n<nv) && (++nlow==32)){
			nv = n;
			vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
		};
		color1 = swapcolor(color1 & 0xFFFFFF) | (color1 & 0xFF000000);
		color2 = swapcolor(color2 & 0xFFFFFF) | (color2 & 0xFF000000);
		color3 = swapcolor(color3 & 0xFFFFFF) | (color3 & 0xFF000000);
		color4 = swapcolor(color4 & 0xFFFFFF) | (color4 & 0xFF000000);
		D3DTLVERTEX *ct = vbuf;
		for (x=0;x<n;x++,ct+=6){
			rgngetrect(drgn,x,&r);
			cfx = r.left/dw;
			ct[0].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[2].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			cfx = r.right/dw;
			ct[1].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[5].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			ct[0].rhw = ct[1].rhw = ct[2].rhw = ct[5].rhw = 1.0;
			ct[0].sx = adx+r.left+dx;
			ct[0].sy = ady+r.top+dy;
			ct[5].sx = adx+r.right+dx;
			ct[5].sy = ady+r.bottom+dy;
			if (src){
				ct[0].tu = (D3DVALUE)((double)r.left*fx+aox)*src->tfx/src->width;
				ct[0].tv = (D3DVALUE)((double)r.top*fy+aoy)*src->tfy/src->height;
				ct[5].tu = (D3DVALUE)((double)r.right*fx+aox)*src->tfx/src->width;
				ct[5].tv = (D3DVALUE)((double)r.bottom*fy+aoy)*src->tfy/src->height;
			};
			ct[1].sx = ct[5].sx;
			ct[1].sy = ct[0].sy;
			ct[1].tu = ct[5].tu;
			ct[1].tv = ct[0].tv;
			ct[2].sx = ct[0].sx;
			ct[2].sy = ct[5].sy;
			ct[2].tu = ct[0].tu;
			ct[2].tv = ct[5].tv;
			ct[3] = ct[2];
			ct[4] = ct[1];
		};
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
		};*/
		cdevice.id3ddev->SetTexture(0,(src)?src->ddsurf:0);
		ok &= (dxerror(cdevice.id3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,vbuf,n*6,0),__FILE__,__LINE__)==0);
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
		};*/
	};
	deletergn(drgn);
	return ok;
}

bool MTDXBitmap7::mdxbltex(MTDXBitmap7 *dest,int dx,int dy,int dw,int dh,MTDXMask7 *src,int ox,int oy,int ow,int oh,int color1,int color2,int color3,int color4)
{
	int x,n;
	int sx,sy;
	double fx,fy,cfx;
	double adx,ady,aox,aoy;
	void *drgn;
	MTRect dr,r;
	bool ok = true;

	MTDXDevice7 &cdevice = *(MTDXDevice7*)dest->device;
	if ((!&cdevice) || (!cdevice.id3ddev)) return false;
	sx = dx-ox;
	sy = dy-oy;
	fx = (double)ow/dw;
	fy = (double)oh/dh;
	dr.left = dx;
	dr.top = dy;
	dr.right = dx+dw;
	dr.bottom = dy+dh;
	drgn = recttorgn(dr);
	intersectrgn(drgn,dest->getclip());
	offsetrgn(drgn,-dx,-dy);
	adx = (double)dx+displayprefs.texeladjustx;
	ady = (double)dy+displayprefs.texeladjusty;
	aox = (double)ox+displayprefs.skintexeladjustx;
	aoy = (double)oy+displayprefs.skintexeladjusty;
	n = rgngetnrects(drgn);
	if (n){
		if (!cdevice.start(dest)) return false;
		if (n>nv){
			nv = ((n+7)>>3)<<3;
			if (vbuf) vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
			else vbuf = (D3DTLVERTEX*)si->memalloc(sizeof(D3DTLVERTEX)*nv*6,MTM_ZERO);
		}
		else if ((n<nv) && (++nlow==32)){
			nv = n;
			vbuf = (D3DTLVERTEX*)si->memrealloc(vbuf,sizeof(D3DTLVERTEX)*nv*6);
		};
		color1 = swapcolor(color1 & 0xFFFFFF) | (color1 & 0xFF000000);
		color2 = swapcolor(color2 & 0xFFFFFF) | (color2 & 0xFF000000);
		color3 = swapcolor(color3 & 0xFFFFFF) | (color3 & 0xFF000000);
		color4 = swapcolor(color4 & 0xFFFFFF) | (color4 & 0xFF000000);
		D3DTLVERTEX *ct = vbuf;
		for (x=0;x<n;x++,ct+=6){
			rgngetrect(drgn,x,&r);
			cfx = r.left/dw;
			ct[0].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[2].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			cfx = r.right/dw;
			ct[1].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.top/dh);
			ct[5].color = calccolor(calccolor(color1,color2,cfx),calccolor(color3,color4,cfx),(float)r.bottom/dh);
			ct[0].rhw = ct[1].rhw = ct[2].rhw = ct[5].rhw = 1.0;
			ct[0].sx = adx+r.left+dx;
			ct[0].sy = ady+r.top+dy;
			ct[5].sx = adx+r.right+dx;
			ct[5].sy = ady+r.bottom+dy;
			if (src){
				ct[0].tu = (D3DVALUE)((double)r.left*fx+aox)*src->tfx/src->width;
				ct[0].tv = (D3DVALUE)((double)r.top*fy+aoy)*src->tfy/src->height;
				ct[5].tu = (D3DVALUE)((double)r.right*fx+aox)*src->tfx/src->width;
				ct[5].tv = (D3DVALUE)((double)r.bottom*fy+aoy)*src->tfy/src->height;
			};
			ct[1].sx = ct[5].sx;
			ct[1].sy = ct[0].sy;
			ct[1].tu = ct[5].tu;
			ct[1].tv = ct[0].tv;
			ct[2].sx = ct[0].sx;
			ct[2].sy = ct[5].sy;
			ct[2].tu = ct[0].tu;
			ct[2].tv = ct[5].tv;
			ct[3] = ct[2];
			ct[4] = ct[1];
		};
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
		};*/
		cdevice.id3ddev->SetTexture(0,(src)?src->ddsurf:0);
		ok &= (dxerror(cdevice.id3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,D3DVT_TLVERTEX,vbuf,n*6,0),__FILE__,__LINE__)==0);
/*		if ((dw>ow) || (dh>oh)){
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
			cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
		};*/
	};
	deletergn(drgn);
	return ok;
}

inline bool MTDXBitmap7::dxblt(MTDXBitmap7 *dest,RECT *dr,MTDXBitmap7 *src,RECT *sr,int flags,DDBLTFX *fx)
{
	int res;

//	if (dest->texture) return false;
	if ((src) && (src->flags & MTB_SKIN)){
		return dxbltex(dest,dr->left,dr->top,dr->right-dr->left,dr->bottom-dr->top,src,sr->left,sr->top,sr->right-sr->left,sr->bottom-sr->top,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF);
	};
	if ((gi) && (!gi->visible) && (dest==screen) && (di->fullscreen)) return false;
	((MTDXDevice7*)dest->device)->end();
//	if ((dest==src) && (dr->right-dr->left==sr->right-sr->left) && (dr->bottom-dr->top==sr->bottom-sr->top) && (intersectrect(*(MTRect*)sr,*(MTRect*)dr))) return cdxblt(dest,dr,sr,flags,fx);
	switch (res = dest->ddsurf->Blt(dr,(src)?src->ddsurf:0,sr,flags,fx)){
	case 0:
		return true;
	case DDERR_SURFACELOST:
		if (gi) gi->needbitmapcheck();
/*
		if (src){
			if (src->ddsurf->IsLost()==DDERR_SURFACELOST){
				src->ddsurf->Restore();
			};
		};
		if (dest->ddsurf->IsLost()==DDERR_SURFACELOST){
			dest->ddsurf->Restore();
		};
*/
		return false;
	case DDERR_WASSTILLDRAWING:
		flags |= DDBLT_WAIT;
		return (dest->ddsurf->Blt(dr,(src)?src->ddsurf:0,sr,flags,fx)==0);
	default:
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
#endif
		return false;
	};
}
//---------------------------------------------------------------------------
MTDXBitmap7::MTDXBitmap7(MTDisplayDevice *d,int f,int w,int h):
MTGDIBitmap(d,f|MTB_DXBITMAP,w,h),
iddraw(((MTDXDevice7*)d)->iddraw),
iddclip(0),
ddsurf(0),
clipper(0),
bcount(0)
{
	FENTER4("MTDXBitmap7::MTDXBitmap7(%.8X,%d,%d,%d)",d,f,w,h);
	mtmemzero(&desc,sizeof(desc));
	if (flags & MTB_SCREEN){
		desc.dwSize = sizeof(desc);
		desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		iddraw->GetDisplayMode(&desc);
		LOG("Display mode:"NL);
		DUMP(&desc,sizeof(desc),0);
		bitcount = desc.ddpfPixelFormat.dwRGBBitCount;
		if (bitcount==16){
			if ((desc.ddpfPixelFormat.dwRBitMask & 8000)==0) bitcount = 15;
		};
		width = desc.dwWidth;
		height = desc.dwHeight;
		mpitch = desc.lPitch;
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE|DDSCAPS_PRIMARYSURFACE;
//		if (!(cdddev->caps.dwCaps & DDCAPS_NOHARDWARE)) desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		if (dxerror(iddraw->CreateSurface(&desc,&ddsurf,0),__FILE__,__LINE__)) return;
		if ((ddsurf) && (di->fullscreen)){
			ddsurf->PageLock(0);
		};
		dxerror(ddsurf->GetCaps(&caps),__FILE__,__LINE__);
		if (dxerror(iddraw->CreateClipper(0,&clipper,0),__FILE__,__LINE__)) return;
		dxerror(ddsurf->SetClipper(clipper),__FILE__,__LINE__);
	}
	else{
		initialize();
		loaded = load();
	};
	LEAVE();
}

MTDXBitmap7::MTDXBitmap7(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,res,resid,colorkey),
iddraw(((MTDXDevice7*)d)->iddraw),
iddclip(0),
ddsurf(0),
clipper(0),
bcount(0)
{
	FENTER5("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%d,%.6X)",d,f,res,resid,colorkey);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap7::MTDXBitmap7(MTDisplayDevice *d,int f,const char *filename,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,filename,colorkey),
iddraw(((MTDXDevice7*)d)->iddraw),
iddclip(0),
ddsurf(0),
clipper(0),
bcount(0)
{
	FENTER3("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%s)",d,f,filename);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap7::MTDXBitmap7(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey):
MTGDIBitmap(d,f|MTB_DXBITMAP,orig,colorkey),
iddraw(((MTDXDevice7*)d)->iddraw),
iddclip(0),
ddsurf(0),
clipper(0),
bcount(0)
{
	FENTER4("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%.6X)",d,f,&orig,colorkey);
	initialize();
	loaded = load();
	LEAVE();
}

MTDXBitmap7::~MTDXBitmap7()
{
	unload();
	if (clipper) clipper->Release();
	LEAVE();
}

void MTDXBitmap7::initialize()
{
	ENTER("MTDXBitmap7::initialize");
	mdc = 0;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
//		desc.ddsCaps.dwCaps = DDSCAPS_OWNDC;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
	if (!(((MTDXDevice7*)device)->cdev->caps.dwCaps & DDCAPS_NOHARDWARE)) desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
	if (screen) bitcount = screen->bitcount;
	LEAVE();
}

void MTDXBitmap7::unload()
{
	ENTER("MTDXBitmap7::unload");
	loaded = false;
	if (mbits){
		dxerror(ddsurf->Unlock(&lockrect),__FILE__,__LINE__);
		mbits = 0;
	};
	((MTDXDevice7*)device)->end(true);
	if (mask){
		DeleteDC(mmdc);
		DeleteObject(mask);
		si->memfree(mmbits);
		mmdc = 0;
		mask = 0;
		mmbits = 0;
	};
	if (ddsurf){
		if (iddclip){
			iddclip->Release();
			iddclip = 0;
		};
		if (ddsurf){
			if (iddraw) ddsurf->Release();
			ddsurf = 0;
		};
	};
	LEAVE();
}

bool MTDXBitmap7::setsize(int w,int h)
{
	int res;
	bool retried = false;
	DDBLTFX fx;
	
	if ((loaded) && (w==width) && (h==height)) return true;
	FENTER2("MTDXBitmap7::setsize(%d,%d)",w,h);
	unload();
	if ((w>0) && (h>0)){
		width = w;
		height = h;
	};
	wr.left = wr.top = 0;
	wr.right = width;
	wr.bottom = height;
	if (flags & MTB_DRAW){
		if (mrgn) DeleteObject(mrgn);
		mrgn = CreateRectRgnIndirect((RECT*)&wr);
	};
	mdc = 0;
	mbits = 0;
	bcount = 0;
	if (!iddraw){
		LOGD("%s - [Display] ERROR: DirectDraw not initialized!"NL);
		LEAVE();
		return false;
	};
	desc.ddsCaps.dwCaps &= (~(DDSCAPS_VISIBLE|DDSCAPS_WRITEONLY|DDSCAPS_LOCALVIDMEM|DDSCAPS_NONLOCALVIDMEM));
	desc.ddsCaps.dwCaps &= (~(DDSCAPS_SYSTEMMEMORY));
	desc.ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM);
	MTDXDevice7 &cdevice = *(MTDXDevice7*)device;
	if (desc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE){
		desc.dwFlags &= (~(DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT));
	}
	else{
		if (flags & MTB_DRAW) desc.ddsCaps.dwCaps |= (DDSCAPS_OFFSCREENPLAIN|DDSCAPS_3DDEVICE);
		tfx = tfy = 1.0;
		if ((flags & MTB_SKIN) && (cdevice.id3d)){
			desc.ddsCaps.dwCaps &= (~DDSCAPS_OFFSCREENPLAIN);
			desc.dwWidth = 16;
			desc.dwHeight = 16;
			while (desc.dwWidth<width) desc.dwWidth *= 2;
			while (desc.dwHeight<height) desc.dwHeight *= 2;
			if (!cdevice.desc3d.dwDevCaps){
				desc.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;
				tfx = (D3DVALUE)width/desc.dwWidth;
				tfy = (D3DVALUE)height/desc.dwHeight;
			}
			else{
				if ((desc.dwWidth>cdevice.desc3d.dwMaxTextureWidth) || (desc.dwHeight>cdevice.desc3d.dwMaxTextureHeight)){
					desc.dwWidth = width;
					desc.dwHeight = height;
					flags &= (~MTB_SKIN);
				}
				else{
					if (desc.dwWidth<cdevice.desc3d.dwMinTextureWidth) desc.dwWidth = cdevice.desc3d.dwMinTextureWidth;
					if (desc.dwHeight<cdevice.desc3d.dwMinTextureHeight) desc.dwWidth = cdevice.desc3d.dwMinTextureHeight;
					desc.ddsCaps.dwCaps |= DDSCAPS_TEXTURE;
					tfx = (D3DVALUE)width/desc.dwWidth;
					tfy = (D3DVALUE)height/desc.dwHeight;
				};
			};
		}
		else{
			desc.dwWidth = width;
			desc.dwHeight = height;
		};
	};
	goto go;
retry:
	if (retried){
		LEAVE();
		return false;
	};
	retried = true;
go:
	res = iddraw->CreateSurface(&desc,&ddsurf,0);
	switch (res){
	case 0:
		if (flags & MTB_DRAW){
			if ((cdevice.id3d) && (!cdevice.id3ddev)){
				if (cdevice.id3d->CreateDevice(IID_IDirect3DHALDevice,ddsurf,&cdevice.id3ddev)){
					if (cdevice.id3d->CreateDevice(IID_IDirect3DMMXDevice,ddsurf,&cdevice.id3ddev)){
						if (cdevice.id3d->CreateDevice(IID_IDirect3DRGBDevice,ddsurf,&cdevice.id3ddev)){
							LOGD("%s - [Display] Cannot create a Direct3D device!"NL);
						};
					};
				};
				if (cdevice.id3ddev){
					mtmemzero(&cdevice.desc3d,sizeof(cdevice.desc3d));
					cdevice.id3ddev->GetCaps(&cdevice.desc3d);
					cdevice.setbitmap(this);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,1);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,1);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
					cdevice.id3ddev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
				};
			};
			dxerror(iddraw->CreateClipper(0,&iddclip,0),__FILE__,__LINE__);
		};
		if (di->fullscreen) ddsurf->PageLock(0);
		dxerror(ddsurf->GetSurfaceDesc(&desc),__FILE__,__LINE__);
		mpitch = desc.lPitch;
		if (mck>=0){
			DDCOLORKEY ddck;
			ddck.dwColorSpaceLowValue = translatecolor(swapcolor(mck));
			ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
			dxerror(ddsurf->SetColorKey(DDCKEY_SRCBLT,&ddck),__FILE__,__LINE__);
		};
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0;
		dxblt(this,0,0,0,DDBLT_COLORFILL,&fx);
		break;
	case DDERR_TOOBIGWIDTH:
		desc.dwWidth = width;
		desc.dwHeight = height;
		flags &= (~MTB_SKIN);
		desc.ddsCaps.dwCaps &= (~DDSCAPS_TEXTURE);
		tfx = tfy = 1.0;
		goto retry;
	case DDERR_OUTOFVIDEOMEMORY:
		desc.ddsCaps.dwCaps &= (~(DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM|DDSCAPS_NONLOCALVIDMEM));
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		goto retry;
	default:
#ifdef _DEBUG
		dxerror(res,__FILE__,__LINE__);
		LOG("desc:"NL);
		DUMP(&desc,desc.dwSize,0);
#endif
		LEAVE();
		return false;
	};
	if ((bitcount==0) && (screen)) bitcount = screen->bitcount;
	LEAVE();
	return true;
}

void *MTDXBitmap7::open(int wantedtype)
{
	int ret;

	mcount++;
	mtextinit = false;
	if (wantedtype==2) return (void*)ddsurf;
	if (!mdc){
		if ((!ddsurf) || (!iddraw)){
			LOGD("%s - [Display] ERROR: DirectDraw not initialized!"NL);
			return 0;
		};
		((MTDXDevice7*)device)->end();
		ret = ddsurf->GetDC(&mdc);
		if (ret==DDERR_SURFACELOST){
			ret = ddsurf->Restore();
			if (ret==DDERR_WRONGMODE){
				((MTDXDevice7*)device)->reloadbitmaps();
			}
			else if (ret!=0){
				dxerror(ret,__FILE__,__LINE__);
			};
			dxerror(ddsurf->GetDC(&mdc),__FILE__,__LINE__);
		};
		if (ret==0){
			if ((flags & MTB_DRAW) && (clipped)) SelectClipRgn(mdc,crgn[clipped-1]);
		};
	};
	return (void*)mdc;
}

void MTDXBitmap7::close(void *o)
{
	if (!mcount) return;
	mcount--;
	if ((!mcount) && (mdc)){
		if (flags & MTB_DRAW){
			if (clipped) SelectClipRgn(mdc,0);
			DeleteObject(GetCurrentObject(mdc,OBJ_PEN));
			DeleteObject(GetCurrentObject(mdc,OBJ_BRUSH));
		};
		if ((ddsurf) && (iddraw)) dxerror(ddsurf->ReleaseDC((HDC)mdc),__FILE__,__LINE__);
		mdc = 0;
	};
}

void MTDXBitmap7::clip(MTRect *rect)
{
	int cr = clipped++;
	int s;
	RGNDATA *rd;
	
	if ((!iddclip) || (clipped==MAX_CLIP)) return;
	crgn[cr] = CreateRectRgnIndirect((RECT*)rect);
	if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
	s = GetRegionData(crgn[cr],0,0);
	rd = (RGNDATA*)si->memalloc(s,0);
	GetRegionData(crgn[cr],s,rd);
	iddclip->SetClipList(rd,0);
	if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
	si->memfree(rd);
}

void MTDXBitmap7::cliprgn(void *rgn)
{
	int cr = clipped++;
	int s;
	RGNDATA *rd;
	
	if ((!iddclip) || (clipped==MAX_CLIP)) return;
	crgn[cr] = CreateRectRgn(0,0,1,1);
	CombineRgn((HRGN)crgn[cr],(HRGN)rgn,(HRGN)rgn,RGN_COPY);
	if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
	s = GetRegionData(crgn[cr],0,0);
	rd = (RGNDATA*)si->memalloc(s,0);
	GetRegionData(crgn[cr],s,rd);
	iddclip->SetClipList(rd,0);
	if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
	si->memfree(rd);
}

void MTDXBitmap7::unclip()
{
	int cr = --clipped;
	
	if (!iddclip) return;
	if (clipped){
		int s = GetRegionData(crgn[clipped-1],0,0);
		RGNDATA *rd = (RGNDATA*)si->memalloc(s,0);
		GetRegionData(crgn[clipped-1],s,rd);
		iddclip->SetClipList(rd,0);
		if (ddsurf) dxerror(ddsurf->SetClipper(iddclip),__FILE__,__LINE__);
		si->memfree(rd);
	}
	else{
		if (ddsurf) dxerror(ddsurf->SetClipper(0),__FILE__,__LINE__);
		iddclip->SetClipList(0,0);
	};
	DeleteObject(crgn[cr]);
	crgn[cr] = 0;
}

void MTDXBitmap7::setwindow(MTWinControl *window)
{
	int l;
	void *oldwnd = mwnd;
	static int cyc = -1,cysc,cxf,cyf,cxdf,cydf;
	
	FENTER1("MTDXBitmap7::setwindow(%.8X)",window);
	if (window){
		if (window->guiid==MTC_DESKTOP){
			mwnd = ((MTDesktop*)window)->mwnd;
		}
		else{
			mwnd = window->dsk->mwnd;
		};
		if (mwnd!=oldwnd) mox = -1;
		else if (!modified){
			LEAVE();
			return;
		}
		else mox = -1;
		modified = false;
	}
	else{
		if (!mwnd) mox = -1;
		mwnd = 0;
		wr.left = wr.top = 0;
		wr.right = width;
		wr.bottom = height;
		if (clipper) clipper->SetHWnd(0,0);
		LEAVE();
		return;
	};
	if (clipper) clipper->SetHWnd(0,(HWND)mwnd);
	if (di->fullscreen){
		wr.left = wr.top = 0;
		wr.right = desc.dwWidth;
		wr.bottom = desc.dwHeight;
	}
	else{
		GetWindowRect((HWND)mwnd,(RECT*)&wr);
		if (mox<0){
			mox = moy = 0;
			l = GetWindowLong((HWND)mwnd,GWL_STYLE);
			if (cyc<0){
				cyc = GetSystemMetrics(SM_CYCAPTION);
				cysc = GetSystemMetrics(SM_CYSMCAPTION);
				cxf = GetSystemMetrics(SM_CXFRAME);
				cyf = GetSystemMetrics(SM_CYFRAME);
				cxdf = GetSystemMetrics(SM_CXDLGFRAME);
				cydf = GetSystemMetrics(SM_CYDLGFRAME);
			};
			if (l & WS_CAPTION){
				if (GetWindowLong((HWND)mwnd,GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
					moy += cysc;
				else
					moy += cyc;
			};
			if (l & WS_BORDER){
				if (l & WS_THICKFRAME){
					mox += cxf;
					moy += cyf;
				}
				else{
					mox += cxdf;
					moy += cydf;
				};
			};
		};
		wr.left += mox;
		wr.right += mox;
		wr.top += moy;
		wr.bottom += moy;
		mox = wr.left;
		moy = wr.top;
	};
	LEAVE();
}

bool MTDXBitmap7::openbits(MTRect &rect,void **bits,int *pitch,void **maskbits,int *maskpitch)
{
	bool in = false;
	DDSURFACEDESC2 desc;
	MTRect r = rect;
	
	if ((!bits) || (!pitch)) return false;
	r.left = (r.left>>5)<<5;
	r.right = ((r.right+31)>>5)<<5;
	if (r.left<0) r.left = 0;
	if (r.top<0) r.top = 0;
	if (r.right>width) r.right = width;
	if (r.bottom>height) r.bottom = height;
	if ((r.right<=r.left) || (r.bottom<=r.top)) return false;
	if (bcount==0){
		ob = r;
	}
	else{
		if ((r.left<ob.left) || (r.top<ob.top) || (r.right>ob.right) || (r.bottom>ob.bottom)) return false;
		in = true;
	};
	bcount++;
	if ((!ddsurf) || (!iddraw)) return false;
	if (in){
		*bits = (char*)mbits+(rect.left-ob.left)*((bitcount+7)>>3)+(rect.top-ob.top)*mpitch;
		*pitch = mpitch;
		return true;
	};
	((MTDXDevice7*)device)->end();
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	lockrect = *(RECT*)&r;
	if (dxerror(ddsurf->Lock(&lockrect,&desc,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,0),__FILE__,__LINE__)==0){
		mbits = desc.lpSurface;
		mpitch = desc.lPitch;
		*bits = (char*)mbits+(rect.left-r.left)*((bitcount+7)>>3)+(rect.top-r.top)*mpitch;
		*pitch = mpitch;
		if (maskbits){
			*maskbits = 0;
			*maskpitch = screen->translatecolor(mck);
		};
		return true;
	};
	return false;
}

void MTDXBitmap7::closebits()
{
	if (--bcount!=0) return;
	if (mbits){
		dxerror(ddsurf->Unlock(&lockrect),__FILE__,__LINE__);
		mbits = 0;
	};
}

bool MTDXBitmap7::blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode)
{
	MTDXBitmap7 *dxdest = (MTDXBitmap7*)dest;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	bool tr = (mck>=0);
	DDBLTFX fx;
	
	if ((!dxdest->ddsurf) || (!ddsurf)) return false;
//	((MTDXDevice7*)device)->end();
	if ((w==0) && (h==0)){
		w = width;
		h = height;
	};
	RECT dr = {dx,dy,dx+w,dy+h};
	RECT or = {ox,oy,ox+w,oy+h};
	if (or.left<0){
		dr.left -= or.left;
		or.left = 0;
	};
	if (or.top<0){
		dr.top -= or.top;
		or.top = 0;
	};
	if (or.right>width){
		dr.right -= (or.right-width);
		or.right = width;
	};
	if (or.bottom>height){
		dr.bottom -= (or.bottom-height);
		or.bottom = height;
	};
	if (dr.left<0){
		or.left -= dr.left;
		dr.left = 0;
	};
	if (dr.top<0){
		or.top -= dr.top;
		dr.top = 0;
	};
	if (dr.right>dest->width){
		or.right -= (dr.right-dest->width);
		dr.right = dest->width;
	};
	if (dr.bottom>dest->height){
		or.bottom -= (dr.bottom-dest->height);
		dr.bottom = dest->height;
	};
	if ((or.bottom<=or.top) || (or.right<=or.left)) return false;
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fxflags = DDBLT_KEYSRC;
	};
	if (mode!=MTBM_COPY){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fx.dwROP = bltmode[mode];
		fxflags |= DDBLT_ROP;
	};
	return dxblt(dxdest,&dr,this,&or,fxflags,fxp);
}

bool MTDXBitmap7::sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode)
{
	MTDXBitmap7 *dxdest = (MTDXBitmap7*)dest;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	bool tr = (mck>=0);
	DDBLTFX fx;
	
	if ((!dxdest->ddsurf) || (!ddsurf)) return false;
//	((MTDXDevice7*)device)->end();
	RECT dr = {dx,dy,dx+dw,dy+dh};
	RECT or = {ox,oy,ox+ow,oy+oh};
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxp = &fx;
		fxflags = DDBLT_KEYSRC;
	};
	if (mode!=MTBM_COPY){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwROP = bltmode[mode];
		fxp = &fx;
		fxflags |= DDBLT_ROP;
	};
	return dxblt(dxdest,&dr,this,&or,fxflags,fxp);
}

bool MTDXBitmap7::skinblt(int x,int y,int w,int h,MTSkinPart &o,int color)
{
	if (w==0) w = o.b.w;
	if (h==0) h = o.b.h;
	int ow,oh,cw,ch;
	int oldx = x;
	int oldw = w;
	bool tr;
	bool ok = true;
	MTDXBitmap7 *skinbmp = (MTDXBitmap7*)::skinbmp[o.bmpid & 0xF];
	
	if (!ddsurf) return false;
	if ((w==0) || (h==0)) return true;
	DDBLTFX fx;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	RECT dr = {x,y,x+w,y+h};
	RECT or = {o.b.x,o.b.y,o.b.x+o.b.w,o.b.y+o.b.h};
	mtmemzero(&fx,sizeof(fx));
	fx.dwSize = sizeof(fx);
	if (o.mode==SKIN_COLOR){
		if (color!=0xFFFFFF) color = combinecolor(color,o.color);
		else color = o.color;
		fx.dwFillColor = screen->translatecolor(swapcolor(color));
		return dxblt(this,&dr,0,0,DDBLT_COLORFILL,&fx);
	};
	if ((!skinbmp) || (!skinbmp->ddsurf)) return false;
	tr = ((o.flags & SKIN_TRANSP) && (skinbmp->mck>=0));
	if (tr){
		fxflags = DDBLT_KEYSRC;
		fxp = &fx;
	};
	if ((o.b.w==w) && (o.b.h==h)) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
	else{
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<o.b.w))) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			ow = o.b.w;
			oh = o.b.h;
			if ((!ow) || (!oh)) return true;
			while (h>0){
				if (h>oh) ch = oh;
				else{
					or.bottom = or.top+h;
					ch = h;
				};
				while (w>0){
					if (w>ow) cw = ow;
					else{
						or.right = or.left+w;
						cw = w;
					};
					dr.right = dr.left+cw;
					dr.bottom = dr.top+ch;
					ok &= dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
					dr.left += cw;
					dr.right += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				or.right = or.left+o.b.w;
				dr.left = x;
				dr.right = x+w;
				dr.top += ch;
				dr.bottom += ch;
				h -= ch;
			};
			return ok;
		};
	};
}

bool MTDXBitmap7::skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color)
{
	int ow,oh,cw,ch;
	int oldx;
	int oldw;
	int aw = o.b.w/nx;
	int ah = o.b.h/ny;
	int caw,cah;
	bool tr;
	bool ok = true;
	MTDXBitmap7 *skinbmp = (MTDXBitmap7*)::skinbmp[o.bmpid & 0xF];
	
	if (!skinbmp) return false;
	if ((w|h)==0){
		w = aw;
		h = ah;
	}
	else if ((w==0) || (h==0)) return true;
	oldx = x;
	oldw = w;
	tr = ((o.flags & SKIN_TRANSP) && (skinbmp->mck>=0));
	caw = (step%nx)*aw;
	cah = (step/nx)*ah;
	if ((!ddsurf) || (!skinbmp->ddsurf)) return false;
	DDBLTFX fx;
	DDBLTFX *fxp = 0;
	int fxflags = 0;
	if (color!=0xFFFFFF){
		color |= 0xFF000000;
		if (dxbltex(this,x,y,w,h,skinbmp,o.b.x+caw,o.b.y+cah,w,h,color,color,color,color)) return true;
	};
	RECT dr = {x,y,x+w,y+h};
	RECT or = {o.b.x+caw,o.b.y+cah,o.b.x+caw+aw,o.b.y+cah+ah};
	if (tr){
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fxflags = DDBLT_KEYSRC;
		fxp = &fx;
	};
	if ((aw==w) && (ah==h)) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
	else{
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<aw))) return dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			ok = true;
			ow = aw;
			oh = ah;
			if ((!ow) || (!oh)) return true;
			while (h>0){
				if (h>oh) ch = oh;
				else{
					or.bottom = or.top+h;
					ch = h;
				};
				while (w>0){
					if (w>ow) cw = ow;
					else{
						or.right = or.left+w;
						cw = w;
					};
					dr.right = dr.left+cw;
					dr.bottom = dr.top+ch;
					ok &= dxblt(this,&dr,skinbmp,&or,fxflags,fxp);
					dr.left += cw;
					dr.right += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				or.right = or.left+o.b.w;
				dr.left = x;
				dr.right = x+w;
				dr.top += ch;
				dr.bottom += ch;
				h -= ch;
			};
			return ok;
		};
	};
}

bool MTDXBitmap7::maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o)
{
	bool ok = MTGDIBitmap::maskblt(mask,x,y,w,h,o);
	if ((ok) && (o.mode!=SKIN_COLOR)) ((MTDXMask7*)mask)->getbits();
	return ok;
}

bool MTDXBitmap7::shade(int x,int y,int w,int h,MTMask *mask,int mx,int my)
{
	MTDXMask7 *dm = (MTDXMask7*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask7*)mask,mx,my,w,h,0,0,0,0)) return false/*MTGDIBitmap::shade(x,y,w,h,mask,mx,my)*/;
	return true;
}

bool MTDXBitmap7::tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
//FIXME
	MTDXMask7 *dm = (MTDXMask7*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask7*)mask,mx,my,mw,mh,0,0,0,0)) return false/*MTGDIBitmap::tshade(x,y,w,h,mask,mx,my,mw,mh)*/;
	return true;
}

bool MTDXBitmap7::sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
	MTDXMask7 *dm = (MTDXMask7*)mask;
	if (!mdxbltex(this,x,y,w,h,(MTDXMask7*)mask,mx,my,mw,mh,0,0,0,0)) return false/*MTGDIBitmap::sshade(x,y,w,h,mask,mx,my,mw,mh)*/;
	return true;
}

bool MTDXBitmap7::blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity)
{
	int color = (opacity<<24)|0xFFFFFF;

	if (dxbltex((MTDXBitmap7*)dest,dx,dy,w,h,(MTDXBitmap7*)this,ox,oy,w,h,color,color,color,color)) return true;
	return MTGDIBitmap::blendblt(dest,dx,dy,w,h,ox,oy,opacity);
}

bool MTDXBitmap7::fill(int x,int y,int w,int h,int color,int opacity)
{
	DDBLTFX fx;
	int fxflags = DDBLT_COLORFILL;
	
	if ((w==0) || (h==0)) return true;
	if (!ddsurf) return false;
	if (opacity<255){
		color = color|(opacity<<24);
		if (dxbltex(this,x,y,w,h,0,0,0,0,0,color,color,color,color)) return true;
		return MTGDIBitmap::fill(x,y,w,h,color,opacity);
	};
	RECT dr = {x,y,x+w,y+h};
	mtmemzero(&fx,sizeof(fx));
	fx.dwSize = sizeof(fx);
	fx.dwFillColor = screen->translatecolor(swapcolor(color));
	return dxblt(this,&dr,0,0,fxflags,&fx);
}
//---------------------------------------------------------------------------
// MTDXMask
//---------------------------------------------------------------------------
MTDXMask::MTDXMask(MTDisplayDevice *d,int w,int h):
MTGDIMask(d,w,h),
ddsurf(0),
ddsurf3(0),
texture(0),
htex(0)
{
	DDBLTFX fx;

	FENTER3("MTDXMask::MTDXMask(%.8X,%d,%d)",d,w,h);
	MTDXDevice &cdevice = *(MTDXDevice*)device;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps |= (DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM);
	desc.dwWidth = 16;
	desc.dwHeight = 16;
	DDPIXELFORMAT &pf = desc.ddpfPixelFormat;
	DDPIXELFORMAT &opf = ((MTDXBitmap*)screen)->desc.ddpfPixelFormat;
	pf = opf;
	pf.dwFlags |= DDPF_ALPHAPIXELS;
	pf.dwRGBAlphaBitMask = 0xFF000000;
	while (desc.dwWidth<width) desc.dwWidth *= 2;
	while (desc.dwHeight<height) desc.dwHeight *= 2;
	if (!cdevice.desc3d.dwSize){
		tfx = (D3DVALUE)width/desc.dwWidth;
		tfy = (D3DVALUE)height/desc.dwHeight;
	}
	else{
		if ((desc.dwWidth>cdevice.desc3d.dwMaxTextureWidth) || (desc.dwHeight>cdevice.desc3d.dwMaxTextureHeight)){
			return;
		}
		else{
			if (desc.dwWidth<cdevice.desc3d.dwMinTextureWidth) desc.dwWidth = cdevice.desc3d.dwMinTextureWidth;
			if (desc.dwHeight<cdevice.desc3d.dwMinTextureHeight) desc.dwWidth = cdevice.desc3d.dwMinTextureHeight;
			tfx = (D3DVALUE)width/desc.dwWidth;
			tfy = (D3DVALUE)height/desc.dwHeight;
		};
	};
	if (dxerror(cdevice.iddraw->CreateSurface(&desc,&ddsurf,0),__FILE__,__LINE__)==0){
		ddsurf->QueryInterface(IID_IDirectDrawSurface3,(void**)&ddsurf3);
		if (ddsurf3){
			if (cdevice.id3d){
				ddsurf3->QueryInterface(IID_IDirect3DTexture2,(void**)&texture);
				if (cdevice.id3ddev) texture->GetHandle(cdevice.id3ddev,&htex);
			};
		};
		dxerror(ddsurf->GetSurfaceDesc(&desc),__FILE__,__LINE__);
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0xFFFFFFFF;
		dxerror(ddsurf->Blt(0,0,0,DDBLT_COLORFILL,&fx),__FILE__,__LINE__);
	};
	LEAVE();	
}

MTDXMask::~MTDXMask()
{
	if (ddsurf){
		if (texture){
			texture->Release();
			texture = 0;
		};
		if (ddsurf3){
			ddsurf3->Release();
			ddsurf3 = 0;
		};
		if (ddsurf){
			ddsurf->Release();
			ddsurf = 0;
		};
	};
}

void MTDXMask::blur(int amount)
{
	if (!amount) return;
	MTGDIMask::blur(amount);
	getbits();
}

void MTDXMask::opacity(int amount)
{
	MTGDIMask::opacity(amount);
	getbits();
}

void MTDXMask::fill(int x,int y,int w,int h,int amount)
{
	MTGDIMask::fill(x,y,w,h,amount);
	getbits();
}

void MTDXMask::getbits()
{
	if (!ddsurf) return;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	if (dxerror(ddsurf->Lock(0,&desc,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,0),__FILE__,__LINE__)==0){
		a_putalpha(desc.lpSurface,mbits,desc.lPitch,mpitch,width,height,desc.ddpfPixelFormat.dwRGBBitCount);
		dxerror(ddsurf->Unlock(desc.lpSurface),__FILE__,__LINE__);
	};
}

int MTDXMask::gettexture()
{
	MTDXDevice &cdevice = *(MTDXDevice*)device;

	if ((!texture) || (!cdevice.id3ddev)) return 0;
	if (htex) return htex;
	if (texture->GetHandle(cdevice.id3ddev,&htex)==0){
		if (!htex) htex = -1;
	}
	else htex = -1;
	return htex;
}
//---------------------------------------------------------------------------
// MTDXMask7
//---------------------------------------------------------------------------
MTDXMask7::MTDXMask7(MTDisplayDevice *d,int w,int h):
MTGDIMask(d,w,h),
ddsurf(0)
{
	DDBLTFX fx;

	FENTER3("MTDXMask7::MTDXMask7(%.8X,%d,%d)",d,w,h);
	MTDXDevice7 &cdevice = *(MTDXDevice7*)device;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps |= (DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM);
	desc.dwWidth = 16;
	desc.dwHeight = 16;
	DDPIXELFORMAT &pf = desc.ddpfPixelFormat;
	DDPIXELFORMAT &opf = ((MTDXBitmap7*)screen)->desc.ddpfPixelFormat;
	pf = opf;
	pf.dwFlags |= DDPF_ALPHAPIXELS;
	pf.dwRGBAlphaBitMask = 0xFF000000;
	while (desc.dwWidth<width) desc.dwWidth *= 2;
	while (desc.dwHeight<height) desc.dwHeight *= 2;
	if (!cdevice.desc3d.dwDevCaps){
		tfx = (D3DVALUE)width/desc.dwWidth;
		tfy = (D3DVALUE)height/desc.dwHeight;
	}
	else{
		if ((desc.dwWidth>cdevice.desc3d.dwMaxTextureWidth) || (desc.dwHeight>cdevice.desc3d.dwMaxTextureHeight)){
			return;
		}
		else{
			if (desc.dwWidth<cdevice.desc3d.dwMinTextureWidth) desc.dwWidth = cdevice.desc3d.dwMinTextureWidth;
			if (desc.dwHeight<cdevice.desc3d.dwMinTextureHeight) desc.dwWidth = cdevice.desc3d.dwMinTextureHeight;
			tfx = (D3DVALUE)width/desc.dwWidth;
			tfy = (D3DVALUE)height/desc.dwHeight;
		};
	};
	if (dxerror(cdevice.iddraw->CreateSurface(&desc,&ddsurf,0),__FILE__,__LINE__)==0){
		dxerror(ddsurf->GetSurfaceDesc(&desc),__FILE__,__LINE__);
		mtmemzero(&fx,sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0xFFFFFFFF;
		dxerror(ddsurf->Blt(0,0,0,DDBLT_COLORFILL,&fx),__FILE__,__LINE__);
	};
	LEAVE();	
}

MTDXMask7::~MTDXMask7()
{
	if (ddsurf){
		ddsurf->Release();
		ddsurf = 0;
	};
}

void MTDXMask7::blur(int amount)
{
	if (!amount) return;
	MTGDIMask::blur(amount);
	getbits();
}

void MTDXMask7::opacity(int amount)
{
	MTGDIMask::opacity(amount);
	getbits();
}

void MTDXMask7::fill(int x,int y,int w,int h,int amount)
{
	MTGDIMask::fill(x,y,w,h,amount);
	getbits();
}

void MTDXMask7::getbits()
{
	if (!ddsurf) return;
	mtmemzero(&desc,sizeof(desc));
	desc.dwSize = sizeof(desc);
	if (dxerror(ddsurf->Lock(0,&desc,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,0),__FILE__,__LINE__)==0){
		a_putalpha(desc.lpSurface,mbits,desc.lPitch,mpitch,width,height,desc.ddpfPixelFormat.dwRGBBitCount);
		dxerror(ddsurf->Unlock(0),__FILE__,__LINE__);
	};
}
//---------------------------------------------------------------------------
#endif
