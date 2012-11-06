//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension File
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXSystem2.cpp 77 2005-08-28 20:51:37Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
void mtmemzero(void *mem,int count)
{
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
}

void mtmemzero64(void *mem,int count)
{
	__asm{
		push	edi
		mov		ecx,count
		mov		edi,mem
		shl		ecx,1
		xor		eax,eax
		rep		stosd
		pop		edi
	};
}
#endif
//---------------------------------------------------------------------------
