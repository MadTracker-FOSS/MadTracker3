//---------------------------------------------------------------------------
//
//  MadTracker 3 Window Class Wrapper
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTWC_Main.cpp 68 2005-08-26 22:19:12Z Yannick $
//
//---------------------------------------------------------------------------
//	Needed includes and variables
//---------------------------------------------------------------------------
#include "MTWC_Main.h"
#include <MTXAPI/RES/MT3RES.h>

//---------------------------------------------------------------------------
extern MTGUIInterface* gi;

MTWC_main* w_main;
//---------------------------------------------------------------------------
//	User includes, variables and functions
//---------------------------------------------------------------------------
#include "MTExtensions.h"
#include "MTInterface.h"
#include <stdlib.h>

//---------------------------------------------------------------------------
extern MT3Interface* mi;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	Wrapper class code
//---------------------------------------------------------------------------
MTWC_main::MTWC_main(MTWindow* w):
    MTWrapper(w)
{
// FIXME
    toolbar = (MTToolBar*) gi->newcontrol(MTC_TOOLBAR, 0, wthis, 0, 0, 512, 32, 0);
    mfile = (MTButton*) wthis->getcontrolfromuid(MTC_mfile);
    medit = (MTButton*) wthis->getcontrolfromuid(MTC_medit);
    mmodule = (MTButton*) wthis->getcontrolfromuid(MTC_mmodule);
    mwindow = (MTButton*) wthis->getcontrolfromuid(MTC_mwindow);
    mhelp = (MTButton*) wthis->getcontrolfromuid(MTC_mhelp);
    w->wrapper = this;
//---------------------------------------------------------------------------
//	User construction code
//---------------------------------------------------------------------------
    mo.x = mo.y = 0;
    moving = triggered = false;

    mfile->setparent(toolbar);
    medit->setparent(toolbar);
    mmodule->setparent(toolbar);
    mwindow->setparent(toolbar);
    mhelp->setparent(toolbar);
    MTMenu* menu = (MTMenu*) gi->newcontrol(MTC_MENU, 0, mtdsk, 0, 0, 0, 0, 0);
    MTMenu* submenu = (MTMenu*) gi->newcontrol(MTC_MENU, 0, mtdsk, 0, 0, 0, 0, 0);
    menu->flags |= MTCF_DONTSAVE;
    submenu->flags |= MTCF_DONTSAVE;
    submenu->additem("Module", -1, 0, false, 0);
    submenu->additem("Workspace", -2, 0, false, 0);
    submenu->additem("Workspace", -3, 0, false, 0);
    submenu->additem("|", -1, 0, false, 0);
    submenu->additem("Pattern", -1, 0, false, 0);
    submenu->additem("Instrument", -1, 0, false, 0);
    submenu->additem("Oscillator", -1, 0, false, 0);
    ((MTMenuItem*) menu->additem("New", -1, 0, false, 0))->submenu = submenu;
    menu->additem("Load...", -1, 0, false, 0);
    menu->additem("Save", -1, 0, false, 0);
    menu->additem("Save As...", -1, 0, false, 0);
    menu->additem("Publish...", -1, 0, false, 0);
    menu->additem("|", -1, 0, false, 0);
    menu->additem("Song Properties...", -1, 0, false, 0);
    menu->additem("|Recent Files", -1, 0, false, 0);
    menu->additem("|", -1, 0, false, 0);
    menu->additem("Quit", -1, 0, false, 0);
    mfile->popup = menu;
}

MTWC_main::~MTWC_main()
{
//---------------------------------------------------------------------------
//	User destruction code
//---------------------------------------------------------------------------

}

bool MTWC_main::onmessage(MTCMessage& msg)
{
//---------------------------------------------------------------------------
//	User messages handler
//---------------------------------------------------------------------------
    switch (msg.msg)
    {
        case MTCM_MOUSEDOWN:
            if (msg.button != DB_LEFT)
            {
                break;
            }
            if (msg.y < 0)
            {
                gi->getmousepos(mo);
                moving = true;
                return true;
            };
            break;
        case MTCM_MOUSEUP:
            if (msg.button != DB_LEFT)
            {
                break;
            }
            if (moving)
            {
                moving = triggered = false;
                return true;
            };
            break;
        case MTCM_MOUSEMOVE:
            if (moving)
            {
                MTPoint mo2;
                gi->getmousepos(mo2);
                if (!triggered)
                {
                    if ((abs(mo2.x - mo.x) > dragx) || (abs(mo2.y - mo.y) > dragy))
                    {
                        triggered = true;
                        gi->getwindowrect(wthis->dsk->mwnd, mwo, false);
                        mwo.left -= mo.x;
                        mwo.top -= mo.y;
                    }
                    else
                    {
                        return true;
                    }
                };
                gi->windowmove(wthis->dsk->mwnd, mwo.left + mo2.x, mwo.top + mo2.y, false);
                si->syswait(10);
                return true;
            };
            break;
        case MTCM_CLOSE:
            gi->windowaction(wthis->dsk->mwnd, MTWA_CLOSE);
            return true;
        default:
            break;
    };
    return false;
}
//---------------------------------------------------------------------------
