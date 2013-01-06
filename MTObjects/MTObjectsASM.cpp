//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTObjectsASM.cpp 100 2005-11-30 20:19:39Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTObjectsASM.h"
//---------------------------------------------------------------------------
bool MTCT a_floattofixed(double f,int &i,unsigned int &d)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			mov		eax,dword ptr [f+4]
			push	edi
			test	eax,0x7F800000
			mov		edx,dword ptr [f]
			jnz		_ok
			xor		eax,eax
			xor		edx,edx
			jmp		_done
		_ok:
			push	eax
			mov		ecx,eax
			shr		ecx,20
			and		eax,0xFFFFF
			sub		ecx,0x3FF+20
			or		eax,0x100000
			neg		cl
			cmp		cl,32
			jae		_s32
			shrd	edx,eax,cl
			shr		eax,cl
			jmp		_sign
		_s32:
			mov		edx,eax
			sub		cl,32
			xor		eax,eax
			shr		edx,cl
		_sign:
			mov		ecx,eax
			pop		eax
			bt		eax,31
			sbb		eax,eax
			and		eax,1
		_done:
			mov		esi,i
			mov		edi,d
			mov		[esi],ecx
			mov		[edi],edx
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			leal	%[f],%%edx\n\
			movl	4(%%edx),%%eax\n\
			testl	$0x7F800000,%%eax\n\
			movl	(%%edx),%%edx\n\
			jnz		_ok\n\
			xorl	%%eax,%%eax\n\
			xorl	%%edx,%%edx\n\
			jmp		_exit1\n\
		_ok:\n\
			pushl	%%eax\n\
			movl	%%eax,%%ecx\n\
			shrl	$20,%%ecx\n\
			andl	$0xFFFFF,%%eax\n\
			subl	$0x413,%%ecx\n\
			orl		$0x100000,%%eax\n\
			negb	%%cl\n\
			cmpb	$32,%%cl\n\
			jae		_s32\n\
			shrd	%%cl,%%eax,%%edx\n\
			shrl	%%cl,%%eax\n\
			jmp		_sign\n\
		_s32:\n\
			movl	%%eax,%%edx\n\
			subb	$32,%%cl\n\
			xor		%%eax,%%eax\n\
			shrl	%%cl,%%edx\n\
		_sign:\n\
			movl	%%eax,%%ecx\n\
			popl	%%eax\n\
			btl		$31,%%eax\n\
			sbbl	%%eax,%%eax\n\
			andl	$1,%%eax\n\
		_exit1:\n\
			movl	%%ecx,(%%esi)\n\
			movl	%%edx,(%%edi)\n\
			"
			:
			:[f]"m"(f),[i]"S"(&i),[d]"D"(&d)
			:"eax","ecx","edx"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_changesign(char *buffer,int count)
{
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			mov		esi,buffer
			mov		ecx,count
			mov		edi,esi
			mov		edx,ecx
			shr		ecx,2
		_loop:
			lodsd
			xor		eax,0x80808080
			dec		ecx
			stosd
			jnz		_loop
			and		edx,0x3
			jz		_done
		_loop2:
			lodsb
			xor		al,0x80
			dec		edx
			stosb
			jnz		_loop2
		_done:
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			movl	%%esi,%%edi\n\
			movl	%%ecx,%%edx\n\
			shrl	$2,%%ecx\n\
		_loop1:\n\
			lodsl\n\
			xorl	$0x80808080,%%eax\n\
			decl	%%ecx\n\
			stosl\n\
			jnz		_loop1\n\
			andl	$0x3,%%edx\n\
			jz		_exit2\n\
		_loop2:\n\
			lodsb\n\
			xorb	$0x80,%%al\n\
			decl	%%edx\n\
			stosb\n\
			jnz		_loop2\n\
		_exit2:\n\
			"
			:
			:[buffer]"S"(buffer),[count]"c"(count)
			:"eax","edx","edi"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_delta_add_8(char *dest,char *source,int count)
{
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
			dec		edi
		_add:
			inc		edi
			lodsb
			add		[edi],al
			dec		ecx
			jnz		_add

			pop		edi
			pop		esi
		};
#	else
		asm ("\
			decl	%%edi\n\
		_add1:\n\
			incl	%%edi\n\
			lodsb\n\
			addb	%%al,(%%edi)\n\
			decl	%%ecx\n\
			jnz		_add1\n\
			"
			:
			:[source]"S"(source),[dest]"D"(dest),[count]"c"(count)
			:"eax"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_delta_add_16(short *dest,short *source,int count)
{
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
			sub		edi,2
		_add:
			add		edi,2
			lodsw
			add		[edi],ax
			dec		ecx
			jnz		_add

			pop		edi
			pop		esi
		};
#	else
		asm ("\
			subl	$2,%%edi\n\
		_add2:\n\
			addl	$2,%%edi\n\
			lodsw\n\
			addw	%%ax,(%%edi)\n\
			decl	%%ecx\n\
			jnz		_add2\n\
			"
			:
			:[source]"S"(source),[dest]"D"(dest),[count]"c"(count)
			:"eax"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_delta_encode_8(char *buffer,int count)
{
//TODO
}
//---------------------------------------------------------------------------
void MTCT a_delta_encode_16(short *buffer,int count)
{
//TODO
}
//---------------------------------------------------------------------------
void MTCT a_delta_decode_8(char *buffer,int count)
{
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			mov		esi,buffer
			mov		ecx,count
			mov		edi,esi
			xor		edx,edx
			dec		edi
		_decode:
			lodsb
			inc		edi
			add		dl,al
			mov		[edi],dl
			dec		ecx
			mov		al,dl
			jnz		_decode

			pop		edi
			pop		esi
		};
#	else
		asm ("\
			movl	%%esi,%%edi\n\
			xorl	%%edx,%%edx\n\
			decl	%%edi\n\
		_decode1:\n\
			lodsb\n\
			incl	%%edi\n\
			addb	%%al,%%dl\n\
			movb	%%dl,(%%edi)\n\
			decl	%%ecx\n\
			movb	%%dl,%%al\n\
			jnz		_decode1\n\
			"
			:
			:[buffer]"S"(buffer),[count]"c"(count)
			:"eax","edx","edi"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_delta_decode_16(short *buffer,int count)
{
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			mov		esi,buffer
			mov		ecx,count
			mov		edi,esi
			xor		edx,edx
			sub		edi,2
		_decode:
			lodsw
			add		edi,2
			add		dx,ax
			mov		[edi],dx
			dec		ecx
			mov		ax,dx
			jnz		_decode

			pop		edi
			pop		esi
		};
#	else
		asm ("\
			movl	%%esi,%%edi\n\
			xorl	%%edx,%%edx\n\
			subl	$2,%%edi\n\
		_decode2:\n\
			lodsw\n\
			addl	$2,%%edi\n\
			addw	%%ax,%%dx\n\
			movw	%%dx,(%%edi)\n\
			decl	%%ecx\n\
			movw	%%dx,%%ax\n\
			jnz		_decode2\n\
			"
			:
			:[buffer]"S"(buffer),[count]"c"(count)
			:"eax","edx","edi"
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_deinterleave_8(char **dest,char *source,int channels,int count)
{
#if 0 // FIXME: We need to port this to C; it's broken in GCC anyway -flibit
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			push	ebx
			mov		edx,channels
			dec		edx
			jl		_done
		_nextchan:
			mov		esi,source
			lea		esi,[esi+edx]
			mov		ebx,dest
			mov		edi,[ebx+edx*4]
			mov		ecx,count
			mov		ebx,channels
			dec		ebx
		_loop:
			lodsb
			add		esi,ebx
			stosb
			dec		ecx
			jnz		_loop
			dec		edx
			jge		_nextchan
		_done:
			pop		ebx
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			movl	%[channels],%%edx\n\
			decl	%%edx\n\
			jl		_exit3\n\
		_nextchan1:\n\
			movl	%[source],%%esi\n\
			leal	(%%esi,%%edx),%%esi\n\
			movl	%[dest],%%ebx\n\
			movl	(%%ebx,%%edx,4),%%edi\n\
			movl	%[count],%%ecx\n\
			mov		%[channels],%%ebx\n\
			dec		%%ebx\n\
		_loop3:\n\
			lodsb\n\
			addl	%%ebx,%%esi\n\
			stosb\n\
			decl	%%ecx\n\
			jnz		_loop3\n\
			decl	%%edx\n\
			jge		_nextchan1\n\
		_exit3:\n\
			"
			:
			:[channels]"m"(channels),[dest]"m"(dest),[count]"m"(count),[source]"m"(source)
			:"eax","ebx","ecx","esi","edi"
			);
#	endif
#endif
}
//---------------------------------------------------------------------------
void MTCT a_deinterleave_16(short **dest,short *source,int channels,int count)
{
#if 0 // FIXME: We need to port this to C; it's broken in GCC anyway -flibit
#	ifndef __GNUC__
		_asm{
			push	esi
			push	edi
			push	ebx
			mov		edx,channels
			dec		edx
			jl		_done
		_nextchan:
			mov		esi,source
			lea		esi,[esi+edx*2]
			mov		ebx,dest
			mov		edi,[ebx+edx*4]
			mov		ecx,count
			mov		ebx,channels
			dec		ebx
			shl		ebx,1
		_loop:
			lodsw
			add		esi,ebx
			stosw
			dec		ecx
			jnz		_loop
			dec		edx
			jge		_nextchan
		_done:
			pop		ebx
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			movl	%[channels],%%edx\n\
			decl	%%edx\n\
			jl		_exit4\n\
		_nextchan2:\n\
			movl	%[source],%%esi\n\
			leal	(%%esi,%%edx,2),%%esi\n\
			movl	%[dest],%%ebx\n\
			movl	(%%ebx,%%edx,4),%%edi\n\
			movl	%[count],%%ecx\n\
			movl	%[channels],%%ebx\n\
			decl	%%ebx\n\
			shll	$1,%%ebx\n\
		_loop4:\n\
			lodsw\n\
			addl	%%ebx,%%esi\n\
			stosw\n\
			decl	%%ecx\n\
			jnz		_loop4\n\
			decl	%%edx\n\
			jge		_nextchan2\n\
		_exit4:\n\
			"
			:
			:[channels]"m"(channels),[dest]"m"(dest),[count]"m"(count),[source]"m"(source)
			:"eax","ebx","ecx","esi","edi"
			);
#	endif
#endif
}
//---------------------------------------------------------------------------
void MTCT a_calcposition(int &posi,unsigned int &posd,int pitch,unsigned int pitchd,int count,bool reverse)
{
#if 0 // FIXME: We need to port this to C; it's broken in GCC anyway -flibit
#	ifndef __GNUC__
		_asm{
			push	ebx
			mov		ebx,posi
			mov		ecx,posd
			xor		edx,edx
			mov		eax,pitchd
			mul		count
			push	eax
			push	edx
			xor		edx,edx
			mov		eax,pitch
			mul		count
			add		[esp],eax
			pop		edx
			pop		eax
			cmp		reverse,0
			jne		_reverse
			add		[ecx],eax
			adc		[ebx],edx
			jmp		_done
		_reverse:
			sub		[ecx],eax
			sbb		[ebx],edx
		_done:
			pop		ebx
		};
#	else
		asm ("\
			xorl	%%edx,%%edx\n\
			mull	%[count]\n\
			pushl	%%eax\n\
			pushl	%%edx\n\
			xorl	%%edx,%%edx\n\
			movl	%[pitch],%%eax\n\
			mull	%[count]\n\
			addl	%%eax,(%%esp)\n\
			popl	%%edx\n\
			popl	%%eax\n\
			cmpb	$0,%[reverse]\n\
			jne		_reverse\n\
			addl	%%eax,(%%ecx)\n\
			adcl	%%edx,(%%ebx)\n\
			jmp		_exit5\n\
		_reverse:\n\
			subl	%%eax,(%%ecx)\n\
			sbbl	%%edx,(%%ebx)\n\
		_exit5:\n\
			"
			:
			:[pitch]"m"(pitch),[pitchd]"a"(pitchd),[posd]"c"(&posd),[posi]"b"(&posi),[reverse]"m"(reverse),[count]"m"(count)
			:"edx"
			);
#	endif
#endif
}
//---------------------------------------------------------------------------
