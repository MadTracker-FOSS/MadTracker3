;//---------------------------------------------------------------------------
;//
;//	MadTracker DSP Core
;//
;//		Platforms:	All
;//		Processors:	x86
;//
;//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
;//
;//	$Id: MTDSPASM.inc 111 2007-02-16 12:58:43Z Yannick $
;//
;//---------------------------------------------------------------------------

; MTDSP1.h

; EAX = <Free>
; EBX = Status
; ECX = Counter
; EDX = Cursor fractional 
; ESI = Cursor integer (+ memory offset)
; EDI = Dest
; EBP = <Free>

struc ChannelStatus
	posi:			resd 1
	posd:			resd 1
	pitch:			resq 1
	pitchvar:		resq 1
	vol:			resq 1
	volvar:			resq 1
	multiplier:		resq 1
	pitchvarlng:	resd 1
	volvarlng:		resd 1
	tc:				resb 1
	sc:				resb 1
	reverse:		resb 1
	reserved:		resb 1
endstruc

%define varsize 32
%define localsize 16+varsize

; void MTACT ResampleProc
; (
;	char *dest,            // +4
;	char *source,          // +8
;	int count,             // +12
;	ChannelStatus &status, // +16
; )

%define dest [esp+localsize+4]
%define source [esp+localsize+8]
%define count [esp+localsize+12]
%define status [esp+localsize+16]
%define pitchd [esp]
%define pitchi [esp+4]
%define var1 [esp+8]
%define var2 [esp+12]
%define var3 [esp+16]
%define var4 [esp+20]

%macro	go_forward 0

	add		edx,pitchd
	adc		esi,pitchi

%endmacro

%macro	go_backward 0

	sub		edx,pitchd
	sbb		esi,pitchi

%endmacro

%macro	do_volvar 0

;	fadd	qword [ebx+volvar]
	fadd	st0,st1

%endmacro

%macro	floattofixed 2

	mov		eax,dword [%1+4]
	test	eax,0x7F800000
	mov		edx,dword [%1]
	jnz		%%ok
	xor		eax,eax
	xor		edx,edx
	jmp		%%exit
%%ok:
	mov		ecx,eax
	shr		ecx,20
	and		eax,0xFFFFF
	sub		ecx,0x3FF+20
	or		eax,0x100000
	neg		cl
	cmp		cl,32
	jae		%%s32
	shrd	edx,eax,cl
	shr		eax,cl
	jmp		%%exit
%%s32:
	mov		edx,eax
	sub		cl,32
	xor		eax,eax
	shr		edx,cl
%%exit:
	mov		dword [%2+4],eax
	mov		dword [%2],edx

%endmacro

%macro	resample_proc 2

%if %2==8
	%assign	bshift 0
%elif %2==16
	%assign	bshift 1
%else
	%assign	bshift 2
%endif

;%ifdef ELF
;cppglobal a_resample_%1_%2,_Z19a_resample_%1_%2PcS_iR13ChannelStatus
;%else
;cppglobal a_resample_%1_%2,?a_resample_%1_%2@@YAXPAD0HAAUChannelStatus@@@Z
;%endif
cglobal a_resample_%1_%2

align 16
a_resample_%1_%2:
	push	ebp
	push	esi
	push	edi
	push	ebx
	sub		esp,varsize
	mov		ebx,status
	floattofixed ebx+pitch,esp
	mov		ecx,count
	mov		esi,source
	mov		edi,dest
%if %2>8
	shr		esi,bshift
%endif
	mov		edx,[ebx+posd]
	fld		qword [ebx+volvar]
%ifidn %1,none
%else
	fmul	qword [d32bit]
%endif
	fld		qword [ebx+vol]
%ifidn %1,none
%else
	fmul	qword [d32bit]
%endif
	sub		edi,s_sample

	cmp		byte [ebx+reverse],0
	jne		near %%reverse

	cmp		dword [ebx+volvarlng],0
	jne		near %%volvarf	

%%mixf:
	add		edi,s_sample
	%1_%2
	go_forward
	dec		ecx
	jnz		%%mixf
	jmp		%%done

%%volvarf:
	add		edi,s_sample
	%1_%2
	go_forward
	do_volvar
	dec		ecx
	jnz		%%volvarf
	jmp		%%done

%%reverse
	cmp		dword [ebx+volvarlng],0
	jne		%%volvarb	

%%mixb:
	add		edi,s_sample
	%1_%2
	go_backward
	dec		ecx
	jnz		%%mixb
	jmp		%%done

%%volvarb:
	add		edi,s_sample
	%1_%2
	go_backward
	do_volvar
	dec		ecx
	jnz		%%volvarb
	
%%done:
	fstp	qword var1
	fstp	qword var1
	add		esp,varsize
	pop		ebx
	pop		edi
	pop		esi
	pop		ebp
	ret

%endmacro

section .data

align 16
m32bit:	dq		2147483647.0e0
d32bit:	dq		4.656612875245796924105750827168e-10
