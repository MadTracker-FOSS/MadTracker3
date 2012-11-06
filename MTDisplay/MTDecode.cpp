//---------------------------------------------------------------------------
//
//	MadTracker Display Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <windows.h>
#include "MTDisplay1.h"
#include "MTDecode.h"
#include "../Headers/MTXSystem.h"
#include "../Headers/MTXSystem2.h"
#define XMD_H
#undef NEED_FAR_POINTERS
#undef FAR
#include <png.h>
#include <jpeglib.h>
//---------------------------------------------------------------------------
// LZH Decoder
//---------------------------------------------------------------------------
int lzhdecode(void *stream,int length,int lzmin,void *bits)
{
	register char *sp = (char*)stream;
	char *ep = sp+length;
	char *dp = (char*)bits;
	register char *cp;
	register unsigned long code,old,cm;
	register unsigned char sbc,cbo;
	unsigned long x;
	unsigned long nc = (1<<lzmin);
	struct st{
		unsigned short p;
		unsigned short length;
		unsigned char c;
	} strings[4096];
	
	ENTER("lzhdecode");
	cbo = 0;
init:
	mtmemzero(strings,sizeof(strings));
	for (x=0;x<nc;x++) strings[x].c = (char)x;
	x += 2;
	sbc = lzmin+1;
	cm = (1<<sbc)-1;
	code = ((*(unsigned long*)sp)>>cbo) & cm;
	cbo += sbc;
	if (cbo>=16){
		sp += 2;
		cbo -= 16;
	};
	if (code==nc) goto init;
	*dp++ = (char)code;
	old = code;
	while (sp<ep){
		code = ((*(unsigned long*)sp)>>cbo) & cm;
		cbo += sbc;
		if (cbo>=16){
			sp += 2;
			cbo -= 16;
		};
		if (code==nc) goto init;
		else if (code==nc+1) break;
		else if (code<x){
			register st *cst = &strings[code];
			cp = dp+cst->length;
			while (true){
				*cp-- = cst->c;
				if (cp>=dp) cst = &strings[cst->p];
				else break;
			};
			strings[x].length = strings[old].length+1;
			strings[x].p = (unsigned short)old;
			strings[x].c = cst->c;
			dp += strings[code].length+1;
		}
		else{
			if (code!=x) break;
			register st *cst = &strings[old];
			cp = dp+cst->length;
			while (true){
				*cp-- = cst->c;
				if (cp>=dp) cst = &strings[cst->p];
				else break;
			};
			strings[x].length = strings[old].length+1;
			strings[x].p = (unsigned short)old;
			strings[x].c = cst->c;
			dp += strings[old].length+1;
			*dp++ = cst->c;
		};
		old = code;
		if (++x==cm+1){
			if (sbc<12){
				sbc++;
				cm = (1<<sbc)-1;
			};
		};
	};
	LEAVE();
	return dp-(char*)bits;
}

#define MAXCODE(n)	((1L<<(n))-1)
#define	BITS_MIN		9
#define	BITS_MAX		12
#define	CODE_CLEAR	256
#define	CODE_EOI		257
#define CODE_FIRST	258
#define	CODE_MAX		MAXCODE(BITS_MAX)
#define	HSIZE				9001L
#define	HSHIFT			(13-8)
#define	CSIZE				(MAXCODE(BITS_MAX)+1L)
#define	getnextcode(sp,code){\
	nextdata = (nextdata<<8)|*(sp)++;\
	nextbits += 8;\
	if (nextbits<nbits){\
		nextdata = (nextdata<<8)|*(sp)++;\
		nextbits += 8;\
	};\
	code = ((nextdata>>(nextbits-nbits)) & nbitsmask);\
	nextbits -= nbits;\
}
struct code_t{
	struct code_t *next;
	unsigned short	length;
	unsigned char	value;
	unsigned char	firstchar;
};

int lzhdecode2(void *stream,int length,void *bits)
{
	register long nbits,nextbits,nextdata,nbitsmask;
	code_t *codep,*oldcodep,*free_entp,*maxcodep,*codetab;
	register unsigned char *sp = (unsigned char*)stream;
	unsigned char *ep = sp+length;
	unsigned char *dp = (unsigned char*)bits;
	register unsigned char *cp;
	register unsigned long code,len;

	ENTER("lzhdecode2");
	codetab = (code_t*)si->memalloc(CSIZE*sizeof(code_t),0);
	for (code=0;code<256;code++){
		codetab[code].value = (unsigned char)code;
		codetab[code].firstchar = (unsigned char)code;
		codetab[code].length = 1;
		codetab[code].next = 0;
	};
	nextbits = nextdata = nbitsmask = 0;
	nbits = 9;
	nbitsmask = (1<<nbits)-1;
	while (sp<ep){
		getnextcode(sp,code);
		if (code==CODE_EOI) break;
		if (code==CODE_CLEAR){
			free_entp = codetab+CODE_FIRST;
			nbits = BITS_MIN;
			nbitsmask = MAXCODE(BITS_MIN);
			maxcodep = codetab+nbitsmask-1;
			getnextcode(sp,code);
			if (code==CODE_EOI) break;
			*dp++ = (unsigned char)code;
			oldcodep = codetab+code;
			continue;
		};
		codep = codetab+code;
		free_entp->next = oldcodep;
		free_entp->firstchar = free_entp->next->firstchar;
		free_entp->length = free_entp->next->length+1;
		free_entp->value = (codep<free_entp)?codep->firstchar:free_entp->firstchar;
		if (++free_entp>maxcodep){
			if (++nbits>BITS_MAX) nbits = BITS_MAX;
			nbitsmask = MAXCODE(nbits);
			maxcodep = codetab+nbitsmask-1;
		};
		oldcodep = codep;
		if (code>=256){
			len = codep->length;
			cp = dp+len;
			do {
				int t;
				--cp;
				t = codep->value;
				codep = codep->next;
				*cp = t;
			} while ((codep) && (cp>dp));
			dp += len;
		}
		else{
			*dp++ = (unsigned char)code;
		};
	};
	si->memfree(codetab);
	LEAVE();
	return dp-(unsigned char*)bits;
}
//---------------------------------------------------------------------------
void* loadgif(MTFile *f,int &colorkey)
{
	BITMAPINFO *bmi;
	void *stream,*bits,*mbits;
	char *sp,*dp;
	int x,y,l,nc,mpitch;
	bool ct,interlaced;
	char lzmin;
	unsigned short w,h;
	HBITMAP bmp = 0;
	unsigned char buf[256];
	
	FENTER2("loadgif(%.8X,%d)",f,colorkey);
	stream = 0;
	l = f->length();
	f->read(buf,3);
	buf[3] = 0;
	if (strcmp((char*)buf,"GIF")==0){
		bmi = (BITMAPINFO*)si->memalloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256,MTM_ZERO);
		f->read(buf,3);
		f->read(&w,2);
		f->read(&h,2);
		f->read(buf,3);
		ct = (buf[0] & 0x80)!=0;
		nc = 1<<((buf[0] & 0x7)+1);
		BITMAPINFOHEADER &bih = bmi->bmiHeader;
		bih.biSize = sizeof(bih);
		bih.biWidth = w;
		bih.biHeight = -h;
		bih.biPlanes = 1;
		bih.biBitCount = 8;
		bih.biCompression = BI_RGB;
		mpitch = ((((w*8)>>3)+3)>>2)<<2;
		if (ct){
			for (x=0;x<nc;x++){
				int &i = *(int*)&bmi->bmiColors[x];
				f->read(&i,3);
				i = ((i & 0xFF)<<16)|(i & 0xFF00)|(i>>16);
			};
		};
		while (f->pos()<l){
			f->read(buf,1);
			if (buf[0]=='!'){
				f->read(buf,2);
				if (buf[0]==0xF9){
					f->read(buf,buf[1]+1);
					ct = (buf[0] & 1);
					if (ct){
						if (colorkey<0) colorkey = 0xFF00FF;
						*(int*)&bmi->bmiColors[buf[3]] = colorkey;
					};
				}
				else f->seek(buf[1]+1,MTF_CURRENT);
			}
			else if (buf[0]==';'){
				break;
			}
			else if (buf[0]==','){
				f->read(buf,9);
				interlaced = (buf[8] & 64)!=0;
				f->read(&lzmin,1);
				x = f->pos();
				y = 0;
				f->read(buf,1);
				while (buf[0]){
					y += buf[0];
					f->seek(buf[0],MTF_CURRENT);
					if (f->read(buf,1)!=1) break;
				};
				f->seek(x,MTF_BEGIN);
				stream = si->memalloc(y,0);
				sp = (char*)stream;
				f->read(buf,1);
				while (buf[0]){
					f->read(sp,buf[0]);
					sp += buf[0];
					if (f->read(buf,1)!=1) break;
				};
			};
		};
		bmp = CreateDIBSection(0,bmi,DIB_RGB_COLORS,&mbits,0,0);
		si->memfree(bmi);
	};
	if (stream){
		bits = si->memalloc(w*h,MTM_ZERO);
		lzhdecode(stream,y,lzmin,bits);
		si->memfree(stream);
		dp = (char*)mbits;
		sp = (char*)bits;
		if (interlaced){
			for (x=0;x<(h>>3);x++){
				memcpy(dp,sp,w);
				dp += (mpitch<<3);
				sp += w;
			};
			dp = (char*)mbits+mpitch*4;
			for (x=0;x<(h>>3);x++){
				memcpy(dp,sp,w);
				dp += (mpitch<<3);
				sp += w;
			};
			dp = (char*)mbits+mpitch*2;
			for (x=0;x<(h>>2);x++){
				memcpy(dp,sp,w);
				dp += (mpitch<<2);
				sp += w;
			};
			dp = (char*)mbits+mpitch;
			for (x=0;x<(h>>1);x++){
				memcpy(dp,sp,w);
				dp += (mpitch<<1);
				sp += w;
			};
		}
		else{
			for (x=0;x<h;x++){
				memcpy(dp,sp,w);
				dp += mpitch;
				sp += w;
			};
		};
		si->memfree(bits);
	};
	LEAVE();
	return bmp;
}
//---------------------------------------------------------------------------
void* loadtif(MTFile *f,int &colorkey)
{
	BITMAPINFO *bmi;
	unsigned short align,version,nentries,tag,type,bps[4];
	int x,y,w,h,offset,eoffset,elength,spp,compression;
	int rowsperstrip,stripsperimage,predictor;
	char value[4];
	void *stream,*mbits;
	char *sp,*ep,*dp;
	int *stripoffsets,*striplengths;
	HBITMAP bmp = 0;
	struct Palette{
		unsigned short red;
		unsigned short green;
		unsigned short blue;
	} *pal;
	bool swap,translated;
	static const int typesize[6] = {0,1,1,2,4,8};
	
	FENTER2("loadtif(%.8X,%d)",f,colorkey);
	if (colorkey<0) colorkey = 0;
	stream = 0;
	f->read(&align,2);
	if ((align=='II') || (align=='MM')){
		bmi = (BITMAPINFO*)si->memalloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256,MTM_ZERO);
#		if (BIG_ENDIAN==1234)
			swap = (align=='II');
#		else
			swap = (align=='MM');
#		endif
		f->read(&version,2);
		if (swap) version = swap_word(version);
		if (version==42){
			f->read(&offset,4);
			if (swap) offset = swap_dword(offset);
			w = h = 0;
			spp = bps[0] = bps[1] = bps[2] = 1;
			stripoffsets = striplengths = 0;
			pal = 0;
			compression = 1;
			while (offset){
				f->seek(offset,MTF_BEGIN);
				f->read(&nentries,2);
				if (swap) nentries = swap_word(nentries);
				while (nentries-->0){
					f->read(&tag,2);
					f->read(&type,2);
					f->read(&elength,4);
					f->read(value,4);
					if (swap){
						tag = swap_word(tag);
						type = swap_word(type);
						elength = swap_dword(elength);
					};
					translated = false;
					if (typesize[type]*elength<=4){
						translated = true;
						switch (type){
						case 1:
							eoffset = value[0];
							break;
						case 3:
							if (swap) eoffset = (unsigned short)swap_word(*(short*)&value);
							else eoffset = *(unsigned short*)&value;
							break;
						case 4:
							if (swap) eoffset = (unsigned long)swap_dword(*(long*)&value);
							else eoffset = *(unsigned long*)&value;
							break;
						};
					}
					else if (swap){
						eoffset = swap_dword(*(long int*)&value);
					}
					else{
						eoffset = *(int*)&value;
					};
					offset = f->pos();
					switch (tag){
					case 256: // ImageWidth
						w = eoffset;
						break;
					case 257: // ImageLength
						h = eoffset;
						break;
					case 258:	// BitsPerSample
						if (translated) bps[0] = eoffset;
						else{
							f->seek(eoffset,MTF_BEGIN);
							f->read(bps,elength*sizeof(short));
							f->seek(offset,MTF_BEGIN);
							if (swap){
								for (x=0;x<elength;x++){
									bps[x] = swap_word(bps[x]);
								};
							};
						};
						break;
					case 259:	// Compression
						compression = eoffset;
						break;
					case 273:	// StripOffsets
						if (translated){
							stripoffsets = (int*)si->memalloc(sizeof(int),0);
							stripsperimage = 1;
							*stripoffsets = eoffset;
						}
						else{
							stripoffsets = (int*)si->memalloc(elength*sizeof(int),MTM_ZERO);
							stripsperimage = elength;
							f->seek(eoffset,MTF_BEGIN);
							if (type==3){
								for (x=0;x<elength;x++){
									f->read(&stripoffsets[x],sizeof(short));
									if (swap) stripoffsets[x] = (unsigned short)swap_word(*(short*)&stripoffsets[x]);
								};
							}
							else{
								f->read(stripoffsets,elength*sizeof(int));
								if (swap){
									for (x=0;x<elength;x++){
										stripoffsets[x] = swap_dword(stripoffsets[x]);
									};
								};
							};
							f->seek(offset,MTF_BEGIN);
						};
						break;
					case 277:	// SamplesPerPixel
						spp = eoffset;
						break;
					case 278:	// RowsPerStrip
						rowsperstrip = eoffset;
						stripsperimage = (h+rowsperstrip-1)/rowsperstrip;
						break;
					case 279:	// StripByteCounts
						if (translated){
							striplengths = (int*)si->memalloc(sizeof(int),0);
							stripsperimage = 1;
							*striplengths = eoffset;
						}
						else{
							striplengths = (int*)si->memalloc(elength*sizeof(int),MTM_ZERO);
							stripsperimage = elength;
							f->seek(eoffset,MTF_BEGIN);
							if (type==3){
								for (x=0;x<elength;x++){
									f->read(&striplengths[x],sizeof(short));
									if (swap) striplengths[x] = (unsigned short)swap_word(*(short*)&striplengths[x]);
								};
							}
							else{
								f->read(striplengths,elength*sizeof(int));
								if (swap){
									for (x=0;x<elength;x++){
										striplengths[x] = swap_dword(striplengths[x]);
									};
								};
							};
							f->seek(offset,MTF_BEGIN);
						};
						break;
					case 317:	// Predictor
						predictor = eoffset;
						break;
					case 320:	// ColorMap
						pal = (Palette*)si->memalloc((1<<bps[0])*sizeof(Palette),0);
						f->seek(eoffset,MTF_BEGIN);
						f->read(pal,elength*sizeof(short));
						f->seek(offset,MTF_BEGIN);
						break;
					};
				};
				f->read(&offset,4);
			};
			if (((predictor==1) || (predictor==2)) && (stripoffsets) && (striplengths) && ((compression==1) || (compression==5))){
				elength = 0;
				if (compression==5){
					int maxlength = 0;
					int rl = h;
					int l = (bps[0]>>3)*spp*w;
					void *tmp;
					for (x=0;x<stripsperimage;x++){
						if (striplengths[x]>maxlength) maxlength = striplengths[x];
					};
					elength = (bps[0]>>3)*spp*w*(h+1);
					tmp = si->memalloc(maxlength,0);
					stream = si->memalloc(elength,0);
					sp = (char*)stream;
					ep = sp;
					for (x=0;x<stripsperimage;x++){
						f->seek(stripoffsets[x],MTF_BEGIN);
						f->read(tmp,striplengths[x]);
						lzhdecode2(tmp,striplengths[x],ep);
						if (rl>rowsperstrip){
							ep += l*rowsperstrip;
							rl -= rowsperstrip;
						}
						else{
							ep += l*rl;
							rl = 0;
						};
					};
					if (predictor==2){
						if (spp==3){
							for (y=0;y<h;y++){
								sp = (char*)stream+y*w*spp;
								dp = sp+spp;
								x = w;
								while (--x>0){
									*dp++ += *sp++;
									*dp++ += *sp++;
									*dp++ += *sp++;
								};
							};
						}
						else if (spp==4){
							for (y=0;y<h;y++){
								sp = (char*)stream+y*w*spp;
								dp = sp+spp;
								x = w;
								while (--x>0){
									*dp++ += *sp++;
									*dp++ += *sp++;
									*dp++ += *sp++;
									*dp++ += *sp++;
								};
							};
						};
					};
					si->memfree(tmp);
				}
				else{
					for (x=0;x<stripsperimage;x++) elength += striplengths[x];
					stream = si->memalloc(elength,0);
					sp = (char*)stream;
					ep = sp;
					for (x=0;x<stripsperimage;x++){
						f->seek(stripoffsets[x],MTF_BEGIN);
						f->read(ep,striplengths[x]);
						ep += striplengths[x];
					};
				};
				switch (spp){
				case 3:
				case 4:
					if ((bps[0]==8) && (bps[1]==8) && (bps[2]==8)){
						BITMAPINFOHEADER &bih = bmi->bmiHeader;
						bih.biSize = sizeof(bih);
						bih.biWidth = w;
						bih.biHeight = -h;
						bih.biPlanes = 1;
						bih.biBitCount = 24;
						bih.biCompression = BI_RGB;
						bmp = CreateDIBSection(0,bmi,DIB_RGB_COLORS,&mbits,0,0);
						sp = (char*)stream;
						dp = (char*)mbits;
						if (spp==4){
							int sample;
							while (sp<ep-4){
								sample = swap_dword(*(int*)sp);
								if (sample & 0xFF) *(int*)dp = sample>>8;
								else *(int*)dp = colorkey;
								sp += 4;
								dp += 3;
							};
							sample = swap_dword(*(int*)sp);
							if (sample & 0xFF) sample >>= 8;
							else sample = colorkey;
							*(unsigned short*)dp = sample & 0xFFFF;
							dp += 2;
							*dp = sample>>16;
						}
						else{
							int sample;
							while (sp<ep-3){
								sample = swap_dword(*(int*)sp);
								*(int*)dp = sample>>8;
								sp += 3;
								dp += 3;
							};
							sample = swap_dword(*(int*)sp)>>8;
							*(unsigned short*)dp = sample & 0xFFFF;
							dp += 2;
							*dp = sample>>16;
						};
					};
					break;
				case 1:
					break;
				};
				si->memfree(stream);
			};
			if (stripoffsets) si->memfree(stripoffsets);
			if (striplengths) si->memfree(striplengths);
			if (pal) si->memfree(pal);
		};
		si->memfree(bmi);
	};
	LEAVE();
	return bmp;
}
//---------------------------------------------------------------------------
void mtpngread(png_structp png_ptr,png_bytep data,png_size_t length)
{
	MTFile *f = (MTFile*)png_get_io_ptr(png_ptr);
	f->read(data,length);
}

void* loadpng(MTFile *f)
{
	HBITMAP bmp = 0;
	png_byte header[16];
	png_byte **rows;
	png_struct *png_ptr;
	png_info *info_ptr,*end_info;
	BITMAPINFO *bmi;
	png_byte *mbits,*ptr;
	int x;

	f->read(header,16);
	if (png_sig_cmp(header,0,16)!=0) return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if (!png_ptr) return 0;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		png_destroy_read_struct(&png_ptr,0,0);
		return 0;
	};
	end_info = png_create_info_struct(png_ptr);
	if (!end_info){
		png_destroy_read_struct(&png_ptr,&info_ptr,0);
		return 0;
	};
	png_set_read_fn(png_ptr,f,mtpngread);
	png_set_sig_bytes(png_ptr,16);

	png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,0);

	bmi = (BITMAPINFO*)si->memalloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256,MTM_ZERO);
	BITMAPINFOHEADER &bih = bmi->bmiHeader;
	bih.biSize = sizeof(bih);
	bih.biWidth = info_ptr->width;
	bih.biHeight = -(int)info_ptr->height;
	bih.biPlanes = 1;
	bih.biBitCount = info_ptr->bit_depth*info_ptr->channels;
	bih.biCompression = BI_RGB;
	bmp = CreateDIBSection(0,bmi,DIB_RGB_COLORS,(void**)&mbits,0,0);
	rows = (png_byte**)si->memalloc(sizeof(png_byte*)*info_ptr->height,MTM_ZERO);
	ptr = mbits;
	for (x=0;x<info_ptr->height;x++,ptr += info_ptr->width*(bih.biBitCount/8)) rows[x] = ptr;
	png_set_rows(png_ptr,info_ptr,rows);

	si->memfree(rows);
	si->memfree(bmi);
	return bmp;
}
//---------------------------------------------------------------------------
void* loadjpg(MTFile *f)
{
	HBITMAP bmp = 0;

	return bmp;
}
//---------------------------------------------------------------------------
