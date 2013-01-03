//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTInstrEditor.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTInstrEditor.h"
#include <MTXAPI/MTXInput.h>
#include "../Interface/MTObjectsRES.h"
//---------------------------------------------------------------------------
MTShortcut ienav[2] = {
	{0,KB_SPACE,0,0,0},
	{0,0,0,0,0}
};
int ieksgroup;
//---------------------------------------------------------------------------
// Instrument editor
//---------------------------------------------------------------------------
void initInstrEditor()
{
	int x;

	if (!gi) return;
	ieksgroup = gi->registershortcutgroup();
	for (x=0;x<sizeof(ienav)/sizeof(MTShortcut);x++){
		if (!ienav[x].user) break;
		ienav[x].description = (char*)res->getresource(MTR_TEXT,ienav[x].user,0);
		gi->registershortcut(&ienav[x]);
	};
}

void uninitInstrEditor()
{
	int x;

	if (!gi) return;
	for (x=0;x<sizeof(ienav)/sizeof(MTShortcut);x++){
		if (!ienav[x].user) break;
		gi->unregistershortcut(&ienav[x]);
		res->releaseresource(ienav[x].description);
		ienav[x].description = 0;
	};
}
//---------------------------------------------------------------------------
bool MTCT InstrumentEdit(MTObject *object,MTWindow *window,int flags,MTUser *user)
{
	MTTabControl *tab;
	MTWindow *instrument;

	if (window->uid==(int)object) return true;
	if (window->tag!=MTO_INSTRUMENT){
		tab = (MTTabControl*)window->getcontrolfromuid(1);
		if (!tab) return false;
		tab->delcontrols(true);
		instrument = tab->loadpage(res,MTW_instrument,true);
		if (!instrument) return false;
		if (!tab->page) tab->setpageid(0);
	}
	else{
		tab = (MTTabControl*)window->getcontrolfromuid(1);
		if (!tab) return false;
		instrument = (MTWindow*)tab->getcontrolfromuid(MTW_instrument);
		if (!instrument) return false;
	};
	return true;
}
//---------------------------------------------------------------------------
