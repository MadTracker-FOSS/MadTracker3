//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTIOInstrument.cpp 78 2005-08-28 20:59:58Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
#include "MTInstrument.h"
#include "MTRIFF.h"
#include "MTObjects1.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
struct SHDR{
	char samplename[20];
	int start;
	int end;
	int loopstart;
	int loopend;
	int samplerate;
	unsigned char note;
	signed char correction;
	short samplelink;
	short type;
};

struct rangesType{
	unsigned char lo;
	unsigned char hi;
};

union genAmountType{
	rangesType ranges;
	short shamount;
	unsigned short wamount;
};

struct IGEN{
	short oper;
	genAmountType amount;
};
//---------------------------------------------------------------------------
// Instrument Load Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Sound Font 2
//---------------------------------------------------------------------------
bool loadSF2(MTObject *object,char *filename,void* process)
{
	MTInstrument &instr = *(MTInstrument*)object;
	MTSample *sample;
	int spldata,oldseek;
	MTFile *f;
	int x,y,tmpl,size,size2,incl;
	SHDR shdr;
	IGEN igen;
	struct{
		short ioffset;
		short moffset;
	} bags[MAX_GRPS];
	int nbags = 0;
	bool first,end;
	unsigned char splmap[MAX_GRPS];
	struct{
		char pan;
		char loop;
		char note;
		char pitch;
		char root;
	} splinfo[MAX_GRPS];
	
	if ((f = si->fileopen(filename,MTF_READ|MTF_SHAREREAD))==0) return false;
	size = f->length();
	RIFF &criff = *(RIFF*)f->getpointer(-1,sizeof(RIFF));
	if ((criff.riffid!=FOURCC('R','I','F','F')) || (criff.wave.waveid!=FOURCC('s','f','b','k'))){
		f->releasepointer(&criff);
		goto error;
	};
	f->seek(12,MTF_CURRENT);
	while (!f->eof()){
		f->read(&tmpl,4);
		f->read(&size,4);
		if (tmpl==FOURCC('L','I','S','T')){
			incl = 0;
			f->read(&tmpl,4);
			size -= 4;
			switch (tmpl){
			case FOURCC('I','N','F','O'):
				while (incl<size){
					f->read(&tmpl,4);
					f->read(&size2,4);
					if (size2 & 1) size2++;
					incl += size2+8;
					switch (tmpl){
					case FOURCC('I','N','A','M'):
						if (size2<32) f->read(instr.name,size2);
						else{
							mtmemzero(instr.name,32);
							f->read(instr.name,31);
							f->seek(size2-31,MTF_CURRENT);
						};
						break;
					default:
						f->seek(size2,MTF_CURRENT);
					};
				};
				break;
			case FOURCC('s','d','t','a'):
				while (incl<size){
					f->read(&tmpl,4);
					f->read(&size2,4);
					if (size2 & 1) size2++;
					incl += size2+8;
					switch (tmpl){
					case FOURCC('s','m','p','l'):
						spldata = f->seek(0,MTF_CURRENT);
						f->seek(size2,MTF_CURRENT);
						break;
					default:
						f->seek(size2,MTF_CURRENT);
					};
				};
				break;
			case FOURCC('p','d','t','a'):
				while (incl<size){
					f->read(&tmpl,4);
					f->read(&size2,4);
					if (size2 & 1) size2++;
					incl += size2+8;
					switch (tmpl){
					case FOURCC('i','n','s','t'):
						f->read(instr.name,20);
						f->seek(size2-20,MTF_CURRENT);
						break;
					case FOURCC('i','b','a','g'):
						nbags = size2/4;
						if (size2<=sizeof(bags)){
							f->read(&bags,size2);
						}
						else{
							f->read(&bags,sizeof(bags));
							f->seek(size2-sizeof(bags),MTF_CURRENT);
						};
						break;
					case FOURCC('i','g','e','n'):
						splmap[0] = 0;
						mtmemzero(splinfo,sizeof(splmap));
						size2 += f->seek(0,MTF_CURRENT);
						for (x=0;x<nbags;x++){
							if (x==MAX_GRPS) break;
							f->seek(bags[x].ioffset*4,MTF_BEGIN);
							first = true;
							end = false;
							while (!end){
								f->read(&igen,sizeof(IGEN));
								switch (igen.oper){
								case 17: // Pan
									splinfo[x].pan = (float)igen.amount.shamount*127/1000;
									break;
								case 43: // Key range
									if (!first){
										end = true;
										break;
									};
									for (y=igen.amount.ranges.lo-11;y<=igen.amount.ranges.hi-11;y++){
										if ((y>=0) && (y<96)) instr.range[0][y] = x;
									};
									break;
								case 44: // Vol range
									
									break;
								case 51: // Pitch
									splinfo[x].note = igen.amount.shamount;
									break;
								case 52: // Fine
									splinfo[x].pitch = (float)igen.amount.shamount*128/100;
									break;
								case 53: // Sample ID
									splmap[x] = igen.amount.ranges.lo;
								case 0:
									end = true;
									break;
								case 54: // Loop mode
									splinfo[x].loop = igen.amount.ranges.lo & 1;
									break;
								case 58: // Root key
									splinfo[x].root = igen.amount.shamount-11;
								};
								first = false;
							};
						};
						f->seek(size2,MTF_BEGIN);
						for (x=0;x<96;x++)
							instr.range[0][x] = splmap[instr.range[0][x]];
						break;
					case FOURCC('s','h','d','r'):
						size2 += f->seek(0,MTF_CURRENT);
						while (f->seek(0,MTF_CURRENT)<size2){
							f->read(&shdr,sizeof(SHDR));
							if ((strcmp(shdr.samplename,"EOS")!=0) && (instr.acceptoscillator())){
								sample = (MTSample*)oi->newobject(MTO_MTSAMPLE,instr.parent,-1);
								if (!sample) goto error;
								tmpl = instr.addoscillator(sample);
								y = 0;
								for (x=0;x<MAX_GRPS;x++){
									if (splmap[x]==tmpl){
										y = x;
										break;
									};
								};
								sample->lock(MTOL_LOCK,true);
								memcpy(sample->name,shdr.samplename,20);
								sample->frequency = shdr.samplerate;
								if (splinfo[tmpl].root)
									sample->note = splinfo[tmpl].root;
								else{
									if (shdr.note<128)
										sample->note = shdr.note-11;
									else
										sample->note = 49;
								};
								sample->note -= splinfo[y].note;
								instr.grp[tmpl].pitch = splinfo[y].pitch;
								sample->length = shdr.end-shdr.start;
								if (sample->length<0) sample->length = 0;
								sample->splalloc(sample->length/sample->sl);
								oldseek = f->seek(1,MTF_CURRENT);
								f->seek(spldata+shdr.start*2,MTF_BEGIN);
								f->read(sample->data,sample->length);
								f->seek(oldseek,MTF_BEGIN);
								sample->loops = shdr.loopstart-shdr.start;
								sample->loope = shdr.loopend-shdr.start;
								instr.grp[tmpl].panx = ((float)splinfo[y].pan-128)/128;
								sample->loop = splinfo[y].loop;
								sample->lock(MTOL_LOCK,false);
							};
						};
						f->seek(size2,MTF_BEGIN);
						break;
					default:
						f->seek(size2,MTF_CURRENT);
					};
				};
			};
		}
		else f->seek(size,MTF_CURRENT);
		};
	delete f;
	return true;
abort:
error:
	for (x=0;x<MAX_GRPS;x++){
		if (instr.grp[x].spl) delete instr.grp[x].spl;
	};
	delete f;
	return false;
}
//---------------------------------------------------------------------------
// Instrument Save Functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
