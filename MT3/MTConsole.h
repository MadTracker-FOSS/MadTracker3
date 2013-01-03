//---------------------------------------------------------------------------
//	$Id: MTConsole.h 100 2005-11-30 20:19:39Z Yannick $
//---------------------------------------------------------------------------
#ifndef MTCONSOLE_INCLUDED
#define MTCONSOLE_INCLUDED

#include <MTXAPI/MTXExtension.h>
#include <MTXAPI/MTXSystem.h>
//---------------------------------------------------------------------------
class MTConsole : public MTFile{
public:
	MTConsole(MTInterface *mti);
	MTConsole(MTFile *parent,int start,int end,int access);
	~MTConsole();
	int MTCT read(void *buffer,int size);
	int MTCT readln(char *buffer,int maxsize);
//	int MTCT reads(char *buffer,int maxsize);
	int MTCT write(const void *buffer,int size);
	int MTCT seek(int pos,int origin);
	void* MTCT getpointer(int offset,int size);
	void MTCT releasepointer(void *mem);
	int MTCT length();
	int MTCT pos();
	bool MTCT eof();
	bool MTCT seteof();
	bool MTCT gettime(int *modified,int *accessed);
	bool MTCT settime(int *modified,int *accessed);
	MTFile* MTCT subclass(int start,int length,int access);
	virtual int MTCT userinput(const char *input);
};
//---------------------------------------------------------------------------
#endif
