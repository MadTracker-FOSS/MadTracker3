;//---------------------------------------------------------------------------
;//
;//	MadTracker 3 Public Extension Header
;//
;//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
;//
;//	http://www.madtracker.org/
;//	info@madtracker.org
;//
;//	$Id: MTXASM.inc 98 2005-11-30 20:07:26Z Yannick $
;//
;//---------------------------------------------------------------------------

bits 32

%macro cglobal 1
	%ifdef PREFIX
		global _%1
		%define %1 _%1
	%else
		global %1
	%endif
%endmacro

%macro cppglobal 2
	%ifdef PREFIX
		global %2
		%define %1 %2
	%else
		global %1
	%endif
%endmacro

%define a_sample dword
%define s_sample 4
