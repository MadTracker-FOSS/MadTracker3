//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSystem.cpp 82 2005-08-30 15:52:57Z Yannick $
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#endif
//---------------------------------------------------------------------------
#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	return TRUE;
}
#else
int main(int argc,const char* argv[])
{
	return 0;
}
#endif
//---------------------------------------------------------------------------
