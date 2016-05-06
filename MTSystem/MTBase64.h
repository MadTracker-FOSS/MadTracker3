//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTBase64.h 68 2005-08-26 22:19:12Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTBASE64_INCLUDED
#define MTBASE64_INCLUDED
//---------------------------------------------------------------------------

/*
 * Base64 strings: https://en.wikipedia.org/wiki/Base64
 * The decode and decode_len functions are unused.
 */
extern "C" {
int mtbase64decode_len(const char *bufcoded);
int mtbase64decode(char *bufplain, const char *bufcoded);
int mtbase64decode_binary(unsigned char *bufplain, const char *bufcoded);
int mtbase64encode_len(int len);
int mtbase64encode(char *encoded, const char *string, int len);
int mtbase64encode_binary(char *encoded, const unsigned char *string, int len);
}
//---------------------------------------------------------------------------
#endif
