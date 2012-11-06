//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTXML.h 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXML_INCLUDED
#define MTXML_INCLUDED
//---------------------------------------------------------------------------
class MTXML;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
#include <libxml/parser.h>
#include <libxml/tree.h>
//---------------------------------------------------------------------------
extern "C"
{
void initXML();
void uninitXML();
MTXML* MTCT mtxmlcreate();
void MTCT mtxmldelete(MTXML *xml);
}
//---------------------------------------------------------------------------
class MTXML{
public:
	MTXML();
	virtual ~MTXML();
	virtual void* MTCT readfile(const char *file,const char *encoding);
	virtual void* MTCT readmemory(const char *buf,int size,const char *encoding);
	virtual char* MTCT getname(void *node);
	virtual void* MTCT getchildren(void *node);
	virtual void* MTCT getlast(void *node);
	virtual void* MTCT getparent(void *node);
	virtual void* MTCT getnext(void *node);
	virtual void* MTCT getprev(void *node);
	virtual char* MTCT getcontent(void *node);
	virtual void* MTCT getproperties(void *node);
private:
	friend MTXML* MTCT mtxmlcreate(const char *file);
	xmlDoc *doc;
	xmlNode *root;
};
//---------------------------------------------------------------------------
#endif
