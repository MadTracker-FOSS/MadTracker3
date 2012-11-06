//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTXML.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTXML.h"
//---------------------------------------------------------------------------
void initXML()
{
	MTXML *xml = mtxmlcreate();
	xml->readfile("test.xml",0);
	mtxmldelete(xml);
}

void uninitXML()
{
	xmlCleanupParser();
}
MTXML* mtxmlcreate()
{
	return new MTXML();
}

void mtxmldelete(MTXML *xml)
{
	delete xml;
}
//---------------------------------------------------------------------------
MTXML::MTXML():
doc(0),
root(0)
{
}

MTXML::~MTXML()
{
	if (doc) xmlFreeDoc(doc);
}

void* MTXML::readfile(const char *file,const char *encoding)
{
	doc = xmlReadFile(file,encoding,0);
	if (doc==0) return 0;
	root = xmlDocGetRootElement(doc);
	return root;
}

void* MTXML::readmemory(const char *buf,int size,const char *encoding)
{
	doc = xmlReadMemory(buf,size,0,encoding,0);
	if (doc==0) return 0;
	root = xmlDocGetRootElement(doc);
	return root;
}

char* MTXML::getname(void *node)
{
	if (node==0) return 0;
	return (char*)((xmlNode*)node)->name;
}

void* MTXML::getchildren(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->children;
}

void* MTXML::getlast(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->last;
}

void* MTXML::getparent(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->parent;
}

void* MTXML::getnext(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->next;
}

void* MTXML::getprev(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->prev;
}

char* MTXML::getcontent(void *node)
{
	if (node==0) return 0;
	return (char*)((xmlNode*)node)->content;
}

void* MTXML::getproperties(void *node)
{
	if (node==0) return 0;
	return ((xmlNode*)node)->properties;
}
//---------------------------------------------------------------------------
