//---------------------------------------------------------------------------
#ifndef MTDISPLAY1_INCLUDED
#define MTDISPLAY1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int displaytype = FOURCC('X','D','I','S');

#define MAX_DISPLAYDEVICES 16

class MTWinControl;

#include "MTDisplayDevice.h"
#include "MTXExtension.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------
struct MTDisplayPreferences{
	int device;
	bool fullscreen;
	float texeladjustx;
	float texeladjusty;
	float skintexeladjustx;
	float skintexeladjusty;
};

struct MTDevice{
	char *devicename;
	MTDisplayDeviceManager *manager;
	int id;
};

class MTDisplayInterface : public MTXInterface{
public:
	bool fullscreen;

	MTDisplayInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual int MTCT getnumdevices();
	virtual const char* MTCT getdevicename(int id);
	virtual void MTCT setdevice(int id,bool silent = false);
	virtual bool MTCT adddevicemanager(MTDisplayDeviceManager *manager);
	virtual void MTCT deldevicemanager(MTDisplayDeviceManager *manager);
	virtual MTBitmap* MTCT newbitmap(int flags,int w,int h);
	virtual MTBitmap* MTCT newresbitmap(int flags,MTResources *res,int resid,int colorkey = -1);
	virtual MTBitmap* MTCT newfilebitmap(int flags,const char *filename,int colorkey = -1);
	virtual MTBitmap* MTCT newbmpbitmap(int flags,MTBitmap &orig,int colorkey = -1);
	virtual void MTCT setskinbitmap(int bmpid,MTBitmap *newskin);
	virtual MTMask* MTCT newmask(int w,int h);
	virtual void MTCT delbitmap(MTBitmap *bmp);
	virtual void MTCT delmask(MTMask *mask);
	virtual void MTCT adddesktop(MTWinControl *dsk);
	virtual void MTCT deldesktop(MTWinControl *dsk);
	virtual MTWinControl* MTCT getdefaultdesktop();
	virtual void MTCT checkbitmaps();
	virtual MTBitmap* MTCT getscreen();
	virtual void MTCT setfocus(bool focused);
	virtual void MTCT sync();
};
//---------------------------------------------------------------------------
inline int swapcolor(int color)
{
	return ((color & 0xFF)<<16)|(color & 0xFF00)|((color>>16) & 0xFF);
}

inline int combinecolor(int color1,int color2)
{
	unsigned int r1,g1,b1,r2,g2,b2;
	r1 = color1 & 0xFF; g1 = color1 & 0xFF00; b1 = color1 & 0xFF0000;
	r2 = color2 & 0xFF; g2 = color2 & 0xFF00; b2 = color2 & 0xFF0000;
	g2 >>= 8; b2 >>= 16;
	if (r2 & 0x80) r2++; if (g2 & 0x80) g2++; if (b2 & 0x80) b2++;
	r1 *= r2; g1 *= g2; b1 *= b2;
	return (r1|g1|b1)>>8;
}
//---------------------------------------------------------------------------
extern MTDisplayInterface *di;
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTGUIInterface *gi;
extern MTDisplayPreferences displayprefs;
extern MTBitmap *skinbmp[16];
extern MTDesktop *desktops[32];
extern int ndesktops;
extern MTBitmap *screen;
extern MTDisplayDevice *cdevive;
//---------------------------------------------------------------------------
#endif
