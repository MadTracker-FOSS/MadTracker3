;//---------------------------------------------------------------------------
;//
;//	MadTracker DSP Core
;//
;//		Platforms:	All
;//		Processors:	x86
;//
;//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
;//
;//	$Id: MTBuffer.asm 96 2005-11-30 20:06:47Z Yannick $
;//
;//---------------------------------------------------------------------------

%include "../Headers/MTXASM.inc"

section .text

; void MTACT a_emptybuffer
; (
;	sample *dest, // +4
;	int count,    // +8
; )

cglobal a_emptybuffer

a_emptybuffer:

	push	edi

	mov		ecx,[esp+4+8]
	xor		eax,eax
	test	ecx,ecx
	mov		edi,[esp+4+4]
	jz		eb_done
	rep		stosd

eb_done:

	pop		edi

	ret

; void MTCT a_replacebuffer
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	int count       // +12
; )

cglobal a_replacebuffer

a_replacebuffer:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+12]
rb_loop:
	fld		a_sample [esi]
	fstp	a_sample [edi]
	add		esi,s_sample
	add		edi,s_sample
	dec		ecx
	jnz		rb_loop

	pop		edi
	pop		esi

	ret

; void MTCT a_replacebuffermul
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	double a,       // +12
;	int count       // +16
; )

cglobal a_replacebuffermul

a_replacebuffermul:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+16]
	fld		qword [esp+8+12]
rbm_loop:
	fld		a_sample [esi]
	fmul	st0,st1
	fstp	a_sample [edi]
	add		esi,s_sample
	add		edi,s_sample
	dec		ecx
	jnz		rbm_loop

	fstp	qword [esp+8+12]

	pop		edi
	pop		esi

	ret

; void MTCT a_addbuffer
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	int count       // +12
; )

cglobal a_addbuffer

a_addbuffer:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+12]
ab_loop:
	fld		a_sample [edi]
	fadd	a_sample [esi]
	fstp	a_sample [edi]
	add		esi,s_sample
	add		edi,s_sample
	dec		ecx
	jnz		ab_loop

	pop		edi
	pop		esi

	ret

; void MTCT a_addbuffermul
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	double a,       // +12
;	int count       // +16
; )

cglobal a_addbuffermul

a_addbuffermul:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+16]
	fld		qword [esp+8+12]
abm_loop:
	fld		a_sample [esi]
	fmul	st0,st1
	fadd	a_sample [edi]
	add		esi,s_sample
	fstp	a_sample [edi]
	add		edi,s_sample
	dec		ecx
	jnz		abm_loop

	fstp	qword [esp+8+12]

	pop		edi
	pop		esi

	ret

; void MTCT a_addbuffermul2
; (
;	sample *dest1,  // +4
;	sample *dest2,  // +8
;	sample *source, // +12
;	double a1,      // +16
;	double a2,      // +24
;	int count       // +32
; )

cglobal a_addbuffermul2

a_addbuffermul2:

	push	esi
	push	edi
	push	ebx

	mov		esi,[esp+12+12]
	mov		edi,[esp+12+4]
	mov		ebx,[esp+12+8]
	mov		ecx,[esp+12+32]
	fld		qword [esp+12+24]
	fld		qword [esp+12+16]
abm2_loop:
	fld		a_sample [esi]
	fld		st0
	fmul	st0,st2
	fadd	a_sample [edi]
	fstp	a_sample [edi]
	fmul	st0,st2
	fadd	a_sample [ebx]
	fstp	a_sample [ebx]
	add		esi,s_sample
	add		edi,s_sample
	add		ebx,s_sample
	dec		ecx
	jnz		abm2_loop

	fstp	qword [esp+12+16]
	fstp	qword [esp+12+24]

	pop		ebx
	pop		edi
	pop		esi

	ret

; void MTCT a_addbufferslide
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	double a,       // +12
;	double i,       // +20
;	int count       // +28
; )

cglobal a_addbufferslide

a_addbufferslide:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+28]
	fld		qword [esp+8+20]
	fld		qword [esp+8+12]
abs_loop:
	fld		a_sample [esi]
	fmul	st0,st1
	fadd	a_sample [edi]
	add		esi,s_sample
	fstp	a_sample [edi]
	add		edi,s_sample
	fadd	st0,st1
	dec		ecx
	jnz		abs_loop

	fstp	qword [esp+8+12]
	fstp	qword [esp+8+20]

	pop		edi
	pop		esi

	ret


; void MTCT a_addbufferslide2
; (
;	sample *dest1,  // +4
;	sample *dest2,  // +8
;	sample *source, // +12
;	double a1,      // +16
;	double a2,      // +24
;	double i1,      // +32
;	double i2,      // +40
;	int count       // +48
; )

cglobal a_addbufferslide2

a_addbufferslide2:

	push	esi
	push	edi
	push	ebx

	mov		esi,[esp+12+12]
	mov		edi,[esp+12+4]
	mov		ebx,[esp+12+8]
	mov		ecx,[esp+12+48]
	fld		qword [esp+12+40]
	fld		qword [esp+12+32]
	fld		qword [esp+12+24]
	fld		qword [esp+12+16]
abs2_loop:
	fld		a_sample [esi]
	fld		st0
	fmul	st0,st2
	fadd	a_sample [edi]
	fstp	a_sample [edi]
	fmul	st0,st2
	fadd	a_sample [ebx]
	fstp	a_sample [ebx]
	fxch
	add		esi,s_sample
	fadd	st0,st3
	add		edi,s_sample
	fxch
	add		ebx,s_sample
	fadd	st0,st2
	dec		ecx
	jnz		abs2_loop

	fstp	qword [esp+12+16]
	fstp	qword [esp+12+24]
	fstp	qword [esp+12+32]
	fstp	qword [esp+12+40]

	pop		ebx
	pop		edi
	pop		esi

	ret

; void MTCT a_ampbuffer
; (
;	sample *dest, // +4
;	double a,     // +8
;	int count     // +16
; )

cglobal a_ampbuffer

a_ampbuffer:

	push	edi

	mov		edi,[esp+4+4]
	mov		ecx,[esp+4+16]
	fld		qword [esp+4+8]
	sub		edi,s_sample
amp_loop:
	fld		a_sample [edi]
	add		edi,s_sample
	fmul	st0,st1
	dec		ecx
	fstp	a_sample [edi]
	jnz		amp_loop

	fstp	qword [esp+4+8]

	pop		edi

	ret

; void MTCT a_modulatebuffer
; (
;	sample *dest,   // +4
;	sample *source, // +8
;	int count       // +12
; )

cglobal a_modulatebuffer

a_modulatebuffer:

	push	esi
	push	edi

	mov		esi,[esp+8+8]
	mov		edi,[esp+8+4]
	mov		ecx,[esp+8+12]
amb_loop:
	fld		a_sample [edi]
	fmul	a_sample [esi]
	fstp	a_sample [edi]
	add		esi,s_sample
	add		edi,s_sample
	dec		ecx
	jnz		amb_loop

	pop		edi
	pop		esi

	ret
