//---------------------------------------------------------------------------
//
//	MadTracker Artificial Intelligence System
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTAI1.cpp 99 2005-11-30 20:16:40Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTAI1.h"
//---------------------------------------------------------------------------
static const char *ainame = {"MadTracker Artificial Intelligence System"};
static const int aiversion = 0x30000;
static const MTXKey aikey = {0,0,0,0};
MTXInterfaces i;
MTAIInterface *ai;
MTInterface *mtinterface;
MTSystemInterface *si;
//---------------------------------------------------------------------------
MTAIInterface::MTAIInterface()
{
	type = aitype;
	key = &aikey;
	name = ainame;
	version = aiversion;
	status = 0;
}

bool MTAIInterface::init()
{
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	if (!si) return false;
	ENTER("MTAIInterface::init");
	LOGD("%s - [AI] Initializing..."NL);
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTAIInterface::uninit()
{
	ENTER("MTAIInterface::uninit");
	LOGD("%s - [AI] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	LEAVE();
}

void MTAIInterface::start()
{

}

void MTAIInterface::stop()
{

}

void MTAIInterface::processcmdline(void *params)
{

}

void MTAIInterface::showusage(void *out)
{

}

int MTAIInterface::config(int command,int param)
{
	return 0;
}

//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* MTCT MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!ai) ai = new MTAIInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)ai;
	return &i;
}

}
//---------------------------------------------------------------------------
