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
#include "MTDisplayMMX.h"
#include "MTDisplay1.h"
//---------------------------------------------------------------------------
void MTCT mmx_shade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits)
{
	__asm{
		push esi
		push edi
		push ebx
		mov edi,surface
		mov esi,mask
		mov ecx,bits
		cmp cl,15
		jl msh_exit
		je msh_b15
		cmp cl,16
		je msh_b16
		cmp cl,24
		je msh_b24
			
// 32-bit
		mov eax,w
		mov edx,eax
		shl eax,2
		sub mpitch,edx
		sub spitch,eax
msh_y32:
		mov ecx,w
msh_x32:
		xor eax,eax
		lodsb
		test al,al
		jz msh_ok32
		neg al
		mov ah,al
		mov edx,eax
		shl eax,16
		mov ax,dx
		pxor mm3,mm3
		movd mm1,eax              // mm1 = ........i1i1i1i1
		movd mm2,[edi]            // mm2 = ..........bbggrr
		punpcklbw mm1,mm3         // mm1 = ..i1..i1..i1..i1
		punpcklbw mm2,mm3         // mm2 = ......bb..gg..rr
		pmullw mm2,mm1            // mm2 = ....BBBBGGGGRRRR
		psrlw mm2,8               // mm2 = ......BB..GG..RR
		packuswb mm2,mm2          // mm2 = ..........BBGGRR
		movd eax,mm2
		dec ecx
		stosd
		jnz msh_x32
		jmp msh_ok32b
msh_ok32:
		add edi,4
		dec cx
		jnz msh_x32
msh_ok32b:
		add edi,spitch
		add esi,mpitch
		dec h
		jnz msh_y32
		jmp msh_exit
		
// 24-bit
msh_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		sub mpitch,edx
		sub spitch,eax
msh_y24:
		mov ecx,w
msh_x24:
		xor eax,eax
		lodsb
		test al,al
		jz msh_ok24
		neg al
		mov ah,al
		mov edx,eax
		shl eax,16
		mov ax,dx
		pxor mm3,mm3
		movd mm1,eax              // mm1 = ........i1i1i1i1
		movd mm2,[edi]            // mm2 = ..........bbggrr
		punpcklbw mm1,mm3         // mm1 = ..i1..i1..i1..i1
		punpcklbw mm2,mm3         // mm2 = ......bb..gg..rr
		pmullw mm2,mm1            // mm2 = ....BBBBGGGGRRRR
		psrlw mm2,8               // mm2 = ......BB..GG..RR
		packuswb mm2,mm2          // mm2 = ..........BBGGRR
		movd eax,mm2
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz msh_x24
		jmp msh_ok24b
msh_ok24:
		add edi,3
		dec cx
		jnz msh_x24
msh_ok24b:
		add edi,spitch
		add esi,mpitch
		dec h
		jnz msh_y24
		jmp msh_exit
		
// 15-bit
msh_b15:
		mov eax,w
		mov edx,eax
		shl eax,1
		sub mpitch,edx
		sub spitch,eax
msh_y15:
		mov ecx,w
msh_x15:
		xor eax,eax
		mov dl,[esi]   // dl = intensity
		mov ax,[edi]   // eax = ....bxxr
		shr dl,3
		mov ebx,eax    // eax = ebx = ....bxxr
		jz msh_ok15
		and eax,0x7C1F // eax = ....bbrr
		xor dl,0x1F
		and ebx,0x03E0 // ebx = .....gg.
		imul eax,dl    // eax = ...BBxRR
		imul ebx,dl    // ebx = ....GGG.
		and eax,0xF83E0
		and ebx,0x7C00
		or eax,ebx
		shr eax,5
msh_ok15:
		stosw
		inc esi
		dec ecx
		jnz msh_x15
		add edi,spitch
		add esi,mpitch
		dec h
		jnz msh_y15
		jmp msh_exit
		
// 16-bit
msh_b16:
		mov eax,w
		mov edx,eax
		shl eax,1
		sub mpitch,edx
		sub spitch,eax
msh_y16:
		mov ecx,w
msh_x16:
		xor eax,eax
		mov dl,[esi]   // dl = intensity
		mov ax,[edi]   // eax = ....bxxr
		shr dl,3
		mov ebx,eax    // eax = ebx = ....bxxr
		jz msh_ok16
		and eax,0xF81F // eax = ....bbrr
		xor dl,0x1F
		and ebx,0x07E0 // ebx = .....gg.
		imul eax,dl    // eax = ...BBxRR
		imul ebx,dl    // ebx = ....GGG.
		and eax,0x1F03E0
		and ebx,0xFC00
		or eax,ebx
		shr eax,5
msh_ok16:
		stosw
		inc esi
		dec ecx
		jnz msh_x16
		add edi,spitch
		add esi,mpitch
		dec h
		jnz msh_y16
		
msh_exit:
		emms
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT mmx_sshade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits)
{
	__asm{
	};
}

void MTCT mmx_blend(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits)
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
		imul	eax,ecx
		imul	edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp cl,15
		jl	mbl_exit
		je	mbl_b15
		cmp	cl,16
		je	mbl_b16
		cmp cl,24
		je	mbl_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_32
		cmp ecx,0x80
		jle mbl_b50_32
		cmp ecx,0xC0
		jle mbl_b75_32
		
// 100% blend
mbl_b100y32:
		mov ecx,w
		rep movsd
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y32
		jmp mbl_exit
		
// 25% blend
mbl_b25_32:
		sub edi,4
mbl_b25y32:
		mov ecx,w
mbl_b25x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y32
		jmp mbl_exit
		
// 50% blend
mbl_b50_32:
		sub edi,4
mbl_b50y32:
		mov ecx,w
mbl_b50x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y32
		jmp mbl_exit

// 75% blend
mbl_b75_32:
		sub edi,4
mbl_b75y32:
		mov ecx,w
mbl_b75x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y32
		jmp mbl_exit

// 24-bit
mbl_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_24
		cmp ecx,0x80
		jle mbl_b50_24
		cmp ecx,0xC0
		jle mbl_b75_24

// 100% blend
mbl_b100y24:
		mov ecx,w
		rep movsw
		mov ecx,w
		rep movsb
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y24
		jmp mbl_exit

// 25% blend
mbl_b25_24:
mbl_b25y24:
		mov ecx,w
mbl_b25x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y24
		jmp mbl_exit

// 50% blend
mbl_b50_24:
mbl_b50y24:
		mov ecx,w
mbl_b50x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y24
		jmp mbl_exit

// 75% blend
mbl_b75_24:
mbl_b75y24:
		mov ecx,w
mbl_b75x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y24
		jmp mbl_exit

// 15-bit
mbl_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_15
		cmp ecx,0x80
		jle mbl_b50_15
		cmp ecx,0xC0
		jle mbl_b75_15

// 100% blend
mbl_b100y15:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y15
		jmp mbl_exit

// 25% blend
mbl_b25_15:
mbl_b25y15:
		mov ecx,w
mbl_b25x15:
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
		jnz mbl_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y15
		jmp mbl_exit

// 50% blend
mbl_b50_15:
mbl_b50y15:
		mov ecx,w
mbl_b50x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x3DEF
		and edx,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz mbl_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y15
		jmp mbl_exit

// 75% blend
mbl_b75_15:
mbl_b75y15:
		mov ecx,w
mbl_b75x15:
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
		jnz mbl_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y15
		jmp	mbl_exit

// 16-bit
mbl_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_16
		cmp ecx,0x80
		jle mbl_b50_16
		cmp ecx,0xC0
		jle mbl_b75_16

// 100% blend
mbl_b100y16:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y16
		jmp mbl_exit

// 25% blend
mbl_b25_16:
mbl_b25y16:
		mov ecx,w
mbl_b25x16:
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
		jnz mbl_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y16
		jmp mbl_exit

// 50% blend
mbl_b50_16:
mbl_b50y16:
		mov ecx,w
mbl_b50x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz mbl_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y16
		jmp mbl_exit

// 75% blend
mbl_b75_16:
mbl_b75y16:
		mov ecx,w
mbl_b75x16:
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
		jnz mbl_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y16
		
mbl_exit:
		emms
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT mmx_blendt(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch)
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
		mov	eax,w
		add edi,edx
		sub	maskpitch,eax
		cmp	cl,15
		jl	mblt_exit
		je	mblt_b15
		cmp	cl,16
		je	mblt_b16
		cmp	cl,24
		je	mblt_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle mblt_exit
		cmp ecx,0x40
		jle mblt_b25_32
		cmp ecx,0x80
		jle mblt_b50_32
		cmp ecx,0xC0
		jle mblt_b75_32
		jmp	mblt_b100_32

// 100% blend
mblt_b100s32:
		add	esi,4
		add	edi,4
		inc	edx
		dec	ecx
		jnz	mblt_b100x32
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y32
		jmp mblt_exit
mblt_b100_32:
		mov edx,maskbits
mblt_b100y32:
		mov ecx,w
mblt_b100x32:
		cmp	byte ptr [edx],0x80
		ja	mblt_b100s32
		movsd
		inc	edx
		dec	ecx
		jnz	mblt_b100x32
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y32
		jmp mblt_exit
		
// 25% blend
mblt_b25s32:
		add	esi,4
		add	edi,4
		inc	edx
		dec	ecx
		jnz	mblt_b25x32
		add	edx,maskpitch
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblt_b25y32
		jmp	mblt_exit
mblt_b25_32:
		mov	edx,maskbits
		sub edi,4
mblt_b25y32:
		mov ecx,w
mblt_b25x32:
		cmp	byte ptr [edx],80
		ja	mblt_b25s32
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		inc	edx
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz	mblt_b25x32
		add	edx,maskpitch
		add	esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b25y32
		jmp mblt_exit
		
// 50% blend
mblt_b50s32:
		add	esi,4
		add	edi,4
		inc	edx
		dec	ecx
		jnz	mblt_b50x32
		add	edx,maskpitch
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblt_b50y32
		jmp	mblt_exit
mblt_b50_32:
		mov	edx,maskbits
		sub edi,4
mblt_b50y32:
		mov ecx,w
mblt_b50x32:
		cmp	byte ptr [edx],80
		ja	mblt_b50s32
		pxor	mm3,mm3
		add	edi,4
		movd	mm1,[esi]
		movd	mm2,[edi]
		add esi,4
		punpcklbw	mm2,mm3
		punpcklbw	mm1,mm3
		psrlw	mm1,1
		psrlw	mm2,1
		inc	edx
		paddw	mm1,mm2
		packuswb	mm1,mm1
		dec	ecx
		movd	[edi],mm1
		jnz	mblt_b50x32
		add	edx,maskpitch
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblt_b50y32
		jmp	mblt_exit

// 75% blend
mblt_b75s32:
		add	esi,4
		add	edi,4
		inc	edx
		dec	ecx
		jnz	mblt_b75x32
		add	edx,maskpitch
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblt_b75y32
		jmp	mblt_exit
mblt_b75_32:
		mov	edx,maskbits
		sub edi,4
mblt_b75y32:
		mov ecx,w
mblt_b75x32:
		cmp	byte ptr [edx],0x80
		ja	mblt_b75s32
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		inc	edx
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mblt_b75x32
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b75y32
		jmp mblt_exit

// 24-bit
mblt_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblt_exit
		cmp ecx,0x40
		jle mblt_b25_24
		cmp ecx,0x80
		jle mblt_b50_24
		cmp ecx,0xC0
		jle mblt_b75_24
		jmp	mblt_b100_24

// 100% blend
mblt_b100s24:
		add	esi,3
		add	edi,3
		inc	edx
		dec	ecx
		jnz	mblt_b100x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y24
		jmp mblt_exit
mblt_b100_24:
		mov	edx,maskbits
mblt_b100y24:
		mov ecx,w
mblt_b100x24:
		cmp	byte ptr [edx],0x80
		ja	mblt_b100s24
		movsw
		inc	edx
		movsb
		dec	ecx
		jnz	mblt_b100x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y24
		jmp mblt_exit

// 25% blend
mblt_b25s24:
		add	esi,3
		add	edi,3
		inc	edx
		dec	ecx
		jnz	mblt_b25x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b25y24
		jmp mblt_exit

mblt_b25_24:
		mov	edx,maskbits
mblt_b25y24:
		mov ecx,w
mblt_b25x24:
		cmp	byte ptr [edx],0x80
		ja	mblt_b25s24
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		inc	edx
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblt_b25x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b25y24
		jmp mblt_exit

// 50% blend
mblt_b50s24:
		add	esi,3
		add	edi,3
		inc	edx
		dec	ecx
		jnz	mblt_b50x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b50y24
		jmp mblt_exit
mblt_b50_24:
		mov	edx,maskbits
mblt_b50y24:
		mov ecx,w
mblt_b50x24:
		cmp	byte ptr [edx],0x80
		ja	mblt_b50s24
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		inc	edx
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblt_b50x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b50y24
		jmp mblt_exit

// 75% blend
mblt_b75s24:
		add	esi,3
		add	edi,3
		inc	edx
		dec	ecx
		jnz	mblt_b75x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b75y24
		jmp mblt_exit
mblt_b75_24:
		mov	edx,maskbits
mblt_b75y24:
		mov ecx,w
mblt_b75x24:
		cmp	byte ptr [edx],0x80
		ja	mblt_b75s24
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		inc	edx
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblt_b75x24
		add	edx,maskpitch
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b75y24
		jmp mblt_exit

// 15-bit
mblt_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblt_exit
		cmp ecx,0x40
		jle mblt_b25_15
		cmp ecx,0x80
		jle mblt_b50_15
		cmp ecx,0xC0
		jle mblt_b75_15

// 100% blend
mblt_b100y15:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y15
		jmp mblt_exit

// 25% blend
mblt_b25_15:
mblt_b25y15:
		mov ecx,w
mblt_b25x15:
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
		jnz mblt_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b25y15
		jmp mblt_exit

// 50% blend
mblt_b50_15:
mblt_b50y15:
		mov ecx,w
mblt_b50x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x3DEF
		and edx,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz mblt_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b50y15
		jmp mblt_exit

// 75% blend
mblt_b75_15:
mblt_b75y15:
		mov ecx,w
mblt_b75x15:
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
		jnz mblt_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b75y15
		jmp	mblt_exit

// 16-bit
mblt_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblt_exit
		cmp ecx,0x40
		jle mblt_b25_16
		cmp ecx,0x80
		jle mblt_b50_16
		cmp ecx,0xC0
		jle mblt_b75_16

// 100% blend
mblt_b100y16:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblt_b100y16
		jmp mblt_exit

// 25% blend
mblt_b25_16:
mblt_b25y16:
		mov ecx,w
mblt_b25x16:
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
		jnz mblt_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b25y16
		jmp mblt_exit

// 50% blend
mblt_b50_16:
mblt_b50y16:
		mov ecx,w
mblt_b50x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz mblt_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b50y16
		jmp mblt_exit

// 75% blend
mblt_b75_16:
mblt_b75y16:
		mov ecx,w
mblt_b75x16:
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
		jnz mblt_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblt_b75y16
		
mblt_exit:
		emms
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT mmx_blendck(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey)
{
	__asm{
		push esi
		push edi
		push ebx
		mov esi,src
		mov eax,oy
		mov ecx,spitch
		mov edx,ox
		mov ebx,bits
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
		mov	eax,w
		add edi,edx
		cmp	cl,15
		jl	mblck_exit
		je	mblck_b15
		cmp	cl,16
		je	mblck_b16
		cmp	cl,24
		je	mblck_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle mblck_exit
		cmp ecx,0x40
		jle mblck_b25_32
		cmp ecx,0x80
		jle mblck_b50_32
		cmp ecx,0xC0
		jle mblck_b75_32
		jmp	mblck_b100_32

// 100% blend
mblck_b100s32:
		add	edi,4
		dec	ecx
		jnz	mblck_b100x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y32
		jmp mblck_exit
mblck_b100_32:
		mov edx,colorkey
mblck_b100y32:
		mov ecx,w
mblck_b100x32:
		lodsd
		cmp	eax,edx
		je	mblck_b100s32
		movsd
		dec	ecx
		jnz	mblck_b100x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y32
		jmp mblck_exit
		
// 25% blend
mblck_b25s32:
		add	esi,4
		dec	ecx
		jnz	mblck_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b25y32
		jmp mblck_exit
mblck_b25_32:
		mov	edx,colorkey
		sub edi,4
mblck_b25y32:
		mov ecx,w
mblck_b25x32:
		pxor mm3,mm3
		add edi,4
		cmp	[esi],edx
		movd mm1,[esi]
		je	mblck_b25s32
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz	mblck_b25x32
		add	esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b25y32
		jmp mblck_exit
		
// 50% blend
mblck_b50s32:
		dec	ecx
		jnz	mblck_b50x32
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblck_b50y32
		jmp	mblck_exit
mblck_b50_32:
		mov	edx,colorkey
		sub esi,4
		sub edi,4
mblck_b50y32:
		mov ecx,w
mblck_b50x32:
		add esi,4
		pxor	mm3,mm3
		add	edi,4
		cmp	[esi],edx
		movd	mm1,[esi]
		je	mblck_b50s32
		movd	mm2,[edi]
		punpcklbw	mm2,mm3
		punpcklbw	mm1,mm3
		psrlw	mm1,1
		psrlw	mm2,1
		paddw	mm1,mm2
		packuswb	mm1,mm1
		dec	ecx
		movd	[edi],mm1
		jnz	mblck_b50x32
		add	esi,spitch
		add	edi,dpitch
		dec	h
		jnz	mblck_b50y32
		jmp	mblck_exit

// 75% blend
mblck_b75s32:
		add	esi,4
		dec	ecx
		jnz	mblck_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b75y32
		jmp mblck_exit
mblck_b75_32:
		mov	edx,colorkey
		sub edi,4
mblck_b75y32:
		mov ecx,w
mblck_b75x32:
		pxor mm3,mm3
		mov	eax,[esi]
		add edi,4
		cmp	eax,colorkey
		movd mm1,[esi]
		je	mblck_b75s32
		movd mm2,[edi]
		add esi,4
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mblck_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b75y32
		jmp mblck_exit

// 24-bit
mblck_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblck_exit
		cmp ecx,0x40
		jle mblck_b25_24
		cmp ecx,0x80
		jle mblck_b50_24
		cmp ecx,0xC0
		jle mblck_b75_24
		jmp	mblck_b100_24

// 100% blend
mblck_b100s24:
		add	edi,3
		dec	ecx
		jnz	mblck_b100x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y24
		jmp mblck_exit
mblck_b100_24:
		mov	edx,colorkey
mblck_b100y24:
		mov ecx,w
mblck_b100x24:
		lodsd
		and	eax,0xFFFFFF
		dec	esi
		cmp	eax,edx
		je	mblck_b100s24
		stosw
		shr	eax,16
		dec	ecx
		stosb
		jnz	mblck_b100x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y24
		jmp mblck_exit

// 25% blend
mblck_b25s24:
		add	esi,3
		add	edi,3
		dec	ecx
		jnz	mblck_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b25y24
		jmp mblck_exit
mblck_b25_24:
		mov	edx,colorkey
mblck_b25y24:
		mov ecx,w
mblck_b25x24:
		mov	eax,[esi]
		pxor mm3,mm3
		and	eax,0xFFFFFF
		movd mm1,[esi]
		cmp	eax,edx
		movd mm2,[edi]
		je	mblck_b25s24
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblck_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b25y24
		jmp mblck_exit

// 50% blend
mblck_b50s24:
		add	esi,3
		add	edi,3
		dec	ecx
		jnz	mblck_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b50y24
		jmp mblck_exit
mblck_b50_24:
		mov	edx,colorkey
mblck_b50y24:
		mov ecx,w
mblck_b50x24:
		mov	eax,[esi]
		pxor mm3,mm3
		and	eax,0xFFFFFF
		movd mm1,[esi]
		cmp	eax,edx
		movd mm2,[edi]
		je	mblck_b50s24
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblck_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b50y24
		jmp mblck_exit

// 75% blend
mblck_b75s24:
		add	esi,3
		add	edi,3
		dec	ecx
		jnz	mblck_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b75y24
		jmp mblck_exit
mblck_b75_24:
		mov	edx,colorkey
mblck_b75y24:
		mov ecx,w
mblck_b75x24:
		mov	eax,[esi]
		pxor mm3,mm3
		and	eax,0xFFFFFF
		movd mm1,[esi]
		cmp	eax,edx
		movd mm2,[edi]
		je	mblck_b75s24
		add esi,3
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mblck_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b75y24
		jmp mblck_exit

// 15-bit
mblck_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblck_exit
		cmp ecx,0x40
		jle mblck_b25_15
		cmp ecx,0x80
		jle mblck_b50_15
		cmp ecx,0xC0
		jle mblck_b75_15
		jmp	mblck_b100_15

// 100% blend
mblck_b100s15:
		add	edi,2
		dec	ecx
		jnz	mblck_b100x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y15
		jmp mblck_exit
mblck_b100_15:
		mov	edx,colorkey
mblck_b100y15:
		mov ecx,w
mblck_b100x15:
		lodsw
		cmp	ax,dx
		je	mblck_b100s15
		dec	ecx
		stosw
		jnz	mblck_b100x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y15
		jmp mblck_exit
		
// 25% blend
mblck_b25s15:
		add	edi,2
		dec	ecx
		jnz	mblck_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b25y15
		jmp mblck_exit
mblck_b25_15:
mblck_b25y15:
		mov ecx,w
mblck_b25x15:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		cmp	ax,word ptr colorkey
		mov ebx,edx
		je	mblck_b25s15
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
		jnz mblck_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b25y15
		jmp mblck_exit

// 50% blend
mblck_b50s15:
		add	edi,2
		dec	ecx
		jnz	mblck_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b50y15
		jmp mblck_exit
mblck_b50_15:
mblck_b50y15:
		mov ecx,w
mblck_b50x15:
		lodsw         // eax = source
		cmp	ax,word ptr colorkey
		mov dx,[edi]  // edx = dest
		je	mblck_b50s15
		shr eax,1
		shr edx,1
		and eax,0x3DEF
		and edx,0x3DEF
		add eax,edx
		dec ecx
		stosw
		jnz mblck_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b50y15
		jmp mblck_exit

// 75% blend
mblck_b75s15:
		add	edi,2
		dec	ecx
		jnz	mblck_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b75y15
		jmp mblck_exit
mblck_b75_15:
mblck_b75y15:
		mov ecx,w
mblck_b75x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		cmp	ax,word ptr colorkey
		mov ebx,eax
		je	mblck_b75s15
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
		jnz mblck_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b75y15
		jmp	mblck_exit
		
// 16-bit
mblck_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mblck_exit
		cmp ecx,0x40
		jle mblck_b25_16
		cmp ecx,0x80
		jle mblck_b50_16
		cmp ecx,0xC0
		jle mblck_b75_16
		jmp	mblck_b100_16

// 100% blend
mblck_b100s16:
		add	edi,2
		dec	ecx
		jnz	mblck_b100x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y16
		jmp mblck_exit
mblck_b100_16:
		mov	edx,colorkey
mblck_b100y16:
		mov ecx,w
mblck_b100x16:
		lodsw
		cmp	ax,dx
		je	mblck_b100s16
		dec	ecx
		stosw
		jnz	mblck_b100x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b100y16
		jmp mblck_exit
		
// 25% blend
mblck_b25s16:
		add	edi,2
		dec	ecx
		jnz	mblck_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b25y16
		jmp mblck_exit
mblck_b25_16:
mblck_b25y16:
		mov ecx,w
mblck_b25x16:
		lodsw         // ax = source
		mov dx,[edi]  // dx = dest
		cmp	ax,word ptr colorkey
		mov ebx,edx
		je	mblck_b25s16
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
		jnz mblck_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b25y16
		jmp mblck_exit

// 50% blend
mblck_b50s16:
		add	edi,2
		dec	ecx
		jnz	mblck_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b50y16
		jmp mblck_exit
mblck_b50_16:
mblck_b50y16:
		mov ecx,w
mblck_b50x16:
		lodsw         // eax = source
		cmp	ax,word ptr colorkey
		mov dx,[edi]  // edx = dest
		je	mblck_b50s16
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz mblck_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b50y16
		jmp mblck_exit

// 75% blend
mblck_b75s16:
		add	edi,2
		dec	ecx
		jnz	mblck_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jne mblck_b75y16
		jmp mblck_exit
mblck_b75_16:
mblck_b75y16:
		mov ecx,w
mblck_b75x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		cmp	ax,word ptr colorkey
		mov ebx,eax
		je	mblck_b75s16
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
		jnz mblck_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mblck_b75y16

mblck_exit:
		emms
		pop ebx
		pop edi
		pop esi
	};
}

void MTCT mmx_fill(void *dest,int pitch,int x,int y,int w,int h,int color,int opacity,int bits)
{
/*	__asm{
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
		imul	eax,ecx
		imul	edx,ebx
		add edi,eax
		shr edx,3
		mov ecx,bits
		add edi,edx
		cmp cl,15
		jl	mbl_exit
		je	mbl_b15
		cmp	cl,16
		je	mbl_b16
		cmp cl,24
		je	mbl_b24
		
// 32-bit
		mov eax,w
		shl eax,2
		sub spitch,eax
		sub dpitch,eax
		mov ecx,opacity
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_32
		cmp ecx,0x80
		jle mbl_b50_32
		cmp ecx,0xC0
		jle mbl_b75_32
		
// 100% blend
mbl_b100y32:
		mov ecx,w
		rep movsd
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y32
		jmp mbl_exit
		
// 25% blend
mbl_b25_32:
		sub edi,4
mbl_b25y32:
		mov ecx,w
mbl_b25x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b25x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y32
		jmp mbl_exit
		
// 50% blend
mbl_b50_32:
		sub edi,4
mbl_b50y32:
		mov ecx,w
mbl_b50x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b50x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y32
		jmp mbl_exit

// 75% blend
mbl_b75_32:
		sub edi,4
mbl_b75y32:
		mov ecx,w
mbl_b75x32:
		pxor mm3,mm3
		add edi,4
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,4
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		dec ecx
		movd [edi],mm1
		jnz mbl_b75x32
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y32
		jmp mbl_exit

// 24-bit
mbl_b24:
		mov eax,w
		mov edx,eax
		shl eax,2
		sub eax,edx
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_24
		cmp ecx,0x80
		jle mbl_b50_24
		cmp ecx,0xC0
		jle mbl_b75_24

// 100% blend
mbl_b100y24:
		mov ecx,w
		rep movsw
		mov ecx,w
		rep movsb
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y24
		jmp mbl_exit

// 25% blend
mbl_b25_24:
mbl_b25y24:
		mov ecx,w
mbl_b25x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		movq mm3,mm2
		psrlw mm1,2
		psrlw mm2,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b25x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y24
		jmp mbl_exit

// 50% blend
mbl_b50_24:
mbl_b50y24:
		mov ecx,w
mbl_b50x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm2,mm3
		punpcklbw mm1,mm3
		psrlw mm1,1
		psrlw mm2,1
		paddw mm1,mm2
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b50x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y24
		jmp mbl_exit

// 75% blend
mbl_b75_24:
mbl_b75y24:
		mov ecx,w
mbl_b75x24:
		pxor mm3,mm3
		movd mm1,[esi]
		movd mm2,[edi]
		add esi,3
		punpcklbw mm1,mm3
		punpcklbw mm2,mm3
		movq mm3,mm1
		psrlw mm2,2
		psrlw mm1,2
		psrlw mm3,1
		paddw mm1,mm2
		paddw mm1,mm3
		packuswb mm1,mm1
		movd eax,mm1
		stosw
		shr eax,16
		dec ecx
		stosb
		jnz mbl_b75x24
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y24
		jmp mbl_exit

// 15-bit
mbl_b15:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_15
		cmp ecx,0x80
		jle mbl_b50_15
		cmp ecx,0xC0
		jle mbl_b75_15

// 100% blend
mbl_b100y15:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y15
		jmp mbl_exit

// 25% blend
mbl_b25_15:
mbl_b25y15:
		mov ecx,w
mbl_b25x15:
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
		jnz mbl_b25x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y15
		jmp mbl_exit

// 50% blend
mbl_b50_15:
mbl_b50y15:
		mov ecx,w
mbl_b50x15:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz mbl_b50x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y15
		jmp mbl_exit

// 75% blend
mbl_b75_15:
mbl_b75y15:
		mov ecx,w
mbl_b75x15:
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
		jnz mbl_b75x15
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y15
		jmp	mbl_exit

// 16-bit
mbl_b16:
		mov eax,w
		shl eax,1
		mov ecx,opacity
		sub spitch,eax
		sub dpitch,eax
		cmp ecx,0x00
		jle mbl_exit
		cmp ecx,0x40
		jle mbl_b25_16
		cmp ecx,0x80
		jle mbl_b50_16
		cmp ecx,0xC0
		jle mbl_b75_16

// 100% blend
mbl_b100y16:
		mov ecx,w
		rep movsw
		add esi,spitch
		add edi,dpitch
		dec h
		jne mbl_b100y16
		jmp mbl_exit

// 25% blend
mbl_b25_16:
mbl_b25y16:
		mov ecx,w
mbl_b25x16:
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
		jnz mbl_b25x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b25y16
		jmp mbl_exit

// 50% blend
mbl_b50_16:
mbl_b50y16:
		mov ecx,w
mbl_b50x16:
		lodsw         // eax = source
		mov dx,[edi]  // edx = dest
		shr eax,1
		shr edx,1
		and eax,0x7BEF
		and edx,0x7BEF
		add eax,edx
		dec ecx
		stosw
		jnz mbl_b50x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b50y16
		jmp mbl_exit

// 75% blend
mbl_b75_16:
mbl_b75y16:
		mov ecx,w
mbl_b75x16:
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
		jnz mbl_b75x16
		add esi,spitch
		add edi,dpitch
		dec h
		jnz mbl_b75y16
		
mbl_exit:
		emms
		pop ebx
		pop edi
		pop esi
	};*/
}
//---------------------------------------------------------------------------
