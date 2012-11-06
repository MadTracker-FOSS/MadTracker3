//---------------------------------------------------------------------------
#ifndef MTDISPLAYASM_INCLUDED
#define MTDISPLAYASM_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
typedef void (MTCT *ShadeProc)(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
typedef void (MTCT *SShadeProc)(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits);
typedef void (MTCT *BlendProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits);
typedef void (MTCT *BlendTProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch);
typedef void (MTCT *BlendCKProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey);
typedef void (MTCT *BlurProc)(void *tmp,void *mask,int w,int h,int pitch,int amount);
typedef void (MTCT *OpacityProc)(void *mask,int w,int h,int pitch,int amount);
typedef void (MTCT *FillProc)(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits);
//---------------------------------------------------------------------------
void extractmask(void *source,void *mask,int color,int *pal,unsigned char **buf);
void MTCT a_putalpha(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
//---------------------------------------------------------------------------
void MTCT a_shade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
void MTCT a_sshade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits);
void MTCT a_blend(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits);
void MTCT a_blendt(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch);
void MTCT a_blendck(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey);
void MTCT a_blur(void *tmp,void *mask,int w,int h,int pitch,int amount);
void MTCT a_opacity(void *mask,int w,int h,int pitch,int amount);
void MTCT a_fill(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits);
//---------------------------------------------------------------------------
#endif
