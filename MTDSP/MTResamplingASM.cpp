//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTResamplingASM.cpp 101 2005-11-30 20:19:50Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTResamplingASM.h"
//---------------------------------------------------------------------------
/*
void MTACT a_resample_none_8(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			mov		ecx,count
			mov		ebx,status
			mov		esi,source
			xor		eax,eax
			mov		edx,dword ptr [ebx].posd
			mov		edi,dest
			fld		double ptr [ebx].vol
			sub		edi,s_sample

			cmp		[ebx].reverse,0
			je		_mixf

		_mixb:
			movsx	eax,byte ptr [esi]
			add		edi,s_sample
			push	eax
			fild	dword ptr [esp]
			fmul	st(0),st(1)
			add		esp,4
			fadd	a_sample ptr [edi]
			mov		eax,[ebx].volvarlng
			sub		edx,pitchd
			sbb		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarb
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
		_novarb:
			dec		ecx
			jnz		_mixb
			jmp		_done

		_mixf:
			movsx	eax,byte ptr [esi]
			add		edi,s_sample
			push	eax
			fild	dword ptr [esp]
			fmul	st(0),st(1)
			add		esp,4
			fadd	a_sample ptr [edi]
			add		edx,pitchd
			mov		eax,[ebx].volvarlng
			adc		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarf
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
		_novarf:
			dec		ecx
			jnz		_mixf

		_done:
			fstp	double ptr [ebx].vol
			mov		eax,esi
			sub		eax,source
			add		dword ptr [ebx].posi,eax
			mov		dword ptr [ebx].posd,edx
			pop		ebx
			pop		edi
			pop		esi
		};
#	else

#	endif
}
//---------------------------------------------------------------------------
void MTACT a_resample_none_16(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			mov		ecx,count
			mov		ebx,status
			mov		esi,source
			xor		eax,eax
			mov		edx,dword ptr [ebx].posd
			mov		edi,dest
			shr		esi,1
			fld		double ptr [ebx].vol
			sub		edi,s_sample

			cmp		[ebx].reverse,0
			je		_mixf

		_mixb:
			fild	word ptr [esi*2]
			add		edi,s_sample
			fmul	st(0),st(1)
			sub		edx,pitchd
			fadd	a_sample ptr [edi]
			mov		eax,[ebx].volvarlng
			sbb		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarb
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
		_novarb:
			dec		ecx
			jnz		_mixb
			jmp		_done

		_mixf:
			fild	word ptr [esi*2]
			add		edi,s_sample
			fmul	st(0),st(1)
			add		edx,pitchd
			fadd	a_sample ptr [edi]
			mov		eax,[ebx].volvarlng
			adc		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarf
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
		_novarf:
			dec		ecx
			jnz		_mixf

		_done:
			fstp	double ptr [ebx].vol
			mov		eax,source
			shr		eax,1
			sub		esi,eax
			add		dword ptr [ebx].posi,esi
			mov		dword ptr [ebx].posd,edx
			pop		ebx
			pop		edi
			pop		esi
		};
#	else

#	endif
}
//---------------------------------------------------------------------------
void MTACT a_resample_none_32(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd)
{
#	ifndef __GNUC__
		__asm{
			push	esi
			push	edi
			push	ebx
			mov		ecx,count
			mov		ebx,status
			mov		esi,source
			xor		eax,eax
			mov		edx,dword ptr [ebx].posd
			mov		edi,dest
			shr		esi,2
			fld		double ptr [ebx].vol
			sub		edi,s_sample

			cmp		[ebx].reverse,0
			je		_mixf

		_mixb:
			fld		float ptr [esi*4]
			add		edi,s_sample
			fmul	st(0),st(1)
			sub		edx,pitchd
			fadd	a_sample ptr [edi]
			mov		eax,[ebx].volvarlng
			sbb		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarb
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
		_novarb:
			dec		ecx
			jnz		_mixb
			jmp		_done

		_mixf:
			fld		float ptr [esi*4]
			add		edi,s_sample
			fmul	st(0),st(1)
			add		edx,pitchd
			fadd	a_sample ptr [edi]
			mov		eax,[ebx].volvarlng
			adc		esi,pitchi
			dec		eax
			fstp	a_sample ptr [edi]
			js		_novarf
			fadd	double ptr [ebx].volvar
			mov		[ebx].volvarlng,eax
	_novarf:
			dec		ecx
			jnz		_mixf

	_done:
			fstp	double ptr [ebx].vol
			mov		eax,source
			shr		eax,2
			sub		esi,eax
			add		dword ptr [ebx].posi,esi
			mov		dword ptr [ebx].posd,edx
			pop		ebx
			pop		edi
			pop		esi
		};
#	else

#	endif
}
//---------------------------------------------------------------------------
*/
