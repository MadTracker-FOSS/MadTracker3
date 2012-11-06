//---------------------------------------------------------------------------
#ifndef MTOSWINDOW_INCLUDED
#define MTOSWINDOW_INCLUDED
//---------------------------------------------------------------------------
#include "MTControl.h"
#include "../Headers/MTXDisplay.h"
#include "../Headers/MTXSystem.h"
#include <windows.h>
//---------------------------------------------------------------------------
class MTOSWindow : public MTControl{
public:
	MTOSWindow(int tg,MTWinControl *p,int l,int t,int w,int h);
	~MTOSWindow();
	void MTCT setbounds(int l,int t,int w,int h);
	void MTCT draw(MTRect &rect);
	bool MTCT message(MTCMessage&);
	virtual void MTCT setcompatible(bool compatible);
	virtual void* MTCT getoshandle();
	virtual bool MTCT patchoscode(void *lib);
private:
	friend void initOSWindow();
	HWND wnd,lastwnd;
	HDC dc;
	HBITMAP b,oldb;
	bool compat,active;
	static LRESULT CALLBACK CustomProc(HWND wnd,unsigned int msg,unsigned int wparam,long lparam);
	static HDC WINAPI mGetDC(HWND hWnd);
	static HDC WINAPI mGetDCEx(HWND hWnd,HRGN hrgnClip,DWORD flags);
	static HDC WINAPI mGetWindowDC(HWND hWnd);
	static int WINAPI mReleaseDC(HWND hWnd,HDC hDC);
	static HDC WINAPI mBeginPaint(HWND hWnd,LPPAINTSTRUCT lpPaint);
	static BOOL WINAPI mEndPaint(HWND hWnd,CONST PAINTSTRUCT *lpPaint);
	static BOOL WINAPI mGetUpdateRect(HWND hWnd,LPRECT lpRect,BOOL bErase);
	static BOOL WINAPI mGetUpdateRgn(HWND hWnd,HRGN hRgn,BOOL bErase);
	static HWND WINAPI mSetCapture(HWND hWnd);
	static BOOL WINAPI mBitBlt(HDC hdcDest,int nXDest,int nYDest,int nWidth,int nHeight,HDC hdcSrc,int nXSrc,int nYSrc,DWORD dwRop);
	static BOOL WINAPI mStretchBlt(HDC hdcDest,int nXDest,int nYDest,int nWidthDest,int nHeightDest,HDC hdcSrc,int nXSrc,int nYSrc,int nWidthSrc,int nHeightSrc,DWORD dwRop);
};
//---------------------------------------------------------------------------
void initOSWindow();
void uninitOSWindow();
//---------------------------------------------------------------------------
#endif
