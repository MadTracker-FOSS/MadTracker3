//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTMiniConfig.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "MTMiniConfig.h"
#include "MTMD5.h"
#include <MTXAPI/MTXSystem2.h>
//---------------------------------------------------------------------------
struct MP{
	char *key;
	int type;
	int size;
	void *value;
};
//---------------------------------------------------------------------------
MTMiniConfig* mtminiconfigcreate()
{
	return new MTMiniConfig();
}

void mtminiconfigdelete(MTMiniConfig *cfg)
{
	delete cfg;
}
//---------------------------------------------------------------------------
void MTCT DeleteProc(void *item,void *param)
{
	MP *cp = (MP*)item;
	mtmemfree(cp->key);
	if (cp->type==MTCT_CONFIG){
		delete (MTMiniConfig*)cp->value;
	}
	else if ((cp->type==MTCT_STRING) || (cp->type==MTCT_BINARY) || ((cp->type==MTCT_FLOAT) && (cp->size>4))){
		mtmemfree(cp->value);
	};
}
//---------------------------------------------------------------------------
MTMiniConfig::MTMiniConfig():
np(0),
mp(0)
{
	mp = new MTHash(8);
}

MTMiniConfig::~MTMiniConfig()
{
	mp->clear(true,DeleteProc);
	delete mp;
}

bool MTMiniConfig::getparameter(const char *paramname,void *value,int desiredtype,int size)
{
	const char *e;
	int l;
	MP *cp;
	char buf[256];

	e = strchr(paramname, '.');
	if (e){
		l = e-paramname;
		if (l>255) return false;
		strncpy(buf,paramname,l);
		buf[l] = 0;
		cp = (MP*)mp->getitem(buf);
		if (!cp) return false;
		if (cp->type!=MTCT_CONFIG) return false;
		return ((MTMiniConfig*)cp->value)->getparameter(e+1,value,desiredtype,size);
	};
	cp = (MP*)mp->getitem(paramname);
	if (!cp) return false;
	if (cp->type!=desiredtype) return false;
	if (cp->size>size) return false;
	mtmemzero(value,size);
	switch (cp->type){
	case MTCT_CONFIG:
		*(MTMiniConfig**) value = (MTMiniConfig*) cp->value;
		break;
	case MTCT_SINTEGER:
		if (cp->size == sizeof(char))
			memcpy(value, &cp->value, sizeof(char));
		else if (cp->size == sizeof(short))
			memcpy(value, &cp->value, sizeof(short));
		else if (cp->size == sizeof(int))
			memcpy(value, &cp->value, sizeof(int));
		else
			return false;
		break;
	case MTCT_UINTEGER:
		if (cp->size == sizeof(unsigned char))
			memcpy(value, &cp->value, sizeof(unsigned char));
		else if (cp->size == sizeof(unsigned short))
			memcpy(value, &cp->value, sizeof(unsigned short));
		else if (cp->size == sizeof(unsigned int))
			memcpy(value, &cp->value, sizeof(unsigned int));
		else
			return false;
		break;
	case MTCT_FLOAT:
		if (cp->size == sizeof(float))
			memcpy(value, &cp->value, sizeof(float));
		else if (cp->size == sizeof(double))
			memcpy(value, &cp->value, sizeof(double));
		else
			return false;
		break;
	case MTCT_BOOLEAN:
		*(bool*)value = ((int) cp->value != 0);
		break;
	case MTCT_STRING:
		strncpy((char*) value, (char*) cp->value, size);
		break;
	case MTCT_BINARY:
		memcpy(value, cp->value, cp->size);
		break;
	default:
		return false;
	};
	return true;
}

bool MTMiniConfig::setparameter(const char *paramname,const void *value,int type,int size)
{
	const char *e;
	int l;
	MP *cp;
	char buf[256];

	e = strchr(paramname,'.');
	if (e){
		l = e-paramname;
		if (l>255) return false;
		strncpy(buf,paramname,l);
		buf[l] = 0;
		cp = (MP*)mp->getitem(buf);
		if ((!cp) || (cp->type!=MTCT_CONFIG)) return false;
		return ((MTMiniConfig*)cp->value)->setparameter(e+1,value,type,size);
	};
	if (mp->getitem(paramname)) mp->delitem(paramname,true,DeleteProc);
	cp = mtnew(MP);
	cp->key = (char*)mtmemalloc(strlen(paramname)+1);
	strcpy(cp->key,paramname);
	cp->type = type;
	if ((size==-1) && (type==MTCT_STRING)) size = strlen((char*)value)+1;
	cp->size = size;
	switch (cp->type){
	case MTCT_CONFIG:
		*(MTMiniConfig**)&cp->value = *(MTMiniConfig**)value;
		break;
	case MTCT_SINTEGER:
		if (size==sizeof(char)) *(char*)&cp->value = *(char*)value;
		else if (size==sizeof(short)) *(short*)&cp->value = *(short*)value;
		else if (size==sizeof(int)) *(int*)&cp->value = *(int*)value;
		else{
			mtmemfree(cp);
			return false;
		};
		break;
	case MTCT_UINTEGER:
		if (cp->size==sizeof(unsigned char)) *(unsigned char*)&cp->value = *(unsigned char*)value;
		else if (cp->size==sizeof(unsigned short)) *(unsigned short*)&cp->value = *(unsigned short*)value;
		else if (cp->size==sizeof(unsigned int)) *(unsigned int*)&cp->value = *(unsigned int*)value;
		else{
			mtmemfree(cp);
			return false;
		};
		break;
	case MTCT_FLOAT:
		if (cp->size==sizeof(float)) *(float*)&cp->value = *(float*)value;
		else if (cp->size==sizeof(double)){
			cp->value = mtmemalloc(sizeof(double));
			*(double*)cp->value = *(double*)value;
		}
		else{
			mtmemfree(cp);
			return false;
		};
		break;
	case MTCT_BOOLEAN:
		*(bool*)&cp->value = *(bool*)value;
		break;
	case MTCT_STRING:
		cp->value = mtmemalloc(strlen((char*)value)+1);
		strcpy((char*)cp->value,(char*)value);
		break;
	case MTCT_BINARY:
		cp->value = mtmemalloc(size);
		memcpy(cp->value,value,size);
		break;
	default:
		mtmemfree(cp);
		return false;
	};
	mp->additem(paramname,cp);
	return true;
}

int MTMiniConfig::loadfromstream(MTFile *f,int flags)
{
	int l,x;
	char id[5];
	MP *cp,*ep;
	unsigned int ikey[4];
	char key[256];

	if (flags & MTMC_HEADER){
		if (!(f->read(&id, 4) == 4 && strcmp(id, "MTMC") == 0))
			return -1;
	};
	if (flags & MTMC_STRUCTURE){
		if (f->read(&np,4)!=4) return -1;
	};
	l = 0;
	cp = mtnew(MP);
	for (x=0;x<np;x++){
		if (flags & MTMC_STRUCTURE){
			if (flags & MTMC_MD5KEYS){
				l += f->read(&ikey,sizeof(ikey));
				ikey[0] = ((ikey[0]^ikey[1])^ikey[2])^ikey[3];
			}
			else{
				l += f->readln(key,256);
				key[255] = 0;
				cp->key = (char*)mtmemalloc(strlen(key)+1);
				strcpy(cp->key,key);
			};
			l += f->read(&cp->type,sizeof(cp->type));
		};
		if (((flags & MTMC_STRUCTURE) && (cp->type!=MTCT_BINARY)) || ((flags & MTMC_DATA) && (cp->type==MTCT_BINARY))) l += f->read(&cp->size,sizeof(cp->size));
		if (flags & MTMC_DATA){
			if (cp->type==MTCT_CONFIG){
				((MTMiniConfig*)cp->value)->loadfromstream(f,flags);
			}
			else if ((cp->type==MTCT_STRING) || (cp->type==MTCT_BINARY) || ((cp->type==MTCT_FLOAT) && (cp->size>4))){
				cp->value = mtmemalloc(cp->size);
				l += f->read(cp->value,cp->size);
			}
			else{
				l += f->read(&cp->value,cp->size);
#				if (BIG_ENDIAN==1234)
					*(int*)cp->value = swap_dword(*(int*)cp->value);
#				endif
			};
		};
		ep = (MP*)(((flags & MTMC_STRUCTURE)==0)?mp->getitemfromid(x):((flags & MTMC_MD5KEYS)?mp->getitem(ikey[0]):mp->getitem(key)));
		if (ep){
			if (ep->size==0) ep->size = cp->size;
			ep->value = cp->value;
			mtmemfree(cp);
		}
		else{
			if (flags & MTMC_MD5KEYS) mp->additem(ikey[0],cp);
			else mp->additem(key,cp);
		};
	};
	return l;
}

int MTMiniConfig::savetostream(MTFile *f,int flags)
{
	int l;
	unsigned int ikey[4];
	MP *cp;

	if (flags & MTMC_HEADER) f->write("MTMC",4);
	if (flags & MTMC_STRUCTURE) f->write(&mp->nitems,4);
	l = 8;
	mp->reset();
	while ((cp = (MP*)mp->next())){
		if (flags & MTMC_STRUCTURE){
			if (flags & MTMC_MD5KEYS){
				md5b((unsigned char*)ikey,cp->key);
				l += f->write(ikey,sizeof(ikey));
			}
			else{
				l += f->write(cp->key,strlen(cp->key)+1);
			};
			l += f->write(&cp->type,sizeof(cp->type));
		};
		if (((flags & MTMC_STRUCTURE) && (cp->type!=MTCT_BINARY)) || ((flags & MTMC_DATA) && (cp->type==MTCT_BINARY))) l += f->write(&cp->size,sizeof(cp->size));
		if (flags & MTMC_DATA){
			if (cp->type==MTCT_CONFIG){
				l += ((MTMiniConfig*)cp->value)->savetostream(f,flags);
			}
			else if ((cp->type==MTCT_STRING) || (cp->type==MTCT_BINARY) || ((cp->type==MTCT_FLOAT) && (cp->size>4))){
				l += f->write(cp->value,cp->size);
			}
			else{
#				if (BIG_ENDIAN==1234)
					int _tmp = swap_dword(*(int*)cp->value);
					l += f->write(&_tmp,cp->size);
#				else
					l += f->write(&cp->value,cp->size);
#				endif
			};
		};
	};
	return l;
}
//---------------------------------------------------------------------------

