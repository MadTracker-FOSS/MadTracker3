//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPattEditor.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTPattEditor.h"
#include "MTPattManager.h"
#include "MTXInput.h"
#include "../Interface/MTObjectsRES.h"
//---------------------------------------------------------------------------
MTShortcut penav[16] = {
	{0,KB_DOWN,MTT_nextline,0,0},
	{0,KB_UP,MTT_prevline,0,0},
	{0,KB_TAB,MTT_nexttrack,0,0},
	{MTSF_SHIFT,KB_TAB,MTT_prevtrack,0,0},
	{0,KB_HOME,MTT_firstline,0,0},
	{0,KB_END,MTT_lastline,0,0},
	{0,KB_PAGEDOWN,MTT_nextbeat,0,0},
	{0,KB_PAGEUP,MTT_prevbeat,0,0},
	{0,KB_RIGHT,MTT_right,0,0},
	{0,KB_LEFT,MTT_left,0,0},
	{0,KB_DELETE,MTT_delete,0,0},
	{0,KB_BACK,MTT_removecell,0,0},
	{MTSF_SHIFT,KB_BACK,MTT_removeline,0,0},
	{0,KB_INSERT,MTT_insertcell,0,0},
	{MTSF_SHIFT,KB_INSERT,MTT_insertline,0,0},
	{0,0,0,0,0}
};
int peksgroup;
//---------------------------------------------------------------------------
// Pattern editor
//---------------------------------------------------------------------------
void initPattEditor()
{
	int x;

	if (!gi) return;
	peksgroup = gi->registershortcutgroup();
	for (x=0;x<sizeof(penav)/sizeof(MTShortcut);x++){
		if (!penav[x].user) break;
		penav[x].description = (char*)res->getresource(MTR_TEXT,penav[x].user,0);
		gi->registershortcut(&penav[x]);
	};
}

void uninitPattEditor()
{
	int x;

	if (!gi) return;
	for (x=0;x<sizeof(penav)/sizeof(MTShortcut);x++){
		if (!penav[x].user) break;
		gi->unregistershortcut(&penav[x]);
		res->releaseresource(penav[x].description);
		penav[x].description = 0;
	};
}
//---------------------------------------------------------------------------
bool MTCT PatternEdit(MTObject *object,MTWindow *window,int flags,MTUser *user)
{
	MTTabControl *tab;
	MTWindow *pattern;
	MTCustomWinControl *data;
	MTPattManager *pm;

	if (window->uid==(int)object) return true;
	if (window->tag!=MTO_PATTERN){
		tab = (MTTabControl*)window->getcontrolfromuid(1);
		if (!tab) return false;
		tab->delcontrols(true);
		pattern = tab->loadpage(res,MTW_pattern,true);
		if (!pattern) return false;
		data = (MTCustomWinControl*)pattern->getcontrolfromuid(MTC_data);
		if (!data) return false;
		pm = new MTPattManager(data);
		data->behaviours = pm;
		data->align = MTCA_CLIENT;
		if (!tab->page) tab->setpageid(0);
	}
	else{
		tab = (MTTabControl*)window->getcontrolfromuid(1);
		if (!tab) return false;
		pattern = (MTWindow*)tab->getcontrolfromuid(MTW_pattern);
		if (!pattern) return false;
		data = (MTCustomWinControl*)pattern->getcontrolfromuid(MTC_data);
		if (!data) return false;
		pm = (MTPattManager*)data->behaviours;
	};
	pm->setpattern((MTPattern*)object);
	pattern->focus(data);
	return true;
}
//---------------------------------------------------------------------------
