//---------------------------------------------------------------------------
//
//	MadTracker Display Core
//
//		Platforms:	Win32
//		Processors:	x86
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <windows.h>
#include "MTDisplayASM.h"
#include "MTDisplay1.h"
//---------------------------------------------------------------------------
void extractmask(void *source,void *mask,int color,int *pal,unsigned char **buf)
{
	register int x,y,w,h;
	int sbc,spitch,mpitch;
	register unsigned char *s,*m;
	register unsigned char *b,*l1,*l2;
	BITMAP sb,mb;
	BITMAPINFO *bi;
	
	GetObject(GetCurrentObject((HDC)source,OBJ_BITMAP),sizeof(sb),&sb);
	GetObject(GetCurrentObject((HDC)mask,OBJ_BITMAP),sizeof(mb),&mb);
	w = sb.bmWidth;
	h = sb.bmHeight;
	sbc = sb.bmBitsPixel;
	s = (unsigned char*)sb.bmBits;
	m = (unsigned char*)mb.bmBits;
	spitch = sb.bmWidthBytes;
	mpitch = mb.bmWidthBytes;
	if (sbc<15){
		for (x=(1<<sbc)-1;x>=0;x--){
			if (pal[x]==color){
				color = x;
				break;
			};
		};
	};
	*buf = (unsigned char*)si->memalloc(w*h,0);
	b = *buf;
	l1 = s;
	l2 = b;
	switch (sbc){
	case 8:
		for (y=h;y>0;y--){
			s = l1;
			b = l2;
			for (x=w;x>0;x--){
				*b++ = (*s++==color)?0xFF:0x00;
			};
			l1 += spitch;
			l2 += w;
		};
		break;
	case 24:
		for (y=h;y>1;y--){
			s = l1;
			b = l2;
			for (x=w;x>0;x--){
				*b++ = ((*(int*)s & 0x00FFFFFF)==color)?0xFF:0x00;
				s += 3;
			};
			l1 += spitch;
			l2 += w;
		};
		for (x=w;x>0;x--){
			*b++ = ((*(unsigned short*)s|(*(s+2)<<16))==color)?0xFF:0x00;
			s += 3;
		};
		break;
	case 32:
		for (y=h;y>0;y--){
			s = l1;
			b = l2;
			for (x=w;x>0;x--){
				*b++ = (*(int*)s==color)?0xFF:0x00;
				s += 4;
			};
			l1 += spitch;
			l2 += w;
		};
		break;
	};
	bi = (BITMAPINFO*)si->memalloc(sizeof(BITMAPINFOHEADER)+4*256,MTM_ZERO);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth = w;
	bi->bmiHeader.biHeight = -h;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = 8;
	register int *cpal = (int*)&bi->bmiColors;
	register int ccol = 0;
	for (x=256;x>0;x--){
		*cpal++ = ccol;
		ccol += 0x010101;
	};
	SetDIBits((HDC)mask,(HBITMAP)GetCurrentObject((HDC)mask,OBJ_BITMAP),0,h,*buf,bi,DIB_RGB_COLORS);
	si->memfree(bi);
}
//---------------------------------------------------------------------------
void MTCT a_putalpha(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits)
{
	__asm{
		push	esi
		push	edi
		push	ebx
		mov		edi,surface
		mov		esi,mask
		mov		ecx,bits
		cmp		cl,32
		jl		pa_exit
// 32-bit
		mov		eax,w
		mov		edx,eax
		shl		eax,2
		sub		mpitch,edx
		sub		spitch,eax
		xor		edx,edx
		add		edi,3
pa_y32:
		mov		ecx,w
pa_x32:
		mov		dl,[esi]
		mov		[edi],dl
		inc		esi
		add		edi,4
		dec		ecx
		jnz		pa_x32
		add		edi,spitch
		add		esi,mpitch
		dec		h
		jnz		pa_y32

pa_exit:
		pop		ebx
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTCT a_shade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits)
{
	__asm{
		push	esi
		push	edi
		push	ebx
		mov		edi,surface
		mov		esi,mask
		mov		ecx,bits
		cmp		cl,15
		jl		sh_exit
		je		sh_b15
		cmp		cl,16
		je		sh_b16
		cmp		cl,24
		je		sh_b24
		
// 32-bit
		mov		eax,w
		mov		edx,eax
		shl		eax,2
		sub		mpitch,edx
		sub		spitch,eax
		xor		edx,edx
sh_y32:
		mov		ecx,w
sh_x32:
		mov		dl,[esi]	// dl = intensity
		xor		ebx,ebx
		test	dl,dl
		mov		eax,[edi]	// eax = ..bbggrr
		jz		sh_ok32
		xchg	ah,bl			// eax = ..bb..rr ebx = ......gg
		neg		dl
		imul	eax,edx		// eax = BBBBRRRR
		imul	ebx,edx		// ebx = ....GGGG
		shr		eax,8			// eax = ..BBBBRR
		mov		ah,bh			// eax = ..BBGGRR
sh_ok32:
		inc esi
		dec ecx
		stosd
		jnz sh_x32
		add edi,spitch
		add esi,mpitch
		dec h
		jnz sh_y32
		jmp sh_exit
		
// 24-bit
sh_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		sub mpitch,edx
		sub spitch,eax
		xor edx,edx
sh_y24:
		mov ecx,w
sh_x24:
		mov dl,[esi]  // dl = intensity
		xor ebx,ebx
		test dl,dl
		mov eax,[edi] // eax = ??bbggrr
		jz sh_ok24
		and eax,0x00FFFFFF
		xchg ah,bl    // eax = ??bb..rr ebx = ......gg
		neg dl
		imul eax,edx  // eax = BBBBRRRR
		imul ebx,edx  // ebx = ....GGGG
		shr eax,8     // eax = ..BBBBRR
		mov ah,bh     // eax = ..BBGGRR
sh_ok24:
		stosw
		inc esi
		shr eax,16
		dec ecx
		stosb
		jnz sh_x24
		add edi,spitch
		add esi,mpitch
		dec h
		jnz sh_y24
		jmp sh_exit

// 15-bit
sh_b15:
		mov eax,w
		mov edx,eax
		shl eax,1
		sub mpitch,edx
		sub spitch,eax
sh_y15:
		mov ecx,w
sh_x15:
		xor eax,eax
		mov dl,[esi]   // dl = intensity
		mov ax,[edi]   // eax = ....bxxr
		shr dl,3
		mov ebx,eax    // eax = ebx = ....bxxr
		jz sh_ok15
		and eax,0x7C1F // eax = ....bbrr
		xor dl,0x1F
		and ebx,0x03E0 // ebx = .....gg.
		imul eax,dl    // eax = ...BBxRR
		imul ebx,dl    // ebx = ....GGG.
		and eax,0xF83E0
		and ebx,0x7C00
		or eax,ebx
		shr eax,5
sh_ok15:
		stosw
		inc esi
		dec ecx
		jnz sh_x15
		add edi,spitch
		add esi,mpitch
		dec h
		jnz sh_y15
		jmp sh_exit
		
// 16-bit
sh_b16:
		mov eax,w
		mov edx,eax
		shl eax,1
		sub mpitch,edx
		sub spitch,eax
sh_y16:
		mov ecx,w
sh_x16:
		xor eax,eax
		mov dl,[esi]   // dl = intensity
		mov ax,[edi]   // eax = ....bxxr
		shr dl,3
		mov ebx,eax    // eax = ebx = ....bxxr
		jz sh_ok16
		and eax,0xF81F // eax = ....bbrr
		xor dl,0x1F
		and ebx,0x07E0 // ebx = .....gg.
		imul eax,dl    // eax = ...BBxRR
		imul ebx,dl    // ebx = ....GGG.
		and eax,0x1F03E0
		and ebx,0xFC00
		or eax,ebx
		shr eax,5
sh_ok16:
		stosw
		inc esi
		dec ecx
		jnz sh_x16
		add edi,spitch
		add esi,mpitch
		dec h
		jnz sh_y16
		
sh_exit:
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_sshade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits)
{
	__asm{
	};
}

void MTCT a_blend(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits)
{
	__asm{
		push esi
		push edi
		push ebx
		mov esi,src
		mov eax,oy
		mov ecx,spitch
		mov ebx,bits
		mov edx,ox
		imul eax,ecx
		imul edx,ebx
		mov edi,dest
		shr edx,3
		add esi,eax
		add esi,edx
		mov eax,y
		mov ecx,dpitch
		mov ebx,bits
		mov edx,x
		imul eax,ecx
		imul edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp cl,15
		jl bl_exit
		je bl_b15
		cmp	cl,16
		je bl_b16
		cmp cl,24
		je bl_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle bl_exit
		cmp ecx,0x40
		jle bl_b25_32
		cmp ecx,0x80
		jle bl_b50_32
		cmp ecx,0xC0
		jle bl_b75_32
		
// 100% blend
bl_b100y32:
		mov ecx,w
		rep movsd
		add esi,spitch
		add edi,dpitch
		dec h
		jne bl_b100y32
		jmp bl_exit
		
// 25% blend
bl_b25_32:
bl_b25y32:
		mov ecx,w
bl_b25x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		dec ecx
		stosd
		jnz bl_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b25y32
		jmp bl_exit

// 50% blend
bl_b50_32:
bl_b50y32:
		mov ecx,w
bl_b50x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7F7F7F7F
		and edx,0x7F7F7F7F
		add eax,edx
		dec ecx
		stosd
		jnz bl_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b50y32
		jmp bl_exit
			
// 75% blend
bl_b75_32:
bl_b75y32:
		mov ecx,w
bl_b75x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		dec ecx
		stosd
		jnz bl_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b75y32
		jmp bl_exit
		
// 24-bit
bl_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle bl_exit
		cmp ecx,0x40
		jle bl_b25_24
		cmp ecx,0x80
		jle bl_b50_24
		cmp ecx,0xC0
		jle bl_b75_24
			
// 100% blend
bl_b100y24:
		mov ecx,w
		rep movsw
		mov ecx,w
		rep movsb
		add esi,spitch
		add edi,dpitch
		dec h
		jne bl_b100y24
		jmp bl_exit

// 25% blend
bl_b25_24:
bl_b25y24:
		mov ecx,w
bl_b25x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		dec esi
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz bl_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b25y24
		jmp bl_exit

// 50% blend
bl_b50_24:
bl_b50y24:
		mov ecx,w
bl_b50x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7F7F7F7F
		and edx,0x7F7F7F7F
		dec esi
		add eax,edx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz bl_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b50y24
		jmp bl_exit

// 75% blend
bl_b75_24:
bl_b75y24:
		mov ecx,w
bl_b75x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		dec esi
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz bl_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b75y24
		jmp bl_exit

// 15-bit
bl_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle bl_exit
		cmp ecx,0x40
		jle bl_b25_15
		cmp ecx,0x80
		jle bl_b50_15
		cmp ecx,0xC0
		jle bl_b75_15
		
// 100% blend
bl_b100y15:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne bl_b100y15
		jmp bl_exit
		
// 25% blend
bl_b25_15:
bl_b25y15:
		mov ecx,w
bl_b25x15:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		and ebx,0x7BEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz bl_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b25y15
		jmp bl_exit

// 50% blend
bl_b50_15:
bl_b50y15:
		mov ecx,w
bl_b50x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz bl_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b50y15
		jmp bl_exit

// 75% blend
bl_b75_15:
bl_b75y15:
		mov ecx,w
bl_b75x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		add eax,edx
		and ebx,0x7BEF
		add eax,ebx
		dec ecx
		stosw
		jnz bl_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b75y15
		jmp	bl_exit

// 16-bit
bl_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle bl_exit
		cmp ecx,0x40
		jle bl_b25_16
		cmp ecx,0x80
		jle bl_b50_16
		cmp ecx,0xC0
		jle bl_b75_16
		
// 100% blend
bl_b100y16:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne bl_b100y16
		jmp bl_exit
		
// 25% blend
bl_b25_16:
bl_b25y16:
		mov ecx,w
bl_b25x16:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		and ebx,0x7BEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz bl_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b25y16
		jmp bl_exit

// 50% blend
bl_b50_16:
bl_b50y16:
		mov ecx,w
bl_b50x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz bl_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b50y16
		jmp bl_exit

// 75% blend
bl_b75_16:
bl_b75y16:
		mov ecx,w
bl_b75x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		add eax,edx
		and ebx,0x7BEF
		add eax,ebx
		dec ecx
		stosw
		jnz bl_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz bl_b75y16
		
bl_exit:
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_blendt(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch)
{
	__asm{
		push esi
		push edi
		push ebx
		mov esi,src
		mov	edx,oy
		mov eax,oy
		imul	edx,maskpitch
		mov ecx,spitch
		add edx,ox
		add	maskbits,edx
		mov ebx,bits
		mov	edx,ox
		imul eax,ecx
		imul edx,ebx
		mov edi,dest
		shr edx,3
		add esi,eax
		add esi,edx
		mov eax,y
		mov ecx,dpitch
		mov ebx,bits
		mov edx,x
		imul eax,ecx
		imul edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp cl,15
		jl	blt_exit
		je	blt_b15
		cmp	cl,16
		je	blt_b16
		cmp cl,24
		je	blt_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle blt_exit
		cmp ecx,0x40
		jle blt_b25_32
		cmp ecx,0x80
		jle blt_b50_32
		cmp ecx,0xC0
		jle blt_b75_32
		
// 100% blend
blt_b100y32:
		mov ecx,w
		rep	movsd
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne blt_b100y32
		jmp blt_exit
		
// 25% blend
blt_b25_32:
blt_b25y32:
		mov ecx,w
blt_b25x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		dec ecx
		stosd
		jnz blt_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b25y32
		jmp blt_exit

// 50% blend
blt_b50_32:
blt_b50y32:
		mov ecx,w
blt_b50x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7F7F7F7F
		and edx,0x7F7F7F7F
		add eax,edx
		dec ecx
		stosd
		jnz blt_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b50y32
		jmp blt_exit
			
// 75% blend
blt_b75_32:
blt_b75y32:
		mov ecx,w
blt_b75x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		dec ecx
		stosd
		jnz blt_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b75y32
		jmp blt_exit
		
// 24-bit
blt_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blt_exit
		cmp ecx,0x40
		jle blt_b25_24
		cmp ecx,0x80
		jle blt_b50_24
		cmp ecx,0xC0
		jle blt_b75_24
			
// 100% blend
blt_b100y24:
		mov ecx,w
		rep movsw
		mov ecx,w
		rep movsb
		add esi,spitch
		add edi,dpitch
		dec h
		jne blt_b100y24
		jmp blt_exit

// 25% blend
blt_b25_24:
blt_b25y24:
		mov ecx,w
blt_b25x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		dec esi
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blt_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b25y24
		jmp blt_exit

// 50% blend
blt_b50_24:
blt_b50y24:
		mov ecx,w
blt_b50x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7F7F7F7F
		and edx,0x7F7F7F7F
		dec esi
		add eax,edx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blt_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b50y24
		jmp blt_exit

// 75% blend
blt_b75_24:
blt_b75y24:
		mov ecx,w
blt_b75x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		dec esi
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blt_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b75y24
		jmp blt_exit
		
// 15-bit
blt_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blt_exit
		cmp ecx,0x40
		jle blt_b25_15
		cmp ecx,0x80
		jle blt_b50_15
		cmp ecx,0xC0
		jle blt_b75_15
		
// 100% blend
blt_b100y15:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne blt_b100y15
		jmp blt_exit
		
// 25% blend
blt_b25_15:
blt_b25y15:
		mov ecx,w
blt_b25x15:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x1CE7
		shr ebx,1
		and edx,0x1CE7
		and ebx,0x3DEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz blt_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b25y15
		jmp blt_exit

// 50% blend
blt_b50_15:
blt_b50y15:
		mov ecx,w
blt_b50x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x3DEF
		and edx,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz blt_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b50y15
		jmp blt_exit

// 75% blend
blt_b75_15:
blt_b75y15:
		mov ecx,w
blt_b75x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x1CE7
		shr ebx,1
		and edx,0x1CE7
		add eax,edx
		and ebx,0x3DEF
		add eax,ebx
		dec ecx
		stosw
		jnz blt_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b75y15
		jmp	blt_exit

// 16-bit
blt_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blt_exit
		cmp ecx,0x40
		jle blt_b25_16
		cmp ecx,0x80
		jle blt_b50_16
		cmp ecx,0xC0
		jle blt_b75_16
		
// 100% blend
blt_b100y16:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne blt_b100y16
		jmp blt_exit
		
// 25% blend
blt_b25_16:
blt_b25y16:
		mov ecx,w
blt_b25x16:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		mov ebx,edx
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		and ebx,0x7BEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz blt_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b25y16
		jmp blt_exit

// 50% blend
blt_b50_16:
blt_b50y16:
		mov ecx,w
blt_b50x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz blt_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b50y16
		jmp blt_exit

// 75% blend
blt_b75_16:
blt_b75y16:
		mov ecx,w
blt_b75x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		add eax,edx
		and ebx,0x7BEF
		add eax,ebx
		dec ecx
		stosw
		jnz blt_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blt_b75y16
		
blt_exit:
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_blendck(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey)
{
	__asm{
		push esi
		push edi
		push ebx
		mov esi,src
		mov eax,oy
		mov ecx,spitch
		mov ebx,bits
		mov edx,ox
		imul eax,ecx
		imul edx,ebx
		mov edi,dest
		shr edx,3
		add esi,eax
		add esi,edx
		mov eax,y
		mov ecx,dpitch
		mov ebx,bits
		mov edx,x
		imul eax,ecx
		imul edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp	cl,15
		jl	blck_exit
		je	blck_b15
		cmp	cl,16
		je	blck_b16
		cmp	cl,24
		je	blck_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle blck_exit
		cmp ecx,0x40
		jle blck_b25_32
		cmp ecx,0x80
		jle blck_b50_32
		cmp ecx,0xC0
		jle blck_b75_32
		jmp	blck_b100_32
		
// 100% blend
blck_b100s32:
		add	edi,4
		dec	ecx
		jnz	blck_b100x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y32
		jmp blck_exit
blck_b100_32:
		mov	edx,colorkey
blck_b100y32:
		mov ecx,w
blck_b100x32:
		lodsd
		cmp	eax,edx
		je	blck_b100s32
		movsd
		dec	ecx
		jnz	blck_b100x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y32
		jmp blck_exit
		
// 25% blend
blck_b25s32:
		add	edi,4
		dec	ecx
		jnz	blck_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne	blck_b25y32
		jmp blck_exit
blck_b25_32:
blck_b25y32:
		mov ecx,w
blck_b25x32:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		cmp	eax,colorkey
		mov ebx,edx
		je	blck_b25s32
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		dec ecx
		stosd
		jnz blck_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b25y32
		jmp blck_exit

// 50% blend
blck_b50s32:
		add	edi,4
		dec	ecx
		jnz	blck_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b50y32
		jmp blck_exit
blck_b50_32:
blck_b50y32:
		mov ecx,w
blck_b50x32:
		lodsd         // eax = source
		cmp	eax,colorkey
		mov edx,[edi] // edx = dest
		je	blck_b50s32
		shr eax,1
		shr edx,1
		and eax,0x7F7F7F7F
		and edx,0x7F7F7F7F
		add eax,edx
		dec ecx
		stosd
		jnz blck_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b50y32
		jmp blck_exit
			
// 75% blend
blck_b75s32:
		add	edi,4
		dec	ecx
		jnz	blck_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b75y32
		jmp blck_exit
blck_b75_32:
blck_b75y32:
		mov ecx,w
blck_b75x32:
		lodsd         // eax = source
		cmp	eax,colorkey
		mov edx,[edi] // edx = dest
		je	blck_b75s32
		mov ebx,eax
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		dec ecx
		stosd
		jnz blck_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b75y32
		jmp blck_exit
		
// 24-bit
blck_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blck_exit
		cmp ecx,0x40
		jle blck_b25_24
		cmp ecx,0x80
		jle blck_b50_24
		cmp ecx,0xC0
		jle blck_b75_24
		jmp	blck_b100_24
			
// 100% blend
blck_b100s24:
		dec	esi
		add	edi,3
		dec	ecx
		jnz	blck_b100x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y24
		jmp blck_exit
blck_b100_24:
		mov	edx,colorkey
blck_b100y24:
		mov ecx,w
blck_b100x24:
		lodsd
		and	eax,0xFFFFFF
		cmp	eax,edx
		je	blck_b100s24
		stosw
		dec	esi
		shr eax,16
		dec ecx
		stosb
		dec	ecx
		jnz	blck_b100x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y24
		jmp blck_exit

// 25% blend
blck_b25s24:
		add	edi,3
		dec	ecx
		jnz	blck_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b25y24
		jmp blck_exit
blck_b25_24:
blck_b25y24:
		mov ecx,w
blck_b25x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		and	eax,0xFFFFFF
		dec esi
		cmp	eax,colorkey
		mov ebx,edx
		je	blck_b25s24
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blck_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b25y24
		jmp blck_exit

// 50% blend
blck_b50s24:
		add	edi,3
		dec	ecx
		jnz	blck_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b50y24
		jmp blck_exit
blck_b50_24:
blck_b50y24:
		mov ecx,w
blck_b50x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		and	eax,0xFFFFFF
		shr edx,1
		cmp	eax,colorkey
		je	blck_b50s24
		shr eax,1
		and edx,0x7F7F7F7F
		and eax,0x7F7F7F7F
		dec esi
		add eax,edx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blck_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b50y24
		jmp blck_exit

// 75% blend
blck_b75s24:
		add	edi,3
		dec	ecx
		jnz	blck_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b50y24
		jmp blck_exit
blck_b75_24:
blck_b75y24:
		mov ecx,w
blck_b75x24:
		lodsd         // eax = source
		mov edx,[edi] // edx = dest
		and	eax,0xFFFFFF
		dec	esi
		mov ebx,eax
		cmp	eax,colorkey
		je	blck_b75s24
		shr eax,2
		shr edx,2
		and eax,0x3F3F3F3F
		shr ebx,1
		and edx,0x3F3F3F3F
		add eax,edx
		and ebx,0x7F7F7F7F
		add eax,ebx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz blck_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b75y24
		jmp blck_exit
		
// 15-bit
blck_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blck_exit
		cmp ecx,0x40
		jle blck_b25_15
		cmp ecx,0x80
		jle blck_b50_15
		cmp ecx,0xC0
		jle blck_b75_15
		jmp	blck_b100_15

// 100% blend
blck_b100s15:
		add	edi,2
		dec	ecx
		jnz	blck_b100x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y15
		jmp blck_exit
blck_b100_15:
		mov	edx,colorkey
blck_b100y15:
		mov ecx,w
blck_b100x15:
		lodsw
		cmp	ax,dx
		je	blck_b100s15
		dec	ecx
		stosw
		jnz	blck_b100x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y15
		jmp blck_exit
		
// 25% blend
blck_b25s15:
		add	edi,2
		dec	ecx
		jnz	blck_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b25y15
		jmp blck_exit
blck_b25_15:
blck_b25y15:
		mov ecx,w
blck_b25x15:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		cmp	ax,word ptr colorkey
		mov ebx,edx
		je	blck_b25s15
		shr eax,2
		shr edx,2
		and eax,0x1CE7
		shr ebx,1
		and edx,0x1CE7
		and ebx,0x3DEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz blck_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b25y15
		jmp blck_exit

// 50% blend
blck_b50s15:
		add	edi,2
		dec	ecx
		jnz	blck_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b50y15
		jmp blck_exit
blck_b50_15:
blck_b50y15:
		mov ecx,w
blck_b50x15:
		lodsw         // eax = source
		cmp	ax,word ptr colorkey
		mov dx,[edi]  // edx = dest
		je	blck_b50s15
		shr eax,1
		shr edx,1
		and eax,0x3DEF
		and edx,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz blck_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b50y15
		jmp blck_exit

// 75% blend
blck_b75s15:
		add	edi,2
		dec	ecx
		jnz	blck_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b75y15
		jmp blck_exit
blck_b75_15:
blck_b75y15:
		mov ecx,w
blck_b75x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		cmp	ax,word ptr colorkey
		mov ebx,eax
		je	blck_b75s15
		shr eax,2
		shr edx,2
		and eax,0x1CE7
		shr ebx,1
		and edx,0x1CE7
		add eax,edx
		and ebx,0x3DEF
		add eax,ebx
		dec ecx
		stosw
		jnz blck_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b75y15
		jmp	blck_exit

// 16-bit
blck_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle blck_exit
		cmp ecx,0x40
		jle blck_b25_16
		cmp ecx,0x80
		jle blck_b50_16
		cmp ecx,0xC0
		jle blck_b75_16
		jmp	blck_b100_16

// 100% blend
blck_b100s16:
		add	edi,2
		dec	ecx
		jnz	blck_b100x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y16
		jmp blck_exit
blck_b100_16:
		mov	edx,colorkey
blck_b100y16:
		mov ecx,w
blck_b100x16:
		lodsw
		cmp	ax,dx
		je	blck_b100s16
		dec	ecx
		stosw
		jnz	blck_b100x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b100y16
		jmp blck_exit
		
// 25% blend
blck_b25s16:
		add	edi,2
		dec	ecx
		jnz	blck_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b25y16
		jmp blck_exit
blck_b25_16:
blck_b25y16:
		mov ecx,w
blck_b25x16:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		cmp	ax,word ptr colorkey
		mov ebx,edx
		je	blck_b25s16
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		and ebx,0x7BEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz blck_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b25y16
		jmp blck_exit

// 50% blend
blck_b50s16:
		add	edi,2
		dec	ecx
		jnz	blck_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b50y16
		jmp blck_exit
blck_b50_16:
blck_b50y16:
		mov ecx,w
blck_b50x16:
		lodsw         // eax = source
		cmp	ax,word ptr colorkey
		mov dx,[edi]  // edx = dest
		je	blck_b50s16
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz blck_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b50y16
		jmp blck_exit

// 75% blend
blck_b75s16:
		add	edi,2
		dec	ecx
		jnz	blck_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne blck_b75y16
		jmp blck_exit
blck_b75_16:
blck_b75y16:
		mov ecx,w
blck_b75x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		cmp	ax,word ptr colorkey
		mov ebx,eax
		je	blck_b75s16
		shr eax,2
		shr edx,2
		and eax,0x39E7
		shr ebx,1
		and edx,0x39E7
		add eax,edx
		and ebx,0x7BEF
		add eax,ebx
		dec ecx
		stosw
		jnz blck_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz blck_b75y16

blck_exit:
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_blur(void *tmp,void *mask,int w,int h,int pitch,int amount)
{
	__asm{
		push esi
		push edi
		push ebx
		sub w,2
		sub h,2
		mov eax,w
		push eax
		mov edx,h
		sub eax,pitch
		neg eax
		push edx
		push eax
blur:
		mov edi,tmp
		mov esi,mask
		mov ecx,[esp+4]
		mov eax,pitch
		add ecx,2
		imul ecx,eax
		shr ecx,2
		rep movsd
		mov esi,tmp
		mov edi,mask
		mov ebx,eax
		inc esi
		inc edi
		mov ecx,esi   // ecx = line
		add esi,ebx   // esi = line+1
		add edi,ebx
		add ebx,esi   // ebp = line+2
		push ebx
blur_y:
		mov eax,w
		mov [esp+12],eax
		xor eax,eax
		push ebp
		mov ebp,[esp+4]
blur_x:
		xor edx,edx
		xor ebx,ebx
		mov al,[ecx-1]
		mov dl,[ecx+1]
		mov bl,[ebp-1]
		add ax,dx
		mov dl,[ebp+1]
		add ax,bx
		add ax,dx
		mov dl,[ecx]
		mov bl,[esi-1]
		add dx,bx
		mov bl,[esi+1]
		add dx,bx
		mov bl,[ebp]
		add dx,bx
		inc ebp
		shl dx,1
		add ax,dx
		mov bl,[esi]
		shl bx,2
		inc ecx
		add ax,bx
		inc esi
		shr ax,4
		stosb
		dec dword ptr [esp+16]
		jnz blur_x
		add ebp,[esp+8]
		add edi,[esp+8]
		mov [esp+4],ebp
		add esi,[esp+8]
		add ecx,[esp+8]
		pop ebp
		dec dword ptr [esp+8]
		jnz blur_y
		add esp,4
		mov eax,h
		mov dword ptr [esp+4],eax
		dec amount
		jnz blur
		add esp,12
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_opacity(void *mask,int w,int h,int pitch,int amount)
{
	__asm{
		push esi
		push edi
		push ebx
		mov esi,mask
		mov ecx,w
		mov edx,amount
		sub pitch,ecx
y:
		shr ecx,2
x:
		lodsd
		mov ebx,eax
		and eax,0xFF  // eax = ......aa
		mul dl        // eax = ....AAAA
		mov dh,ah
		shl eax,8     // eax = ..AAAA..
		mov al,bh     // eax = ..AAAAbb
		mul dl        // eax = ..AAbbbb
		shr ebx,16    // ebx = ....ddcc
		shl eax,8     // eax = AABBBB..
		mov al,bl     // eax = AABBBBcc
		mul dl        // eax = AABBCCCC
		shl eax,8     // eax = BBCCCC..
		mov al,bh     // eax = BBCCCCdd
		mul dl        // eax = BBCCDDDD
		bswap eax     // eax = DDDDCCBB
		shl eax,8     // eax = DDCCBB..
		mov al,dh     // eax = DDCCBBAA
		mov [esi-4],eax
		dec ecx
		jnz x
		add esi,pitch
		dec h
		mov ecx,w
		jnz y
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT a_fill(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits)
{
	__asm{
		push esi
		push edi
		push ebx
		mov edi,surface
		mov eax,y
		mov ecx,pitch
		mov ebx,bits
		mov edx,x
		imul eax,ecx
		imul edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp cl,15
		mov esi,edi
		jl f_exit
		je f_b15
		cmp	cl,16
		je f_b16
		cmp cl,24
		je f_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub pitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle f_exit
		cmp ecx,0x40
		jle f_b25_32
		cmp ecx,0x80
		jle f_b50_32
		cmp ecx,0xC0
		jle f_b75_32
		
// 100% blend
		mov	eax,color
f_b100y32:
		mov	ecx,w
		rep	stosd
		add	edi,pitch
		dec	h
		jne f_b100y32
		jmp f_exit
		
// 25% blend
f_b25_32:
		mov	edx,color
		shr	edx,2
		and	edx,0x3F3F3F3F
f_b25y32:
		mov ecx,w
f_b25x32:
		lodsd
		mov	ebx,eax
		shr	eax,2
		shr	ebx,1
		and	eax,0x3F3F3F3F
		and	ebx,0x7F7F7F7F
		add	eax,edx
		add	eax,ebx
		dec	ecx
		stosd
		jnz f_b25x32
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b25y32
		jmp f_exit

// 50% blend
f_b50_32:
		mov edx,color
		shr edx,1
		and	edx,0x7F7F7F7F
f_b50y32:
		mov ecx,w
f_b50x32:
		lodsd
		shr eax,1
		and eax,0x7F7F7F7F
		add eax,edx
		dec ecx
		stosd
		jnz f_b50x32
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b50y32
		jmp f_exit
			
// 75% blend
f_b75_32:
		mov	edx,color
		mov ebx,edx
		shr	edx,2
		shr ebx,1
		and	edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add edx,ebx
f_b75y32:
		mov ecx,w
f_b75x32:
		lodsd
		shr eax,2
		and eax,0x3F3F3F3F
		add eax,edx
		dec ecx
		stosd
		jnz f_b75x32
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b75y32
		jmp f_exit
		
// 24-bit
f_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub pitch,eax
		cmp ecx,0x00
		jle f_exit
		cmp ecx,0x40
		jle f_b25_24
		cmp ecx,0x80
		jle f_b50_24
		cmp ecx,0xC0
		jle f_b75_24
			
// 100% blend
		mov	edx,color
f_b100y24:
		mov	ecx,w
f_b100x24:
		mov	eax,edx
		stosw
		shr	eax,16
		dec	ecx
		stosb
		jnz	f_b100x24
		add edi,pitch
		dec h
		jnz f_b100y24
		jmp f_exit

// 25% blend
f_b25_24:
		mov	edx,color
		shr	edx,2
		and	edx,0x3F3F3F3F
f_b25y24:
		mov ecx,w
f_b25x24:
		lodsd
		dec esi
		mov ebx,eax
		shr eax,2
		shr ebx,1
		and eax,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz f_b25x24
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b25y24
		jmp f_exit

// 50% blend
f_b50_24:
		mov	edx,color
		shr	edx,1
		and	edx,0x7F7F7F7F
f_b50y24:
		mov ecx,w
f_b50x24:
		lodsd
		shr eax,1
		dec esi
		and eax,0x7F7F7F7F
		add eax,edx
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz f_b50x24
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b50y24
		jmp f_exit

// 75% blend
f_b75_24:
		mov	edx,color
		mov ebx,edx
		shr	edx,2
		shr ebx,1
		and	edx,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add edx,ebx
f_b75y24:
		mov ecx,w
f_b75x24:
		lodsd
		mov ebx,eax
		shr eax,2
		shr ebx,1
		and eax,0x3F3F3F3F
		and ebx,0x7F7F7F7F
		add eax,edx
		add eax,ebx
		dec esi
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz f_b75x24
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b75y24
		jmp f_exit

// 15-bit
f_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub pitch,eax
		cmp ecx,0x00
		jle f_exit
		cmp ecx,0x40
		jle f_b25_15
		cmp ecx,0x80
		jle f_b50_15
		cmp ecx,0xC0
		jle f_b75_15
		
// 100% blend
		mov	eax,color
f_b100y15:
		mov ecx,w
		rep stosw
		add edi,pitch
		dec h
		jne f_b100y15
		jmp f_exit
		
// 25% blend
f_b25_15:
		mov	edx,color
		shr	edx,2
		and	edx,0x1CE7
f_b25y15:
		mov ecx,w
f_b25x15:
		lodsw
		mov ebx,eax
		shr eax,2
		shr ebx,1
		and eax,0x1CE7
		and ebx,0x3DEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz f_b25x15
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b25y15
		jmp f_exit

// 50% blend
f_b50_15:
		mov	edx,color
		shr	edx,1
		and	edx,0x3DEF
f_b50y15:
		mov ecx,w
f_b50x15:
		lodsw         // eax = source
		shr eax,1
		and eax,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz f_b50x15
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b50y15
		jmp f_exit

// 75% blend
f_b75_15:
		mov	edx,color
		mov ebx,edx
		shr	edx,2
		shr ebx,1
		and	edx,0x1CE7
		and ebx,0x3DEF
		add edx,ebx
f_b75y15:
		mov ecx,w
f_b75x15:
		lodsw
		shr eax,2
		and eax,0x1CE7
		add eax,edx
		dec ecx
		stosw
		jnz f_b75x15
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b75y15
		jmp	f_exit

// 16-bit
f_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub pitch,eax
		cmp ecx,0x00
		jle f_exit
		cmp ecx,0x40
		jle f_b25_16
		cmp ecx,0x80
		jle f_b50_16
		cmp ecx,0xC0
		jle f_b75_16
		
// 100% blend
		mov	eax,color
f_b100y16:
		mov ecx,w
		rep stosw
		add edi,pitch
		dec h
		jne f_b100y16
		jmp f_exit
		
// 25% blend
f_b25_16:
		mov	edx,color
		shr	edx,2
		and	edx,0x39E7
f_b25y16:
		mov ecx,w
f_b25x16:
		lodsw
		mov ebx,eax
		shr eax,2
		shr ebx,1
		and eax,0x39E7
		and ebx,0x7BEF
		add eax,edx
		add eax,ebx
		dec ecx
		stosw
		jnz f_b25x16
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b25y16
		jmp f_exit

// 50% blend
f_b50_16:
		mov	edx,color
		shr	edx,1
		and	edx,0x39E7
f_b50y16:
		mov ecx,w
f_b50x16:
		lodsw
		shr eax,1
		and eax,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz f_b50x16
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b50y16
		jmp f_exit

// 75% blend
f_b75_16:
		mov	edx,color
		mov ebx,edx
		shr	edx,2
		shr ebx,1
		and	edx,0x39E7
		and ebx,0x7BEF
		add edx,ebx
f_b75y16:
		mov ecx,w
f_b75x16:
		lodsw
		shr eax,2
		and eax,0x39E7
		add eax,edx
		dec ecx
		stosw
		jnz f_b75x16
		add esi,pitch
		add edi,pitch
		dec h
		jnz f_b75y16
		
f_exit:
		pop ebx
		pop edi
		pop esi
	};
}
//---------------------------------------------------------------------------
