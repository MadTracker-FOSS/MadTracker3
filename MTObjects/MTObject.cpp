//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObject.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTModule.h"
#include "MTObjects1.h"
//---------------------------------------------------------------------------
// MadTracker object functions
//---------------------------------------------------------------------------
MTObject::MTObject(MTObject *parent,mt_uint32 type,mt_int32 i):
modified(0),
lockread(0),
lockwrite(0),
modifying(0),
parent(parent),
owner(0),
lastowner(0),
objecttype(type),
id(i),
flags(0),
color(MTColor(160,160,160,0))
{
	if (parent) module = parent->module;
	else module = 0;
	access.creatorid = MTUID_EVERYONE;
	access.caccess = MTOA_CANALL;
	name = (char*)si->memalloc(256,MTM_ZERO);
}

MTObject::~MTObject()
{
	si->memfree(name);
}

bool MTObject::islocked()
{
	return (lockread|lockwrite)!=0;
}

bool MTObject::ismodified()
{
	return modified!=0;
}

bool MTObject::lock(int flags,bool lock,int timeout)
{
	bool critical = false;
	
	if ((parent) && (parent->lockread) && (parent->lockwrite)) return false;
	if (lock){
		if ((flags & MTOL_READ) && (lockread==0)) critical = true;
		if ((flags & MTOL_WRITE) && (lockwrite==0)) critical = true;
	}
	else{
		if ((flags & MTOL_READ) && (lockread==1)) critical = true;
		if ((flags & MTOL_WRITE) && (lockwrite==1)) critical = true;
	};
	if ((critical) && (output->ndevices)){
		if (!output->lock->lock(timeout)) return false;
	}
	else critical = false;
	if (lock){
		if (flags & MTOL_READ) lockread++;
		if (flags & MTOL_WRITE) lockwrite++;
	}
	else{
		if (flags & MTOL_READ) lockread--;
		if (flags & MTOL_WRITE) lockwrite--;
	};
	if (critical){
		if (output->ndevices) output->lock->unlock();
		mtinterface->notify(this,(lock)?MTN_LOCK:MTN_UNLOCK,flags);
	};
	return true;
}

void MTObject::setname(char *newname)
{
	if (strcmp(name,newname)==0) return;
	strncpy(name,newname,255);
	setmodified(1,(objecttype & MTO_TYPEMASK)|MTOM_NAME);
	mtinterface->notify(this,MTN_RENAME,0);
}

void MTObject::setmodified(int value,int flags)
{
	if (value){
		modified += value;
		modifying++;
		if (parent) parent->setmodified(value,flags);
		modifying--;
	}
	else{
		modified = 0;
		modifying = 0;
	};
}

void MTObject::notify(MTObject *source,int message,int param1,void *param2)
{
}

void MTObject::enumchildren(MTObjectEnum enumproc,void *data)
{
}

int MTObject::loadfromstream(MTFile *f,int size,void *params)
{
	return 0;
}

int MTObject::savetostream(MTFile *f,void *params)
{
	return 0;
}

MTObject* MTObject::duplicate(mt_uint32 targettype)
{
	return 0;
}

int MTObject::getnumparams()
{
	return 0;
}

MTObjectParamType MTObject::getparamtype(int pid)
{
	return MTPT_CONTINUOUS;
}

double MTObject::getparam(int pid)
{
	return 0.0;
}

void MTObject::setparam(int pid,double value)
{
}
//---------------------------------------------------------------------------
// MadTracker object wrapperfunctions
//---------------------------------------------------------------------------
void MTObjectWrapper::create(void *object,void *parent,mt_uint32 type,mt_int32 i)
{
	MTObject &o = *(MTObject*)object;
	o.modified = 0;
	o.lockread = 0;
	o.lockwrite = 0;
	o.modifying = 0;
	o.parent = (MTObject*)parent;
	if (o.parent) o.module = o.parent->module;
	else o.module = 0;
	o.owner = 0;
	o.lastowner = 0;
	o.objecttype = type;
	o.id = i;
	o.flags = 0;
	o.access.creatorid = MTUID_EVERYONE;
	o.access.caccess = MTOA_CANALL;
	o.name = (char*)si->memalloc(256,MTM_ZERO);
	o.color = 0xC0C0C0;
}

void MTObjectWrapper::destroy(void *object)
{
	MTObject &o = *(MTObject*)object;
	si->memfree(o.name);
}

bool MTObjectWrapper::lock(void *object,int flags,bool lock,int timeout)
{
	MTObject &o = *(MTObject*)object;
	bool critical = false;
	
	if ((o.parent) && (o.parent->lockread) && (o.parent->lockwrite)) return false;
	if (lock){
		if ((flags & MTOL_READ) && (o.lockread==0)) critical = true;
		if ((flags & MTOL_WRITE) && (o.lockwrite==0)) critical = true;
	}
	else{
		if ((flags & MTOL_READ) && (o.lockread==1)) critical = true;
		if ((flags & MTOL_WRITE) && (o.lockwrite==1)) critical = true;
	};
	if ((critical) && (output->ndevices)){
		if (!output->lock->lock(timeout)) return false;
	}
	else critical = false;
	if (lock){
		if (flags & MTOL_READ) o.lockread++;
		if (flags & MTOL_WRITE) o.lockwrite++;
	}
	else{
		if (flags & MTOL_READ) o.lockread--;
		if (flags & MTOL_WRITE) o.lockwrite--;
	};
	if (critical){
		if (output->ndevices) output->lock->unlock();
		mtinterface->notify(object,(lock)?MTN_LOCK:MTN_UNLOCK,flags);
	};
	return true;
}

void MTObjectWrapper::setname(void *object,char *newname)
{
	MTObject &o = *(MTObject*)object;

	if (strcmp(o.name,newname)==0) return;
	strncpy(o.name,newname,255);
	setmodified(object,1,(o.objecttype & MTO_TYPEMASK)|MTOM_NAME);
	mtinterface->notify(object,MTN_RENAME,0);
}

void MTObjectWrapper::setmodified(void *object,int value,int flags)
{
	MTObject &o = *(MTObject*)object;

	if (value){
		o.modified += value;
		o.modifying++;
		if (o.parent) o.parent->setmodified(value,flags);
		o.modifying--;
	}
	else{
		o.modified = 0;
		o.modifying = 0;
	};
}
//---------------------------------------------------------------------------
