//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTResources.h 76 2005-08-28 20:43:23Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTRESOURCES_INCLUDED
#define MTRESOURCES_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
#define MTR_WINDOW   FOURCC('M','T','W','N')
#define MTR_SKIN     FOURCC('M','T','S','K')
#define MTR_TEXT     FOURCC('M','T','T','X')
#define MTR_SHORTCUT FOURCC('M','T','S','C')
#define MTR_BITMAP   FOURCC('M','T','B','M')
#define MTR_HTML     FOURCC('M','T','H','T')
#define MTR_SAMPLE   FOURCC('M','T','S','P')
//---------------------------------------------------------------------------
class MTResources;
//---------------------------------------------------------------------------
#include "MTFile.h"
//---------------------------------------------------------------------------
class MTResources{
public:
	MTResources(MTFile *f,bool ownfile);
	virtual ~MTResources();
	virtual int MTCT getnumresources();
	virtual bool MTCT getresourceinfo(int id,int *type,int *uid,int *size);
	virtual int MTCT loadresource(int type,int uid,void *buffer,int size);
	virtual int MTCT loadstring(int uid,char *buffer,int size);
	virtual int MTCT loadstringf(int uid,char *buffer,int size,...);
	virtual void* MTCT getresource(int type,int uid,int *size);
	virtual void MTCT releaseresource(void *res);
	virtual MTFile* MTCT getresourcefile(int type,int uid,int *size);
	virtual void MTCT releaseresourcefile(MTFile *f);
	virtual bool MTCT addresource(int type,int uid,void *res,int size);
	virtual bool MTCT addfile(int type,int uid,MTFile *f);
	virtual const char* MTCT getresourceurl();
private:
	struct MTResTable{
		int type;
		int uid;
		int offset;
		int size;
	} *table;
	MTFile *mf;
	int nres,onres,nares;
	bool modified;
	bool mownfile;
	void MTCT setmodified();
};
//---------------------------------------------------------------------------
extern "C"
{
MTResources* MTCT mtresfind(const char *filename,bool write);
MTResources* MTCT mtresopen(MTFile *f,bool ownfile);
void MTCT mtresclose(MTResources *res);
}
//---------------------------------------------------------------------------
#endif
