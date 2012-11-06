//---------------------------------------------------------------------------
//
//	MadTracker 3 Winamp/XMMS Plugin
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTPluginInterface.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTPluginInterface.h"
#include <malloc.h>
#ifdef _WIN32
#	include <shlobj.h>
#else
#	include <sys/stat.h>
#	include <stdlib.h>
#	include <stdio.h>
#	include <unistd.h>
#	include <dlfcn.h>
#	include <dirent.h>
#endif
//---------------------------------------------------------------------------
#ifdef _WIN32
	static const char *mtpname = {"MadTracker Winamp Plugin"};
#else
	static const char *mtpname = {"MadTracker XMMS Plugin"};
#endif
static const int mtpversion = 0x30000;
MTXKey mtpkey = {0,0,0,0};

#ifdef _WIN32
	static const char *audioname = {"Winamp Audio Wrapper"};
#else
	static const char *audioname = {"XMMS Audio Wrapper"};
#endif
static const int audioversion = 0x30000;
static const MTXKey audiokey = {0,0,0,0};

MTPreferences prefs;
int next;
MTExtension *ext[MAX_EXTENSIONS];
//---------------------------------------------------------------------------
MTPluginInterface::MTPluginInterface()
{
	char *cmd,*e;
	char applpath[512],userpath[512];

	type = mtptype;
	key = &mtpkey;
	name = mtpname;
	version = mtpversion;
	_ow = 0;

	prefs.syspath[SP_ROOT] = (char*)malloc(512);
	prefs.syspath[SP_USER] = (char*)malloc(512);
	prefs.syspath[SP_CONFIG] = (char*)malloc(512);
	prefs.syspath[SP_USERCONFIG] = (char*)malloc(512);
	prefs.syspath[SP_EXTENSIONS] = (char*)malloc(512);
	prefs.syspath[SP_SKINS] = (char*)malloc(512);
	prefs.syspath[SP_INTERFACE] = (char*)malloc(512);
	prefs.syspath[SP_HELP] = (char*)malloc(512);

	prefs.path[UP_TEMP] = (char*)malloc(512);
	prefs.path[UP_CACHE] = (char*)malloc(512);

#	ifdef _WIN32
		bool quote = false;
		cmd = GetCommandLine();
		if (*cmd=='"'){
			cmd++;
			quote = true;
		};
		if ((cmd[0]!='/') && (cmd[0]!='\\') && (cmd[1]!=':')){
			e = applpath+GetCurrentDirectory(sizeof(applpath),applpath);
			*e++ = '\\';
			strcpy(e,cmd);
		}
		else{
			strcpy(applpath,cmd);
		};
		if (quote){
			cmd = strchr(applpath,'"');
			if (cmd) cmd++;
		}
		else{
			cmd = strchr(applpath,' ');
		};
		e = strchr(applpath,':');
		if (e!=strrchr(applpath,':')){
			e++;
			e = strchr(e,':');
		}
		else{
			e = strchr(applpath,0);
		};
		if ((cmd) && (e>=cmd)) e = cmd-1;
		while ((e>applpath) && (*e!='\\') && (*e!='/')) *e-- = 0;
		SHGetSpecialFolderPath(0,userpath,CSIDL_APPDATA,false);
		e = strchr(userpath,0);
		if (*(e-1)!='\\'){
			*e++ = '\\';
			*e = 0;
		};
		strcpy(e,"MadTracker/");
//TODO
//		strcpy(applpath,"M:\\MadTracker\\3\\debug\\");
#	else
		const char *binpath = getenv("_");
		char *b1 = strrchr(binpath,'/')+1;
		int l;
		l = readlink(binpath,applpath,sizeof(applpath)-1);
		if (l==-1) strcpy(applpath,binpath);
		else applpath[l] = 0;
		e = strchr(applpath,0)-1;
		while (*e!='/') *e-- = 0;
		strcpy(userpath,getenv("HOME"));
		e = strchr(userpath,0);
		if (*(e-1)!='/'){
			*e++ = '/';
			*e = 0;
		};
		strcpy(e,".MadTracker/");
//TODO
//		strcpy(applpath,"/usr/local/madtracker/");
#	endif

	strcpy(prefs.syspath[SP_ROOT],applpath);
	strcpy(prefs.syspath[SP_USER],userpath);
	strcpy(prefs.syspath[SP_CONFIG],applpath);
	strcpy(prefs.syspath[SP_USERCONFIG],userpath);
	strcpy(prefs.syspath[SP_EXTENSIONS],applpath);
	strcpy(prefs.syspath[SP_INTERFACE],applpath);
	strcpy(prefs.syspath[SP_SKINS],applpath);
	strcpy(prefs.syspath[SP_HELP],applpath);
	strcat(prefs.syspath[SP_CONFIG],"Conf/System/");
	strcat(prefs.syspath[SP_USERCONFIG],"Conf/");

	strcat(prefs.syspath[SP_EXTENSIONS],"Extensions/");
	strcat(prefs.syspath[SP_INTERFACE],"Interface/");
	strcat(prefs.syspath[SP_SKINS],"Skins/");
	strcat(prefs.syspath[SP_HELP],"Help/");

	strcpy(prefs.path[UP_TEMP],applpath);
	strcpy(prefs.path[UP_CACHE],prefs.syspath[SP_USERCONFIG]);
	strcat(prefs.path[UP_TEMP],"Temp/");
	strcat(prefs.path[UP_CACHE],"Cache/");

#	ifdef _WIN32
		CreateDirectory(prefs.syspath[SP_USER],0);
		CreateDirectory(prefs.syspath[SP_USERCONFIG],0);
#	else
		mkdir(prefs.syspath[SP_USER],0700);
		mkdir(prefs.syspath[SP_USERCONFIG],0700);
#	endif
}

MTPluginInterface::~MTPluginInterface()
{
	free(prefs.syspath[SP_ROOT]);
	free(prefs.syspath[SP_USER]);
	free(prefs.syspath[SP_CONFIG]);
	free(prefs.syspath[SP_USERCONFIG]);
	free(prefs.syspath[SP_EXTENSIONS]);
	free(prefs.syspath[SP_INTERFACE]);
	free(prefs.syspath[SP_SKINS]);
	free(prefs.syspath[SP_HELP]);
}

MTXInterface* MTPluginInterface::getinterface(int id)
{
	switch (id){
	case 0:
#		ifdef _WIN32
			return (MTXInterface*)mod.hDllInstance;
#		else
			return (MTXInterface*)mod.handle;
#		endif
	case systemtype: return (MTXInterface*)si;
	case objectstype: return (MTXInterface*)oi;
	case displaytype: return (MTXInterface*)di;
//	case guitype: return (MTXInterface*)gi;
	case audiotype: return (MTXInterface*)ai;
	case dsptype: return (MTXInterface*)dspi;
	};
	return 0;
}

MTPreferences* MTPluginInterface::getprefs()
{
	return &prefs;
}

void *MTPluginInterface::getcurrentuser()
{
	return 0;
}

int MTPluginInterface::getnummodules()
{
	return (module!=0)?1:0;
}

void* MTPluginInterface::getmodule(int id)
{
	return module;
}

void MTPluginInterface::addmodule(void *m)
{

}

void MTPluginInterface::delmodule(void *m)
{

}

void MTPluginInterface::setmodule(void *module)
{

}

bool MTPluginInterface::addchannel()
{
	return true;
}

void MTPluginInterface::notify(void *object,int notify,int param)
{

}

bool MTPluginInterface::editobject(void *object,bool newwindow)
{
	return false;
}

void* MTPluginInterface::getconf(char *name,bool user)
{
	return 0;
}

void MTPluginInterface::releaseconf(void *conf)
{

}

int MTPluginInterface::addrefreshproc(RefreshProc proc,void *param)
{
	return -1;
}

void MTPluginInterface::delrefreshproc(int id)
{

}

char* MTPluginInterface::getextension(void *ptr)
{
	int x;
	char *e;

	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		if ((ptr>=cext.start) && (ptr<cext.end)){
			e = strrchr(cext.filename,'/');
			if (!e) return cext.filename;
			return e+1;
		};
	};
	return 0;
}

bool MTPluginInterface::canclose()
{
	return true;
}

void* MTPluginInterface::getconsole()
{
	return 0;
}

int MTPluginInterface::processinput(const char *input)
{
	return 0;
}
//---------------------------------------------------------------------------
MTPluginDevice::MTPluginDevice()
{
	type = audiotype;
	key = &audiokey;
	name = audioname;
	version = audioversion;
	status = MTX_INITIALIZED;
}
//---------------------------------------------------------------------------
MTExtension *addextension()
{
	if (next==MAX_EXTENSIONS) return 0;
	ext[next] = (MTExtension*)calloc(1,sizeof(MTExtension));
	ext[next]->id = next;
	return ext[next++];
}

void delextension(MTExtension *dext)
{
	ext[dext->id] = ext[next-1];
	ext[dext->id]->id = dext->id;
	ext[--next] = 0;
	free(dext);
}

bool loadExtension(const char *file)
{
	MTXMainCall *mtxmain;
	MTXInterfaces *xis;
	int x;
	
#	ifdef _WIN32
		HINSTANCE hi = LoadLibrary(file);
#	else
		void *hi = dlopen(file,RTLD_LAZY);
#	endif
	if (hi){
#		ifdef _WIN32
			mtxmain = (MTXMainCall*)GetProcAddress(hi,"MTXMain");
#		else
			mtxmain = (MTXMainCall*)dlsym(hi,"MTXMain");
#		endif
		if (mtxmain){
			MTExtension *cext = addextension();
			cext->library = (void*)hi;
			cext->filename = (char*)malloc(strlen(file)+1);
			strcpy(cext->filename,file);
			xis = mtxmain(mtinterface);
			cext->i = xis;
			cext->loaded = true;
			cext->system = true;
			cext->start = hi;
			cext->end = hi;
			for (x=0;x<xis->ninterfaces;x++){
				MTXInterface *xi = xis->interfaces[x];
				switch (xi->type){
				case systemtype:
					si = (MTSystemInterface*)xi;
					break;
				case objectstype:
					oi = (MTObjectsInterface*)xi;
					break;
				case displaytype:
					di = (MTDisplayInterface*)xi;
					break;
				case guitype:
					gi = (MTGUIInterface*)xi;
					break;
/*
				case audiotype:
					ai = (MTAudioInterface*)xi;
					break;
*/
				case dsptype:
					dspi = (MTDSPInterface*)xi;
					break;
				default:
					cext->system = false;
					break;
				};
			};
			return true;
		};
	};
	return false;
}

void loadDirectory(const char *dir)
{
	char find[512];
	char *e,*ext;
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	HANDLE fh;
	bool ok;
#else
	DIR *d;
	struct dirent *de;
	struct stat s;
#endif
	
	strcpy(find,dir);
	e = strchr(find,0);
#	ifdef _WIN32
		strcat(find,"*.*");
		fh = FindFirstFile(find,&fd);
		ok = (fh!=INVALID_HANDLE_VALUE);
		while (ok){
			if (fd.cFileName[0]!='.'){
				strcpy(e,fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
					strcat(e,"\\");
					SetCurrentDirectory(find);
					loadDirectory(find);
				}
				else{
					ext = strrchr(fd.cFileName,'.');
					if (stricmp(ext,".mtx")==0){
						loadExtension(find);
					};
				};
			};
			ok = FindNextFile(fh,&fd)!=0;
		};
		FindClose(fh);
#	else
		if (*(e-1)!='/'){
			*e++ = '/';
			*e = 0;
		};
		d = opendir(find);
		while ((de = readdir(d))){
			if (de->d_name[0]!='.'){
				strcpy(e,de->d_name);
				stat(find,&s);
				if ((s.st_mode & S_IFMT)==S_IFDIR){
					chdir(find);
					loadDirectory(find);
				}
				else{
					ext = strrchr(de->d_name,'.');
					if (stricmp(ext,".mtx")==0) loadExtension(find);
				};
			};
		};
		closedir(d);
#	endif
}

bool loadExtensions()
{
#ifdef _WIN32
	int oldmode;
#endif
	bool ok = false;

#	ifdef _WIN32
		oldmode = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
		ok = SetCurrentDirectory(prefs.syspath[SP_EXTENSIONS]);
#	else
		ok = (chdir(prefs.syspath[SP_EXTENSIONS])==0);
#	endif
	if (ok) loadDirectory(prefs.syspath[SP_EXTENSIONS]);
#	ifdef _WIN32
		SetErrorMode(oldmode);
		SetCurrentDirectory(prefs.syspath[SP_ROOT]);
#	else
		chdir(prefs.syspath[SP_ROOT]);
#	endif
	return ok;
}

void unloadExtensions()
{
	while (next>0){
#		ifdef _WIN32
			FreeLibrary((HINSTANCE)ext[0]->library);
#		else
			dlclose(ext[0]->library);
#		endif
		free(ext[0]->filename);
		delextension(ext[0]);
	};
}

bool initExtensions()
{
	int x,y,displayok,audiook;
	bool ok = true;
	bool ok2;
	bool ok3 = true;
	bool required = false;
	char *error,*e;
	static const char *sok = {"OK --"NL};
	static const char *sfailed = {"FAILED --"NL};
	
	ENTER("initExtensions");
	displayok = audiook = 0;
	error = (char*)si->memalloc(1024);
	strcpy(error,"The following extensions could not be initialized:"NL);
	LOGD("Found extensions:"NL);
	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		e = strrchr(cext.filename,'/');
		if (!e) e = strrchr(cext.filename,'\\');
		if (!e) e = cext.filename;
		else e++;
		si->getlibmemoryrange(cext.start,0,&cext.start,&y);
		cext.end = (char*)cext.start+y;
		FLOG3("%s (%.8X - %.8X):"NL,e,cext.start,cext.end);
		for (y=0;y<cext.i->ninterfaces;y++){
			int v = cext.i->interfaces[y]->version;
			LOG("  ");
			LOG(cext.i->interfaces[y]->name);
			FLOG3(" %d.%d.%d"NL,v>>16,(v>>8) & 0xFF,v & 0xFF);
		};
	};
	
	LOGD("%s - Initializing extensions..."NL);
	
	ok2 = false;
	if (di){
		MTTRY
			ok2 = di->init();
		MTCATCH
		MTEND
	};
	if (!ok2){
		ok = false;
		required = true;
		strcat(error,"MTXDisplay (*)"NL);
	}
	else displayok++;
	LOG("-- Display ");
	LOG((ok2)?sok:sfailed);
	
	ok2 = false;
	if (gi){
		MTTRY
			ok2 = gi->init();
		MTCATCH
		MTEND
	};
	if (!ok2){
		ok = false;
		required = true;
		strcat(error,"MTXGUI (*)"NL);
	}
	else displayok++;
	LOG("-- GUI ");
	LOG((ok2)?sok:sfailed);

	if (displayok!=2){
		displayok = 0;
		LOGD("%s - [MT3] ERROR: Cannot initialize the interface!"NL);
	};

	ok2 = false;
	if (oi){
		MTTRY
			ok2 = oi->init();
		MTCATCH
		MTEND
	};
	if (!ok2){
		ok = false;
		required = true;
		strcat(error,"MTXObjects (*)"NL);
	};
	LOG("-- Objects ");
	LOG((ok2)?sok:sfailed);
	
	ok2 = false;
	if (ai){
		MTTRY
			ok2 = ai->init();
		MTCATCH
		MTEND
	};
	if (!ok2){
		strcat(error,"MTXAudio"NL);
	}
	else audiook++;
	LOG("-- Audio ");
	LOG((ok2)?sok:sfailed);
	
	ok2 = false;
	if (ai){
		MTTRY
			ok2 = dspi->init();
		MTCATCH
		MTEND
	};
	if (!ok2){
		strcat(error,"MTXDSP"NL);
	}
	else audiook++;
	LOG("-- DSP ");
	LOG((ok2)?sok:sfailed);
	
	if (audiook==2){
		output = ai->getoutput();
		ai->activatedevices();
	};
	
	ok &= ok2;

	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		if (!cext.system){
			for (y=0;y<cext.i->ninterfaces;y++){
				ok2 = false;
				MTTRY
					ok2 = cext.i->interfaces[y]->init();
				MTCATCH
				MTEND
				if (!ok2){
					ok3 = false;
					e = strrchr(cext.filename,'\\');
					if (!e) e = strrchr(cext.filename,'/');
					if (e){
						e++;
						strcat(error,e);
						strcat(error,": ");
						strcat(error,cext.i->interfaces[y]->name);
						strcat(error,NL);
					};
				};
			};
		};
	};

	if ((!ok) || (!ok3)){
		strcat(error,"(*) = Required extensions"NL);
		if (required) strcat(error,"One or more required extensions could not be found or initialized!"NL"You should re-install MadTracker.");
		si->dialog(error,"MadTracker",MTD_OK,(required)?(MTD_ERROR|MTD_MODAL):MTD_EXCLAMATION,0);
	};
	si->memfree(error);
	
	LEAVE();
	return ok;
}

bool startExtensions()
{
	int x,y;

	LOGD("%s - Starting extensions..."NL);
	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		for (y=0;y<cext.i->ninterfaces;y++){
			if ((cext.i->interfaces[y]->status & MTX_INITIALIZED)==0) continue;
			if (cext.i->interfaces[y]==gi) continue;
			MTTRY
				cext.i->interfaces[y]->start();
			MTCATCH
				FLOGD1("%s - ERROR: Exception while starting '%s'!"NL,cext.i->interfaces[y]->name);
			MTEND
		};
	};

	LEAVE();
	return true;
}

void stopExtensions()
{
	int x,y;

	ENTER("stopExtensions");
	LOGD("%s - Stopping extensions..."NL);
	for (x=next-1;x>=0;x--){
		MTExtension &cext = *ext[x];
		for (y=cext.i->ninterfaces-1;y>=0;y--){
			MTXInterface *ci = cext.i->interfaces[y];
			if ((ci==gi) || (ci==si)) continue;
			MTTRY
				ci->stop();
			MTCATCH
				LOGD("%s - ERROR: Exception while stopping '");
				LOG(cext.i->interfaces[y]->name);
				LOG("'!"NL);
			MTEND
		};
	};
	LOGD("%s - Stopping interface..."NL);
	LEAVE();
}

void uninitExtensions()
{
	int x,y;
	
	ENTER("uninitExtensions");
	LOGD("%s - Uninitializing extensions..."NL);
	for (x=next-1;x>=0;x--){
		MTExtension &cext = *ext[x];
		if (!cext.system){
			for (y=cext.i->ninterfaces-1;y>=0;y--){
				MTTRY
					if (cext.i->interfaces[y]->status & MTX_INITIALIZED){
						cext.i->interfaces[y]->uninit();
					};
				MTCATCH
					LOGD("%s - ERROR: Exception while uninitializing '");
					LOG(cext.i->interfaces[y]->name);
					LOG("'!"NL);
				MTEND
			};
		};
	};
	MTTRY
		if ((dspi) && (dspi->status & MTX_INITIALIZED)) dspi->uninit();
	MTCATCH
		LOGD("%s - ERROR: Exception while uninitializing MTDSP!"NL);
	MTEND
	MTTRY
		if ((ai) && (ai->status & MTX_INITIALIZED)) ai->uninit();
	MTCATCH
		LOGD("%s - ERROR: Exception while uninitializing MTAudio!"NL);
	MTEND
	MTTRY
		if ((gi) && (gi->status & MTX_INITIALIZED)) gi->uninit();
	MTCATCH
		LOGD("%s - ERROR: Exception while uninitializing MTGUI!"NL);
	MTEND
	MTTRY
		if ((di) && (di->status & MTX_INITIALIZED)) di->uninit();
	MTCATCH
		LOGD("%s - ERROR: Exception while uninitializing MTDisplay!"NL);
	MTEND
	MTTRY
		if ((oi) && (oi->status & MTX_INITIALIZED)) oi->uninit();
	MTCATCH
		LOGD("%s - ERROR: Exception while uninitializing MTObjects!"NL);
	MTEND
	LEAVE();
}

bool initSystem()
{
	try{
		if (si) return si->init();
	}
	catch(...){
#		ifdef _WIN32
			MessageBox(0,"Exception while initializing MTSystem!","System Error",MB_ICONERROR|MB_OK);
#		else
			fputs("Exception while initializing MTSystem!"NL,stderr);
#		endif
	};
	return false;
}

void uninitSystem()
{
	try{
		if ((si) && (si->status & MTX_INITIALIZED)) si->uninit();
	}
	catch(...){
#		ifdef _WIN32
			MessageBox(0,"Exception while uninitializing MTSystem!","System Error",MB_ICONERROR|MB_OK);
#		else
			fputs("Exception while uninitializing MTSystem!"NL,stderr);
#		endif
	};
}
//---------------------------------------------------------------------------
