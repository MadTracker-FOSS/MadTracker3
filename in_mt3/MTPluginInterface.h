//---------------------------------------------------------------------------
//
//	MadTracker 3 Winamp/XMMS Plugin
//
//		Platforms:	Win32
//		Processors:	All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPluginInterface.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTPLUGININTERFACE_INCLUDED
#define MTPLUGININTERFACE_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "in_mt3.h"
//---------------------------------------------------------------------------
static const int mtptype = FOURCC('X','M','T','P');
#define MAX_EXTENSIONS 256
//---------------------------------------------------------------------------
struct MTExtension{
	int id;
	MTXInterfaces *i;
	void *library;
	char *filename;
	bool loaded;
	bool system;
	void *start,*end;
};
//---------------------------------------------------------------------------
class MTPluginInterface : public MTInterface{
public:
	MTPluginInterface();
	~MTPluginInterface();
	MTXInterface* MTCT getinterface(int id);
	MTPreferences* MTCT getprefs();
	void* MTCT getcurrentuser();
	int MTCT getnummodules();
	void* MTCT getmodule(int id);
	void MTCT addmodule(void *module);
	void MTCT delmodule(void *module);
	void MTCT setmodule(void *module);
	bool MTCT addchannel();
	void MTCT notify(void *object,int notify,int param);
	bool MTCT editobject(void *object,bool newwindow);
	void* MTCT getconf(char *name,bool user);
	void MTCT releaseconf(void *conf);
	int MTCT addrefreshproc(RefreshProc proc,void *param);
	void MTCT delrefreshproc(int id);
	char* MTCT getextension(void *ptr);
	bool MTCT canclose();
	void* MTCT getconsole();
	int MTCT processinput(const char *input);
};

class MTPluginDevice : public MTAudioInterface{
public:
	MTPluginDevice();
	bool MTCT init(){ return true; };
	void MTCT uninit(){ };
	void MTCT start(){ };
	void MTCT stop(){ };
	void MTCT processcmdline(void *params){ };
	void MTCT showusage(void *out){ };
	int MTCT config(int command,int param){ return 0; };
	int MTCT processinput(const char *input){ return 0; };
	int MTCT getnumdevices(){ return 1; };
#ifdef _WIN32
	const char* MTCT getdevicename(int id){ return "Winamp Engine"; };
#else
	const char* MTCT getdevicename(int id){ return "XMMS Engine"; };
#endif
	void MTCT selectdevice(int id){ };
	void MTCT unselectdevices(){ };
	void MTCT activatedevices(){ };
	void MTCT deactivatedevices(){ };
	bool MTCT adddevicemanager(MTAudioDeviceManager *manager){ return false; };
	void MTCT deldevicemanager(MTAudioDeviceManager *manager){ };
	WaveOutput* MTCT getoutput(){ return &waoutput; };
	void MTCT debugpoint(int offset,const char *text){ };
};
//---------------------------------------------------------------------------
bool loadExtensions();
void unloadExtensions();
bool initExtensions();
bool startExtensions();
void stopExtensions();
void uninitExtensions();
bool initSystem();
void uninitSystem();
//---------------------------------------------------------------------------
#endif
