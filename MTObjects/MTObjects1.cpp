//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObjects1.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTModule.h"
#include "MTColumns.h"
#include "MTAutomation.h"
#include "MTIOModule_MT2.h"
#include "MTIOInstrument.h"
#include "MTSeqManager.h"
#include "MTInstrEditor.h"
#include "MTPattEditor.h"
#include "MTGraphs.h"
#include "MTDelay.h"
#include "MTGenerator.h"
#include <MTXAPI/RES/MTObjectsRES.h>

#ifdef _DEBUG

#endif

//---------------------------------------------------------------------------
static const char* objectsname = {"MadTracker Objects"};

static const int objectsversion = 0x30000;

static const MTXKey objectskey = {0, 0, 0, 0};

#ifndef MTBUILTIN

MTXInterfaces i;

MTObjectsInterface* oi;

MTInterface* mtinterface;

MTSystemInterface* si;

MTAudioInterface* ai;

MTDSPInterface* dspi;

MTGUIInterface* gi;

#endif

MTObjectsPreferences objectsprefs = {false, false, 32.0};

Skin* skin;

#ifdef MTSYSTEM_RESOURCES

MTResources* res;

#endif

MTWindow* monitor;

MTHash* objecttype;

MTArray* load, * save, * edit, * info;

MTLock* objectlock;

WaveOutput* output;

MTObjectWrapper ow;

//#ifdef MTVERSION_PROFESSIONAL
bool smpsupport = true;
//#endif

//---------------------------------------------------------------------------
bool grantaccess(MTObject* object, int access, bool silent = true, bool lock = false)
{
    int a = 0;
    int o;
    char as[16], os[16];
    bool read = false;
    bool write = false;
    MTUser* oowner, * cuser;

    if (!object)
    {
        return false;
    }
    if (!access)
    {
        return true;
    }
    if (!objectlock->lock())
    {
        return false;
    }
    cuser = (MTUser*) mtinterface->getcurrentuser();
    if (object->owner == cuser)
    {
        objectlock->unlock();
        return true;
    };
    if (access & (MTOA_CANREAD | MTOA_CANCOPY | MTOA_CANDELETE | MTOA_CANPLAY))
    {
        read = true;
    }
    if (access & (MTOA_CANWRITE | MTOA_CANDELETE))
    {
        write = true;
    }
    switch (object->objecttype & MTO_TYPEMASK)
    {
        case MTO_MODULE:
            o = MTT_lock_module;
            break;
        case MTO_PATTERN:
            o = MTT_lock_pattern;
            break;
        case MTO_AUTOMATION:
            o = MTT_lock_automation;
            break;
        case MTO_INSTRUMENT:
            o = MTT_lock_instrument;
            break;
        case MTO_OSCILLATOR:
            o = MTT_lock_oscillator;
            break;
        case MTO_TRACK:
            o = MTT_lock_track;
            break;
        case MTO_EFFECT:
        case MTO_TRACKEFFECT:
            o = MTT_lock_effect;
            break;
        default:
            o = MTT_lock_object;
            break;
    };
    if (((read) && (object->lockread)) || ((write) && (object->lockwrite)))
    {
        objectlock->unlock();
#		ifdef MTSYSTEM_RESOURCES
        if (!silent)
        {
            oowner = object->owner;
            res->loadstring(o, os, 15);
            if (!(oowner) || (oowner == cuser))
            {
                si->resdialog(res, MTT_lock1, "MTObjects", MTD_OK, MTD_INFORMATION | MTD_MODAL, 5000, os);
            }
            else
            {
                si->resdialog(res, MTT_lock2, "MTObjects", MTD_OK, MTD_INFORMATION | MTD_MODAL, 5000, os, oowner->name);
            };
        };
#		endif
        return false;
    };
    if (object->access.caccess & access)
    {
        goto granted;
    }
    if ((object->owner) && (object->owner->id == cuser->id))
    {
        goto granted;
    }
    if (access & MTOA_CANREAD)
    {
        a = MTT_can_read;
    }
    if (access & MTOA_CANWRITE)
    {
        a = MTT_can_write;
    }
    if (access & MTOA_CANCOPY)
    {
        a = MTT_can_copy;
    }
    if (access & MTOA_CANDELETE)
    {
        a = MTT_can_delete;
    }
    if (access & MTOA_CANPLAY)
    {
        a = MTT_can_play;
    }
    if (a == 0)
    {
        objectlock->unlock();
        return false;
    };
    objectlock->unlock();
    if (!silent)
    {
#		ifdef MTSYSTEM_RESOURCES
        res->loadstring(a, as, 15);
        res->loadstring(o, os, 15);
        si->resdialog(res, MTT_noaccess, "MTObjects", MTD_OK, MTD_EXCLAMATION | MTD_MODAL, 5000, as, os);
#		endif
    };
    return false;
    granted:
    if (lock)
    {
        if (read)
        {
            a |= MTOL_READ;
        }
        if (write)
        {
            a |= MTOL_WRITE;
        }
        if (a)
        {
            object->lock(a, true);
        }
    };
    object->owner = cuser;
    objectlock->unlock();
    return true;
}

bool freeaccess(MTObject* object, int access, bool lock = false)
{
    int a = 0;
    bool read = false;
    bool write = false;
    MTUser* cuser;

    if (!object)
    {
        return false;
    }
    if (!access)
    {
        return true;
    }
    if (!objectlock->lock())
    {
        return false;
    }
    if (access & (MTOA_CANREAD | MTOA_CANCOPY | MTOA_CANDELETE | MTOA_CANPLAY))
    {
        read = true;
    }
    if (access & (MTOA_CANWRITE | MTOA_CANDELETE))
    {
        write = true;
    }
    cuser = (MTUser*) mtinterface->getcurrentuser();
    if ((object->access.caccess & access) || ((object->owner) && (object->owner == cuser)))
    {
        if (lock)
        {
            if (read)
            {
                a |= MTOL_READ;
            }
            if (write)
            {
                a |= MTOL_WRITE;
            }
            if (a)
            {
                object->lock(a, false);
            }
        };
        object->owner = 0;
        objectlock->unlock();
        return true;
    };
    objectlock->unlock();
    return false;
}

//---------------------------------------------------------------------------
MTObjectsInterface::MTObjectsInterface()
{
    type = objectstype;
    key = &objectskey;
    name = objectsname;
    version = objectsversion;
    status = 0;
}

bool MTObjectsInterface::init()
{
    MTConfigFile* conf;
    int x;

    si = (MTSystemInterface*) mtinterface->getinterface(systemtype);
    ai = (MTAudioInterface*) mtinterface->getinterface(audiotype);
    dspi = (MTDSPInterface*) mtinterface->getinterface(dsptype);
    gi = (MTGUIInterface*) mtinterface->getinterface(guitype);
    if (!ai)
    {
        LOGD("%s - [Objects] ERROR: Missing MTAudio extension!"
                 NL);
        return false;
    };
    if (!dspi)
    {
        LOGD("%s - [Objects] ERROR: Missing MTDSP extension!"
                 NL);
        return false;
    };
    ENTER("MTObjectsInterface::init");
    LOGD("%s - [Objects] Initializing..."
             NL);
#	ifdef MTSYSTEM_RESOURCES
    res = si->resfind("MTObjects.mtr", false);
    if (!res)
    {
        LOGD("%s - [Objects] ERROR: Missing MTObjects.mtr!"
                 NL);
        LEAVE();
        return false;
    };
#	endif
//#	ifdef MTVERSION_PROFESSIONAL
    nthreads = si->ncpu;
//#	endif
#	ifdef MTSYSTEM_CONFIG
    if ((conf = (MTConfigFile*) mtinterface->getconf("Global", false)))
    {
        if (conf->setsection("MTObjects"))
        {
//#				ifdef MTVERSION_PROFESSIONAL
            if (smpsupport){
                conf->getparameter("SMPSupport",&smpsupport,MTCT_BOOLEAN,sizeof(smpsupport));
                conf->getparameter("Threads",&nthreads,MTCT_UINTEGER,sizeof(nthreads));
                if (nthreads<1) nthreads = 1;
                else if (nthreads>128) nthreads = 128;
            };
//#				endif
        };
        mtinterface->releaseconf(conf);
    };
#	endif
    objectlock = si->lockcreate();
    output = ai->getoutput();
    objecttype = si->hashcreate(4);
    load = si->arraycreate(4, 0);
    save = si->arraycreate(4, 0);
    edit = si->arraycreate(4, 0);
    info = si->arraycreate(4, 0);
    instrumenttype = new InstrumentType();
    sampletype = new SampleType();
    patterntype = new PatternType();
    delaytype = new DelayType();
    generatortype = new GeneratorType();
    addobjecttype(instrumenttype);
    addobjecttype(sampletype);
    addobjecttype(patterntype);
    addobjecttype(delaytype);
    addobjecttype(generatortype);
    initColumns();
#	ifdef MTOBJECTS_EDITORS
    initInstrEditor();
    initPattEditor();
    addedit(MTO_MODULE, ModuleEdit, "Sequencer");
    addedit(MTO_INSTRUMENT, InstrumentEdit, "Instrument Editor");
    addedit(MTO_PATTERN, PatternEdit, "Pattern Editor");
#	endif
    addload(MTO_MODULE, loadMT2, ".mt2", "MadTracker 2 Module");
    addinfo(MTO_MODULE, infoMT2, ".mt2", "MadTracker 2 Module");
    addload(MTO_OSCILLATOR, loadWAV, ".wav", "Wave Sample");
    addload(MTO_INSTRUMENT, loadSF2, ".sf2", "Sound Font 2");
    status |= MTX_INITIALIZED;
//#	ifdef MTVERSION_PROFESSIONAL
    if (smpsupport){
        if (nthreads>1){
            FLOGD1("%s - [Objects] Preparing engine for %d CPU's..."NL,nthreads);
            for (x=1;x<nthreads;x++){
                thread[x] = si->threadcreate(engineproc,true,true,(void*)x,MTT_REALTIME,"Engine");
            };
        };
    };
//#	endif
    LEAVE();
    return true;
}

void MTObjectsInterface::uninit()
{
    int x;
    MTLock* oldlock;

    ENTER("MTObjectsInterface::uninit");
    LOGD("%s - [Objects] Uninitializing..."
             NL);
//#	ifdef MTVERSION_PROFESSIONAL
    if (smpsupport){
        if (nthreads>1){
            LOGD("%s - [Objects] Stopping engine threads..."NL);
            for (x=1;x<nthreads;x++){
                thread[x]->terminate();
            };
        };
    };
//#	endif
    status &= (~MTX_INITIALIZED);
    oldlock = objectlock;
    objectlock = 0;
    si->lockdelete(oldlock);
/*
	for (x=0;x<load->nitems;x++){
		si->memfree(((ObjectIO*)load->a[x])->filetypes);
		si->memfree(((ObjectIO*)load->a[x])->description);
		si->memfree(load->a[x]);
	};
	for (x=0;x<save->nitems;x++){
		si->memfree(((ObjectIO*)save->a[x])->filetypes);
		si->memfree(((ObjectIO*)save->a[x])->description);
		si->memfree(save->a[x]);
	};
	for (x=0;x<edit->nitems;x++){
		si->memfree(((ObjectEdit*)edit->a[x])->description);
		si->memfree(edit->a[x]);
	};
	for (x=0;x<info->nitems;x++){
		si->memfree(((ObjectInfo*)info->a[x])->filetypes);
		si->memfree(((ObjectInfo*)info->a[x])->description);
		si->memfree(info->a[x]);
	};
*/
    delload(MTO_MODULE, loadMT2);
    delinfo(MTO_MODULE, infoMT2);
    delload(MTO_OSCILLATOR, loadWAV);
    delload(MTO_INSTRUMENT, loadSF2);
#	ifdef MTOBJECTS_EDITORS
    deledit(MTO_MODULE, ModuleEdit);
    deledit(MTO_INSTRUMENT, InstrumentEdit);
    deledit(MTO_PATTERN, PatternEdit);
    uninitPattEditor();
    uninitInstrEditor();
#	endif
    uninitColumns();
    delobjecttype(generatortype);
    delobjecttype(instrumenttype);
    delobjecttype(sampletype);
    delobjecttype(patterntype);
    delobjecttype(delaytype);
    delete generatortype;
    delete instrumenttype;
    delete sampletype;
    delete patterntype;
    delete delaytype;
    instrumenttype = 0;
    sampletype = 0;
    patterntype = 0;
    delaytype = 0;
    generatortype = 0;
    si->hashdelete(objecttype);
    si->arraydelete(load);
    si->arraydelete(save);
    si->arraydelete(edit);
    si->arraydelete(info);
#	ifdef MTSYSTEM_RESOURCES
    si->resclose(res);
    res = 0;
#	endif
    LEAVE();
}

void MTObjectsInterface::start()
{
    MTCustomControl* cc;

#	ifdef MTSYSTEM_RESOURCES
    if (gi)
    {
        skin = gi->getskin();
        objectlock->lock();
        monitor = gi->loadwindow(res, MTW_monitor, 0);
        if (monitor)
        {
            cc = (MTCustomControl*) monitor->getcontrolfromuid(MTC_cpumonitor);
            cc->behaviours = new MTCPUGraph(cc);
            cc = (MTCustomControl*) monitor->getcontrolfromuid(MTC_chanmonitor);
            cc->behaviours = new MTChannelsGraph(cc);
        };
        objectlock->unlock();
    };
#	endif
#	ifdef _DEBUG
//		_test_start();
#	endif
}

void MTObjectsInterface::stop()
{
#	ifdef _DEBUG
//		_test_stop();
#	endif
    if (!gi)
    {
        return;
    }
    if (monitor)
    {
        if (objectlock)
        {
            objectlock->lock();
        }
        gi->delcontrol(monitor);
        monitor = 0;
        if (objectlock)
        {
            objectlock->unlock();
        }
    };
}

void MTObjectsInterface::processcmdline(void* params)
{
//#	ifdef MTVERSION_PROFESSIONAL
    MTArray *pa = (MTArray*)params;
    MTCLParam *np;

    pa->reset();
    while ((np = (MTCLParam*)pa->next())){
        if (!np->name) continue;
        if (strcmp(np->name,"nosmp")==0){
            smpsupport = false;
            break;
        };
    };
//#	endif
}

void MTObjectsInterface::showusage(void* out)
{
//#	ifdef MTVERSION_PROFESSIONAL
    MTFile *co = (MTFile*)out;
    const char *usage = {"      --nosmp       Disable SMP support"NL};

    co->write(usage,strlen(usage));
//#	endif
}

int MTObjectsInterface::config(int command, int param)
{
    return 0;
}

MTObject* MTObjectsInterface::newobject(mt_uint32 type, MTObject* parent, mt_int32 id, void* param, bool locked, bool assign)
{
    int x;
    MTModule* module = 0;
    MTObject* object = 0;
    ObjectType* ctype;

    FENTER5("MTObjectsInterface::newobject(%d,%.8X,%d,%.8X,%d)", type, parent, id, param, locked);
//	objectlock->lock();
    if (parent)
    {
        module = parent->module;
    }
    if (module)
    {
        module->mlock->lock();
        if ((id < 0) && (assign) && (parent == module))
        {
            switch (type & MTO_TYPEMASK)
            {
                case MTO_PATTERN:
                    for(x = 0; x < module->patt->nitems; x++)
                    {
                        if (module->patt->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
                case MTO_AUTOMATION:
                    for(x = 0; x < module->apatt->nitems; x++)
                    {
                        if (module->apatt->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
                case MTO_INSTRUMENT:
                    for(x = 0; x < module->instr->nitems; x++)
                    {
                        if (module->instr->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
                case MTO_OSCILLATOR:
                    for(x = 0; x < module->spl->nitems; x++)
                    {
                        if (module->spl->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
                case MTO_TRACK:
                    for(x = 0; x < module->trk->nitems; x++)
                    {
                        if (module->trk->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
                case MTO_EFFECT:
                case MTO_TRACKEFFECT:
                    for(x = 0; x < module->trk->nitems; x++)
                    {
                        if (module->fx->a[x] == 0)
                        {
                            id = x;
                            break;
                        };
                    };
                    break;
            };
        };
    };
    switch (type)
    {
        case MTO_MODULE:
            object = new MTModule(id);
            break;
        case MTO_AUTOMATION:
            object = new Automation(parent, id);
            if ((parent) && (assign) && (parent == module))
            {
                module->apatt->a[id] = object;
            }
            break;
        case MTO_TRACK:
            object = new Track(parent, id);
            if ((parent) && (assign) && (parent == module))
            {
                if (id < MAX_TRACKS)
                {
                    module->trk->a[id] = object;
                }
                else
                {
                    module->master->a[id - MAX_TRACKS] = object;
                }
            };
            break;
        default:
            ctype = (ObjectType*) objecttype->getitem(type);
            if ((ctype) && (ctype->type == type))
            {
                object = ctype->create(parent, id, param);
            };
            if ((parent) && (assign) && (parent == module))
            {
                switch (type & MTO_TYPEMASK)
                {
                    case MTO_PATTERN:
                        module->patt->a[id] = object;
                        break;
                    case MTO_INSTRUMENT:
                        module->instr->a[id] = object;
                        break;
                    case MTO_OSCILLATOR:
                        module->spl->a[id] = object;
                        break;
                    case MTO_EFFECT:
                    case MTO_TRACKEFFECT:
                        module->fx->a[id] = object;
                        break;
                };
            };
            break;
    };
    if (!object)
    {
        LOGD("%s [Objects] ERROR: Unknown object!");
    };
    if ((object) && (locked))
    {
        object->lock(MTOL_LOCK, true);
    }
    if (module)
    {
        module->mlock->unlock();
    }
//	objectlock->unlock();
    if (object)
    {
        mtinterface->notify(object, MTN_NEW, id);
    }
    LEAVE();
    return object;
}

int MTCT candeleteenum(MTObject* object, void* data)
{
    if (object->islocked())
    {
        *(bool*) data = false;
        return 0;
    };
    return 1;
}

bool MTObjectsInterface::deleteobject(MTObject* object)
{
    if (!object)
    {
        return false;
    }
    bool candelete = true;
    int type = 0;
    int id = -1;
    MTObject* parent = object->parent;
    MTModule* module = object->module;

    if (object->islocked())
    {
        return false;
    }
    if (!grantaccess(object, MTOA_CANDELETE, true, true))
    {
        return false;
    }
    FENTER1("MTObjectsInterface::deleteobject(%.8X)", object);
    if ((object->objecttype & MTO_TYPEMASK) == MTO_MODULE)
    {
        object->enumchildren(candeleteenum, &candelete);
    }
    if (candelete)
    {
        mtinterface->notify(object, MTN_DELETE, 0);
        MTTRY
//			objectlock->lock();
            if (module)
            {
                module->mlock->lock();
            }
            type = object->objecttype;
            id = object->id;
            delete object;
        MTCATCH
        MTEND
        if ((parent) && (parent == module))
        {
            if (id >= 0)
            {
                switch (type & MTO_TYPEMASK)
                {
                    case MTO_INSTRUMENT:
                        module->instr->a[id] = 0;
                        break;
                    case MTO_OSCILLATOR:
                        module->spl->a[id] = 0;
                        break;
                    case MTO_PATTERN:
                        module->patt->a[id] = 0;
                        break;
                    case MTO_AUTOMATION:
                        module->apatt->a[id] = 0;
                        break;
                    case MTO_EFFECT:
                    case MTO_TRACKEFFECT:
                        module->fx->a[id] = 0;
                        break;
                };
            };
            module->mlock->unlock();
        };
//		objectlock->unlock();
        LEAVE();
        return true;
    };
    LEAVE();
    return false;
}

bool MTObjectsInterface::loadobject(MTObject* object, const char* filename, void* process)
{
    int x;
    char filetype[32];
    bool ret = false;

    si->filetype(filename, filetype, 32);
    if (filetype[0] == 0)
    {
        return false;
    }
    FENTER3("MTObjectsInterface::loadobject(%.8X,%s,%.8X)", object, filename, process);
    for(x = 0; x < load->nitems; x++)
    {
        ObjectIO& cio = *(ObjectIO*) load->a[x];
        if (cio.type != object->objecttype)
        {
            continue;
        }
        if (strstr(cio.filetypes, filetype))
        {
            MTTRY
                ret = cio.func(object, (char*) filename, process);
            MTCATCH
                ret = false;
            MTEND
            LEAVE();
            return ret;
        };
    };
    LEAVE();
    return false;
}

bool MTObjectsInterface::saveobject(MTObject* object, const char* filename, void* process)
{
    int x;
    char filetype[32];

    si->filetype(filename, filetype, 32);
    if (filetype[0] == 0)
    {
        return false;
    }
    FENTER3("MTObjectsInterface::saveobject(%.8X,%s,%.8X)", object, filename, process);
    for(x = 0; x < save->nitems; x++)
    {
        ObjectIO& cio = *(ObjectIO*) save->a[x];
        if (cio.type != object->objecttype)
        {
            continue;
        }
        if (strstr(cio.filetypes, filetype))
        {
            LEAVE();
            return cio.func(object, (char*) filename, process);
        };
    };
    LEAVE();
    return false;
}

bool MTObjectsInterface::ownobject(MTObject* object, MTUser* user, bool silent)
{
    int o;
    char os[16];
    MTUser* oowner, * cuser;

    cuser = (MTUser*) mtinterface->getcurrentuser();
    /* if ((user==&cuser) && (nwmanager) && (nwmanager->connected) && (!nwmanager->server) && (nwmanager->cmodule)){
    if (object->parent==nwmanager->cmodule){
        return nwmanager->nwownobject(object);
    };
};*/
    objectlock->lock();
    if ((object->owner == 0) || (object->owner == user))
    {
        if (user != cuser)
        {
            object->lock(MTOL_LOCK, true);
        }
        object->owner = user;
        object->lastowner = user;
        objectlock->unlock();
        return true;
    };
    if (user != cuser)
    {
        objectlock->unlock();
        return false;
    };
    oowner = object->owner;
    switch (object->objecttype & MTO_TYPEMASK)
    {
        case MTO_MODULE:
            o = MTT_lock_module;
            break;
        case MTO_PATTERN:
            o = MTT_lock_pattern;
            break;
        case MTO_AUTOMATION:
            o = MTT_lock_automation;
            break;
        case MTO_INSTRUMENT:
            o = MTT_lock_instrument;
            break;
        case MTO_OSCILLATOR:
            o = MTT_lock_oscillator;
            break;
        default:
            o = MTT_lock_track;
            break;
    };
    objectlock->unlock();
#	ifdef MTSYSTEM_RESOURCES
    res->loadstring(o, os, 15);
    if (!(oowner) || (oowner == cuser))
    {
        si->resdialog(res, MTT_lock1, "MTObjects", MTD_OK, MTD_INFORMATION, 5000, os);
    }
    else
    {
        si->resdialog(res, MTT_lock2, "MTObjects", MTD_OK, MTD_INFORMATION, 5000, os, oowner->name);
    }
#	endif
    return false;
}

void MTObjectsInterface::freeobject(MTObject* object, MTUser* user)
{
    MTUser* cuser;

    cuser = (MTUser*) mtinterface->getcurrentuser();
    objectlock->lock();
    if (object->owner == user)
    {
        if (user != cuser)
        {
            object->lock(MTOL_LOCK, false);
        }
        object->owner = 0;
    };
    objectlock->unlock();
}

bool MTObjectsInterface::editobject(MTObject* object, MTWindow* window, int flags)
{
    MTUser* cuser;
    int x;
    bool ok = false;

    cuser = (MTUser*) mtinterface->getcurrentuser();
    if ((grantaccess(object, MTOA_CANREAD, false)) && (ownobject(object, cuser)))
    {
        gi->setmouseshape(DCUR_WORKING);
        for(x = 0; x < edit->nitems; x++)
        {
            ObjectEdit& cedit = *(ObjectEdit*) edit->a[x];
            if (cedit.type == object->objecttype)
            {
                ok = cedit.func(object, window, flags, cuser);
                break;
            };
        };
        gi->restoremouseshape();
    };
    return ok;
}

void MTObjectsInterface::closeobject(MTObject* object)
{
    MTUser* cuser;

    if (!object)
    {
        return;
    }
    cuser = (MTUser*) mtinterface->getcurrentuser();
    // if ((nwmanager) && (nwmanager->connected) && (!nwmanager->server)) uploadobject(object,-2,false);
    freeobject(object, cuser);
    freeaccess(object, MTOA_CANREAD);
}

bool MTObjectsInterface::infoobject(MTMiniConfig* data, const char* filename, void* process)
{
    int x;
    char filetype[32];
    bool ret = false;

    si->filetype(filename, filetype, 32);
    if (filetype[0] == 0)
    {
        return false;
    }
    FENTER3("MTObjectsInterface::infoobject(%.8X,%s,%.8X)", info, filename, process);
    for(x = 0; x < info->nitems; x++)
    {
        ObjectInfo& cinfo = *(ObjectInfo*) info->a[x];
        if (strstr(cinfo.filetypes, filetype))
        {
            MTTRY
                ret = cinfo.func(data, (char*) filename, process);
            MTCATCH
                ret = false;
            MTEND
            LEAVE();
            return ret;
        };
    };
    LEAVE();
    return false;
}

int MTObjectsInterface::getnumtypes()
{
    return objecttype->nitems;
}

mt_uint32 MTObjectsInterface::gettype(int id)
{
    ObjectType* ctype = (ObjectType*) objecttype->getitemfromid(id);
    if (!ctype)
    {
        return 0;
    }
    return ctype->type;
}

ObjectType* MTObjectsInterface::getobjecttype(mt_uint32 type)
{
    return (ObjectType*) objecttype->getitem(type);
}

ObjectType* MTObjectsInterface::getobjecttype(const char* description)
{
    ObjectType* ctype;

    objecttype->reset();
    while((ctype = (ObjectType*) objecttype->next()))
    {
        if (strcmp(ctype->description, description) == 0)
        {
            return ctype;
        };
    };
    return 0;
}

mt_uint32 MTObjectsInterface::addobjecttype(ObjectType* type)
{
    FLOGD2("%s - [Objects] Adding object type: %s (%.8X)"
               NL, type->description, type->type);
    objecttype->additem(type->type, type);
    return type->type;
}

bool MTObjectsInterface::addload(mt_uint32 type, ObjectIOFunc loadfunc, const char* filetypes, const char* description)
{
    ObjectIO* cio;

    cio = mtnew(ObjectIO);
    load->push(cio);
    cio->type = type;
    cio->func = loadfunc;
    cio->filetypes = (char*) si->memalloc(strlen(filetypes) + 1, 0);
    cio->description = (char*) si->memalloc(strlen(description) + 1, 0);
    strcpy(cio->filetypes, filetypes);
    strcpy(cio->description, description);
    return true;
}

bool MTObjectsInterface::addsave(mt_uint32 type, ObjectIOFunc savefunc, const char* filetypes, const char* description)
{
    ObjectIO* cio;

    cio = mtnew(ObjectIO);
    save->push(cio);
    cio->type = type;
    cio->func = savefunc;
    cio->filetypes = (char*) si->memalloc(strlen(filetypes) + 1, 0);
    cio->description = (char*) si->memalloc(strlen(description) + 1, 0);
    strcpy(cio->filetypes, filetypes);
    strcpy(cio->description, description);
    return true;
}

bool MTObjectsInterface::addedit(mt_uint32 type, ObjectEditFunc editfunc, const char* description)
{
    ObjectEdit* cedit;

    cedit = mtnew(ObjectEdit);
    edit->push(cedit);
    cedit->type = type;
    cedit->func = editfunc;
    cedit->description = (char*) si->memalloc(strlen(description) + 1, 0);
    strcpy(cedit->description, description);
    return true;
}

bool MTObjectsInterface::addinfo(mt_uint32 type, ObjectInfoFunc infofunc, const char* filetypes, const char* description)
{
    ObjectInfo* cinfo;

    cinfo = mtnew(ObjectInfo);
    info->push(cinfo);
    cinfo->type = type;
    cinfo->func = infofunc;
    cinfo->filetypes = (char*) si->memalloc(strlen(filetypes) + 1, 0);
    cinfo->description = (char*) si->memalloc(strlen(description) + 1, 0);
    strcpy(cinfo->filetypes, filetypes);
    strcpy(cinfo->description, description);
    return true;
}

void MTObjectsInterface::delobjecttype(ObjectType* type)
{
    objecttype->delitem(type->type);
}

void MTObjectsInterface::delload(mt_uint32 type, ObjectIOFunc loadfunc)
{
    int x;

    for(x = 0; x < load->nitems; x++)
    {
        ObjectIO& cio = *(ObjectIO*) load->a[x];
        if ((cio.type == type) && (cio.func == loadfunc))
        {
            si->memfree(cio.filetypes);
            si->memfree(cio.description);
            si->memfree(&cio);
            load->delitems(x, 1);
        };
    };
}

void MTObjectsInterface::delsave(mt_uint32 type, ObjectIOFunc savefunc)
{
    int x;

    for(x = 0; x < save->nitems; x++)
    {
        ObjectIO& cio = *(ObjectIO*) save->a[x];
        if ((cio.type == type) && (cio.func == savefunc))
        {
            si->memfree(cio.filetypes);
            si->memfree(cio.description);
            si->memfree(&cio);
            save->delitems(x, 1);
        };
    };
}

void MTObjectsInterface::deledit(mt_uint32 type, ObjectEditFunc editfunc)
{
    int x;

    for(x = 0; x < edit->nitems; x++)
    {
        ObjectEdit& cedit = *(ObjectEdit*) edit->a[x];
        if ((cedit.type == type) && (cedit.func == editfunc))
        {
            si->memfree(cedit.description);
            si->memfree(&cedit);
            edit->delitems(x, 1);
        };
    };
}

void MTObjectsInterface::delinfo(mt_uint32 type, ObjectInfoFunc infofunc)
{
    int x;

    for(x = 0; x < info->nitems; x++)
    {
        ObjectInfo& cinfo = *(ObjectInfo*) info->a[x];
        if ((cinfo.type == type) && (cinfo.func == infofunc))
        {
            si->memfree(cinfo.filetypes);
            si->memfree(cinfo.description);
            si->memfree(&cinfo);
            info->delitems(x, 1);
        };
    };
}
//---------------------------------------------------------------------------
extern "C" {

MTXInterfaces* MTCT MTXMain(MTInterface* mti)
{
    mtinterface = mti;
    mtinterface->_ow = &ow;
    if (!oi)
    {
        oi = new MTObjectsInterface();
    }
    i.ninterfaces = 1;
    i.interfaces[0] = (MTXInterface*) oi;
    return &i;
}

}
//---------------------------------------------------------------------------
