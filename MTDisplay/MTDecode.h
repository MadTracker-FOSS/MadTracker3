//---------------------------------------------------------------------------
#ifndef MTDECODE_INCLUDED
#define MTDECODE_INCLUDED
//---------------------------------------------------------------------------
void* loadgif(MTFile *f,int &colorkey);
void* loadtif(MTFile *f,int &colorkey);
void* loadpng(MTFile *f);
void* loadjpg(MTFile *f);
//---------------------------------------------------------------------------
#endif
