//---------------------------------------------------------------------------
#ifndef MTDISPLAY1_INCLUDED
#define MTDISPLAY1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXDisplay.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------

class MTWinControl;

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
