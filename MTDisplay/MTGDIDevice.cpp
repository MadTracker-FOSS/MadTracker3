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
#include "MTDisplay1.h"
#include "MTGDIDevice.h"
//---------------------------------------------------------------------------
bool MTGDIDevice::init(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTGDIDevice::init(%d)",fullscreen);
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	dsk = GetWindowLong(wnd,GWL_USERDATA);
	SetWindowLong(wnd,GWL_USERDATA,0);
	if (wnd){
		if (fullscreen){
			if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
			SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
			SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
			ShowWindow(wnd,SW_SHOWMAXIMIZED);
		}
		else if (wasfullscreen){
			SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
			SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
			if (gi) gi->restorewindowstate(wnd);
		};
	};
	SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return true;
}

void MTGDIDevice::uninit()
{
}

MTBitmap* MTGDIDevice::newbitmap(int flags,int width,int height)
{
	MTBitmap *newbmp = new MTGDIBitmap(this,flags,width,height);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTGDIDevice::newresbitmap(int flags,MTResources *res,int id,int colorkey)
{
	MTBitmap *newbmp = new MTGDIBitmap(this,flags,res,id,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTGDIDevice::newfilebitmap(int flags,const char *filename,int colorkey)
{
	MTBitmap *newbmp = new MTGDIBitmap(this,flags,filename,colorkey);
	add(newbmp);
	return newbmp;
}

MTBitmap* MTGDIDevice::newbmpbitmap(int flags,MTBitmap &orig,int colorkey)
{
	MTBitmap *newbmp = new MTGDIBitmap(this,flags,orig,colorkey);
	add(newbmp);
	return newbmp;
}

void MTGDIDevice::delbitmap(MTBitmap *bitmap)
{
	remove(bitmap);
	delete (MTGDIBitmap*)bitmap;
}

MTMask* MTGDIDevice::newmask(int w,int h)
{
	MTMask *newmask = new MTGDIMask(this,w,h);
	add(newmask);
	return newmask;
}

void MTGDIDevice::delmask(MTMask *mask)
{
	remove(mask);
	delete (MTGDIMask*)mask;
}

bool MTGDIDevice::setfullscreen(bool &fullscreen)
{
	HWND wnd = 0;
	int dsk;
	bool wasfullscreen = di->fullscreen;
	
	FENTER1("MTGDIDevice::setfullscreen(%d)",fullscreen);
	if (desktops[0]) wnd = (HWND)desktops[0]->mwnd;
	dsk = GetWindowLong(wnd,GWL_USERDATA);
	SetWindowLong(wnd,GWL_USERDATA,0);
	if (wnd){
		if (fullscreen){
			if ((gi) && (!wasfullscreen)) gi->savewindowstate(wnd);
			SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_POPUP);
			SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
			ShowWindow(wnd,SW_SHOWMAXIMIZED);
		}
		else if (wasfullscreen){
			SetWindowLong(wnd,GWL_STYLE,WS_VISIBLE|WS_SYSMENU|WS_TABSTOP|WS_CAPTION|WS_MINIMIZEBOX|WS_SIZEBOX);
			SetWindowLong(wnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
			if (gi) gi->restorewindowstate(wnd);
		};
	};
	SetWindowLong(wnd,GWL_USERDATA,dsk);
	LEAVE();
	return true;
}
//---------------------------------------------------------------------------
MTGDIDeviceManager::MTGDIDeviceManager()
{
	devicename[0] = "Windows GDI (Compatible mode)";
	if (si->sysflags & MTS_MMX){
		asm_shade = mmx_shade;
		asm_sshade = mmx_sshade;
		asm_blend = mmx_blend;
		asm_blendt = mmx_blendt;
		asm_blendck = mmx_blendck;
		asm_blur = a_blur;
		asm_opacity = a_opacity;
		asm_fill = a_fill;
	}
	else{
		asm_shade = a_shade;
		asm_sshade = a_sshade;
		asm_blend = a_blend;
		asm_blendt = a_blendt;
		asm_blendck = a_blendck;
		asm_blur = a_blur;
		asm_opacity = a_opacity;
		asm_fill = a_fill;
	};
}

MTGDIDeviceManager::~MTGDIDeviceManager()
{
}

MTDisplayDevice* MTGDIDeviceManager::newdevice(int id)
{
	return (MTDisplayDevice*)(new MTGDIDevice());
}

void MTGDIDeviceManager::deldevice(MTDisplayDevice *device)
{
	delete (MTGDIDevice*)device;
}
//---------------------------------------------------------------------------
