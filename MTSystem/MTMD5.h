//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTMD5.h 49 2005-08-21 10:00:44Z Yannick $
//
//---------------------------------------------------------------------------
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
// 
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
// 
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
// 
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
// 
// These notices must be retained in any copies of any part of this
// documentation and/or software.
//---------------------------------------------------------------------------
#ifndef MTMD5_INCLUDED
#define MTMD5_INCLUDED
//---------------------------------------------------------------------------

// I guess this isn't too bad, altough I'd like to have a C++ interface to it
// instead of a char* destination buffer. What's kind of annoying about it is
// that there's two versions for signed and unsigned char.
// Also, password is an unused parameter in both versions.

struct MD5_CTX{
	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];
};
//---------------------------------------------------------------------------
extern "C"
{
void md5(char *dest,const char *src,int size = 0,char *password = 0);
void md5b(unsigned char *dest,const char *src,int size = 0,char *password = 0);
}
//---------------------------------------------------------------------------
#endif
