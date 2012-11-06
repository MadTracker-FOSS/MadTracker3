//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObjects1.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTOBJECTS1_INCLUDED
#define MTOBJECTS1_INCLUDED
//---------------------------------------------------------------------------
#include "../MT3Config.h"
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int objectstype = FOURCC('X','O','B','J');
//---------------------------------------------------------------------------
class MTObject;
class MTModule;

class ObjectType{
public:
	mt_uint32 type;
	char *description;
	virtual MTObject* MTCT create(MTObject *parent,mt_int32 id,void *param) = 0;
};
//---------------------------------------------------------------------------
#include "MTObject.h"
#include "MTXAudio.h"
#include "MTXDSP.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------
struct MTObjectsPreferences{
	bool hexadecimal;
	bool showzeroes;
	double maxsleeptime;
};

typedef bool (MTCT *ObjectIOFunc)(MTObject *object,char *filename,void *process);
typedef bool (MTCT *ObjectEditFunc)(MTObject *object,MTWindow *window,int flags,MTUser *user);
typedef bool (MTCT *ObjectInfoFunc)(MTMiniConfig *data,char *filename,void *process);

struct ObjectIO{
	mt_uint32 type;
	ObjectIOFunc func;
	char *filetypes;
	char *description;
};

struct ObjectEdit{
	mt_uint32 type;
	ObjectEditFunc func;
	char *description;
};

struct ObjectInfo{
	mt_uint32 type;
	ObjectInfoFunc func;
	char *filetypes;
	char *description;
};

class MTObjectsInterface : public MTXInterface{
public:
	MTObjectsInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual MTObject* MTCT newobject(mt_uint32 type,MTObject *parent,mt_int32 id,void *param = 0,bool locked = false,bool assign = true);
	virtual bool MTCT deleteobject(MTObject *object);
	virtual bool MTCT loadobject(MTObject *object,const char *filename,void *process = 0);
	virtual bool MTCT saveobject(MTObject *object,const char *filename,void *process = 0);
	virtual bool MTCT ownobject(MTObject *object,MTUser *user,bool silent = true);
	virtual void MTCT freeobject(MTObject *object,MTUser *user);
	virtual bool MTCT editobject(MTObject *object,MTWindow *window,int flags = 0);
	virtual void MTCT closeobject(MTObject *object);
	virtual bool MTCT infoobject(MTMiniConfig *data,const char *filename,void *process = 0);
	virtual int MTCT getnumtypes();
	virtual mt_uint32 MTCT gettype(int id);
	virtual ObjectType* MTCT getobjecttype(mt_uint32 type);
	virtual ObjectType* MTCT getobjecttype(const char *description);
	virtual mt_uint32 MTCT addobjecttype(ObjectType *type);
	virtual bool MTCT addload(mt_uint32 type,ObjectIOFunc loadfunc,const char *filetypes,const char *description);
	virtual bool MTCT addsave(mt_uint32 type,ObjectIOFunc savefunc,const char *filetypes,const char *description);
	virtual bool MTCT addedit(mt_uint32 type,ObjectEditFunc editfunc,const char *description);
	virtual bool MTCT addinfo(mt_uint32 type,ObjectInfoFunc infofunc,const char *filetypes,const char *description);
	virtual void MTCT delobjecttype(ObjectType *type);
	virtual void MTCT delload(mt_uint32 type,ObjectIOFunc loadfunc);
	virtual void MTCT delsave(mt_uint32 type,ObjectIOFunc savefunc);
	virtual void MTCT deledit(mt_uint32 type,ObjectEditFunc editfunc);
	virtual void MTCT delinfo(mt_uint32 type,ObjectInfoFunc infofunc);
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTObjectsInterface *oi;
extern MTSystemInterface *si;
extern MTAudioInterface *ai;
extern MTDSPInterface *dspi;
extern MTGUIInterface *gi;
extern MTObjectsPreferences objectsprefs;
extern Skin *skin;
#ifdef MTSYSTEM_RESOURCES
	extern MTResources *res;
#endif
extern MTWindow *monitor;
extern MTHash *objecttype;
extern MTArray *load,*save,*edit,*info;
extern MTLock *objectlock;
extern WaveOutput *output;
#ifdef MTVERSION_PROFESSIONAL
	extern bool smpsupport;
#endif
//---------------------------------------------------------------------------
#endif
