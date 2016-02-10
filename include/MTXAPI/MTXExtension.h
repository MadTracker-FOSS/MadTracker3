//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id: MTXExtension.h 106 2005-11-30 21:08:42Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTXEXTENSION_INCLUDED
#define MTXEXTENSION_INCLUDED

//
//	Calling Convention
//

#include <stdint.h>

//TODO This entire call convention bit may be useless as soon as the inline assembly is gone.
#ifdef __GNUC__
#	ifndef __stdcall
#		define __stdcall __attribute__((__stdcall__))
#	endif
#	ifndef __cdecl
#		define __cdecl
#	endif
#	define MTEXPORT __attribute__((visibility("default")))
#else
#	define MTEXPORT __declspec( dllexport )
#endif

#define MTCT __cdecl

#undef WINDOWS
#undef WIN32
#undef WIN64

#if defined(_MSC_VER)
#	define WINDOWS
#	if defined(_WIN64)
#		define WIN64
#	elif defined(_WIN32)
#		define WIN32
#	endif
#elif defined(__BORLANDC__)
#	define WINDOWS
#	if defined(_WIN64)
#		define WIN64
#	elif defined(_WIN32)
#		define WIN32
#	endif
#elif defined(__MACOSX__)
#	define MACOSX
#else
#	define LINUX
#	if defined(__x86_64) || defined(_x86_64)
#		define LINUX64
#	else
#		define LINUX32
#	endif
#endif

#ifdef WINDOWS
#	define NL "\r\n"
#else
#	define NL "\n"
#endif

#ifdef MTVERSION_COMMERCIAL
#	define MTVERSION_PROFESSIONAL
#endif
#ifdef MTVERSION_PROFESSIONAL
#	define MTVERSION_BASIC
#endif

//
//	Uncomment the following line if you compile under a big endian processor
//	(PowerPC, etc.)
//

//#define BIG_ENDIAN 1234

//
//	Uncomment the following line if you compile for the 64-bit version
//

//#define MT3_64BIT

#if (BIG_ENDIAN==1234)
#	define FOURCC(a,b,c,d) ((((long)a)<<24) | (((long)b)<<16) | (((long)c)<<8) | (((long)d)<<0))
#else
#	define FOURCC(a,b,c,d) ((((long)d)<<24) | (((long)c)<<16) | (((long)b)<<8) | (((long)a)<<0))
#endif

//TODO replace by function templates std::min and std::max to avoid type erasure.
#ifndef MIN
#	define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#	define MAX(a,b) (((a)>(b))?(a):(b))
#endif

//
//	Data types
//

// Commented out by irrenhaus3, replaced down below with standard library typedefs.

//#if defined(_WIN32)
//#	if defined(__CYGWIN__)
//#		include <_G_config.h>
//		typedef _G_int64_t mt_int64;
//		typedef _G_int32_t mt_int32;
//		typedef _G_uint32_t mt_uint32;
//		typedef _G_int16_t mt_int16;
//		typedef _G_uint16_t mt_uint16;
//#	elif defined(__MINGW32__)
//		typedef short mt_int16;
//		typedef unsigned short mt_uint16;
//		typedef int mt_int32;
//		typedef unsigned int mt_uint32;
//		typedef long long mt_int64;
//		typedef unsigned long long mt_uint64;
//#	elif defined(__MWERKS__)
//		typedef long long mt_int64;
//		typedef int mt_int32;
//		typedef unsigned int mt_uint32;
//		typedef short mt_int16;
//		typedef unsigned short mt_uint16;
//#	else	/* MSVC/Borland */
//		typedef __int64 mt_int64;
//		typedef __int32 mt_int32;
//		typedef unsigned __int32 mt_uint32;
//		typedef __int16 mt_int16;
//		typedef unsigned __int16 mt_uint16;
//#	endif
//#elif defined(__MACOS__)
//#	include <sys/types.h>
//	typedef SInt16 mt_int16;
//	typedef UInt16 mt_uint16;
//	typedef SInt32 mt_int32;
//	typedef UInt32 mt_uint32;
//	typedef SInt64 mt_int64;
//#elif defined(__MACOSX__)	/* MacOS X Framework build */
//#	include <sys/types.h>
//	typedef int16_t mt_int16;
//	typedef u_int16_t mt_uint16;
//	typedef int32_t mt_int32;
//	typedef u_int32_t mt_uint32;
//	typedef int64_t mt_int64;
//#elif defined(__BEOS__)		/* Be */
//#	include <inttypes.h>
//	typedef int16_t mt_int16;
//	typedef u_int16_t mt_uint16;
//	typedef int32_t mt_int32;
//	typedef u_int32_t mt_uint32;
//	typedef int64_t mt_int64;
//#elif defined (__EMX__)		/* OS/2 GCC */
//	typedef short mt_int16;
//	typedef unsigned short mt_uint16;
//	typedef int mt_int32;
//	typedef unsigned int mt_uint32;
//	typedef long long mt_int64;
//#elif defined (DJGPP)		/* DJGPP */
//	typedef short mt_int16;
//	typedef int mt_int32;
//	typedef unsigned int mt_uint32;
//	typedef long long mt_int64;
//#elif defined(R5900)		/* PS2 EE */
//	typedef long mt_int64;
//	typedef int mt_int32;
//	typedef unsigned mt_uint32;
//	typedef short mt_int16;
//#elif defined(__SYMBIAN32__)	/* Symbian GCC */
//	typedef signed short mt_int16;
//	typedef unsigned short mt_uint16;
//	typedef signed int mt_int32;
//	typedef unsigned int mt_uint32;
//	typedef long long int mt_int64;
//#else
//	#include <sys/types.h>
//	typedef int16_t mt_int16;
//	typedef u_int16_t mt_uint16;
//	typedef int32_t mt_int32;
//	typedef u_int32_t mt_uint32;
//	typedef int64_t mt_int64;
//#endif
//typedef char mt_int8;
//typedef unsigned char mt_uint8;

//
// Standard data types from stdint.h
//
using mt_int8  = int8_t;
using mt_int16 = int16_t;
using mt_int32 = int32_t;
using mt_int64 = int64_t;
using mt_uint8 = uint8_t;
using mt_uint16= uint16_t;
using mt_uint32= uint32_t;
using mt_uint64= uint64_t;

#ifdef MT3_64BIT
	typedef double sample;
#	define s_sample 8
#	ifdef __GNUC__
#		define spls "l"
#		define spll "8"
#	else
#		define a_sample qword
#	endif
#else
	typedef float sample;
#	define s_sample 4
#	ifdef __GNUC__
#		define spls "s"
#		define spll "4"
#	else
#		define a_sample dword
#	endif
#endif

#define MTX_INITIALIZED	1

// SP stands for system path! Also, I really really hope no parts of this code
// rely on the actual integer values of this enum.
enum{
	SP_ROOT = 0,
	SP_USER,
	SP_CONFIG,
	SP_USERCONFIG,
	SP_EXTENSIONS,
	SP_SKINS,
	SP_INTERFACE,
	SP_HELP,
	SP_MAX
};

// User path, probably.
enum{
	UP_TEMP = 0,
	UP_CACHE,
	UP_MAX
};

// Functions that are also represented by GUI buttons.
enum{
	P_LOADMODULE = 0x1000,
	P_SAVEMODULE,
	P_LOADINSTRUMENT,
	P_SAVEINSTRUMENT,
	P_LOADSAMPLE,
	P_SAVESAMPLE,
	P_BUILDPEAKS =  0x2000
};
//---------------------------------------------------------------------------
struct MTPreferences{
	char options[16];   // <- this is just cruel. There don't even seem to be usages.
	char *path[8];      // <- 8 paths. For whatever.
	char *syspath[8];   // <- 8 syspaths. And yes, there is an iteration over these.
};

// Some apparently meaningless key constants that will come back and haunt
// my wildest nightmares in about a week.
struct MTXKey{
	int key1,key2,key3,key4;
};

// Operator overloading! It almost feels like C++. Except operator= returns bool.
// Apparently I deserve this.
struct MTUserID{
	mt_uint32 id1;
	mt_uint32 id2;
	inline bool operator == (MTUserID &u){ return ((id1==u.id1) && (id2==u.id2)); }
	inline bool operator = (MTUserID u){ id1 = u.id1; id2 = u.id2; return true; }
};

// Access control. TODO: Remove this; this software is open source now.
const MTUserID MTUID_EVERYONE   = {0,0x00000000};
const MTUserID MTUID_BASICREG   = {0,0x00000001};
const MTUserID MTUID_PROREG     = {0,0x00000002};
const MTUserID MTUID_BETATESTER = {0,0x00000003};
const MTUserID MTUID_MTSTAFF    = {0,0x00000004};

// Color representation struct.
// TODO Use constexpr constructor and a constexpr function to calculate RGBA.
// This should be trivial.
struct MTColor{
	union{
		struct{
			mt_uint8 R,G,B,A;
		};
		mt_uint32 RGBA;
	};
	MTColor(mt_uint32 rgba = 0){ RGBA = rgba; }
	MTColor(mt_uint8 r,mt_uint8 g,mt_uint8 b,mt_uint8 a){ R = r; G = g; B = b; A = a; }
	inline bool operator == (MTColor &c){ return (RGBA==c.RGBA); }
	inline bool operator = (mt_uint32 i){ RGBA = i; return true; }
	inline MTColor operator + (MTColor &c){ return MTColor(MIN(255,(int)R+c.R),MIN(255,(int)G+c.G),MIN(255,(int)B+c.B),MIN(255,(int)A+c.A)); }
	inline MTColor operator * (MTColor &c){ return MTColor((int)R*c.R/255,(int)G*c.G/255,(int)B*c.B/255,(int)A*c.A/255); }
	inline bool operator += (MTColor &c){ R = MIN(255,(int)R+c.R); G = MIN(255,(int)G+c.G); B = MIN(255,(int)B+c.B); A = MIN(255,(int)A+c.A); return true; }
	inline bool operator *= (MTColor &c){ R = (int)R*c.R/255; G = (int)G*c.G/255; B = (int)B*c.B; A = (int)A*c.A/255; return true; }
};

// CL stands for command line. And that pretty much tells us everything.
// Please let the usage of this be reasonable.
struct MTCLParam{
	char *name;
	char *value;
};

// Parent class of The Big Bad Wolf MTSystemInterface (aka si).
// I guess that's what the virtual functions are for.
// TODO declare the overriding ones as "override" in the wolf.
class MTXInterface{
public:
	int type;
	const char *name;
	int version;
	const MTXKey *key;
	int status;

	virtual bool MTCT init() = 0;
	virtual void MTCT uninit() = 0;
	virtual void MTCT start() = 0;
	virtual void MTCT stop() = 0;
	virtual void MTCT processcmdline(void *params){ };
	virtual void MTCT showusage(void *out){ };
	virtual int MTCT config(int command,int param){ return 0; };
	virtual int MTCT processinput(const char *input){ return 0; };
};

// Are you shitting me?
struct MTXInterfaces{
	int ninterfaces;
	MTXInterface* interfaces[1];
};

// Alright? "Refresh" could mean a lot of things, but nothing too bad so far.
// Will have to see how and where it's used to get an idea of how bad it is.
// My guess is that it means screen refresh.
typedef void (MTCT *RefreshProc)(void *param);

// Another generic term. "Object". But since there's a lock function and
// a modification flag on it, I'll take a guess and say that this includes
// anything that's part of the creative process. Modules, patterns, instruments,
// samples, etc. etc. etc.
// and that this type provides the common interface for all of them.
// However, this class itself appears to be pointless since its only usage
// is that it's inherited by MTObjectWrapper.
class ObjectWrapper{
public:
	virtual void MTCT create(void *object,void *parent,mt_uint32 type,mt_int32 i) = 0;
	virtual void MTCT destroy(void *object) = 0;
	virtual bool MTCT lock(void *object,int flags,bool lock,int timeout = -1) = 0;
	virtual void MTCT setname(void *object,char *newname) = 0;
	virtual void MTCT setmodified(void *object,int value,int flags) = 0;
};

// So now there's MTXInterface and also MTInterface. And both of them are
// abstract and have some members with identical signatures.
// I don't like where this is going.
class MTInterface{
public:
	int type;
	const char *name;
	int version;
	const MTXKey *key;

	ObjectWrapper *_ow;

	virtual MTXInterface* MTCT getinterface(int id) = 0;
	virtual MTPreferences* MTCT getprefs() = 0;
	virtual void* MTCT getcurrentuser() = 0;
	virtual int MTCT getnummodules() = 0;
	virtual void* MTCT getmodule(int id) = 0;
	virtual void MTCT addmodule(void *module) = 0;
	virtual void MTCT delmodule(void *module) = 0;
	virtual void MTCT setmodule(void *module) = 0;
	virtual bool MTCT addchannel() = 0;
	virtual void MTCT notify(void *object,int notify,int param) = 0;
	virtual bool MTCT editobject(void *object,bool newwindow) = 0;
	virtual void* MTCT getconf(const char *name,bool user) = 0;
	virtual void MTCT releaseconf(void *conf) = 0;
	virtual int MTCT addrefreshproc(RefreshProc proc,void *param) = 0;
	virtual void MTCT delrefreshproc(int id) = 0;
	virtual char* MTCT getextension(void *ptr) = 0;
	virtual bool MTCT canclose() = 0;
	virtual void* MTCT getconsole(){ return 0; };
	virtual int MTCT processinput(const char *input){ return 0; };
};

typedef MTXInterfaces* MTCT MTXMainCall(MTInterface *mti);
//---------------------------------------------------------------------------
//	Swapping functions to convert little<->big endian
//---------------------------------------------------------------------------

//TODO These may all become unneccessary as soon as the assembly stuff is gone.
// There may be some I/O on binary files though that depends on it.
// We'll have to see.

#ifdef FASTCALL
#error FASTCALL previously defined?
#endif

#if defined(__GNUC__)
	#if defined(__i386__)
		#define FASTCALL __attribute__((fastcall))
	#else
		#define FASTCALL
	#endif
#elif defined(_MSC_VER)
	#if defined(_WIN64) // FIXME: What about other architectures? -flibit
		#define FASTCALL
	#else
		#define FASTCALL __fastcall
	#endif
#else
	#error Need a fastcall macro for this compiler!
#endif

inline short FASTCALL swap_word(short a)
{
	return (a>>8)|((a & 0xFF)<<8);
}

inline long FASTCALL swap_dword(long a)
{
	return (a>>24)|((a & 0xFF0000)>>8)|((a & 0xFF00)<<8)|((a & 0xFF)<<24);
}

inline void FASTCALL int64todouble(void *int64,double *d)
{
	*d = ((double)(*(int*)int64))*4294967296.0+(double)(*(((int*)int64)+1));
}

#undef FASTCALL

//---------------------------------------------------------------------------
//	Some functions that are not part of the C++ standard library
//---------------------------------------------------------------------------

// Why only on non-windows platforms? This can be done with the standard
// library tools using std::locale - though it might be finnicky with some
// setups. idk, we'll have to go trial&error on that one.

#ifndef _WIN32
#include <ctype.h>

inline char* strupr(char* str)
{
	char* orig = str;
	for (;*str!='\0';str++) *str = toupper(*str);
	return orig;
}

inline char* strlwr(char* str)
{
	char* orig = str;
	for (;*str!='\0';str++) *str = tolower(*str);
	return orig;
}

#define stricmp strcasecmp
#endif
//---------------------------------------------------------------------------
#endif // MTXEXTENSION_INCLUDED
