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
#include "MTGDIBitmap.h"
#include "MTDecode.h"
#include "../MTGUI/MTGUITools.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
ShadeProc asm_shade;
SShadeProc asm_sshade;
BlendProc asm_blend;
BlendTProc asm_blendt;
BlendCKProc asm_blendck;
BlurProc asm_blur;
OpacityProc asm_opacity;
FillProc asm_fill;
//---------------------------------------------------------------------------
// MTGDIBitmap
//---------------------------------------------------------------------------
MTGDIBitmap::MTGDIBitmap(MTDisplayDevice *d,int f,int w,int h):
mcount(0),
mbits(0),
mmbits(0),
mdc(0),
mmdc(0),
bmp(0),
mask(0),
mpitch(0),
clipped(0),
mtextinit(false),
mtxtcolor(0),
mfont(0),
baseline(0),
textheight(0),
mrgn(0)
{
	FENTER4("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%d,%d)",d,f,w,h);
	device = d;
	flags = f;
	width = w;
	height = h;
	bitcount = 24;
	changeproc = 0;
	param = 0;
	loaded = false;
	mres = 0;
	mresid = -1;
	mfilename = 0;
	morig = 0;
	mck = -1;
	modified = true;
	if (flags & MTB_DRAW){
		RECT r = {0,0,width,height};
		mrgn = CreateRectRgnIndirect(&r);
	};
	if (flags & MTB_SCREEN){
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		bitcount = GetDeviceCaps(GetDC(0),BITSPIXEL)*GetDeviceCaps(GetDC(0),PLANES);
		if (bitcount==16){
			bitcount = 15;
		};
	}
	else{
		width = w;
		height = h;
		if ((f & 0x80000000)==0){
			initialize();
			loaded = load();
		};
	};
	LEAVE();
}

MTGDIBitmap::MTGDIBitmap(MTDisplayDevice *d,int f,MTResources *res,int resid,int colorkey):
mcount(0),
mbits(0),
mmbits(0),
mdc(0),
mmdc(0),
bmp(0),
mask(0),
mpitch(0),
clipped(0),
mtextinit(false),
mtxtcolor(0),
mfont(0),
baseline(0),
textheight(0),
mrgn(0)
{
	FENTER5("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%d,%.6X)",d,f,res,resid,colorkey);
	device = d;
	flags = f;
	width = height = 128;
	bitcount = 24;
	changeproc = 0;
	param = 0;
	loaded = false;
	mres = res;
	mresid = resid;
	mfilename = 0;
	morig = 0;
	mck = colorkey;
	modified = true;
	if ((f & 0x80000000)==0){
		initialize();
		loaded = load();
	};
	LEAVE();
}

MTGDIBitmap::MTGDIBitmap(MTDisplayDevice *d,int f,const char *filename,int colorkey):
mcount(0),
mbits(0),
mmbits(0),
mdc(0),
mmdc(0),
bmp(0),
mask(0),
mpitch(0),
clipped(0),
mtextinit(false),
mtxtcolor(0),
mfont(0),
baseline(0),
textheight(0),
mrgn(0)
{
	FENTER3("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%s)",d,f,filename);
	device = d;
	flags = f;
	width = height = 128;
	bitcount = 24;
	changeproc = 0;
	param = 0;
	loaded = false;
	mres = 0;
	mresid = 0;
	mfilename = (char*)si->memalloc(strlen(filename)+1,MTM_ZERO);
	strcpy(mfilename,filename);
	morig = 0;
	mck = -1;
	modified = true;
	if ((f & 0x80000000)==0){
		initialize();
		loaded = load();
	};
	LEAVE();
}

MTGDIBitmap::MTGDIBitmap(MTDisplayDevice *d,int f,MTBitmap &orig,int colorkey):
mcount(0),
mbits(0),
mmbits(0),
mdc(0),
mmdc(0),
bmp(0),
mask(0),
mpitch(0),
clipped(0),
mtextinit(false),
mtxtcolor(0),
mfont(0),
baseline(0),
textheight(0),
mrgn(0)
{
	FENTER4("MTGDIBitmap::MTGDIBitmap(%.8X,%d,%.8X,%.6X)",d,f,&orig,colorkey);
	device = d;
	flags = f;
	width = orig.width;
	height = orig.height;
	bitcount = 24;
	changeproc = 0;
	param = 0;
	loaded = false;
	mres = 0;
	mresid = 0;
	mfilename = 0;
	morig = &orig;
	mck = colorkey;
	modified = true;
	if ((f & 0x80000000)==0){
		initialize();
		loaded = load();
	};
	LEAVE();
}

MTGDIBitmap::~MTGDIBitmap()
{
	ENTER("MTGDIBitmap::~MTGDIBitmap");
	if ((flags & 0x80000000)==0){
		unload();
	};
	if (mfilename) si->memfree(mfilename);
	if (mdc) DeleteDC(mdc);
	if (mrgn) DeleteObject(mrgn);
	if ((mwnd) && (mdc)) ReleaseDC((HWND)mwnd,mdc);
	LEAVE();
}

void MTGDIBitmap::initialize()
{
	ENTER("MTGDIBitmap::initialize");
	mdc = 0;
	bitcount = (screen)?screen->bitcount:24;
	mtmemzero(&bmi,sizeof(bmi));
	bmi.biSize = sizeof(bmi);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = ((bitcount+7) & 0xF8);
	mdc = CreateCompatibleDC(GetDC(0));
	if ((!screen) || (screen->bitcount>16)) SetStretchBltMode(mdc,HALFTONE);
	if (screen) bitcount = screen->bitcount;
	LEAVE();
}

bool MTGDIBitmap::load()
{
	int omck;
	int nc = 0;
	MTFile *f;
	char ext[16];
	HBITMAP bmp = 0;
	BITMAP b;
	RGBQUAD rgb[256];
	
	ENTER("MTGDIBitmap::load");
	unload();
	if (mfilename){
		si->filetype(mfilename,ext,16);
		f = si->fileopen(mfilename,MTF_READ|MTF_SHAREREAD);
		if (f){
			MTTRY
				if (ext[0]){
					if (stricmp(ext,".gif")==0) bmp = (HBITMAP)loadgif(f,mck);
					else if (stricmp(ext,".tif")==0) bmp = (HBITMAP)loadtif(f,mck);
					else{
						bmp = (HBITMAP)loadgif(f,mck);
						f->seek(0,MTF_BEGIN);
						if (!bmp) bmp = (HBITMAP)loadtif(f,mck);
					};
				}
				else{
					bmp = (HBITMAP)loadgif(f,mck);
					f->seek(0,MTF_BEGIN);
					if (!bmp) bmp = (HBITMAP)loadtif(f,mck);
				};
			MTCATCH
				bmp = 0;
			MTEND
			si->fileclose(f);
		};
	}
	else if (mres){
		f = mres->getresourcefile(MTR_BITMAP,mresid,0);
		if (f){
			MTTRY
				bmp = (HBITMAP)loadgif(f,mck);
				f->seek(0,MTF_BEGIN);
				if (!bmp) bmp = (HBITMAP)loadtif(f,mck);
			MTCATCH
				bmp = 0;
			MTEND
			mres->releaseresourcefile(f);
		};
	}
	else if (morig){
		HDC odc = (HDC)morig->open(0);
		if (morig->mck>=0) mck = morig->mck;
		omck = mck;
		if (morig->bitcount<15){
			nc = GetDIBColorTable(odc,0,256,rgb);
			if ((nc) && (mck & 0x1000000)) mck = swapcolor(*(int*)&rgb[mck & 0xFF]);
		};
		if (!setsize(morig->width,morig->height)){
			LOGD("%s - [Display] ERROR: Cannot resize bitmap!"NL);
			LEAVE();
			return false;
		};
		HDC dc = (HDC)open(0);
		if (mck>=0){
			mask = CreateCompatibleBitmap(GetDC(0),width,height);
			mmdc = CreateCompatibleDC(GetDC(0));
			SelectObject(mmdc,mask);
			extractmask(odc,mmdc,mck,(int*)rgb,&mmbits);
		}; 
		BitBlt(dc,0,0,width,height,odc,0,0,SRCCOPY);
		if ((mck>=0) && ((flags & MTB_KEEPMASK)==0)){
			BitBlt(mmdc,0,0,width,height,0,0,0,DSTINVERT);
			BitBlt(dc,0,0,width,height,mmdc,0,0,SRCAND);
			BitBlt(mmdc,0,0,width,height,0,0,0,DSTINVERT);
		};
		close(0);
		morig->close(0);
		LEAVE();
		return true;
	}
	else{
		if (!setsize(width,height)){
			LOGD("%s - [Display] ERROR: Cannot resize bitmap!"NL);
			LEAVE();
			return false;
		};
		LEAVE();
		return true;
	};
	if (bmp){
		GetObject(bmp,sizeof(b),&b);
		if (!setsize(b.bmWidth,b.bmHeight)){
			LOGD("%s - [Display] ERROR: Cannot resize bitmap!"NL);
			LEAVE();
			return false;
		};
		HDC odc = CreateCompatibleDC(GetDC(0));
		DeleteObject(SelectObject(odc,bmp));
		if (mck>=0){
			if (b.bmBitsPixel<15){
				nc = GetDIBColorTable(odc,0,256,rgb);
				if ((nc) && (mck & 0x1000000)) mck = swapcolor(*(int*)&rgb[mck & 0xFF]);
			};
			mask = CreateCompatibleBitmap(GetDC(0),width,height);
			mmdc = CreateCompatibleDC(GetDC(0));
			SelectObject(mmdc,mask);
			extractmask(odc,mmdc,mck,(int*)rgb,&mmbits);
		}; 
		HDC dc = (HDC)open(0);
		BitBlt(dc,0,0,width,height,odc,0,0,SRCCOPY);
		DeleteObject(bmp);
		DeleteDC(odc);
		if ((mck>=0) && ((flags & MTB_KEEPMASK)==0)){
			BitBlt(mmdc,0,0,width,height,0,0,0,DSTINVERT);
			BitBlt(dc,0,0,width,height,mmdc,0,0,SRCAND);
			BitBlt(mmdc,0,0,width,height,0,0,0,DSTINVERT);
		};
		close(dc);
		LEAVE();
		return true;
	};
	LEAVE();
	return false;
}

void MTGDIBitmap::unload()
{
	ENTER("MTGDIBitmap::unload");
	loaded = false;
	mbits = 0;
	if (mask){
		DeleteDC(mmdc);
		DeleteObject(mask);
		si->memfree(mmbits);
		mmdc = 0;
		mask = 0;
		mmbits = 0;
	};
	if (bmp){
		SelectObject(mdc,oldbmp);
		DeleteObject(bmp);
		bmp = 0;
	};
	LEAVE();
}

bool MTGDIBitmap::setsize(int w,int h)
{
	if ((loaded) && (w==width) && (h==height)) return true;
	FENTER2("MTGDIBitmap::setsize(%d,%d)",w,h);
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
	if (!bitcount) bitcount = (screen)?screen->bitcount:0;
	bmi.biWidth = width;
	bmi.biHeight = -height;
	bmp = CreateDIBSection(0,(BITMAPINFO*)&bmi,DIB_RGB_COLORS,&mbits,0,0);
	mpitch = calcpitch(width,bitcount,2);
	if (!bmp){
		LEAVE();
		return false;
	};
	oldbmp = (HBITMAP)SelectObject(mdc,bmp);
	LEAVE();
	return true;
}

void *MTGDIBitmap::open(int wantedtype)
{
	mcount++;
	return (void*)mdc;
}

void MTGDIBitmap::close(void *o)
{
	if (!mcount) return;
	mcount--;
}

void MTGDIBitmap::clip(MTRect *rect)
{
	int cr = clipped++;
	HDC dc;
	
	if (clipped==MAX_CLIP) return;
	if (flags & MTB_SCREEN){
		if (!mwnd) return;
		crgn[cr] = CreateRectRgnIndirect((RECT*)rect);
		dc = GetDC((HWND)mwnd);
		ExtSelectClipRgn(dc,crgn[cr],RGN_AND);
		ReleaseDC((HWND)mwnd,dc);
	}
	else{
		crgn[cr] = CreateRectRgnIndirect((RECT*)rect);
		if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
		SelectClipRgn(mdc,crgn[cr]);
	};
}

void MTGDIBitmap::cliprgn(void *rgn)
{
	int cr = clipped++;
	HDC dc;
	
	if (clipped==MAX_CLIP) return;
	if (flags & MTB_SCREEN){
		if (!mwnd) return;
		crgn[cr] = CreateRectRgn(0,0,1,1);
		CombineRgn((HRGN)crgn[cr],(HRGN)rgn,(HRGN)rgn,RGN_COPY);
		dc = GetDC((HWND)mwnd);
		ExtSelectClipRgn(dc,(HRGN)crgn[cr],RGN_AND);
		ReleaseDC((HWND)mwnd,dc);
	}
	else{
		crgn[cr] = CreateRectRgn(0,0,1,1);
		CombineRgn((HRGN)crgn[cr],(HRGN)rgn,(HRGN)rgn,RGN_COPY);
		if (cr>0) CombineRgn(crgn[cr],crgn[cr],crgn[cr-1],RGN_AND);
		SelectClipRgn(mdc,crgn[cr]);
	};
}

void MTGDIBitmap::unclip()
{
	int cr = --clipped;
	HDC dc;
	
	if (flags & MTB_SCREEN){
		if (!mwnd) return;
		dc = GetDC((HWND)mwnd);
		if (clipped) SelectClipRgn(dc,crgn[clipped-1]);
		else SelectClipRgn(dc,0);
		DeleteObject(crgn[cr]);
		crgn[cr] = 0;
		ReleaseDC((HWND)mwnd,dc);
	}
	else{
		if (clipped) SelectClipRgn(mdc,crgn[clipped-1]);
		else SelectClipRgn(mdc,0);
		DeleteObject(crgn[cr]);
		crgn[cr] = 0;
	};
}

void *MTGDIBitmap::getclip()
{
	if (!clipped) return mrgn;
	return crgn[clipped-1];
}

void MTGDIBitmap::setwindow(MTWinControl *window)
{
	int l;
	void *oldwnd = mwnd;
	static int cyc = -1,cysc,cxf,cyf,cxdf,cydf;
	
	FENTER1("MTGDIBitmap::setwindow(%.8X)",window);
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
		if (oldwnd!=mwnd){
			if (oldwnd) ReleaseDC((HWND)oldwnd,mdc);
			mdc = 0;
		};
		return;
	};
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
			mox += cxf;
			moy += cyf;
		}
		else if (l & WS_BORDER){
			mox += cxdf;
			moy += cydf;
		};
		mox += wr.left;
		moy += wr.top;
	};
	wr.right = wr.right-wr.left;
	wr.bottom = wr.bottom-wr.top;
	wr.left = 0;
	wr.top = 0;
	if (oldwnd!=mwnd){
		if (oldwnd) ReleaseDC((HWND)oldwnd,mdc);
		if (mwnd) mdc = GetDC((HWND)mwnd);
		else mdc = 0;
	};
}

void MTGDIBitmap::toscreen(MTPoint &p)
{
	p.x += mox;
	p.y += moy;
}

bool MTGDIBitmap::openbits(MTRect &rect,void **bits,int *pitch,void **maskbits,int *maskpitch)
{
	if ((!bits) || (!pitch)) return false;
	mcount++;
	allocated = false;
	if (mbits){
		if (mpitch<0) *bits = (char*)mbits-(height-1)*mpitch;
		else *bits = mbits;
		*bits = (char*)*bits+rect.top*mpitch+rect.left*((bitcount+7)>>3);
		*pitch = mpitch;
		if (maskbits){
			*maskpitch = width;
			*maskbits = (char*)mmbits+rect.left+rect.top*width;
		};
		return true;
	}
	else{
		return false;
	};
}

void MTGDIBitmap::closebits()
{
	if (--mcount!=0) return;
	if (allocated){
		SetDIBits(mdc,bmp,ob.top,ob.bottom-ob.top,mbits,(BITMAPINFO*)&bmi,DIB_RGB_COLORS);
		si->memfree(mbits);
		mbits = 0;
		allocated = false;
	};
}

bool MTGDIBitmap::blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode)
{
	MTGDIBitmap *gdidest = (MTGDIBitmap*)dest;
	int fxflags = 0;
	bool tr = (mck>=0);
	bool ok;
	
	if ((w<0) || (h<0)) return false;
	if ((w==0) && (h==0)){
		w = width;
		h = height;
	};
	if (tr){
		ok = BitBlt(gdidest->mdc,dx,dy,w,h,mmdc,ox,oy,SRCAND);
		ok &= BitBlt(gdidest->mdc,dx,dy,w,h,mdc,ox,oy,trmode[mode]);
	}
	else ok = BitBlt(gdidest->mdc,dx,dy,w,h,mdc,ox,oy,bltmode[mode]);
	return ok;
}

bool MTGDIBitmap::sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode)
{
	MTGDIBitmap *gdidest = (MTGDIBitmap*)dest;
	int fxflags = 0;
	bool tr = (mck>=0);
	bool ok;

	if (tr){
		ok = StretchBlt(gdidest->mdc,dx,dy,dw,dh,mmdc,ox,oy,ow,oh,SRCAND);
		ok &= StretchBlt(gdidest->mdc,dx,dy,dw,dh,mdc,ox,oy,ow,oh,trmode[mode]);
	}
	else ok = StretchBlt(gdidest->mdc,dx,dy,dw,dh,mdc,ox,oy,ow,oh,bltmode[mode]);
	return ok;
}

bool MTGDIBitmap::skinblt(int x,int y,int w,int h,MTSkinPart &o,int color)
{
	if (w==0) w = o.b.w;
	if (h==0) h = o.b.h;
	int ox,oy,ow,oh,cw,ch;
	int oldx = x;
	int oldw = w;
	bool tr;
	bool ok = true;
	MTGDIBitmap *skinbmp = (MTGDIBitmap*)::skinbmp[o.bmpid & 0xF];
	
	if ((w|h)==0) return true;
	ox = o.b.x;
	oy = o.b.y;
	if (o.mode==SKIN_COLOR){
		if (color!=0xFFFFFF) color = combinecolor(color,o.color);
		else color = o.color;
		DeleteObject(SelectObject(mdc,CreateSolidBrush(color)));
		return PatBlt(mdc,x,y,w,h,PATCOPY);
	}
	if (!skinbmp) return false;
	tr = ((o.flags & SKIN_TRANSP) && (skinbmp->mck>=0));
	HDC odc = (HDC)skinbmp->open(0);
	HDC omdc = skinbmp->mmdc;
	if ((o.b.w==w) && (o.b.h==h)){
		if (tr){
			ok = BitBlt(mdc,x,y,w,h,omdc,ox,oy,SRCAND);
			ok &= BitBlt(mdc,x,y,w,h,odc,ox,oy,SRCPAINT);
		}
		else ok = BitBlt(mdc,x,y,w,h,odc,ox,oy,SRCCOPY);
	}
	else{
		ow = o.b.w;
		oh = o.b.h;
		if ((!ow) || (!oh)){
			skinbmp->close(0);
			return true;
		};
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<o.b.w))){
			if (tr){
				ok = StretchBlt(mdc,x,y,w,h,omdc,ox,oy,ow,oh,SRCAND);
				ok &= StretchBlt(mdc,x,y,w,h,odc,ox,oy,ow,oh,SRCPAINT);
			}
			else ok = StretchBlt(mdc,x,y,w,h,odc,ox,oy,ow,oh,SRCCOPY);
		}
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			while (h>0){
				ch = (h>oh)?oh:h;
				while (w>0){
					cw = (w>ow)?ow:w;
					if (tr){
						ok &= BitBlt(mdc,x,y,cw,ch,omdc,ox,oy,SRCAND);
						ok &= BitBlt(mdc,x,y,cw,ch,odc,ox,oy,SRCPAINT);
					}
					else ok &= BitBlt(mdc,x,y,cw,ch,odc,ox,oy,SRCCOPY);
					x += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				y += ch;
				h -= ch;
			};
		};
	};
	skinbmp->close(0);
	return ok;
}

bool MTGDIBitmap::skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color)
{
	int ox,oy,ow,oh,cw,ch;
	int oldx;
	int oldw;
	int aw = o.b.w/nx;
	int ah = o.b.h/ny;
	int caw;
	int cah;
	bool tr;
	bool ok = true;
	MTGDIBitmap *skinbmp = (MTGDIBitmap*)::skinbmp[o.bmpid & 0xF];
	
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
	HDC odc = (HDC)skinbmp->open(0);
	HDC omdc = skinbmp->mmdc;
	ox = o.b.x+caw;
	oy = o.b.y+cah;
	if ((aw==w) && (ah==h)){
		if (color!=0xFFFFFF){
			DeleteObject(SelectObject(mdc,CreateSolidBrush(color)));
			if (tr){
				ok = BitBlt(mdc,x,y,w,h,omdc,ox,oy,SRCAND);
				ok &= BitBlt(mdc,x,y,w,h,odc,ox,oy,0xEA02E9);
			}
			else ok = BitBlt(mdc,x,y,w,h,odc,ox,oy,0xEA02E9);
		}
		else{
			if (tr){
				ok = BitBlt(mdc,x,y,w,h,omdc,ox,oy,SRCAND);
				ok &= BitBlt(mdc,x,y,w,h,odc,ox,oy,SRCPAINT);
			}
			else ok = BitBlt(mdc,x,y,w,h,odc,ox,oy,SRCCOPY);
		};
	}
	else{
		ow = aw;
		oh = ah;
		if ((!ow) || (!oh)) return true;
		if ((o.mode==SKIN_STRETCH) && ((o.mode==SKIN_FIXED) && (w<aw))){
			if (tr){
				ok = StretchBlt(mdc,x,y,w,h,omdc,ox,oy,ow,oh,SRCAND);
				ok &= StretchBlt(mdc,x,y,w,h,odc,ox,oy,ow,oh,SRCPAINT);
			}
			else ok = StretchBlt(mdc,x,y,w,h,odc,ox,oy,ow,oh,SRCCOPY);
		}
		else{
#ifdef _DEBUG
			if (w>4096) FLOGD2("%s - [Display] ERROR: Excessively high width! (File %s at line %d)"NL,__FILE__,__LINE__);
			if (h>4096) FLOGD2("%s - [Display] ERROR: Excessively high height! (File %s at line %d)"NL,__FILE__,__LINE__);
			if ((w>4096) || (h>4096)) return false;
#endif
			ok = true;
			while (h>0){
				ch = (h>oh)?oh:h;
				while (w>0){
					cw = (w>ow)?ow:w;
					if (tr){
						ok &= BitBlt(mdc,x,y,cw,ch,omdc,ox,oy,SRCAND);
						ok &= BitBlt(mdc,x,y,cw,ch,odc,ox,oy,SRCPAINT);
					}
					else ok &= BitBlt(mdc,x,y,cw,ch,odc,ox,oy,SRCCOPY);
					x += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				y += ch;
				h -= ch;
			};
		};
	};
	skinbmp->close(0);
	return ok;
}

bool MTGDIBitmap::maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o)
{
	if (w==0) w = o.b.w;
	if (h==0) h = o.b.h;
	int ox,oy,ow,oh,cw,ch;
	int oldx = x;
	int oldw = w;
	
	if (((o.flags & SKIN_TRANSP)==0) || (o.mode==SKIN_COLOR)){
		mask->fill(x,y,w,h,0);
		return true;
	};
	bool ok;
	HDC ddc = ((MTGDIMask*)mask)->mdc;
	ox = o.b.x;
	oy = o.b.y;
	if ((o.b.w==w) && (o.b.h==h)) return BitBlt(ddc,x,y,w,h,mmdc,ox,oy,SRCCOPY);
	else{
		ow = o.b.w;
		oh = o.b.h;
		if ((!ow) || (!oh)) return true;
		if ((o.mode==SKIN_STRETCH) || ((o.mode==SKIN_FIXED) && (w<o.b.w))){
			return StretchBlt(ddc,x,y,w,h,mmdc,ox,oy,ow,oh,SRCCOPY);
		}
		else{
			ok = true;
			while (h>0){
				ch = (h>oh)?oh:h;
				while (w>0){
					cw = (w>ow)?ow:w;
					ok &= BitBlt(ddc,x,y,cw,ch,mmdc,ox,oy,SRCCOPY);
					x += cw;
					w -= cw;
				};
				x = oldx;
				w = oldw;
				y += ch;
				h -= ch;
			};
		};
		return ok;
	};
}

bool MTGDIBitmap::shade(int x,int y,int w,int h,MTMask *mask,int mx,int my)
{
	void *bits;
	int pitch;
	int n;
	void *rgn;
	MTRect r = {x,y,x+w,y+h};
	MTGDIMask *gdimask = (MTGDIMask*)mask;
	
	if (!openbits(r,&bits,&pitch)) return false;
	rgn = recttorgn(r);
	if (r.right-r.left>mask->width-mx)
		r.right = r.left+mask->width-mx;
	if (r.bottom-r.top>mask->height-my)
		r.bottom = r.top+mask->height-my;
	intersectrgn(rgn,getclip());
	offsetrgn(rgn,-x,-y);
	for (n=rgngetnrects(rgn)-1;n>=0;n--){
		rgngetrect(rgn,n,&r);
		asm_shade((char*)bits+r.left*((bitcount+7)>>3)+r.top*pitch,(char*)gdimask->mbits+mx+r.left+(my+r.top)*gdimask->mpitch,pitch,gdimask->mpitch,r.right-r.left,r.bottom-r.top,bitcount);
	};
	deletergn(rgn);
	closebits();
	return true;
} 

bool MTGDIBitmap::tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
	void *bits;
	int pitch;
	int n,bc,ox,oy,cx,cy,cw,ch,iw,ih;
	void *rgn;
	MTRect r = {x,y,x+w,y+h};
	MTGDIMask *gdimask = (MTGDIMask*)mask;
	
	if ((mh<=0) || (!openbits(r,&bits,&pitch))) return false;
	rgn = recttorgn(r);
	intersectrgn(rgn,getclip());
	offsetrgn(rgn,-x,-y);
	bc = ((bitcount+7)>>3);
	for (n=rgngetnrects(rgn)-1;n>=0;n--){
		rgngetrect(rgn,n,&r);
		cy = r.top*pitch;
		oy = r.top%mh;
		ch = r.bottom-r.top;
		while (ch>0){
			ih = ((mh-oy)>ch)?ch:(mh-oy);
			cx = r.left;
			ox = r.left%mw;
			cw = r.right-r.left;
			while (cw>0){
				iw = ((mw-ox)>cw)?cw:(mw-ox);
				asm_shade((char*)bits+cx*bc+cy,(char*)gdimask->mbits+mx+ox+(my+oy)*gdimask->mpitch,pitch,gdimask->mpitch,iw,ih,bitcount);
				cx += iw;
				ox += iw;
				if (ox>=mw) ox -= mw;
				cw -= iw;
			};
			cy += ih*pitch;
			oy += ih;
			if (oy>=mh) oy -= mh;
			ch -= ih;
		};
	};
	deletergn(rgn);
	closebits();
	return true;
}

bool MTGDIBitmap::sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh)
{
	void *bits;
	int pitch;
	int n;
	void *rgn,*op;
	MTRect r = {x,y,x+w,y+h};
	MTGDIMask *gdimask = (MTGDIMask*)mask;
	
	if (!openbits(r,&bits,&pitch)) return false;
	r.left = x;
	r.top = y;
	r.right = x+w;
	r.bottom = y+h;
	rgn = recttorgn(r);
	r.left = x;
	r.top = y;
	r.right = x+mask->width-mx;
	r.bottom = y+mask->height-my;
	op = recttorgn(r);
	intersectrgn(rgn,getclip());
	intersectrgn(rgn,op);
	deletergn(op);
	offsetrgn(rgn,-x,-y);
	for (n=rgngetnrects(rgn)-1;n>=0;n--){
		rgngetrect(rgn,n,&r);
		asm_sshade((char*)bits+((r.left*(bitcount+7))>>3)+r.top*pitch,gdimask->mbits,pitch,gdimask->mpitch,mx+r.left-x,my+r.top,r.right-r.left,r.bottom-r.top,mw,mh,bitcount);
	};
	deletergn(rgn);
	closebits();
	return true;
}

bool MTGDIBitmap::blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity)
{
	void *bits,*dbits,*mbits;
	int sx,sy;
	void *orgn,*drgn;
	int pitch,dpitch,mpitch;
	int n;
	MTRect or,dr,r;
	
	if (bitcount!=dest->bitcount) return false;
	sx = dx-ox;
	sy = dy-oy;
//	Origin rectangle
	or.left = ox;
	or.top = oy;
	or.right = ox+w;
	or.bottom = oy+h;
//	Destination rectangle
	dr.left = dx;
	dr.top = dy;
	dr.right = dx+w;
	dr.bottom = dy+h;
//	Origin region
	orgn = recttorgn(or);
	intersectrgn(orgn,getclip());
//	Destination region
	drgn = recttorgn(dr);
	intersectrgn(drgn,dest->getclip());
//	Make destination relative to origin
	offsetrgn(drgn,-sx,-sy);
//	Intersect the regions
	intersectrgn(orgn,drgn);
	offsetrgn(orgn,-or.left,-or.top);
//	Get origin bounding rect
	rgntorect(orgn,or);
	deletergn(drgn);
//	Get destination bounding rect
	dr.left = or.left+sx;
	dr.top = or.top+sy;
	dr.right = or.right+sx;
	dr.bottom = or.bottom+sy;
//	Go ahead
	mbits = 0;
	if (mck>=0){
		if (!openbits(or,&bits,&pitch,&mbits,&mpitch)) return false;
	}
	else{
		if (!openbits(or,&bits,&pitch)) return false;
	};
	if (!dest->openbits(dr,&dbits,&dpitch)){
		closebits();
		return false;
	};
	ox -= or.left;
	oy -= or.top;
	dx -= dr.left;
	dy -= dr.top;
	if (mck>=0){
		if (mbits){
			for (n=rgngetnrects(orgn)-1;n>=0;n--){
				rgngetrect(orgn,n,&r);
				asm_blendt(dbits,dpitch,r.left+dx,r.top+dy,r.right-r.left,r.bottom-r.top,bits,pitch,r.left+ox,r.top+oy,opacity,bitcount,mbits,mpitch);
			};
		}
		else{
			for (n=rgngetnrects(orgn)-1;n>=0;n--){
				rgngetrect(orgn,n,&r);
				asm_blendck(dbits,dpitch,r.left+dx,r.top+dy,r.right-r.left,r.bottom-r.top,bits,pitch,r.left+ox,r.top+oy,opacity,bitcount,mpitch);
			};
		};
	}
	else{
		for (n=rgngetnrects(orgn)-1;n>=0;n--){
			rgngetrect(orgn,n,&r);
			asm_blend(dbits,dpitch,r.left+dx,r.top+dy,r.right-r.left,r.bottom-r.top,bits,pitch,r.left+ox,r.top+oy,opacity,bitcount);
		};
	};
	deletergn(orgn);
	dest->closebits();
	closebits();
	return true;
}

int MTGDIBitmap::translatecolor(int color)
{
	switch (bitcount){
	case 15:
		return ((color & 0xF8)>>3)|((color & 0xF800)>>6)|((color & 0xF80000)>>9);
	case 16:
		return ((color & 0xF8)>>3)|((color & 0xFC00)>>5)|((color & 0xF80000)>>8);
	case 24:
	case 32:
		return color;
	};
	return 0;
}

void MTGDIBitmap::setpen(int color)
{
	if (!mdc) return;
	DeleteObject(SelectObject(mdc,CreatePen(PS_SOLID,0,color)));
}

void MTGDIBitmap::setbrush(int color)
{
	HBRUSH brush;
	
	if (!mdc) return;
	if (color==-1) brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	else brush = CreateSolidBrush(color);
	DeleteObject(SelectObject(mdc,brush));
}

bool MTGDIBitmap::fill(int x,int y,int w,int h,int color,int opacity)
{
	if ((w==0) || (h==0)) return true;
	if (opacity<255){
		void *bits;
		int pitch;
		int n;
		void *rgn;
		MTRect r = {x,y,x+w,y+h};
		
		if (!openbits(r,&bits,&pitch)) return false;
		color = translatecolor(swap_dword(color)>>8);
		rgn = recttorgn(r);
		intersectrgn(rgn,getclip());
		offsetrgn(rgn,-x,-y);
		for (n=rgngetnrects(rgn)-1;n>=0;n--){
			rgngetrect(rgn,n,&r);
			asm_fill(bits,pitch,r.left,r.top,r.right-r.left,r.bottom-r.top,color,opacity,bitcount);
		};
		deletergn(rgn);
		closebits();
		return true;
	};
	if (!mdc) return false;
	DeleteObject(SelectObject(mdc,CreateSolidBrush(color)));
	return PatBlt(mdc,x,y,w,h,PATCOPY);
}

bool MTGDIBitmap::fillex(int x,int y,int w,int h,int mode)
{
	if (!mdc) return false;
	return PatBlt(mdc,x,y,w,h,patmode[mode]);
}

void MTGDIBitmap::point(int x,int y,int color)
{
	if (!mdc) return;
	SetPixel(mdc,x,y,color);
}

void MTGDIBitmap::moveto(int x,int y)
{
	if (!mdc) return;
	MoveToEx(mdc,x,y,0);
}

void MTGDIBitmap::lineto(int x,int y)
{
	if (!mdc) return;
	LineTo(mdc,x,y);
}

void MTGDIBitmap::polygon(const MTPoint *pt,int np)
{
	if (!mdc) return;
	Polygon(mdc,(POINT*)pt,np);
}

void MTGDIBitmap::polyline(const MTPoint *pt,int np)
{
	if (!mdc) return;
	Polyline(mdc,(POINT*)pt,np);
}

void MTGDIBitmap::rectangle(int x,int y,int w,int h)
{
	if (!mdc) return;
	Rectangle(mdc,x,y,x+w,y+h);
}

void MTGDIBitmap::ellipse(int x,int y,int w,int h)
{
	if (!mdc) return;
	Ellipse(mdc,x,y,x+w,y+h);
}

void MTGDIBitmap::settextcolor(int color)
{
	if (color!=mtxtcolor){
		mtxtcolor = color;
		mtextinit = false;
		baseline = textheight = 0;
	};
}

void MTGDIBitmap::setfont(void *font)
{
	if ((HFONT)font!=mfont){
		mfont = (HFONT)font;
		mtextinit = false;
		baseline = textheight = 0;
	};
}

void MTGDIBitmap::drawtext(const char *text,int length,MTRect &rect,int flags)
{
	int wflags;
	
	if (!mdc) return;
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	wflags = DT_NOPREFIX|DT_SINGLELINE;
	if (flags & DTXT_RIGHT) wflags |= DT_RIGHT;
	if (flags & DTXT_CENTER) wflags |= DT_CENTER;
	if (flags & DTXT_BOTTOM) wflags |= DT_BOTTOM;
	if (flags & DTXT_VCENTER) wflags |= DT_VCENTER;
	if (flags & DTXT_PREFIX) wflags &= (~DT_NOPREFIX);
	if (flags & DTXT_MULTILINE){
		wflags &= (~DT_SINGLELINE);
		wflags |= DT_WORDBREAK;
	};
	DrawText(mdc,text,length,(RECT*)&rect,wflags);
}

bool MTGDIBitmap::gettextsize(const char *text,int length,MTPoint *size,int maxwidth)
{
	char *sp,*ep,*ep2;
	
	if (!mdc){
		LOGD("%s - [Display] ERROR: Context not opened!"NL);
		return false;
	};
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	if (!textheight){
		TEXTMETRIC tm;
		POINT p;
		if (GetTextMetrics(mdc,&tm)){
			p.x = tm.tmAscent;
			p.y = tm.tmHeight;
			LPtoDP(mdc,&p,1);
			baseline = p.x;
			textheight = p.y;
		};
	};
	if (length<0) length = strlen(text);
	ep = strchr(text,'\n');
	if (ep){
		int cl,cl2,ml;
		SIZE csize;
		size->x = 0;
		size->y = 0;
		sp = (char*)text;
		do{
			ep = strchr(sp,'\n');
			if (!ep){
				ep = strchr(sp,0);
				ml = ep-sp;
			}
			else{
				ml = ep-sp+1;
				if (*(ep-1)=='\r') ep--;
			};
			cl = ep-sp;
			if (!cl){
				csize.cx = 0;
				csize.cy = textheight;
			}
			else if (maxwidth<0){
				if (!GetTextExtentPoint32(mdc,sp,cl,&csize)){
					LOGD("%s - [Display] ERROR: GetTextExtentPoint32 failed!"NL);
					return false;
				};
			}
			else{
				if (!GetTextExtentExPoint(mdc,sp,cl,maxwidth,&cl2,0,&csize)){
					LOGD("%s - [Display] ERROR: GetTextExtentExPoint failed!"NL);
					return false;
				};
				ep2 = sp+cl2;
				while ((*ep2!=' ') && (*ep2!='\r') && (*ep2!='\n') && (ep2>sp)) ep2--;
				if (ep2<=sp){
					if (csize.cx>maxwidth) csize.cx = maxwidth;
					ep2 = sp;
					while (((*ep2!=' ') && (*ep2!='\r') && (*ep2!='\n')) && (ep2<ep)) ep2++;
				}
				else while (((*ep2==' ') || (*ep2=='\r') || (*ep2=='\n')) && (ep2<ep)) ep2++;
				ep = ep2;
				cl = ep-sp;
				if (!GetTextExtentPoint32(mdc,sp,cl,&csize)){
					LOGD("%s - [Display] ERROR: GetTextExtentPoint32 failed!"NL);
					return false;
				};
			};
			if (csize.cx>size->x) size->x = csize.cx;
			size->y += csize.cy;
			sp = ep;
			while (*sp==' ') sp++;
			if (*sp=='\r') sp++;
			if (*sp=='\n') sp++;
		} while (*sp!=0);
		return true;
	}
	else{
		if (GetTextExtentPoint32(mdc,text,length,(SIZE*)size)) return true;
		LOGD("%s - [Display] ERROR: GetTextExtentPoint32 failed!"NL);
	};
	return false;
}

int MTGDIBitmap::gettextextent(const char *text,int length,int maxextent)
{
	int fit = 0;
	SIZE size;
	
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	if (length<0) length = strlen(text);
	GetTextExtentExPoint(mdc,text,length,maxextent,&fit,0,&size);
	return fit;
}

int MTGDIBitmap::gettextheight()
{
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	if (!textheight){
		TEXTMETRIC tm;
		POINT p;
		if (GetTextMetrics(mdc,&tm)){
			p.x = tm.tmAscent;
			p.y = tm.tmHeight;
			LPtoDP(mdc,&p,1);
			baseline = p.x;
			textheight = p.y;
		};
	};
	return textheight;
}

int MTGDIBitmap::getcharwidth(char c)
{
	int w;
	
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	if (GetCharWidth(mdc,(int)c,(int)c,&w)) return w;
	else return 0;
}

int MTGDIBitmap::getbaseline()
{
	if (!mtextinit){
		SetBkMode(mdc,TRANSPARENT);
		SelectObject(mdc,mfont);
		SetTextColor(mdc,mtxtcolor);
		mtextinit = true;
	};
	if (!baseline){
		TEXTMETRIC tm;
		POINT p;
		if (GetTextMetrics(mdc,&tm)){
			p.x = tm.tmAscent;
			p.y = tm.tmHeight;
			LPtoDP(mdc,&p,1);
			baseline = p.x;
			textheight = p.y;
		};
	};
	return baseline;
}
//---------------------------------------------------------------------------
// MTGDIMask
//---------------------------------------------------------------------------
MTGDIMask::MTGDIMask(MTDisplayDevice *d,int w,int h)
{
	int x,y;

	device = d;
	width = w;
	height = h;
	mtmemzero(&bmi,sizeof(bmi));
	bmi.biSize = sizeof(bmi);
	bmi.biWidth = width;
	bmi.biHeight = -height;
	bmi.biPlanes = 1;
	bmi.biBitCount = 8;
	for (x=0,y=255;x<256;x++,y--) *(int*)&pal[x] = y*0x010101;
	mdc = CreateCompatibleDC(GetDC(0));
	bmp = CreateDIBSection(0,(BITMAPINFO*)&bmi,DIB_RGB_COLORS,&mbits,0,0);
	oldbmp = (HBITMAP)SelectObject(mdc,bmp);
	mpitch = ((width+3)>>2)<<2;
}

MTGDIMask::~MTGDIMask()
{
	SelectObject(mdc,oldbmp);
	DeleteObject(bmp);
	DeleteDC(mdc);
}

void MTGDIMask::blur(int amount)
{
	if (!amount) return;
	void *tmp = si->memalloc(mpitch*height,0);
	asm_blur(tmp,mbits,width,height,mpitch,amount);
	si->memfree(tmp);
}

void MTGDIMask::opacity(int amount)
{
	asm_opacity(mbits,width,height,mpitch,amount);
}

void MTGDIMask::fill(int x,int y,int w,int h,int amount)
{
	DeleteObject(SelectObject(mdc,CreateSolidBrush(amount*0x010101)));
	PatBlt(mdc,x,y,w,h,PATCOPY);
}
//---------------------------------------------------------------------------
