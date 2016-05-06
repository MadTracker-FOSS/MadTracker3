//---------------------------------------------------------------------------
//	$Id: MTWC_Main.h 30 2005-08-10 12:39:22Z Yannick $
//---------------------------------------------------------------------------
#ifndef MTWC_MAIN_INCLUDED
#define MTWC_MAIN_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXWrapper.h>

//---------------------------------------------------------------------------
class MTWC_main: public MTWrapper
{
public:
    MTToolBar *toolbar;
    MTButton *mfile;
    MTButton *medit;
    MTButton *mmodule;
    MTButton *mwindow;
    MTButton *mhelp;

    MTWC_main(MTWindow *w);

    ~MTWC_main();

    bool MTCT onmessage(MTCMessage &msg);

private:
//	Your private declarations
    MTPoint mo;
    MTRect mwo;
    bool moving, triggered;
};

//---------------------------------------------------------------------------
extern MTWC_main *w_main;
//---------------------------------------------------------------------------
#endif
