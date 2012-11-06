//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTBufferASM.cpp 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTBufferASM.h"
//---------------------------------------------------------------------------
/*
void MTCT a_emptybuffer(sample *dest,int count)
{
#	ifndef __GNUC__
		__asm{
			push	edi
			mov		ecx,count
#			ifdef MT3_64BIT
				shl		ecx,1
#			endif
			xor		eax,eax
			test	ecx,ecx
			mov		edi,dest
			jz		_done
			rep		stosd
		_done:
			pop		edi
		};
#	else
		asm ("\
			"
#			ifdef MT3_64BIT
				"shll	$1,%%ecx\n"
#			endif
			"\
			xorl	%%eax,%%eax\n\
			testl	%%ecx,%%ecx\n\
			jz		_done1\n\
			rep		stosl\n\
		_done1:\n\
			"
			:
			:[count]"c"(count),[dest]"D"(dest)
			:"eax"
		);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_replacebuffer(sample *dest,sample *source,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
		_add:
			fld		a_sample ptr [esi]
			fstp	a_sample ptr [edi]
			add		esi,s_sample
			add		edi,s_sample
			dec		ecx
			jnz		_add

			pop		edi
			pop		esi
		};
#	else
		asm ("\
		_radd1:\n\
			fld"spls"	(%%esi)\n\
			fstp"spls"	(%%edi)\n\
			add		$"spll",%%esi\n\
			add		$"spll",%%edi\n\
			dec		%%ecx\n\
			jnz		_radd1\n\
			"
			:
			:[source]"S"(source),[dest]"D"(dest),[count]"c"(count)
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_replacebuffermul(sample *dest,sample *source,double a,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
			fld		a
		_add:
			fld		a_sample ptr [esi]
			fmul	st(0),st(1)
			fstp	a_sample ptr [edi]
			add		esi,s_sample
			add		edi,s_sample
			dec		ecx
			jnz		_add

			fstp	a
			pop		edi
			pop		esi
		};
#	else
		asm ("\
			fld		%[a]\n\
		_radd2:\n\
			fld"spls"	(%%esi)\n\
			fmul	%%st(1),%%st(0)\n\
			fstp"spls"	(%%edi)\n\
			add		$"spll",%%esi\n\
			add		$"spll",%%edi\n\
			dec		%%ecx\n\
			jnz		_radd2\n\
			fstp	%[a]\n\
			"
			:
			:[source]"S"(source),[dest]"D"(dest),[count]"c"(count),[a]"m"(a)
			);
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_addbuffer(sample *dest,sample *source,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
		_add:
			fld		a_sample ptr [edi]
			fadd	a_sample ptr [esi]
			fstp	a_sample ptr [edi]
			add		esi,s_sample
			add		edi,s_sample
			dec		ecx
			jnz		_add

			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_addbuffermul(sample *dest,sample *source,double a,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
			fld		a
		_add:
			fld		a_sample ptr [esi]
			fmul	st(0),st(1)
			fadd	a_sample ptr [edi]
			add		esi,s_sample
			fstp	a_sample ptr [edi]
			add		edi,s_sample
			dec		ecx
			jnz		_add

			fstp	a
			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_addbuffermul2(sample *dest1,sample *dest2,sample *source,double a1,double a2,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			mov		esi,source
			mov		edi,dest1
			mov		ebx,dest2
			mov		ecx,count
			fld		a2
			fld		a1
		_add:
			fld		a_sample ptr [esi]
			fld		st(0)
			fmul	st(0),st(2)
			fadd	a_sample ptr [edi]
			fstp	a_sample ptr [edi]
			fmul	st(0),st(2)
			fadd	a_sample ptr [ebx]
			fstp	a_sample ptr [ebx]
			add		esi,s_sample
			add		edi,s_sample
			add		ebx,s_sample
			dec		ecx
			jnz		_add

			fstp	a1
			fstp	a2
			pop		ebx
			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_addbufferslide(sample *dest,sample *source,double a,double i,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
			fld		i
			fld		a
		_add:
			fld		a_sample ptr [esi]
			fmul	st(0),st(1)
			fadd	a_sample ptr [edi]
			add		esi,s_sample
			fstp	a_sample ptr [edi]
			add		edi,s_sample
			fadd	st(0),st(1)
			dec		ecx
			jnz		_add

			fstp	a
			fstp	i
			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_addbufferslide2(sample *dest1,sample *dest2,sample *source,double a1,double a2,double i1,double i2,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			mov		esi,source
			mov		edi,dest1
			mov		ebx,dest2
			mov		ecx,count
			fld		i2
			fld		i1
			fld		a2
			fld		a1
		_add:
			fld		a_sample ptr [esi]
			fld		st(0)
			fmul	st(0),st(2)
			fadd	a_sample ptr [edi]
			fstp	a_sample ptr [edi]
			fmul	st(0),st(2)
			fadd	a_sample ptr [ebx]
			fstp	a_sample ptr [ebx]
			fxch
			add		esi,s_sample
			fadd	st(0),st(3)
			add		edi,s_sample
			fxch
			add		ebx,s_sample
			fadd	st(0),st(2)
			dec		ecx
			jnz		_add

			fstp	a1
			fstp	a2
			fstp	i1
			fstp	i2
			pop		ebx
			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_ampbuffer(sample *dest,double a,int count)
{
#	ifndef __GNUC__
		__asm{
			push	edi
			mov		edi,dest
			mov		ecx,count
			fld		a
			sub		edi,s_sample
		_add:
			fld		a_sample ptr [edi]
			add		edi,s_sample
			fmul	st(0),st(1)
			dec		ecx
			fstp	a_sample ptr [edi]
			jnz		_add

			fstp	a
			pop		edi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
void MTCT a_modulatebuffer(sample *dest,sample *source,int count)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			mov		esi,source
			mov		edi,dest
			mov		ecx,count
		_add:
			fld		a_sample ptr [edi]
			fmul	a_sample ptr [esi]
			fstp	a_sample ptr [edi]
			add		esi,s_sample
			add		edi,s_sample
			dec		ecx
			jnz		_add

			pop		edi
			pop		esi
		};
#	else
//TODO
#	endif
}
//---------------------------------------------------------------------------
*/