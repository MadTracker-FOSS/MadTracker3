//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTIOOscillator.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTOscillator.h"
#include "MTObjects1.h"
#include "MTRIFF.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
// Sample Load Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Microsoft Wave
//---------------------------------------------------------------------------
bool loadWAV(MTObject *object,char *filename,void *process)
{
	MTFile *f;
	MTSample &sample = *(MTSample*)object;
	int tmpl,size,size2,incl;
	char tmpc[512];
	char *e;
	RIFFfmt *fmt;
	void *data;
	
	if ((f = si->fileopen(filename,MTF_READ|MTF_SHAREREAD))==0) return false;
	size = f->length();
	RIFF &criff = *(RIFF*)f->getpointer(-1,sizeof(RIFF));
	if ((criff.riffid!=FOURCC('R','I','F','F')) || (criff.wave.waveid!=FOURCC('W','A','V','E'))){
		f->releasepointer(&criff);
		goto error;
	};
	f->releasepointer(&criff);
	f->seek(12,MTF_CURRENT);
	sample.filename = (char*)si->memalloc(strlen(filename)+1,0);
	strcpy(sample.filename,filename);
	e = strrchr(filename,'/');
	if (!e) e = strrchr(filename,'\\');
	if (e) strcpy(tmpc,e+1);
	else strcpy(tmpc,filename);
	tmpl = (int)strchr(tmpc,'.')-(int)tmpc;
	if (tmpl>31) tmpl = 31;
	tmpc[tmpl] = '\0';
	strcpy(sample.name,tmpc);
	while (!f->eof()){
		f->read(&tmpl,4);
		f->read(&size,4);
		switch (tmpl){
		case FOURCC('f','m','t',' '):
			fmt = (RIFFfmt*)f->getpointer(f->pos()-8,sizeof(RIFFfmt));
			f->seek(size,MTF_CURRENT);
			if (fmt->tag!=1){
				f->releasepointer(fmt);
				goto error;
			};
			sample.nchannels = (char)fmt->nchannels;
			sample.frequency = fmt->nsamplepersec;
			sample.sl = fmt->nalign;
			sample.depth = fmt->nbits>>3;
			sample.loop = 0;
			sample.loops = 0;
			sample.loope = 0;
			f->releasepointer(fmt);
			break;
		case FOURCC('d','a','t','a'):
			if (!sample.splalloc(size/sample.sl)) goto error;
			sample.fileoffset = f->pos();
			sample.length = size;
			sample.ns = size/sample.sl;
//			sample.loadfromstream(f,size,0);
			data = f->getpointer(-1,size);
			if (sample.depth==1) a_deinterleave_8((char**)sample.data,(char*)data,sample.nchannels,sample.ns);
			else a_deinterleave_16((short**)sample.data,(short*)data,sample.nchannels,sample.ns);
//			f->read(sample.data[0],size);
			if (sample.depth==1) sample.changesign();
			break;
		case FOURCC('L','I','S','T'):
			incl = 0;
			f->read(&tmpl,4);
			size -= 4;
			if (tmpl==FOURCC('I','N','F','O')){
				while (incl<size){
					f->read(&tmpl,4);
					f->read(&size2,4);
					if (size2 & 1) size2++;
					incl += size2+8;
					switch (tmpl){
					case FOURCC('I','N','A','M'):
						if (size2<32) f->read(sample.name,size2);
						else{
							mtmemzero(sample.name,32);
							f->read(sample.name,31);
							f->seek(size2-31,MTF_CURRENT);
						};
						break;
					default:
						f->seek(size2,MTF_CURRENT);
					};
				};
			}
			else f->seek(size,MTF_CURRENT);
			break;
		case FOURCC('s','m','p','l'):
			f->read(&tmpc,size);
			sample.loops = *(int*)&tmpc[44];
			sample.loope = *(int*)&tmpc[48];
			break;
		default:
			f->seek(size,MTF_CURRENT);
		};
	};
	f->gettime(&sample.time,0);
	si->fileclose(f);
	return true;
error:
	si->fileclose(f);
	return false;
}
//---------------------------------------------------------------------------
// Sample Save Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
