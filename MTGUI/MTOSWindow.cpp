//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTOSWindow.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
extern HINSTANCE instance;
struct OS_PROCS{
	HWND wnd;
	WNDPROC proc;
	MTOSWindow *oswnd;
};
MTHash *procs;
HDC lastdc;
OS_PROCS *lastproc;
//---------------------------------------------------------------------------
void initOSWindow()
{
	WNDCLASSEX wndclass;

	procs = si->hashcreate(4);
	mtmemzero(&wndclass,sizeof(wndclass));
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_BYTEALIGNCLIENT|CS_PARENTDC;
	wndclass.lpfnWndProc = MTOSWindow::CustomProc;
	wndclass.hInstance = instance;
	wndclass.hCursor = (HCURSOR)LoadCursor(0,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndclass.lpszClassName = "MT3OSWindow";
	RegisterClassEx(&wndclass);
}

void uninitOSWindow()
{
	procs->clear(true);
	si->hashdelete(procs);
	UnregisterClass("MT3OSWindow",instance);
}
//---------------------------------------------------------------------------
// MTControl
//   MTOSWindow
//---------------------------------------------------------------------------
MTOSWindow::MTOSWindow(int tag,MTWinControl *p,int l,int t,int w,int h):
MTControl(MTC_OSWINDOW,tag,p,l,t,w,h),
compat(false),
active(true),
lastwnd(0),
dc(0),
b(0)
{
	OS_PROCS *osproc = 0;
	MTPoint op = {left,top};

	flags |= (MTCF_ACCEPTINPUT|MTCF_NOTIFYPOS);
	if ((!p) || (!p->dsk) || (!p->dsk->mwnd)) return;
	parent->toscreen(op);
	wnd = CreateWindowEx((compat)?0:WS_EX_TRANSPARENT,"MT3OSWindow","",WS_POPUP,op.x,op.y,width,height,(HWND)p->dsk->mwnd,0,instance,0);
	osproc = mtnew(OS_PROCS);
	osproc->wnd = wnd;
	osproc->oswnd = this;
	procs->additem((int)wnd,osproc);
	gi->setcontrolname(this,"oswindow");
	if (compat){
		ShowWindow(wnd,SW_NORMAL);
	};
	b = CreateCompatibleBitmap(GetDC(0),width,height);
	dc = CreateCompatibleDC(GetDC(0));
	oldb = (HBITMAP)SelectObject(dc,b);
}

MTOSWindow::~MTOSWindow()
{
	if (lastdc==dc){
		lastdc = 0;
		lastproc = 0;
	};
	DeleteDC(dc);
	DeleteObject(b);
	dc = 0;
//	switchflags(MTCF_NOTIFYPOS,false);
	procs->delitem((int)wnd,true);
	DestroyWindow(wnd);
}

void MTOSWindow::setbounds(int l,int t,int w,int h)
{
	int oldw = width;
	int oldh = height;
	int bflags = flags;
	MTPoint p;
	void *rgn;
	OS_PROCS *osproc;

	if (!compat) flags |= MTCF_DONTDRAW;
	MTControl::setbounds(l,t,w,h);
	if ((width!=oldw) || (height!=oldh)){
		SelectObject(dc,oldb);
		DeleteObject(b);
		b = CreateCompatibleBitmap(GetDC(0),width,height);
		SelectObject(dc,b);
	};
	rgn = parent->dsk->getvisiblergn(false);
	offsetrgn(rgn,-left-parent->box,-top-parent->boy);
	p.x = left;
	p.y = top;
	parent->toscreen(p);
	SetWindowRgn(wnd,(HRGN)rgn,false);
	MoveWindow(wnd,p.x,p.y,width,height,false);
	if (!compat){
		procs->reset();
		while ((osproc = (OS_PROCS*)procs->next())){
			if (osproc->oswnd==this){
				SendMessage(osproc->wnd,WM_PAINT,0,0);
			};
		};
		flags = bflags;
		if (parent){
			MTCMessage msg = {MTCM_CHANGE,0,this};
			parent->message(msg);
		};
	};
}

void MTOSWindow::draw(MTRect &rect)
{
	MTBitmap *mb;
	int ox = left;
	int oy = top;
	HDC ddc;
	MTRect r = {0,0,width,height};

	if ((!compat) || (!active)){
		if (&rect) cliprect(r,rect);
		preparedraw(&mb,ox,oy);
		ddc = (HDC)mb->open(0);
		BitBlt(ddc,ox+r.left,oy+r.top,r.right-r.left,r.bottom-r.top,dc,r.left,r.top,SRCCOPY);
		mb->close(ddc);
	};
	MTControl::draw(rect);
}

bool MTOSWindow::message(MTCMessage &msg)
{
	int wm,delta;
	MTPoint p;
	void *rgn;

	if (msg.msg==MTCM_POSCHANGED){
		rgn = parent->dsk->getvisiblergn(false);
		offsetrgn(rgn,-left-parent->box,-top-parent->boy);
		p.x = left;
		p.y = top;
		parent->toscreen(p);
		SetWindowRgn(wnd,(HRGN)rgn,true);
		MoveWindow(wnd,p.x,p.y,width,height,true);
		return true;
	};
	if (compat){
		if (msg.msg==MTCM_ACTIVE){
			active = (msg.param1!=0);
			if (msg.param1){
				ShowWindow(wnd,SW_NORMAL);
			}
			else{
				BitBlt(dc,0,0,width,height,GetDC(wnd),0,0,SRCCOPY);
				ShowWindow(wnd,SW_HIDE);
			};
		};
		return MTControl::message(msg);
	};
	switch (msg.msg){
/*	case MTCM_POSCHANGED:
		rgn = parent->dsk->getvisiblergn(false);
		offsetrgn(rgn,-left-parent->box,-top-parent->boy);
		p.x = left;
		p.y = top;
		parent->toscreen(p);
		SetWindowRgn(wnd,(HRGN)rgn,false);
		MoveWindow(wnd,p.x,p.y,width,height,true);
		break;*/
	case MTCM_MOUSEMOVE:
		SendMessage(wnd,WM_MOUSEMOVE,lastwparam,msg.x|(msg.y<<16));
		break;
	case MTCM_MOUSEDOWN:
		if (msg.button==DB_LEFT) wm = WM_LBUTTONDOWN;
		else if (msg.button==DB_RIGHT) wm = WM_RBUTTONDOWN;
		else wm = WM_MBUTTONDOWN;
		if (msg.buttons & DB_DOUBLE){
			SendMessage(wnd,wm,lastwparam,msg.x|(msg.y<<16));
			if (msg.button==DB_LEFT) wm = WM_LBUTTONDBLCLK;
			else if (msg.button==DB_RIGHT) wm = WM_RBUTTONDBLCLK;
			else wm = WM_MBUTTONDBLCLK;
		};
		SendMessage(wnd,wm,lastwparam,msg.x|(msg.y<<16));
		break;
	case MTCM_MOUSEUP:
		if (msg.button==DB_LEFT) wm = WM_LBUTTONUP;
		else if (msg.button==DB_RIGHT) wm = WM_RBUTTONUP;
		else wm = WM_MBUTTONUP;
		SendMessage(wnd,wm,lastwparam,msg.x|(msg.y<<16));
		break;
	case MTCM_MOUSEWHEEL:
		delta = msg.param3*40;
		SendMessage(wnd,0x020A,lastwparam,lastlparam);
		break;
	case MTCM_CHAR:
		SendMessage(wnd,WM_CHAR,lastwparam,lastlparam);
		break;
	case MTCM_KEYDOWN:
		SendMessage(wnd,WM_KEYDOWN,lastwparam,lastlparam);
		break;
	case MTCM_KEYUP:
		SendMessage(wnd,WM_KEYUP,lastwparam,lastlparam);
		break;
	};
	return MTControl::message(msg);
}

void MTOSWindow::setcompatible(bool compatible)
{
	compat = compatible;
	SetWindowLong(wnd,GWL_EXSTYLE,(compat)?0:WS_EX_TRANSPARENT);
	if ((compat) && (active)) ShowWindow(wnd,SW_NORMAL);
}

void* MTOSWindow::getoshandle()
{
	return wnd;
}

bool MTOSWindow::patchoscode(void *lib)
{
	static int search[] = {(int)GetDC,(int)GetDCEx,(int)GetWindowDC,(int)ReleaseDC,(int)BeginPaint,(int)EndPaint,(int)GetUpdateRect,(int)GetUpdateRgn,(int)SetCapture,(int)BitBlt,(int)StretchBlt};
	static int replace[] = {(int)mGetDC,(int)mGetDCEx,(int)mGetWindowDC,(int)mReleaseDC,(int)mBeginPaint,(int)mEndPaint,(int)mGetUpdateRect,(int)mGetUpdateRgn,(int)mSetCapture,(int)mBitBlt,(int)mStretchBlt};
	const nsearch = sizeof(search)/sizeof(int);
	int x,c;
	char *s,*e;
	char *p,*m;

	si->getlibmemoryrange(lib,0,(void**)&s,(int*)&e);
	if (!VirtualProtect(s,(int)e,PAGE_EXECUTE_READWRITE,(DWORD*)&x)){
		MEMORY_BASIC_INFORMATION meminfo;
		FLOGD1("%s - [VST] WARNING: VirtualProtect error %d!"NL,GetLastError());
		x = VirtualQuery(s,&meminfo,sizeof(meminfo));
		if (x>0) DUMP(&meminfo,x,0);
		return false;
	};
	if (x & PAGE_EXECUTE_READWRITE) return true;
	e += (int)s;
	for (x=0;x<nsearch;x++){
		c = 0;
		m = (char*)&search[x];
		for (p=s;p<e;p++){
retry:
			if (*p==*m){
				if (++c==4){
					*(int*)(p-3) = replace[x];
					m = (char*)&search[x];
					c = 0;
				}
				else m++;
			}
			else if (c>0){
				m = (char*)&search[x];
				c = 0;
				goto retry;
			};
		};
	};
	return true;
}

LRESULT CALLBACK MTOSWindow::CustomProc(HWND wnd,unsigned int msg,unsigned int wparam,long lparam)
{
	MTOSWindow *oswnd;
	OS_PROCS *osproc = 0;
	HWND fwd;
	int l;
	POINT p;
	
	FENTER4("MTOSWindow::CustomProc(%.8X,%.8X,%.8X,%.8X)",wnd,msg,wparam,lparam);
	osproc = (OS_PROCS*)procs->getitem((int)wnd);
	if (!osproc){
		LEAVE();
		return DefWindowProc(wnd,msg,wparam,lparam);
	};
	oswnd = osproc->oswnd;
	if (msg==WM_PARENTNOTIFY){
		switch (wparam & 0xFFFF){
		case WM_CREATE:
			if (oswnd->compat) break;
			l = GetWindowLong((HWND)lparam,GWL_STYLE);
			if (l & (WS_CAPTION|WS_POPUP|WS_BORDER|WS_THICKFRAME|WS_VSCROLL|WS_HSCROLL|WS_DLGFRAME)) break;
			osproc = mtnew(OS_PROCS);
			osproc->wnd = (HWND)lparam;
			osproc->proc = (WNDPROC)GetWindowLong((HWND)lparam,GWL_WNDPROC);
			osproc->oswnd = oswnd;
			procs->additem(lparam,osproc);
			SetWindowLong((HWND)lparam,GWL_WNDPROC,(LONG)CustomProc);
			PostMessage((HWND)lparam,WM_PAINT,0,0);
			break;
		case WM_DESTROY:
			if (lastproc==(OS_PROCS*)procs->getitem(lparam)){
				lastdc = 0;
				lastproc = 0;
			};
			procs->delitem(lparam,true);
			break;
		};
	};
	if (!osproc->proc){
		fwd = osproc->oswnd->lastwnd;
		switch (msg){
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case 0x20A:
			p.x = lparam & 0xFFFF;
			p.y = lparam>>16;
			fwd = ChildWindowFromPoint(wnd,p);
			if (!fwd) fwd = osproc->oswnd->lastwnd;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
			if ((fwd) && (fwd!=wnd)){
				osproc->oswnd->lastwnd = fwd;
				LEAVE();
				return SendMessage(fwd,msg,wparam,lparam);
			};
			break;
		};
	};
/*
	case WM_PAINT:
		if (osproc->proc){
			rgn = oswnd->parent->getvisiblergn(true,oswnd);
			if (isemptyrgn(rgn)){
				return 0;
			};
			offsetrgn(rgn,-oswnd->left,-oswnd->top);
			SetWindowRgn(oswnd->wnd,(HRGN)rgn,false);
			ret = CallWindowProc(osproc->proc,wnd,msg,wparam,lparam);
			SetWindowRgn(wnd,0,false);
			deletergn(rgn);
			if (oswnd->parent){
				MTCMessage msg = {MTCM_CHANGE,0,oswnd};
				GetUpdateRect(wnd,(RECT*)&msg.dr,false);
				HDC dc = GetDC(wnd);
				HDC ddc = (HDC)oswnd->b->open(0);
				if (msg.dr.right<msg.dr.left) BitBlt(ddc,msg.dr.left,msg.dr.top,msg.dr.right-msg.dr.left,msg.dr.bottom-msg.dr.top,dc,msg.dr.left,msg.dr.top,SRCCOPY);
				else BitBlt(ddc,0,0,oswnd->width,oswnd->height,dc,0,0,SRCCOPY);
				ReleaseDC(wnd,dc);
				oswnd->b->close(ddc);
				oswnd->parent->message(msg);
			};
			return ret;
		};
		break;
*/
	LEAVE();
	if (osproc->proc){
//		FLOG4("%.8X->Message(%X,%.8X,%.8X)"NL,wnd,msg,wparam,lparam);
		MTTRY
			return CallWindowProc(osproc->proc,wnd,msg,wparam,lparam);
		MTCATCH
			return 0;
		MTEND
	}
	else return DefWindowProc(wnd,msg,wparam,lparam);
}

HDC WINAPI MTOSWindow::mGetDC(HWND hWnd)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return GetDC(hWnd);
	lastdc = osproc->oswnd->dc;
	lastproc = osproc;
	return osproc->oswnd->dc;
}

HDC WINAPI MTOSWindow::mGetDCEx(HWND hWnd,HRGN hrgnClip,DWORD flags)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return GetDCEx(hWnd,hrgnClip,flags);
	lastdc = osproc->oswnd->dc;
	lastproc = osproc;
	return osproc->oswnd->dc;
}

HDC WINAPI MTOSWindow::mGetWindowDC(HWND hWnd)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return GetWindowDC(hWnd);
	lastdc = osproc->oswnd->dc;
	lastproc = osproc;
	return osproc->oswnd->dc;
}

int WINAPI MTOSWindow::mReleaseDC(HWND hWnd,HDC hDC)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return ReleaseDC(hWnd,hDC);
/*
	MTCMessage msg = {MTCM_CHANGE,0,osproc->oswnd};
	osproc->oswnd->parent->message(msg);
	osproc->oswnd->parent->dsk->flushend();
*/
	return 1;
}

HDC WINAPI MTOSWindow::mBeginPaint(HWND hWnd,LPPAINTSTRUCT lpPaint)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return BeginPaint(hWnd,lpPaint);
	lpPaint->fErase = false;
	lpPaint->hdc = osproc->oswnd->dc;
	RECT &r = lpPaint->rcPaint;
	r.left = r.top = 0;
	r.right = osproc->oswnd->width;
	r.bottom = osproc->oswnd->height;
	return osproc->oswnd->dc;
}

BOOL WINAPI MTOSWindow::mEndPaint(HWND hWnd,CONST PAINTSTRUCT *lpPaint)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return EndPaint(hWnd,lpPaint);
	MTCMessage msg = {MTCM_CHANGE,0,osproc->oswnd};
	osproc->oswnd->parent->message(msg);
	return TRUE;
}

BOOL WINAPI MTOSWindow::mGetUpdateRect(HWND hWnd,LPRECT lpRect,BOOL bErase)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return GetUpdateRect(hWnd,lpRect,bErase);
	if (!lpRect) return TRUE;
	RECT &r = *lpRect;
	r.left = r.top = 0;
	r.right = osproc->oswnd->width;
	r.bottom = osproc->oswnd->height;
	return TRUE;
}

BOOL WINAPI MTOSWindow::mGetUpdateRgn(HWND hWnd,HRGN hRgn,BOOL bErase)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return GetUpdateRgn(hWnd,hRgn,bErase);
	SetRectRgn(hRgn,0,0,osproc->oswnd->width,osproc->oswnd->height);
	return TRUE;
}

HWND WINAPI MTOSWindow::mSetCapture(HWND hWnd)
{
	OS_PROCS *osproc = (OS_PROCS*)procs->getitem((int)hWnd);

	if ((!osproc) || (osproc->oswnd->compat)) return SetCapture(hWnd);
	return NULL;
}

BOOL WINAPI MTOSWindow::mBitBlt(HDC hdcDest,int nXDest,int nYDest,int nWidth,int nHeight,HDC hdcSrc,int nXSrc,int nYSrc,DWORD dwRop)
{
	BOOL ok;

	ok = BitBlt(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
	if (!ok) return FALSE;
	if (hdcDest==lastdc){
		MTCMessage msg = {MTCM_CHANGE,0,lastproc->oswnd};
		msg.dr.left = nXDest;
		msg.dr.top = nYDest;
		msg.dr.right = nXDest+nWidth;
		msg.dr.bottom = nYDest+nHeight;
		lastproc->oswnd->parent->message(msg);
		lastproc->oswnd->parent->dsk->flushend();
	};
	return TRUE;
}

BOOL WINAPI MTOSWindow::mStretchBlt(HDC hdcDest,int nXDest,int nYDest,int nWidthDest,int nHeightDest,HDC hdcSrc,int nXSrc,int nYSrc,int nWidthSrc,int nHeightSrc,DWORD dwRop)
{
	BOOL ok;

	ok = StretchBlt(hdcDest,nXDest,nYDest,nWidthDest,nHeightDest,hdcSrc,nXSrc,nYSrc,nWidthSrc,nHeightSrc,dwRop);
	if (!ok) return FALSE;
	if (hdcDest==lastdc){
		MTCMessage msg = {MTCM_CHANGE,0,lastproc->oswnd};
		msg.dr.left = nXDest;
		msg.dr.top = nYDest;
		msg.dr.right = nXDest+nWidthDest;
		msg.dr.bottom = nYDest+nHeightDest;
		lastproc->oswnd->parent->message(msg);
		lastproc->oswnd->parent->dsk->flushend();
	};
	return TRUE;
}
//---------------------------------------------------------------------------
