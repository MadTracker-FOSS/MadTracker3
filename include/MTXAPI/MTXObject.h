//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXObject.h 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXOBJECT_INCLUDED
#define MTXOBJECT_INCLUDED

#define MTO_MODULE      0x01000000
#define MTO_PATTERN     0x02000000
#define MTO_INSTRUMENT  0x03000000
#define MTO_OSCILLATOR  0x04000000
#define MTO_AUTOMATION  0x05000000
#define MTO_TRACK       0x06000000
#define MTO_EFFECT      0x07000000
#define MTO_TRACKEFFECT 0x08000000
#define MTO_IDMASK      0x00FFFFFF
#define MTO_TYPEMASK    0x7F000000
#define MTO_DEVELOPMENT 0x80000000

enum MTObjectLock
{
    MTOL_READ = 1, MTOL_WRITE = 2, MTOL_LOCK = 3
};

enum MTObjectAccess
{
    MTOA_CANREAD = 0x01,
    MTOA_CANWRITE = 0x02,
    MTOA_CANCOPY = 0x04,
    MTOA_CANDELETE = 0x08,
    MTOA_CANPLAY = 0x10,
    MTOA_CANALL = 0xFF,
    MOTA_ISOWNER = 0x80000000
};

enum MTObjectNotify
{
    MTN_NEW = 1,
    MTN_DELETE,
    MTN_MODIFY,
    MTN_LOCK,
    MTN_UNLOCK,
    MTN_RENAME,
    MTN_FREQUENCY = 0x80,
    MTN_TEMPO,
    MTN_PINSTANCEDELETE = 0x100,
    MTN_IINSTANCEDELETE
};

enum MTObjectParamType
{
    MTPT_CONTINUOUS = 0, MTPT_DISCRETE, MTPT_ONOFF
};

#define MTOM_ADDDELETE  0x010000
#define MTOM_NAME       0x020000
#define MTOM_PARAM      0x040000
#define MTOM_DATA       0x080000
#define MTOM_NOUPDATE   0x100000

#define MTEP_LINEAR 0x01
#define MTEP_CURVED 0x02
#define MTEP_EXP    0x04
//---------------------------------------------------------------------------
struct EnvPoint;
struct MTUser;
struct MTRule;
struct MTAccess;

class MTObject;

class MTModule;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXSystem.h"

//---------------------------------------------------------------------------
struct EnvPoint
{
    mt_uint32 flags;
    float x;
    float y;
};

struct MTUser
{
    MTUserID id;
    char name[32];
    char handle[32];
    char group[32];
    char address[128];
    char email[64];
    char url[128];
};

// Access
struct MTACL
{
    MTUser* user;
    mt_uint32 access;
    MTACL* next;
};

struct MTAccess
{
    MTUserID creatorid;
    mt_uint32 caccess;
    MTACL* acl;
};

typedef int (MTCT* MTObjectEnum)(MTObject* object, void* data);

// MadTracker object
class MTObject
{
protected:
    int modified;
public:
    MTAccess access;
    int lockread;
    int lockwrite;
    int modifying;
    MTObject* parent;
    MTModule* module;
    MTUser* owner;
    MTUser* lastowner;
    mt_uint32 objecttype;
    mt_int32 id;
    mt_uint32 flags;
    char* name;
    MTColor color;

#	ifdef IMPLEMENT_MTOBJECT
    MTObject(MTObject *parent,mt_uint32 type,mt_int32 i){ mtinterface->_ow->create(this,parent,type,i); };
    virtual ~MTObject(){ mtinterface->_ow->destroy(this); };

    virtual bool MTCT islocked(){ return (lockread|lockwrite)!=0; };
    virtual bool MTCT ismodified(){ return modified!=0; };
    virtual bool MTCT lock(int flags,bool lock,int timeout = -1){ return mtinterface->_ow->lock(this,flags,lock); };
    virtual void MTCT setname(char *newname){ mtinterface->_ow->setname(this,newname); };
    virtual void MTCT setmodified(int value,int flags){ mtinterface->_ow->setmodified(this,value,flags); };
    virtual void MTCT notify(MTObject *source,int message,int param1,void *param2){ };
    virtual void MTCT enumchildren(MTObjectEnum enumproc,void *data){ };
    virtual int MTCT loadfromstream(MTFile *f,int size,void *params){ return 0; };
    virtual int MTCT savetostream(MTFile *f,void *params){ return 0; };
    virtual MTObject* MTCT duplicate(mt_uint32 targettype){ return 0; };
    virtual int MTCT getnumparams(){ return 0; };
    virtual MTObjectParamType MTCT getparamtype(int pid){ return MTPT_CONTINUOUS; };
    virtual double MTCT getparam(int pid){ return 0.0; };
    virtual void MTCT setparam(int pid,double value){ };
#	else

    MTObject(MTObject* parent, mt_uint32 type, mt_int32 i);

    virtual ~MTObject() = 0;

    virtual bool MTCT islocked() = 0;

    virtual bool MTCT ismodified() = 0;

    virtual bool MTCT lock(int flags, bool lock, int timeout = -1) = 0;

    virtual void MTCT setname(char* newname) = 0;

    virtual void MTCT setmodified(int value, int flags) = 0;

    virtual void MTCT notify(MTObject* source, int message, int param1, void* param2) = 0;

    virtual void MTCT enumchildren(MTObjectEnum enumproc, void* data) = 0;

    virtual int MTCT loadfromstream(MTFile* f, int size, void* params) = 0;

    virtual int MTCT savetostream(MTFile* f, void* params) = 0;

    virtual MTObject* MTCT duplicate(mt_uint32 targettype) = 0;

    virtual int MTCT getnumparams() = 0;

    virtual MTObjectParamType MTCT getparamtype(int pid) = 0;

    virtual double MTCT getparam(int pid) = 0;

    virtual void MTCT setparam(int pid, double value) = 0;

#	endif
};
//---------------------------------------------------------------------------
#endif
