//---------------------------------------------------------------------------
//	$Id: MTWC_Main2.h 30 2005-08-10 12:39:22Z Yannick $
//---------------------------------------------------------------------------
#ifndef MTWC_MAIN2_INCLUDED
#define MTWC_MAIN2_INCLUDED
//---------------------------------------------------------------------------
#include <MTXAPI/MTXWrapper.h>
//---------------------------------------------------------------------------

// This is the tracker's control toolbar where the Play, Pause, Stop buttons etc. are.
// Some of these pointer members have names that don't correlate
// with the toolbar in MT2, e.g. Oscillator. These are probably MT3 features.
class MTWC_main2: public MTWrapper
{
public:
    MTButton* bconfig;
    MTButton* bnetwork;
    MTButton* bhelp;
    MTButton* btools;
    MTButton* bview;
    MTButton* binfo;
    MTButton* bfile;
    MTButton* bnew;
    MTPanel* panel1;
    MTButton* brecord;
    MTButton* bplaypause;
    MTButton* bstop;
    MTButton* bend;
    MTButton* bfastforward;
    MTButton* brewind;
    MTButton* bbegin;
    MTPanel* panel2;
    MTUserList* list2;
    MTUserList* list1;

    MTWC_main2(MTWindow* w);

    ~MTWC_main2();

    bool MTCT onmessage(MTCMessage& msg);

private:
//	Your private declarations
};

//---------------------------------------------------------------------------
extern MTWC_main2* w_main2;
//---------------------------------------------------------------------------
#endif
