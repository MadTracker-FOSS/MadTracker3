;//---------------------------------------------------------------------------
;//
;//	MadTracker DSP Core
;//
;//		Platforms:	All
;//		Processors:	x86
;//
;//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
;//
;//	$Id: MTResampling.asm 96 2005-11-30 20:06:47Z Yannick $
;//
;//---------------------------------------------------------------------------

%include "../Headers/MTXASM.inc"
%include "../MTDSP/MTDSPASM.inc"

section .text

%macro	none_8 0

	movsx	eax,byte [esi]
	mov		var1,eax
	fild	dword var1
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

%macro	none_16 0

	fild	word [esi*2]
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

%macro	none_32 0

	fld		dword [esi*4]
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

%macro	linear_8 0

	movsx	eax,byte [esi]
	movsx	ebp,byte [esi+1]
	mov		var1,eax
	mov		var2,ebp
	mov		var3,edx
	mov		var4,edx
	not		dword var3
	shr		dword var3,1
	shr		dword var4,1
	fild	dword var1
	fimul	dword var3
	fild	dword var2
	fimul	dword var4
	faddp	st1
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

%macro	linear_16 0

	mov		var1,edx
	mov		var2,edx
	not		dword var1
	shr		dword var1,1
	shr		dword var2,1
	fild	word [esi*2]
	fimul	dword var1
	fild	word [esi*2+2]
	fimul	dword var2
	faddp	st1
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

%macro	linear_32 0

	mov		var1,edx
	mov		var2,edx
	not		dword var1
	shr		dword var1,1
	shr		dword var2,1
	fld		dword [esi*4]
	fimul	dword var1
	fld		dword [esi*4+4]
	fimul	dword var2
	faddp	st1
	fmul	st0,st1
	fadd	a_sample [edi]
	fstp	a_sample [edi]

%endmacro

	resample_proc	none,8
	resample_proc	none,16
	resample_proc	none,32

	resample_proc	linear,8
	resample_proc	linear,16
	resample_proc	linear,32
