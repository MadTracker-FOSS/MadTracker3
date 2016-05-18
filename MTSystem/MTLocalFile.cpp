//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTLocalFile.cpp 111 2007-02-16 12:58:43Z Yannick $
//
//---------------------------------------------------------------------------
#include <time.h>

#ifndef _WIN32

#	include <utime.h>
#	include <sys/mman.h>

#endif

#include "MTLocalFile.h"
#include <MTXAPI/MTXSystem2.h>

//---------------------------------------------------------------------------
MTLocalHook localhook;

int allocalign = 0;

char rootn[64] = {"Desktop"};

//---------------------------------------------------------------------------
MTLocalHook::MTLocalHook()
{
}

MTFile *MTLocalHook::fileopen(const char *url, int flags)
{
    return new MTLocalFile(url, flags);
}

MTFolder *MTLocalHook::folderopen(char *url)
{
    return new MTLocalFolder(url);
}

bool MTLocalHook::filecopy(char *source, char *dest)
{
#	ifdef _WIN32
    return (CopyFile(source,dest,false)==TRUE);
#	else
    const int BUFSIZE = 4096;
    char buf[BUFSIZE];
    int n;
    int sf, df;
    sf = open(source, 0);
    if (sf == 0)
    { return false; }
    df = creat(dest, 0644);
    if (df == 0)
    {
        close(sf);
        return false;
    };
    while((n = read(sf, buf, BUFSIZE)) != -1)
    {
        if (write(df, buf, n) != n)
        {
            close(sf);
            close(df);
            return false;
        };
    };
    return true;
#	endif
}

bool MTLocalHook::filerename(char *source, char *dest)
{
#	ifdef _WIN32
    return (MoveFile(source,dest)==TRUE);
#	else
    return (rename(source, dest) == 0);
#	endif
}

bool MTLocalHook::filedelete(char *url)
{
#	ifdef _WIN32
    return (DeleteFile(url)==TRUE);
#	else
    return (unlink(url) == 0);
#	endif
}

void MTLocalHook::filetype(const char *url, char *type, int length)
{
    char *e;

    e = (char *) strrchr(url, '.');
    if ((e) && ((strchr(e, '\\')) || (strchr(e, '/'))))
    { e = 0; }
    if (e)
    {
        strncpy(type, e, length);
        strlwr(type);
    };
}

//---------------------------------------------------------------------------
MTLocalFile::MTLocalFile(const char *path, int access):
    maccess(access), stdhandle(true), cpos(0), from(0), to(0x7FFFFFFF),
#ifdef _WIN32
    maph(0),
#endif
    mmap(0)
{
    // FIXME: This function deals with strings like crap. -flibit

    mtsetlasterror(0);
    const char *e = strstr(path, "://");
    if (e)
    {
        path = e + 3;
    }
    e = path;
    if (strcmp(path, "stdin") == 0)
    {
#		ifdef _WIN32
        h = GetStdHandle(STD_INPUT_HANDLE);
#		else
        fs = stdin;
#		endif
    }
    else if (strcmp(path, "stdout") == 0)
    {
#		ifdef _WIN32
        h = GetStdHandle(STD_OUTPUT_HANDLE);
#		else
        fs = stdout;
#		endif
    }
    else if (strcmp(path, "stderr") == 0)
    {
#		ifdef _WIN32
        h = GetStdHandle(STD_ERROR_HANDLE);
#		else
        fs = stderr;
#		endif
    }
    else
    {
        stdhandle = false;
#		ifdef _WIN32
        int waccess = 0;
        int wshare = 0;
        int wcreate = 0;
        int wattr = 0;
        if (access & MTF_READ) waccess |= GENERIC_READ;
        if (access & MTF_WRITE) waccess |= GENERIC_WRITE;
        if (access & MTF_SHAREREAD) wshare |= FILE_SHARE_READ;
        if (access & MTF_SHAREWRITE) wshare |= FILE_SHARE_WRITE;
        if (access & MTF_CREATE) wcreate |= OPEN_ALWAYS;
        else wcreate |= OPEN_EXISTING;
        if (access & MTF_TEMP) wattr |= FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE;
        h = CreateFile(e,waccess,wshare,0,wcreate,wattr,0);
#		else
        const char *laccess;
        switch (access & (MTF_READ | MTF_WRITE | MTF_CREATE))
        {
            case MTF_READ | MTF_WRITE:
                laccess = "r+";
                break;
            case MTF_WRITE:
            case MTF_WRITE | MTF_CREATE:
                laccess = "w";
                break;
            case MTF_READ | MTF_WRITE | MTF_CREATE:
                laccess = "a+";
                break;
            default:
                laccess = "r";
                break;
        };
        fs = fopen(e, laccess);
        if (fs)
        { fseek(fs, 0, SEEK_SET); }
#		endif
    };
#	ifdef _WIN32
    if (h==INVALID_HANDLE_VALUE){
        switch (GetLastError()){
        case 2:
        case 3:
        case 15:	// File not found
            mtsetlasterror(1);
            break;
        case 5:
        case 19:
        case 32:	// Access denied
            mtsetlasterror(3);
            break;
        default:
            mtsetlasterror(-1);
            break;
        };
    };
#	else
    if (fs == 0)
    {
        switch (errno)
        {
            case ENODEV:
            case ENOENT:
                mtsetlasterror(1);
                break;
            case EACCES:
                mtsetlasterror(3);
                break;
            default:
                mtsetlasterror(-1);
                break;
        };
    };
#	endif
}

MTLocalFile::MTLocalFile(MTFile *parent, int start, int end, int access):
    maccess(access), cpos(start), from(start), to(start + end),
#ifdef _WIN32
    maph(0),
#endif
    mmap(0), stdhandle(((MTLocalFile *) parent)->stdhandle)
{
    mtsetlasterror(0);
#	ifdef _WIN32
    int waccess = 0;
    HANDLE p = GetCurrentProcess();
    if (access & MTF_READ) waccess |= GENERIC_READ;
    if (access & MTF_WRITE) waccess |= GENERIC_WRITE;
    if (!DuplicateHandle(p,((MTLocalFile*)parent)->h,p,&h,waccess,true,0)){
        switch (GetLastError()){
        case 2:
        case 3:
        case 15:	// File not found
            mtsetlasterror(1);
            break;
        case 5:
        case 19:	// Access denied
            mtsetlasterror(3);
            break;
        default:
            mtsetlasterror(-1);
            break;
        };
        FLOGD1("%s - [System] WARNING: Cannot subclass file! (Error %d)"NL,mtgetlasterror());
    };
    SetFilePointer(h,start,0,FILE_BEGIN);
#	else
    const char *laccess;
    switch (access & (MTF_READ | MTF_WRITE | MTF_CREATE))
    {
        case MTF_READ | MTF_WRITE:
            laccess = "r+";
            break;
        case MTF_WRITE:
        case MTF_WRITE | MTF_CREATE:
            laccess = "w";
            break;
        case MTF_READ | MTF_WRITE | MTF_CREATE:
            laccess = "w+";
            break;
        default:
            laccess = "r";
            break;
    };
    fs = fdopen(dup(fileno(((MTLocalFile *) parent)->fs)), laccess);
#	endif
}

MTLocalFile::~MTLocalFile()
{
#	ifdef _WIN32
    if (maph) CloseHandle(maph);
    if ((!stdhandle) && (h!=INVALID_HANDLE_VALUE)) CloseHandle(h);
#	else
    if (mmap)
    { munmap(mmap, maplength); }
    if ((!stdhandle) && (fs))
    { fclose(fs); }
#	endif
}

int MTLocalFile::read(void *buffer, int size)
{
    int read = 0;

    if (cpos + size > to)
    { size = to - cpos; }
    if (size <= 0)
    { return 0; }
#	ifdef _WIN32
    ReadFile(h,buffer,size,(DWORD*)&read,0);
#	else
    read = fread(buffer, 1, size, fs);
#	endif
    cpos += read;
    return read;
}

int MTLocalFile::readln(char *buffer, int maxsize)
{
    int read = 0;
    int x, y, i, r, r2;
    char *e;

    if (cpos + maxsize > to)
    { maxsize = to - cpos; }
    if (maxsize <= 0)
    { return 0; }
#	ifdef _WIN32
    x = maxsize;
    while (x>0){
        i = x;
        if (i>32) i = 32;
        ReadFile(h,buffer,i,(DWORD*)&r,0);
        if (buffer[i-1]=='\r'){
            e = strchr(buffer,'\r');
            if ((e==&buffer[i-1]) && (i<x)){
                ReadFile(h,&buffer[i++],1,(DWORD*)&r2,0);
                r += r2;
            };
        };
        if (r){
            for (y=0;y<r;y++){
                char c = *buffer++;
                if ((c==0) || (c=='\n') || (c=='\r')){
                    cpos++;
                    if ((y<r-1) && (c=='\r') && (*buffer=='\n')){
                        y++;
                        cpos++;
                    };
                    mtmemzero(buffer-1,maxsize-read);
                    SetFilePointer(h,y-r+1,0,FILE_CURRENT);
                    goto done;
                };
                read++;
            };
            if (r<i) break;
        }
        else break;
        x -= r;
    };
#	else
/*
		read = ftell(fs);
		fgets(buffer,maxsize,fs);
		read = ftell(fs)-read;
		e = strrchr(buffer,0);
		if (e>buffer){
			e--;
			while ((e>=buffer) && (*e=='\r') || (*e=='\n')){
				*e = 0;
				e--;
			};
		};
*/
    x = maxsize;
    while(x > 0)
    {
        i = x;
        if (i > 32)
        { i = 32; }
        r = fread(buffer, 1, i, fs);
        if (buffer[i - 1] == '\r')
        {
            e = strchr(buffer, '\r');
            if ((e == &buffer[i - 1]) && (i < x))
            {
                r2 = fread(&buffer[i++], 1, 1, fs);
                r += r2;
            };
        };
        if (r)
        {
            for(y = 0; y < r; y++)
            {
                char c = *buffer++;
                if ((c == 0) || (c == '\n') || (c == '\r'))
                {
                    cpos++;
                    if ((y < r - 1) && (c == '\r') && (*buffer == '\n'))
                    {
                        y++;
                        cpos++;
                    };
                    mtmemzero(buffer - 1, maxsize - read);
                    fseek(fs, y - r + 1, 1);
                    goto done;
                };
                read++;
            };
            if (r < i)
            { break; }
        }
        else
        { break; }
        x -= r;
    };
#	endif
    done:
    cpos += read;
    return read;
}

/*
int MTLocalFile::reads(char *buffer,int maxsize)
{
	int read = 0;
	
	if (cpos+maxsize>to) maxsize = to-cpos;
	if (maxsize<=0) return 0;
#	ifdef _WIN32
		int x,y,i,r;
		x = maxsize;
		while (x>0){
			i = x;
			if (i>32) i = 32;
			ReadFile(h,buffer,i,(DWORD*)&r,0);
			if (r){
				for (y=0;y<r;y++){
					char c = *buffer++;
					if (c==0){
						cpos++;
						mtmemzero(buffer-1,maxsize-read);
						SetFilePointer(h,y-r+1,0,FILE_CURRENT);
						goto done;
					};
					read++;
				};
				if (r<i) break;
			}
			else break;
			x -= r;
		};
done:
#	else
		read = ftell(fs);
		fgets(buffer,maxsize,fs);
		read = ftell(fs)-read;
#	endif
	cpos += read;
	return read;
}
*/
int MTLocalFile::write(const void *buffer, int size)
{
    int written = 0;

    if (cpos + size > to)
    { size = to - cpos; }
    if (size <= 0)
    { return 0; }
#	ifdef _WIN32
    WriteFile(h,buffer,size,(DWORD*)&written,0);
#	else
    written = fwrite(buffer, 1, size, fs);
#	endif
    cpos += written;
    return written;
}

int MTLocalFile::seek(int pos, int origin)
{
    if (origin == MTF_BEGIN)
    { pos += from; }
    if ((to < 0x7FFFFFFF) && (origin == MTF_END))
    {
        origin = MTF_BEGIN;
        pos = to - pos;
    };
#	ifdef _WIN32
    cpos = SetFilePointer(h,pos,0,origin);
#	else
    fseek(fs, pos, origin);
    cpos = ftell(fs);
#	endif
    return cpos - from;
}

int MTLocalFile::length()
{
    if (to < 0x7FFFFFFF)
    { return to - from; }
#	ifdef _WIN32
    return GetFileSize(h,0);
#	else
    struct stat s;
    fstat(fileno(fs), &s);
    return s.st_size;
#	endif
}

void *MTLocalFile::getpointer(int offset, int size)
{
    int waccess = 0;

#	ifdef _WIN32
    if ((!h) || (mmap)) return 0;
    if (offset<0) offset = cpos;
    else offset += from;
    if (size<0) size = GetFileSize(h,0)-offset;
    if (maccess & MTF_WRITE) waccess = PAGE_READWRITE;
    else if (maccess & MTF_READ) waccess = PAGE_READONLY;
    if (!maph) maph = CreateFileMapping(h,0,waccess,0,0,0);
    if (!maph) return 0;
    if (maccess & MTF_WRITE) waccess = FILE_MAP_WRITE;
    else if (maccess & MTF_READ) waccess = FILE_MAP_READ;
    if (allocalign==0) allocalign = sysinfo.dwAllocationGranularity;
    mapoffset = (offset/allocalign)*allocalign;
    mapoffset = offset-mapoffset;
    mmap = MapViewOfFile(maph,waccess,0,offset-mapoffset,size+mapoffset);
#	else
    if (mmap)
    { return 0; }
    if (offset < 0)
    {
        offset = cpos;
    }
    else
    { offset += from; }
    if (size < 0)
    { size = length() - offset; }
    if (maccess & MTF_READ)
    { waccess |= PROT_READ; }
    if (maccess & MTF_WRITE)
    { waccess |= PROT_WRITE; }
    if (allocalign == 0)
    { allocalign = getpagesize(); }
    mapoffset = (offset / allocalign) * allocalign;
    mapoffset = offset - mapoffset;
    maplength = size + mapoffset;
    mmap = ::mmap(0, maplength, waccess, MAP_PRIVATE, fileno(fs), offset - mapoffset);
#	endif
    if (!mmap)
    { return 0; }
    return (char *) mmap + mapoffset;
}

void MTLocalFile::releasepointer(void *mem)
{
#	ifdef _WIN32
    if ((maph) && (mmap) && ((char*)mmap==(char*)mem-mapoffset)){
        UnmapViewOfFile(mmap);
        mmap = 0;
    };
#	else
    if ((mmap) && ((char *) mmap == (char *) mem - mapoffset))
    {
        munmap(mmap, maplength);
        mmap = 0;
    };
#	endif
}

int MTLocalFile::pos()
{
    return cpos - from;
}

bool MTLocalFile::eof()
{
    if (cpos >= to)
    { return true; }
#	ifdef _WIN32
    return (cpos>=(int)GetFileSize(h,0));
#	else
    return (feof(fs) != 0);
#	endif
}

bool MTLocalFile::seteof()
{
#	ifdef _WIN32
    if (mmap) return false;
    if (maph){
        CloseHandle(maph);
        maph = 0;
    };
    return (SetEndOfFile(h)!=0);
#	else
    return (ftruncate(fileno(fs), ftell(fs)) == 0);
#	endif
}

bool MTLocalFile::gettime(int *modified, int *accessed)
{
#	ifdef _WIN32
    FILETIME fa,fm;
    FILETIME *pfa,*pfm;
    SYSTEMTIME stime;
    struct tm utime;

    pfm = pfa = 0;
    if (modified) pfm = &fm;
    if (accessed) pfa = &fa;
    if (GetFileTime(h,0,pfa,pfm)==0) return false;
    stime.wMilliseconds = 0;
    if (modified){
        FileTimeToSystemTime(pfm,&stime);
        utime.tm_year = stime.wYear-1900;
        utime.tm_mon = stime.wMonth-1;
        utime.tm_mday = stime.wDay+1;
        utime.tm_hour = stime.wHour;
        utime.tm_min = stime.wMinute;
        utime.tm_sec = stime.wSecond;
        *modified = mktime(&utime);
    };
    if (accessed){
        FileTimeToSystemTime(pfa,&stime);
        utime.tm_year = stime.wYear-1900;
        utime.tm_mon = stime.wMonth-1;
        utime.tm_mday = stime.wDay+1;
        utime.tm_hour = stime.wHour;
        utime.tm_min = stime.wMinute;
        utime.tm_sec = stime.wSecond;
        *accessed = mktime(&utime);
    };
#	else
    struct stat s;

    fstat(fileno(fs), &s);
    if (modified)
    { *modified = s.st_mtime; }
    if (accessed)
    { *accessed = s.st_atime; }
#	endif
    return true;
}

bool MTLocalFile::settime(int *modified, int *accessed)
{
#	ifdef _WIN32
    FILETIME fm,fa;
    FILETIME *pfm,*pfa;
    SYSTEMTIME stime;
    struct tm *utime;

    pfm = pfa = 0;
    stime.wMilliseconds = 0;
    if (modified){
        pfm = &fm;
        utime = gmtime((time_t*)modified);
        stime.wYear = utime->tm_year+1900;
        stime.wMonth = utime->tm_mon+1;
        stime.wDay = utime->tm_mday-1;
        stime.wHour = utime->tm_hour;
        stime.wMinute = utime->tm_min;
        stime.wSecond = utime->tm_sec;
        SystemTimeToFileTime(&stime,pfm);
    };
    if (accessed){
        pfa = &fa;
        utime = gmtime((time_t*)accessed);
        stime.wYear = utime->tm_year+1900;
        stime.wMonth = utime->tm_mon+1;
        stime.wDay = utime->tm_mday-1;
        stime.wHour = utime->tm_hour;
        stime.wMinute = utime->tm_min;
        stime.wSecond = utime->tm_sec;
        SystemTimeToFileTime(&stime,pfa);
    };
    return (SetFileTime(h,0,pfa,pfm)!=0);
#	else
    struct utimbuf utb;
    struct stat s;

    fstat(fileno(fs), &s);
    if (*modified)
    {
        utb.modtime = *modified;
    }
    else
    { utb.modtime = s.st_mtime; }
    if (*accessed)
    {
        utb.actime = *accessed;
    }
    else
    { utb.actime = s.st_atime; }
    return (utime(url, &utb) == 0);
#	endif
}

MTFile *MTLocalFile::subclass(int start, int length, int access)
{
    if (access == -1)
    { access = maccess; }
    if (start < 0)
    { start = cpos; }
    return new MTLocalFile(this, start, length, access);
}

//---------------------------------------------------------------------------
MTLocalFolder::MTLocalFolder(char *path):

#ifdef _WIN32

search(INVALID_HANDLE_VALUE),
#endif
id(0)
{
#	ifdef _WIN32
    char *me;
    int em;
    strcpy(mpath,path);
    me = strchr(mpath,0);
    if (me>mpath){
        if (*(me-1)=='\\') *(me-1) = 0;
    };
    if (mpath[1]==':'){
        strcat(mpath,"\\*.*");
        em = SetErrorMode(SEM_FAILCRITICALERRORS);
        search = FindFirstFile(mpath,&data);
        SetErrorMode(em);
    };
#	else
    d = opendir(path);
#	endif
}

MTLocalFolder::~MTLocalFolder()
{
#	ifdef _WIN32
    if (search!=INVALID_HANDLE_VALUE) FindClose(search);
#	endif
}

bool MTLocalFolder::getfile(const char **name, int *attrib, double *size)
{
    char tmp[512];

#	ifdef _WIN32
    if (search==INVALID_HANDLE_VALUE){
        if (mpath[1]==':') return false;
        if (mpath[0]==0){
            if (name) *name = rootn;
            if (attrib) *attrib = MTFA_ROOT;
            if (size) *size = 0.0;
            return true;
        };
        if (strcmp(mpath,"Root")) return false;
        char root[4] = {"C:\\"};
        SHFILEINFO info;
        mtmemzero(&info,sizeof(info));
        root[0] = id+'A';
        if (GetDriveType(root)==DRIVE_CDROM){
            strcpy(data.cFileName,"CD-ROM (");
            root[2] = ')';
            strcat(data.cFileName,root);
        }
        else{
            SHGetFileInfo(root,0,&info,sizeof(info),SHGFI_DISPLAYNAME);
            strcpy(data.cFileName,info.szDisplayName);
        };
        if (name) *name = data.cFileName;
        if (attrib) *attrib = MTFA_DISK;
        if (size) *size = 0.0;
        return true;
    };
    while ((data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) || (data.cFileName[0]=='.')){
        if (!next()) return false;
    };
    strcpy(tmp,data.cFileName);
    strupr(tmp);
    if (strcmp(data.cFileName,tmp)==0){
        strlwr(data.cFileName);
        data.cFileName[0] = tmp[0];
    };
    if (name) *name = data.cFileName;
    if (attrib){
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) *attrib = MTFA_FOLDER;
        else *attrib = MTFA_FILE;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) *attrib |= MTFA_HIDDEN;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) *attrib |= MTFA_READONLY;
    };
    if (size){
        int64todouble((mt_int64*)&data.nFileSizeHigh,size);
    };
#	else
    struct dirent *de;
    de = readdir(d);
    if (de == 0)
    { return false; }
    if (name)
    { *name = de->d_name; }
    if (attrib)
    {
        if (de->d_name[0] == '.')
        {
            if ((strcmp(de->d_name, ".") == 0) && (strcmp(de->d_name, "..") == 0))
            { *attrib |= MTFA_HIDDEN; }
        };
    };
    if ((attrib) || (size))
    {
        struct stat s;
        strcpy(tmp, mpath);
        strcat(tmp, "/");
        strcat(tmp, de->d_name);
        stat(tmp, &s);
        if (attrib)
        {
            if ((s.st_mode & S_IFMT) == S_IFDIR)
            { *attrib |= MTFA_FOLDER; }
        };
        if (size)
        { *size = s.st_size; }
    };
#	endif
    return true;
}

bool MTLocalFolder::next()
{
#	ifdef _WIN32
    if (search!=INVALID_HANDLE_VALUE) return FindNextFile(search,&data)!=0;
    if (mpath[0]==0) return false;
    if (strcmp(mpath,"Root")==0){
        int ld = GetLogicalDrives();
        ld >>= id;
        do{
            id++;
            ld >>= 1;
        } while ((ld) && (!(ld & 1)));
        if (ld) return true;
    };
    return false;
#	else
    return true;
#	endif
}
//---------------------------------------------------------------------------
