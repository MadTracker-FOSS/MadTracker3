//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXSystem2.h 77 2005-08-28 20:51:37Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXSYSTEM2_INCLUDED
#define MTXSYSTEM2_INCLUDED
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#include "MTXSystem2.cpp"
#else
#ifdef __GNUG__
#include <string.h>
#endif
inline void mtmemzero(void *mem,int count)
{
#ifndef __GNUG__
	__asm{
		push	edi
		mov		ecx,count
		mov		edi,mem
		test	ecx,ecx
		mov		edx,ecx
		jz		lexit
		and		edx,3
		xor		eax,eax
		sub		ecx,edx
		jz		part2
		shr		ecx,2
		rep		stosd
part2:
		add		ecx,edx
		jz		lexit
		rep		stosb
lexit:
		pop		edi
	};
#else
	memset(mem,0,count);
#endif
}

inline void mtmemzero64(void *mem,int count)
{
#ifndef __GNUG__
	__asm{
		push	edi
		mov		ecx,count
		mov		edi,mem
		shl		ecx,1
		xor		eax,eax
		rep		stosd
		pop		edi
	};
#else
	memset(mem,0,count*8);
#endif
}
#endif
//---------------------------------------------------------------------------
#endif
