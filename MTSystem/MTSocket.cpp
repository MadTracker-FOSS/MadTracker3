//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSocket.cpp 82 2005-08-30 15:52:57Z Yannick $
//
//---------------------------------------------------------------------------
#include "MTSocket.h"
#include <MTXAPI/MTXSystem2.h>
#include "MTBase64.h"
#include <string.h>
//---------------------------------------------------------------------------
#ifdef _WIN32
	bool socketinit = false;
	WSADATA wsadata;
	HMODULE hws;
	int (WSAAPI *wsstartup)(WORD,LPWSADATA);
	int (WSAAPI *wscleanup)();
	int (WSAAPI *wsgetlasterror)();
	SOCKET (WSAAPI *mtaccept)(SOCKET,struct sockaddr*,mt_uint32*);
	int (WSAAPI *mtbind)(SOCKET,const struct sockaddr*,int);
	int (WSAAPI *mtclosesocket)(SOCKET);
	int (WSAAPI *mtconnect)(SOCKET,const struct sockaddr*,int);
	struct hostent* (WSAAPI *mtgethostbyaddr)(const char*,int,int);
	struct hostent* (WSAAPI *mtgethostbyname)(const char*);
	int (WSAAPI *mtgethostname)(char*,int);
	u_short (WSAAPI *mthtons)(u_short);
	unsigned long (WSAAPI *mtinet_addr)(const char*);
	char* (WSAAPI *mtinet_ntoa)(struct in_addr);
	int (WSAAPI *mtioctlsocket)(SOCKET,long,u_long*);
	int (WSAAPI *mtsetsockopt)(SOCKET,int,int,const char*,int);
	int (WSAAPI *mtlisten)(SOCKET,int);
	int (WSAAPI *mtrecv)(SOCKET,char*,int,mt_uint32);
	int (WSAAPI *mtsend)(SOCKET,const char*,int,mt_uint32);
	int (WSAAPI *mtrecvfrom)(SOCKET s, char*,int,int,struct sockaddr*,mt_uint32*);
	int (WSAAPI *mtsendto)(SOCKET,const char*,int,int,const struct sockaddr*,mt_uint32);
	int (WSAAPI *mtshutdown)(SOCKET,int);
	int (WSAAPI *mtsocket)(int,int,int);
#endif
//---------------------------------------------------------------------------
void initSocket()
{
#	ifdef _WIN32
		LOGD("%s - [System] Initializing WinSock..."NL);
		if (!hws){
			hws = LoadLibrary("WS2_32.DLL");
			if (!hws){
				LOGD("%s - [System] ERROR: Cannot load WS2_32.DLL!"NL);
				return;
			};
			*(int*)&wsstartup = (int)GetProcAddress(hws,"WSAStartup");
			*(int*)&wscleanup = (int)GetProcAddress(hws,"WSACleanup");
			*(int*)&wsgetlasterror = (int)GetProcAddress(hws,"WSAGetLastError");
			*(int*)&mtaccept = (int)GetProcAddress(hws,"WSAStartup");
			*(int*)&mtaccept = (int)GetProcAddress(hws,"accept");
			*(int*)&mtbind = (int)GetProcAddress(hws,"bind");
			*(int*)&mtclosesocket = (int)GetProcAddress(hws,"closesocket");
			*(int*)&mtconnect = (int)GetProcAddress(hws,"connect");
			*(int*)&mtgethostbyaddr = (int)GetProcAddress(hws,"gethostbyaddr");
			*(int*)&mtgethostbyname = (int)GetProcAddress(hws,"gethostbyname");
			*(int*)&mtgethostname = (int)GetProcAddress(hws,"gethostname");
			*(int*)&mthtons = (int)GetProcAddress(hws,"htons");
			*(int*)&mtinet_addr = (int)GetProcAddress(hws,"inet_addr");
			*(int*)&mtinet_ntoa = (int)GetProcAddress(hws,"inet_ntoa");
			*(int*)&mtioctlsocket = (int)GetProcAddress(hws,"ioctlsocket");
			*(int*)&mtsetsockopt = (int)GetProcAddress(hws,"setsockopt");
			*(int*)&mtlisten = (int)GetProcAddress(hws,"listen");
			*(int*)&mtrecv = (int)GetProcAddress(hws,"recv");
			*(int*)&mtsend = (int)GetProcAddress(hws,"send");
			*(int*)&mtrecvfrom = (int)GetProcAddress(hws,"recvfrom");
			*(int*)&mtsendto = (int)GetProcAddress(hws,"sendto");
			*(int*)&mtshutdown = (int)GetProcAddress(hws,"shutdown");
			*(int*)&mtsocket = (int)GetProcAddress(hws,"socket");
		};
		if (wsstartup(0x101,&wsadata)==0) socketinit = true;
#	endif
}

void uninitSocket()
{
#	ifdef _WIN32
		if (socketinit){
			wscleanup();
			FreeLibrary(hws);
			socketinit = false;
		};
#	endif
}
//---------------------------------------------------------------------------
MTSocket::MTSocket(bool datagram):
server(0),
isdatagram(datagram),
connected(false),
ip(0),
port(0),
s(0)
{
#	ifdef _WIN32
		if (!socketinit) initSocket();
#	endif
	mtmemzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
}

MTSocket::MTSocket(MTServer *cserver,SOCKET cs,sockaddr_in *caddr):
server(cserver),
isdatagram(cserver->isdatagram),
connected(true),
ip(0),
port(0),
s(cs)
{
#	ifdef _WIN32
		if (!socketinit) initSocket();
#	endif
	memcpy(&addr,caddr,sizeof(addr));
	ip = addr.sin_addr.s_addr;
	port = mthtons(addr.sin_port);
}

MTSocket::~MTSocket()
{
	disconnect();
}

bool MTSocket::isconnected()
{
	return connected;
}

void MTSocket::disconnect()
{
	if (connected){
		mtshutdown(s,SD_BOTH);
		mtclosesocket(s);
		connected = false;
	};
	if (server) server->disconnectclient(this);
}

int MTSocket::getlasterror()
{
	return lasterror;
}

int MTSocket::read(void *buffer,int size,sockaddr *from,mt_uint32 *fromsize)
{
	int res;
	
	if (!connected) return 0;
	if (isdatagram){
		res = mtrecvfrom(s,(char*)buffer,size,0,from,fromsize);
		if (res>0) return res;
		return -1;
	}
	else{
		res = mtrecv(s,(char*)buffer,size,0);
		if (res>0) return res;
		lasterror = wsgetlasterror();
		if ((lasterror) && (lasterror!=WSAEWOULDBLOCK)){
			mtshutdown(s,SD_SEND);
			connected = false;
			if (server) server->disconnectclient(this);
			return -1;
		};
	};
	return 0;
}

int MTSocket::write(const void *buffer,mt_uint32 size)
{
	if (!connected) return 0;
	if (isdatagram){
		if (mtsendto(s,(char*)buffer,size,0,(struct sockaddr*)&addr,sizeof(addr))==size) return size;
		return -1;
	}
	else{
		if (mtsend(s,(const char*)buffer,size,0)==size) return size;
		lasterror = wsgetlasterror();
		if ((lasterror) && (lasterror!=WSAEWOULDBLOCK)){
			mtshutdown(s,SD_SEND);
			connected = false;
			if (server) server->disconnectclient(this);
			return -1;
		};
	};
	return 0;
}

void MTSocket::setblocking(bool b)
{
#	ifdef _WIN32
		b ^= true;
		mtioctlsocket(s,FIONBIO,(unsigned long*)&b);
#	else
		int state = mtioctlsocket(s,F_GETFL);
		if (b) state &= (!O_NONBLOCK);
		else state |= O_NONBLOCK;
		mtioctlsocket(s,F_SETFL,state);
#	endif
}

const char* MTSocket::getname()
{
	hostent *he = mtgethostbyaddr((char*)&addr.sin_addr.s_addr,sizeof(ip),AF_INET);
	if (!he) return 0;
	return he->h_name;
}

int MTSocket::getip()
{
	return ip;
}

int MTSocket::getendip()
{
#	ifdef _WIN32
		return addr.sin_addr.S_un.S_addr;
#	else
		return addr.sin_addr.s_addr;
#	endif
}

int MTSocket::makesocket()
{
	int optval;
	SOCKET s;
	
	if (isdatagram) s = mtsocket(AF_INET,SOCK_DGRAM,0);
	else s = mtsocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (addr.sin_addr.s_addr==INADDR_BROADCAST){
		optval = 1;
		mtsetsockopt(s,SOL_SOCKET,SO_BROADCAST,(char*)&optval,sizeof(optval));
	};
	return s;
}
//---------------------------------------------------------------------------
MTServer::MTServer(int p,int max,bool datagram):
MTSocket(datagram),
maxconn(max),
nclients(0)
{
	hostent *he;
	char buf[1024];
	
	port = p;
	mtgethostname(buf,1024);
	he = mtgethostbyname(buf);
	if (he) ip = *(int*)he->h_addr_list[0];
	addr.sin_port = mthtons(p);
	addr.sin_addr.s_addr = INADDR_ANY;
	clients = (MTSocket**)mtmemalloc(4*max,MTM_ZERO);
	s = makesocket();
	if (s!=INVALID_SOCKET){
		if (mtbind(s,(sockaddr*)&addr,sizeof(addr))==0){
			if (isdatagram){
				connected = true;
			}
			else if (mtlisten(s,max)==0){
				connected = true;
			}
			else{
				lasterror = wsgetlasterror();
				mtclosesocket(s);
			};
		}
		else{
			lasterror = wsgetlasterror();
			mtclosesocket(s);
		};
	};
}

MTServer::~MTServer()
{
	int x;
	
	if (connected){
		connected = false;
		mtshutdown(s,SD_BOTH);
		mtclosesocket(s);
		for (x=0;x<nclients;x++) delete clients[x];
	};
	mtmemfree(clients);
}

int MTServer::write(const void *buffer,int size)
{
	int x;
	
	if ((!connected) || (nclients==0) || (isdatagram)) return 0;
	for (x=0;x<nclients;x++) clients[x]->write(buffer,size);
	return size;
}

MTSocket* MTServer::accept()
{
	mt_uint32 l;
	SOCKET cc;
	sockaddr_in client;
	
	if ((nclients==maxconn) || (isdatagram)) return 0;
	l = sizeof(client);
	cc = ::mtaccept(s,(sockaddr*)&client,&l);
	if (cc!=INVALID_SOCKET){
		clients[nclients] = new MTSocket(this,cc,&client);
		clients[nclients]->setblocking(false);
//		l = 1;
//		mtioctlsocket(cc,FIONBIO,(unsigned long*)&l);
		return clients[nclients++];
	};
	lasterror = wsgetlasterror();
	return 0;
}

void MTServer::disconnectclient(MTSocket *s)
{
	int x,y;
	
	for (x=0;x<nclients;x++){
		MTSocket *c = clients[x];
		if (c==s){
			for (y=x;y<nclients-1;y++){
				clients[y] = clients[y+1];
			};
			clients[--nclients] = 0;
			break;
		};
	};
}

int MTServer::getnumclients()
{
	return nclients;
}

MTSocket* MTServer::getclient(int id)
{
	return clients[id];
}
//---------------------------------------------------------------------------
MTClient::MTClient(const char *a,int p,bool datagram):
MTSocket(datagram)
{
	int ip;
	hostent *he;
	char buf[1024];
	
	port = p;
	if (strcmp(a,"broadcast")==0){
		ip = -1;
	}
	else{
		ip = mtinet_addr(a);
		if ((ip==0) || (ip==INADDR_NONE)){
			if (a[0]){
				he = mtgethostbyname(a);
				if (he) ip = *(int*)he->h_addr_list[0];
				else return;
			}
			else{
				mtgethostname(buf,1024);
				he = mtgethostbyname(buf);
				if (he) ip = *(int*)he->h_addr_list[0];
				else return;
			};
		};
	};
	addr.sin_port = mthtons(p);
	addr.sin_addr.s_addr = ip;
	reconnect();
}

MTClient::MTClient(int ip,int p,bool datagram):
MTSocket(datagram)
{
	port = p;
	addr.sin_port = mthtons(p);
	addr.sin_addr.s_addr = ip;
	reconnect();
}

bool MTClient::reconnect()
{
	if (connected){
		if (!isdatagram) mtshutdown(s,SD_BOTH);
		mtclosesocket(s);
		connected = false;
	};
	s = makesocket();
	if (s!=INVALID_SOCKET){
		if (isdatagram){
			connected = true;
		}
		else if (mtconnect(s,(sockaddr*)&addr,sizeof(addr))==0){
			connected = true;
		}
		else{
			lasterror = wsgetlasterror();
			mtclosesocket(s);
		};
	};
	return connected;
}
//---------------------------------------------------------------------------
