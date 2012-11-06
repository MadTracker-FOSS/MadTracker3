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
#include <windows.h>
#include <stdio.h>
#include "MTDisplay1.h"
#include "MTDXDevice.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
HMODULE hddraw;
HRESULT (WINAPI *ddcreate)(LPGUID,LPVOID*,IUnknown*);
HRESULT (WINAPI *ddcreateex)(LPGUID,LPVOID*,REFIID,IUnknown*);
HRESULT (WINAPI *ddenum)(LPDDENUMCALLBACK,LPVOID);

void (WINAPI *switchwindow)(HWND,BOOL);

DDDevice *dddev[32];
int ndddev;
int ddver = 3;
const GUID IID_IDirectDraw2 = {0xB3A6F3E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56};
const GUID IID_IDirectDraw7 = {0x15E65EC0,0x3B9C,0x11D2,0xB9,0x2F,0x00,0x60,0x97,0x97,0xEA,0x5B};
const GUID IID_IDirectDrawSurface3 = {0xDA044E00,0x69B2,0x11D0,0xA1,0xD5,0x00,0xAA,0x00,0xB8,0xDF,0xBB};
const GUID IID_IDirectDrawSurface7 = {0x06675A80,0x3B9B,0x11D2,0xB9,0x2F,0x00,0x60,0x97,0x97,0xEA,0x5B};
const GUID IID_IDirect3D2 = {0x6AAE1EC1,0x662A,0x11D0,0x88,0x9D,0x00,0xAA,0x00,0xBB,0xB7,0x6A};
//const GUID IID_IDirect3D3 = {0xBB223240,0xE72B,0x11D0,0xA9,0xB4,0x00,0xAA,0x00,0xC0,0x99,0x3E};
const GUID IID_IDirect3D7 = {0xF5049E77,0x4861,0x11D2,0xA4,0x7,0x0,0xA0,0xC9,0x6,0x29,0xA8};
const GUID IID_IDirect3DTexture = {0x2CDCD9E0,0x25A0,0x11CF,0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56};
const GUID IID_IDirect3DTexture2 = {0x93281502, 0x8CF8,0x11D0,0x89,0xAB,0x0,0xA0,0xC9,0x5,0x41,0x29};
const GUID IID_IDirect3DRampDevice = {0xF2086B20,0x259F,0x11CF,0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56};
const GUID IID_IDirect3DRGBDevice = {0xA4665C60,0x2673,0x11CF,0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56};
const GUID IID_IDirect3DHALDevice = {0x84E63DE0,0x46AA,0x11CF,0x81,0x6F,0x00,0x00,0xC0,0x20,0x15,0x6E};
const GUID IID_IDirect3DMMXDevice = {0x881949A1,0xD6F3,0x11D0,0x89,0xAB,0x00,0xA0,0xC9,0x05,0x41,0x29};
//---------------------------------------------------------------------------
#ifdef _DEBUG
char cres[256];

int dxerror(unsigned int code,const char *file,const int line)
{
	if (!code) return 0;
	if ((code>>16)==0x8876) sprintf(cres,"%%s - [Display] ERROR: Unexpected DirectDraw error %d! (File %s at line %d)"NL,code & 0xFFFF,file,line);
	else sprintf(cres,"%%s - [Display] ERROR: Unexpected DirectDraw error %.8X! (File %s at line %d)"NL,code,file,line);
	LOGD(cres);
	return code;
}
#else
#define dxerror(C,F,L) (C)
#endif
//---------------------------------------------------------------------------
MTDXDevice::MTDXDevice(DDDevice *dev):
MTDisplayDevice(),
cdev(dev),
started(false),
iddraw(0),
id3d(0),
id3ddev(0),
view(0),
lastb(0)
{
	mtmemzero(&desc3d,sizeof(desc3d));
	mtmemzero(&viewport,sizeof(viewport));
	viewport.dwSize = sizeof(viewport);
	viewport.dwWidth = 640;
	viewport.dwHeight = 480;
	viewport.dvScaleX = 320;
	viewport.dvScaleY = 240;
	viewport.dvMaxX = viewport.dvMaxX = 1.0;
	dxerror(ddcreate(cdev->guid,(void**)&iddraw,0),__FILE__,__LINE__);
	if (iddraw){
		iddraw->QueryInterface(IID_IDirect3D2,(void**)&id3d);
	};
}

MTDXDevice::~MTDXDevice()
{
	if (id3ddev){
		if (view){
			view->Release();
			view = 0;
		};
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
	};
	if (id3d){
		id3d->Release();
		id3d = 0;
	};
	if (iddraw){
		iddraw->Release();
		iddraw = 0;
	};
}

bool MTDXDevice::init(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk = 0;
	bool ok = true;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTDXDevice::init(%d)",fullscreen);
	end();
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if (wnd){
		dsk = GetWindowLong(wnd,GWL_USERDATA);
		SetWindowLong(wnd,GWL_USERDATA,0);
	};
	if (!ndddev) goto fail;
	if (!iddraw) goto fail;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	fullscreen &= ((ndesktops<=1) && (wnd));
	di->fullscreen = fullscreen;
	if (id3ddev){
		if (view){
			view->Release();
			view = 0;
		};
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
		lastb = 0;
	};
	if (fullscreen){
		if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
		SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
		SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
		SetWindowPos(wnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
		BringWindowToTop(wnd);
		if (switchwindow) switchwindow(wnd,true);
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN),__FILE__,__LINE__)) goto fail;
	}
	else{
		if (wasfullscreen){
			if (wnd){
				SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
				SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
				if (gi) gi->restorewindowstate(wnd);
				BringWindowToTop(wnd);
				if (switchwindow) switchwindow(wnd,true);
			};
		};
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__)) goto fail;
	};
success:
	if (dsk) SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return ok;
fail:
	ok = false;
	goto success;
}

void MTDXDevice::uninit()
{
	HWND wnd = 0;
	
	if (vbuf){
		si->memfree(vbuf);
		vbuf = 0;
		nv = nlow = 0;
	};
	if (id3ddev){
		end(true);
		if (view){
			view->Release();
			view = 0;
		};
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
	};
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__);
}

MTBitmap* MTDXDevice::newbitmap(int flags,int width,int height)
{
	MTBitmap *newbmp = new MTDXBitmap(this,flags,width,height);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice::newresbitmap(int flags,MTResources *res,int id,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap(this,flags,res,id,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice::newfilebitmap(int flags,const char *filename,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap(this,flags,filename,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice::newbmpbitmap(int flags,MTBitmap &orig,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap(this,flags,orig,colorkey);
	add(newbmp);
	return newbmp;
}

void MTDXDevice::delbitmap(MTBitmap *bitmap)
{
	remove(bitmap);
	delete (MTDXBitmap*)bitmap;
}

MTMask* MTDXDevice::newmask(int w,int h)
{
	MTDXMask *newmask = new MTDXMask(this,w,h);
	add(newmask);
	return newmask;
}

void MTDXDevice::delmask(MTMask *mask)
{
	remove(mask);
	delete (MTDXMask*)mask;
}

bool MTDXDevice::setfullscreen(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk = 0;
	bool ok = true;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTDXDevice::setfullscreen(%d)",fullscreen);
	end(true);
	lastb = 0;
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if (wnd){
		dsk = GetWindowLong(wnd,GWL_USERDATA);
		SetWindowLong(wnd,GWL_USERDATA,0);
	};
	if (!ndddev) goto fail;
	if (!iddraw) goto fail;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	fullscreen &= ((ndesktops<=1) && (wnd));
	di->fullscreen = fullscreen;
	if (id3ddev){
		if (view){
			view->Release();
			view = 0;
		};
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
		lastb = 0;
	};
	if (fullscreen){
		if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
		SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
		SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
		SetWindowPos(wnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
		BringWindowToTop(wnd);
		if (switchwindow) switchwindow(wnd,true);
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN),__FILE__,__LINE__)) goto fail;
	}
	else{
		if (wasfullscreen){
			if (wnd){
				SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
				SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
				if (gi) gi->restorewindowstate(wnd);
				BringWindowToTop(wnd);
				if (switchwindow) switchwindow(wnd,true);
			};
		};
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__)) goto fail;
	};
success:
	if (dsk) SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return ok;
fail:
	ok = false;
	goto success;
}

void MTDXDevice::checkbitmaps()
{
	int x;

	lastb = 0;
	if (nbitmaps<1) return;
	ENTER("MTDXDevice::checkbitmaps");
	MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[0];
	if (cb.ddsurf){
		if (cb.ddsurf->IsLost()==DDERR_SURFACELOST){
			x = cb.ddsurf->Restore();
			if (x==DDERR_WRONGMODE){
				reloadbitmaps();
				LEAVE();
				return;
			};
			if (x==0){
//				if (cb.texture) cb.resettexture();
				cb.load();
			};
		};
	};
	for (x=1;x<nbitmaps;x++){
		MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
		if (!cb.ddsurf) continue;
		if (cb.ddsurf->IsLost()==DDERR_SURFACELOST){
			if (cb.ddsurf->Restore()==0){
//				if (cb.texture) cb.resettexture();
				cb.load();
			};
		};
	};
	LEAVE();
}

void MTDXDevice::reloadbitmaps()
{
	int x;
	IDirectDrawClipper *clipper[32];

	ENTER("MTDXDevice::reloadbitmaps");
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
		if (!cb.ddsurf) continue;
		if (cb.ddsurf->GetClipper(&clipper[x])){
			clipper[x] = 0;
		}
		else{
			cb.ddsurf->SetClipper(0);
		};
		cb.unload();
	};
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
		cb.setsize(cb.width,cb.height);
		if (cb.load()){
			if (clipper[x]) dxerror(cb.ddsurf->SetClipper(clipper[x]),__FILE__,__LINE__);
		};
	};
	LEAVE();
}

void MTDXDevice::setfocus(bool focused)
{
	int x;

	if (!di->fullscreen) return;
	MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[0];
	if ((!&cb) || (!cb.ddsurf3)) return;
	if (focused){
		for (x=0;x<nbitmaps;x++){
			MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
			if (!cb.ddsurf3) continue;
			cb.ddsurf3->PageLock(0);
		};
	}
	else{
		for (x=0;x<nbitmaps;x++){
			MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
			if (!cb.ddsurf3) continue;
			cb.ddsurf3->PageUnlock(0);
		};
	};
}

void MTDXDevice::sync()
{
	if (iddraw) iddraw->WaitForVerticalBlank(DDWAITVB_BLOCKEND,0);
}

void MTDXDevice::setbitmap(MTDXBitmap *b)
{
	int x;

	if (lastb==b) return;
	end();
	lastb = b;
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap &cb = *(MTDXBitmap*)bitmaps[x];
		if (cb.flags & MTB_SKIN) cb.htex = 0;
	};
	for (x=0;x<nmasks;x++){
		MTDXMask &cm = *(MTDXMask*)masks[x];
		cm.htex = 0;
	};
	id3ddev->SetRenderTarget(b->ddsurf,0);
	viewport.dwWidth = b->width;
	viewport.dwHeight = b->height;
	if (!view){
		if (id3d->CreateViewport(&view,0)==0){
			id3ddev->AddViewport(view);
		};
	};
	if (!view) return;
	if (view->SetViewport(&viewport)==0){
		id3ddev->SetCurrentViewport(view);
	};
}

bool MTDXDevice::start(MTDXBitmap *b)
{
	setbitmap(b);
	if ((!started) && (id3ddev)){
		if (dxerror(id3ddev->BeginScene(),__FILE__,__LINE__)==0) started = true;
	};
	return started;
}

void MTDXDevice::end(bool reset)
{
	if (started){
		if (id3ddev) dxerror(id3ddev->EndScene(),__FILE__,__LINE__);
		started = false;
	};
	if (reset) lastb = 0;
}
//---------------------------------------------------------------------------
MTDXDevice7::MTDXDevice7(DDDevice *dev):
MTDisplayDevice(),
cdev(dev),
started(false),
iddraw(0),
id3d(0),
id3ddev(0),
lastb(0)
{
	mtmemzero(&desc3d,sizeof(desc3d));
	mtmemzero(&viewport,sizeof(viewport));
	viewport.dwWidth = 640;
	viewport.dwHeight = 480;
	dxerror(ddcreateex(cdev->guid,(void**)&iddraw,IID_IDirectDraw7,0),__FILE__,__LINE__);
	if (iddraw){
		iddraw->QueryInterface(IID_IDirect3D7,(void**)&id3d);
	};
}

MTDXDevice7::~MTDXDevice7()
{
	if (id3ddev){
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
	};
	if (id3d){
		id3d->Release();
		id3d = 0;
	};
	if (iddraw){
		iddraw->Release();
		iddraw = 0;
	};
}

bool MTDXDevice7::init(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk = 0;
	bool ok = true;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTDXDevice7::init(%d)",fullscreen);
	end();
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if (wnd){
		dsk = GetWindowLong(wnd,GWL_USERDATA);
		SetWindowLong(wnd,GWL_USERDATA,0);
	};
	if (!ndddev) goto fail;
	if (!iddraw) goto fail;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	fullscreen &= ((ndesktops<=1) && (wnd));
	di->fullscreen = fullscreen;
	if (id3ddev){
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
		lastb = 0;
	};
	if (fullscreen){
		if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
		SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
		SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
		SetWindowPos(wnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
		BringWindowToTop(wnd);
		if (switchwindow) switchwindow(wnd,true);
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN),__FILE__,__LINE__)) goto fail;
	}
	else{
		if (wasfullscreen){
			if (wnd){
				SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
				SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
				if (gi) gi->restorewindowstate(wnd);
				BringWindowToTop(wnd);
				if (switchwindow) switchwindow(wnd,true);
			};
		};
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__)) goto fail;
	};
success:
	if (dsk) SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return ok;
fail:
	ok = false;
	goto success;
}

void MTDXDevice7::uninit()
{
	HWND wnd = 0;
	
	if (vbuf){
		si->memfree(vbuf);
		vbuf = 0;
		nv = nlow = 0;
	};
	if (id3ddev){
		end(true);
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
	};
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__);
}

MTBitmap* MTDXDevice7::newbitmap(int flags,int width,int height)
{
	MTBitmap *newbmp = new MTDXBitmap7(this,flags,width,height);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice7::newresbitmap(int flags,MTResources *res,int id,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap7(this,flags,res,id,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice7::newfilebitmap(int flags,const char *filename,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap7(this,flags,filename,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTDXDevice7::newbmpbitmap(int flags,MTBitmap &orig,int colorkey)
{
	MTBitmap *newbmp = new MTDXBitmap7(this,flags,orig,colorkey);
	add(newbmp);
	return newbmp;
}

void MTDXDevice7::delbitmap(MTBitmap *bitmap)
{
	remove(bitmap);
	delete (MTDXBitmap7*)bitmap;
}

MTMask* MTDXDevice7::newmask(int w,int h)
{
	MTDXMask7 *newmask = new MTDXMask7(this,w,h);
	add(newmask);
	return newmask;
}

void MTDXDevice7::delmask(MTMask *mask)
{
	remove(mask);
	delete (MTDXMask7*)mask;
}

bool MTDXDevice7::setfullscreen(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk = 0;
	bool ok = true;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTDXDevice7::setfullscreen(%d)",fullscreen);
	end(true);
	lastb = 0;
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	if (wnd){
		dsk = GetWindowLong(wnd,GWL_USERDATA);
		SetWindowLong(wnd,GWL_USERDATA,0);
	};
	if (!ndddev) goto fail;
	if (!iddraw) goto fail;
	if ((gi) && (!wnd)) wnd = (HWND)gi->getappwindow();
	fullscreen &= ((ndesktops<=1) && (wnd));
	di->fullscreen = fullscreen;
	if (id3ddev){
		if (id3ddev){
			id3ddev->Release();
			id3ddev = 0;
		};
		lastb = 0;
	};
	if (fullscreen){
		if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
		SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
		SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
		SetWindowPos(wnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
		BringWindowToTop(wnd);
		if (switchwindow) switchwindow(wnd,true);
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN),__FILE__,__LINE__)) goto fail;
	}
	else{
		if (wasfullscreen){
			if (wnd){
				SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
				SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
				if (gi) gi->restorewindowstate(wnd);
				BringWindowToTop(wnd);
				if (switchwindow) switchwindow(wnd,true);
			};
		};
		if (dxerror(iddraw->SetCooperativeLevel(wnd,DDSCL_NORMAL),__FILE__,__LINE__)) goto fail;
	};
success:
	if (dsk) SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return ok;
fail:
	ok = false;
	goto success;
}

void MTDXDevice7::checkbitmaps()
{
	int x;

	lastb = 0;
	if (nbitmaps<1) return;
	ENTER("MTDXDevice7::checkbitmaps");
	MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[0];
	if (cb.ddsurf){
		if (cb.ddsurf->IsLost()==DDERR_SURFACELOST){
			x = cb.ddsurf->Restore();
			if (x==DDERR_WRONGMODE){
				reloadbitmaps();
				LEAVE();
				return;
			};
			if (x==0){
//				if (cb.texture) cb.resettexture();
				cb.load();
			};
		};
	};
	for (x=1;x<nbitmaps;x++){
		MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
		if (!cb.ddsurf) continue;
		if (cb.ddsurf->IsLost()==DDERR_SURFACELOST){
			if (cb.ddsurf->Restore()==0){
//				if (cb.texture) cb.resettexture();
				cb.load();
			};
		};
	};
	LEAVE();
}

void MTDXDevice7::reloadbitmaps()
{
	int x;
	IDirectDrawClipper *clipper[32];

	ENTER("MTDXDevice7::reloadbitmaps");
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
		if (!cb.ddsurf) continue;
		if (cb.ddsurf->GetClipper(&clipper[x])){
			clipper[x] = 0;
		}
		else{
			cb.ddsurf->SetClipper(0);
		};
		cb.unload();
	};
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
		cb.setsize(cb.width,cb.height);
		if (cb.load()){
			if (clipper[x]) dxerror(cb.ddsurf->SetClipper(clipper[x]),__FILE__,__LINE__);
		};
	};
	LEAVE();
}

void MTDXDevice7::setfocus(bool focused)
{
	int x;

	if (!di->fullscreen) return;
	MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[0];
	if ((!&cb) || (!cb.ddsurf)) return;
	if (focused){
		for (x=0;x<nbitmaps;x++){
			MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
			if (!cb.ddsurf) continue;
			cb.ddsurf->PageLock(0);
		};
	}
	else{
		for (x=0;x<nbitmaps;x++){
			MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
			if (!cb.ddsurf) continue;
			cb.ddsurf->PageUnlock(0);
		};
	};
}

void MTDXDevice7::sync()
{
	if (iddraw) iddraw->WaitForVerticalBlank(DDWAITVB_BLOCKEND,0);
}

void MTDXDevice7::setbitmap(MTDXBitmap7 *b)
{
	int x;

	if (lastb==b) return;
	end();
	lastb = b;
	for (x=0;x<nbitmaps;x++){
		MTDXBitmap7 &cb = *(MTDXBitmap7*)bitmaps[x];
	};
	for (x=0;x<nmasks;x++){
		MTDXMask &cm = *(MTDXMask*)masks[x];
	};
	id3ddev->SetRenderTarget(b->ddsurf,0);
	viewport.dwWidth = b->width;
	viewport.dwHeight = b->height;
	id3ddev->SetViewport(&viewport);
}

bool MTDXDevice7::start(MTDXBitmap7 *b)
{
	setbitmap(b);
	if ((!started) && (id3ddev)){
		if (dxerror(id3ddev->BeginScene(),__FILE__,__LINE__)==0) started = true;
	};
	return started;
}

void MTDXDevice7::end(bool reset)
{
	if (started){
		if (id3ddev) dxerror(id3ddev->EndScene(),__FILE__,__LINE__);
		started = false;
	};
	if (reset) lastb = 0;
}
//---------------------------------------------------------------------------
int WINAPI ddenumproc(GUID *guid,char *desc,char *name,void *context)
{
	IDirectDraw *cidd = 0;
	MTConfigFile *conf;
	bool skipdx7 = false;

	if (ndddev>=MAX_DISPLAYDEVICES) return 0;

	if ((ddcreateex) && (ddcreateex(guid,(void**)&cidd,IID_IDirectDraw7,0)==0)){
		if (cidd){
			if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
				if (conf->setsection("MTDisplay")){
					conf->getparameter("NoDirectX7",&skipdx7,MTCT_BOOLEAN,sizeof(skipdx7));
				};
				mtinterface->releaseconf(conf);
			};
			if (skipdx7){
				LOG("[Display] Ignoring DirectX 7 interfaces"NL);
			}
			else{
				LOG("[Display] Using DirectX 7 interfaces"NL);
				ddver = 7;
			};
		};
	};
	if ((cidd) || (ddcreate(guid,(void**)&cidd,0)==0)){
		dddev[ndddev] = mtnew(DDDevice);
		DDCAPS &ccaps = dddev[ndddev]->caps;
		ccaps.dwSize = sizeof(ccaps);
		cidd->GetCaps(&ccaps,0);
		cidd->Release();
		if (guid){
			dddev[ndddev]->guid = mtnew(GUID);
			memcpy(dddev[ndddev]->guid,guid,sizeof(GUID));;
		};
		dddev[ndddev]->desc = (char*)si->memalloc(strlen(desc)+1,0);
		strcpy(dddev[ndddev]->desc,desc);
		dddev[ndddev]->name = (char*)si->memalloc(strlen(name)+1,0);
		strcpy(dddev[ndddev]->name,name);
#ifdef _DEBUG
		LOG("[Display] Found device: ");
		LOG(dddev[ndddev]->desc);
		LOG(NL);
		FLOG2("  Total memory: % 10d bytes"NL"   Free memory: % 10d bytes"NL,ccaps.dwVidMemTotal,ccaps.dwVidMemFree);
		LOG("  Capabilities:"NL);
		FLOG2("        Driver:   %.8X %.8X"NL,ccaps.dwCaps,ccaps.dwCaps2);
		FLOG1("     Color Key:   %.8X"NL,ccaps.dwCKeyCaps);
		FLOG1("       Effects:   %.8X"NL,ccaps.dwFXCaps);
		FLOG1("         Alpha:   %.8X"NL,ccaps.dwFXAlphaCaps);
		FLOG1("       Surface:   %.8X"NL,ccaps.ddsCaps);
#endif
		ndddev++;
	};
	return 1;
}
//---------------------------------------------------------------------------
MTDXDeviceManager::MTDXDeviceManager()
{
	int x;
	HMODULE huser;

	huser = GetModuleHandle("USER32.DLL");
	if (huser){
		*(int*)&switchwindow = (int)GetProcAddress(huser,"SwitchToThisWindow");
	};
	hddraw = LoadLibrary("DDRAW.DLL");
	if (hddraw!=0){
#ifdef _DEBUG
		int infosize,fixedsize;
		void *info;
		VS_FIXEDFILEINFO *fixed;

		infosize = GetFileVersionInfoSize("DDRAW.DLL",(LPDWORD)&x);
		if (infosize>0){
			info = si->memalloc(infosize,MTM_ZERO);
			*(short*)info = infosize;
			if ((GetFileVersionInfo("DDRAW.DLL",0,infosize,info)) && (VerQueryValue(info,"\\",(void**)&fixed,(unsigned int*)&fixedsize))){
				FLOG4("[Display] DirectDraw version %d.%d.%d.%d"NL,fixed->dwFileVersionMS>>16,fixed->dwFileVersionMS & 0xFFFF,fixed->dwFileVersionLS>>16,fixed->dwFileVersionMS & 0xFFFF);
			};
			si->memfree(info);
		};
#endif
		*(int*)&ddcreate = (int)GetProcAddress(hddraw,"DirectDrawCreate");
		*(int*)&ddcreateex = (int)GetProcAddress(hddraw,"DirectDrawCreateEx");
		*(int*)&ddenum = (int)GetProcAddress(hddraw,"DirectDrawEnumerateA");
		if (((ddcreate) || (ddcreateex)) && (ddenum)) ddenum(ddenumproc,0);
	};
	for (x=0;x<ndddev;x++){
		devicename[x] = dddev[x]->desc;
	};
}

MTDXDeviceManager::~MTDXDeviceManager()
{
	int x;

	for (x=0;x<ndddev;x++){
		si->memfree(dddev[x]->guid);
		si->memfree(dddev[x]->desc);
		si->memfree(dddev[x]->name);
		si->memfree(dddev[x]);
	};
	FreeLibrary(hddraw);
	ndddev = 0;
	mtmemzero(devicename,sizeof(devicename));
}

MTDisplayDevice* MTDXDeviceManager::newdevice(int id)
{
	if (ddver==7) return (MTDisplayDevice*)(new MTDXDevice7(dddev[id]));
	else return (MTDisplayDevice*)(new MTDXDevice(dddev[id]));
}

void MTDXDeviceManager::deldevice(MTDisplayDevice *device)
{
	if (ddver==7) delete (MTDXDevice7*)device;
	else delete (MTDXDevice*)device;
}
//---------------------------------------------------------------------------
#endif