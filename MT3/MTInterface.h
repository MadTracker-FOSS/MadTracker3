//---------------------------------------------------------------------------
//	$Id: MTInterface.h 100 2005-11-30 20:19:39Z Yannick $
//---------------------------------------------------------------------------
#ifndef MTINTERFACE_INCLUDED
#define MTINTERFACE_INCLUDED

#include "MTXExtension.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
#include "MTXControls.h"
#include "MTXModule.h"
//---------------------------------------------------------------------------
struct RefreshStruct{
	int id;
	RefreshProc proc;
	void *param;
};

class MTSplashLogo : public MTCustomWinBehaviours{
public:
	MTSplashLogo(MTCustomWinControl *control);
	void MTCT ondestroy();
	void MTCT ondraw(MTRect &rect);
	bool MTCT onmessage(MTCMessage &msg);
	void MTCT setalpha(int alpha,int interval,int increment);
private:
	int destalpha;
	int interval,increment;
	int timer;
};
//---------------------------------------------------------------------------
bool initInterface();
void uninitInterface();
bool startInterface();
void stopInterface();
void showInterface();
//---------------------------------------------------------------------------
void loadmodule(const char *filename);
void setmodule(void *module);
//---------------------------------------------------------------------------
extern void *wnd;
extern MTDesktop *mtdsk;
extern MTResources *mtres,*skinres;
extern Skin *skin;
extern MTImageList *sysimages;
extern MTModule *cmodule;
extern MTArray *refreshprocs;
extern bool candesign;
extern int dragx,dragy;
//---------------------------------------------------------------------------
#endif
