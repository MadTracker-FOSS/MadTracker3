//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright   1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTInternet.cpp 68 2005-08-26 22:19:12Z Yannick $
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTSocket.h"
#include "MTInternet.h"
#include <MTXAPI/MTXSystem2.h>
#include "MTBase64.h"

//---------------------------------------------------------------------------
struct MTCredentials
{
    int id;
    char* server;
    char* path;
    char* domain;
    char* authkey;
};

MTHash* creds;

//---------------------------------------------------------------------------
void initInternet()
{
    creds = new MTHash(4);
}

void MTCT delproc(void* data, void*)
{
    MTCredentials* ccred = (MTCredentials*) data;
    mtmemfree(ccred->server);
    mtmemfree(ccred->path);
    mtmemfree(ccred->domain);
    mtmemfree(ccred->authkey);
}

void uninitInternet()
{
    if (creds)
    {
        creds->clear(true, delproc);
        delete creds;
    };
}

//---------------------------------------------------------------------------
MTCredentials* newcredential(const char* server, const char* path, const char* domain, const char* key)
{
    MTCredentials* cred = mtnew(MTCredentials);
    char ckey[1024];

    if (creds->nitems == MAX_CREDENTIALS)
    {
        creds->delitemfromid(0, true, delproc);
    };
    cred->server = (char*) mtmemalloc(strlen(server) + 1);
    cred->path = (char*) mtmemalloc(strlen(path) + 1);
    cred->domain = (char*) mtmemalloc(strlen(domain) + 1);
    strcpy(cred->server, server);
    strcpy(cred->domain, domain);
    cred->authkey = (char*) mtmemalloc(mtbase64encode_len(strlen(key)));
    mtbase64encode(cred->authkey, key, strlen(key));
    strcpy(ckey, server);
    strcat(ckey, domain);
    cred->id = creds->additem(ckey, cred);
    return cred;
}

void deletecredentials(MTCredentials* cred)
{
    creds->delitemfromid(cred->id, true, delproc);
}

void newrequest(char* buffer)
{
    buffer[0] = 0;
}

void endrequest(char* buffer)
{
    strcat(buffer, "\r\n");
}

void addkey(char* buffer, const char* key, char* value)
{
    strcat(buffer, key);
    strcat(buffer, " ");
    strcat(buffer, value);
    if (strcmp(key, "GET") == 0)
    {
        strcat(buffer, " HTTP/1.0");
    }
    strcat(buffer, "\r\n");
}

bool getkey(const char* buffer, const char* key, char* value, int max)
{
    char* s, * e;
    int l;

    s = (char*) strstr(buffer, key);
    if (!s)
    {
        return false;
    }
    s += strlen(key);
    if (*s != ' ')
    {
        while(++*s != ' ')
        {
        }
    }
    while(++*s == ' ')
    {
    }
    e = strstr(s, "\r\n");
    l = e - s;
    if (l > max - 1)
    {
        l = max - 1;
    }
    memcpy(value, s, l);
    value[l] = 0;
    return true;
}

int ClientThread(MTThread* thread, void* f)
{
/*	MTFile &cf = *(MTFile*)f;
	char *request,*header,*status,*e,*e2;
	int l,result,rclass,len;
	bool h = true;
	bool cancontinue = false;
	bool needauth = false;
	int retries;
	char cpath[256];
	char realm[64];
	static char value[256];
	static const char *ftpuser = {"USER %s\r\n"};
	static const char *ftppass = {"PASS %s\r\n"};
	static const char *ftpport = {"PORT %d,%d,%d,%d,%d,%d\r\n"};
	static const char *ftpretr = {"RETR %s\r\n"};
	static char login[64];
	static char password[64];
	static char ckey[1024];
	static MTCredentials *lastcred;
	
	realm[0] = 0;
	cf.c = new MTClient(cf.host,(cf.filetype==MTFT_HTTP)?80:21);
	if (cf.filetype==MTFT_FTP) cf.s = new MTServer(7000,1);
	if (cf.filetype==MTFT_HTTP){
		strcpy(cpath,cf.filename);
		e = strrchr(cpath,'/');
		if (e) *(e+1) = 0;
		request = (char*)mtmemalloc(1024,MTM_ZERO);
		header = (char*)mtmemalloc(4096,MTM_ZERO);
		retries = -1;
retry:
		newrequest(request);
		if (retries++>=0){
			if (!cf.c->reconnect()) goto ftpexit;
		};
		addkey(request,"GET",cf.filename);
		addkey(request,"Host:",cf.host);
		addkey(request,"User-Agent:","MTSystem");
		addkey(request,"Referer:","MTSystem");
		strcpy(ckey,cf.host);
		strcat(ckey,realm);
		if ((!needauth) && (!lastcred)){
			lastcred = (MTCredentials*)creds->getitem(ckey);
		};
		if (lastcred){
			addkey(request,"Authorization: Basic",lastcred->authkey);
		};
		endrequest(request);
		cf.c->write(request,strlen(request));
		while (cf.c->isconnected()){
			if (h){
				l = cf.c->read(header,4096);
				if (l>0){
					e = strstr(header,"\r\n\r\n");
					if (!e) goto ftpexit;
					e += 2;    
					*e++ = 0;
					e++;
					if (!getkey(header,"HTTP",value,256)) goto ftpexit;
					result = strtol(value,&status,10);
					if (status){
						while (*status==' ') status++;
					};
					rclass = result/100;
					switch (rclass){
					case 1:
					case 2:
						break;
					case 3:
						if (!getkey(header,"Location:",value,256)) goto ftpexit;
						mtmemfree(cf.filename);
						len = strlen(value);
						cf.filename = (char*)mtmemalloc(len+1);
						memcpy(cf.filename,value,len);
						cf.filename[len] = 0;
						if (retries>2) goto ftpexit;
						goto retry;
					case 4:
						switch (result){
						case 400:
							LOGD("%s - [HTTP] Bad request:"NL);
							LOG(request);
							goto ftpexit;
						case 401:
							if (!getkey(header,"WWW-Authenticate:",value,256)) goto ftpexit;
							cancontinue = true;
							e = strstr(value,"Basic");
							if (!e) break;
							e = strstr(e,"realm=\"");
							if (!e) break;
							e += 7;
							e2 = strchr(e,'\"');
							if (!e2) break;
							l = (int)e2-(int)e;
							memcpy(realm,e,l);
							realm[l] = 0;
							needauth = true;
							if (retries>2) break;
							if ((retries) && (lastcred)){
								deletecredentials(lastcred);
								lastcred = 0;
							}
							else{
								lastcred = (MTCredentials*)creds->getitem(ckey);
								if (lastcred){
									retries--;
									goto retry;
								};
							};
							e = (char*)sysres->getresource('XTTM',1,&l);
							sprintf(value,e,realm,cf.host);
							sysres->releaseresource(e);
							if (mtauthdialog(value,login,password)<0) break;
							strcpy(value,login);
							strcat(value,":");
							strcat(value,password);
							lastcred = newcredential(cf.host,cpath,realm,value);
							goto retry;
						case 403:
						case 404:
						case 410:
							cancontinue = true;
							break;
						};
						if (cancontinue) break;
						LOGD("%s - [HTTP] ");
						LOG(status);
						LOG(NL"Request:"NL);
						LOG(request);
						goto ftpexit;
					default:
						goto ftpexit;
					};
					if (getkey(header,"Content-Length:",value,256)){
						cf.loadlength = strtol(value,0,10);
						cf.alloclength = cf.loadlength;
					}
					else{
						cf.loadlength = -1;
						cf.alloclength = 262144;
					};
					if (cf.e) cf.e->set();
					cf.tmplock->lock();
					cf.loadpos = l-(int)(e-header);
					cf.tmpfile->seek(0,MTF_BEGIN);
					cf.tmpfile->write(e,cf.loadpos);
					cf.tmplock->unlock();
					h = false;
					cf.ready = true;
				}
				else mtsyswait(0);
			}
			else{
				l = cf.c->read(header,4096);
				if (l>0){
					cf.tmplock->lock();
					cf.tmpfile->seek(cf.loadpos,MTF_BEGIN);
					cf.tmpfile->write(header,l);
					cf.loadpos += l;
					cf.tmplock->unlock();
				}
				else{
					cf.loadlength = cf.loadpos;
					mtsyswait(0);
				};
			};
		};
ftpexit:
		mtmemfree(request);
		mtmemfree(header);
	}
	else if (cf.filetype==MTFT_FTP){
	};
	if (cf.e) cf.e->set();
	cf.ct = 0;*/
    return 0;
}
//---------------------------------------------------------------------------
