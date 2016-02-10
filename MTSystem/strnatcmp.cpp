//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: strnatcmp.cpp 73 2005-08-28 12:52:11Z Yannick $
//
//---------------------------------------------------------------------------
#include <string.h>
//---------------------------------------------------------------------------

static // limit function scope to this TLU, it's never needed elsewhere.
int cmpright(const char **a,const char **b)
{
	int bias = 0;
	char ca,cb;

	for (;;(*a)++,(*b)++){
		ca = **a;
		cb = **b;
		if (((ca<'0') || (ca>'9')) && ((cb<'0') || (cb>'9'))) return bias;
		if ((ca<'0') || (ca>'9')) return -1;
		if ((cb<'0') || (cb>'9')) return 1;
		if (ca<cb){
			if (!bias)
				bias = -1;
		}
		else if (ca>cb){
			if (!bias)
                bias = 1;
		};
	};
	return 0;
}

static // same as above
int cmpleft(const char **a,const char **b)
{
	char ca,cb;

	for (;;(*a)++,(*b)++){
		ca = **a;
		cb = **b;
		if (((ca<'0') || (ca>'9')) && ((cb<'0') || (cb>'9'))) return 0;
		if ((ca<'0') || (ca>'9')) return -1;
		if ((cb<'0') || (cb>'9')) return 1;
		if (ca<cb)return -1;
		else if (ca>cb) return 1;
	};
	return 0;
}

// A natural string comparison, meaning it would procude x1,x2,x10 instead of standard strcomp x1,x10,x2.
// Other than having it use std::string instead of const char*, there's not much of a point in refactoring.
// However, using std::string might produce encoding problems, so maybe nothing needs to be done at all.
int strnatcmp(const char *a,const char *b)
{
	char ca,cb;
	const char *pa,*pb;
	int fractional,result;
	int la = strlen(a);
	int lb = strlen(b);

	if ((la==0) || (lb==0)) return la-lb;
	pa = a;
	pb = b;
	fractional = result = 0;
	while (true){
		ca = *pa;
		cb = *pb;
		if (ca==0){
			if (cb==0) return 0;
			return -1;
		}
		else if (cb==0) return 1;
		while ((ca==' ') || (ca=='\t')) ca = *++pa;
		while ((cb==' ') || (cb=='\t')) cb = *++pb;
		if (((ca>='0') && (ca<='9')) && ((cb>='0') && (cb<='9'))){
			fractional = ((ca=='0') || (cb=='0'));
			if (fractional) result = cmpleft(&pa,&pb);
			else result = cmpright(&pa,&pb);
			if (result) return result;
			if ((*pa==0) && (*pb==0)) return 0;
			ca = *pa;
			cb = *pb;
		};
		if (ca<cb) return -1;
		else if (ca>cb) return 1;
		pa++;
		pb++;
	};
}
//---------------------------------------------------------------------------
