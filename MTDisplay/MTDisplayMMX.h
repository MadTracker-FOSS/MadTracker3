//---------------------------------------------------------------------------
#ifndef MTDISPLAYMMX_INCLUDED
#define MTDISPLAYMMX_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
void MTCT mmx_shade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
void MTCT mmx_sshade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits);
void MTCT mmx_blend(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits);
void MTCT mmx_blendt(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch);
void MTCT mmx_blendck(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey);
void MTCT mmx_fill(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits);
//---------------------------------------------------------------------------
#endif
