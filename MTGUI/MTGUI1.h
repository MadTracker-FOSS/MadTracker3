//---------------------------------------------------------------------------
#ifndef MTGUI1_INCLUDED
#define MTGUI1_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
#include <MTXAPI/MTXGUI.h>

//---------------------------------------------------------------------------
static const int guietype = FOURCC('G', 'U', 'I', 'E');

#define NICE_RATIO 2.5
#define MTW_CAPTION     1
#define MTW_RESIZABLE   2
#define MTW_TOPMOST     4
#define MTW_TRANSPARENT 16

#define MTTF_SKINTIMER 0x80000000

#define MTSF_MOUSE     0x01
#define MTSF_JOYSTICK  0x02
#define MTSF_MIDI      0x04
#define MTSF_GROUP     0x08
#define MTSF_SHIFT     0x10
#define MTSF_CONTROL   0x20
#define MTSF_ALT       0x40
#define MTSF_UICONTROL 0x80

struct MTUndo;
struct MTShortcut;

class MTControl;

class MTWinControl;

class MTWindow;

class MTDesktop;

//---------------------------------------------------------------------------
typedef void (MTCT *MTCommand)(MTShortcut *, MTControl *, MTUndo *);
//---------------------------------------------------------------------------
#include "MTGUITools.h"
#include <MTXAPI/MTXSystem.h>
#include <MTXAPI/MTXDisplay.h>
#include <MTXAPI/MTXSkin.h>

//---------------------------------------------------------------------------
struct MTGUIPreferences
{
    char skin[256];
    unsigned char cursorspeed;
    bool shadows;
    bool dialogshadows;
    char shadowx, shadowy;
    unsigned char shadowop, shadowblur;
    unsigned char scroll1, scroll2;
    bool fadeout;
    bool transpmenus;
    bool animctrl;
    int fadeouttime;
    int fadeoutinterval;
    int animtime;
    int animinterval;
};


//---------------------------------------------------------------------------
#ifdef _DEBUG

void tracergn(void *rgn, MTBitmap *bmp);

void tracerect(MTRect &r, MTBitmap *bmp);

#endif

//---------------------------------------------------------------------------
extern bool candesign;

extern bool design;

extern bool blockinput;

extern void *hinst;

extern MTInterface *mtinterface;

extern MTSystemInterface *si;

extern MTDisplayInterface *di;

extern MTGUIInterface *gi;

extern MTGUIPreferences guiprefs;

extern MTBitmap *screen;

extern int doubleclick, dragx, dragy, gridx, gridy;

extern int guitick;

extern int lastwparam, lastlparam;

extern Skin *skin;
//---------------------------------------------------------------------------
#endif
