//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id: MTSocket.h 82 2005-08-30 15:52:57Z Yannick $
//
//---------------------------------------------------------------------------
#ifndef MTSOCKET_INCLUDED
#define MTSOCKET_INCLUDED
//---------------------------------------------------------------------------
class MTSocket;
class MTServer;
class MTClient;
//---------------------------------------------------------------------------
#include <MTXAPI/MTXExtension.h>
#ifdef _WIN32
#	include <winsock2.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <errno.h>
#	define SOCKET int
#	define INVALID_SOCKET -1
#	define WSAEWOULDBLOCK EWOULDBLOCK
#	define SD_SEND SHUT_WR
#	define SD_BOTH SHUT_RDWR
#	define mtaccept accept
#	define mtbind bind
#	define mtclosesocket close
#	define mtconnect connect
#	define mtgethostbyaddr gethostbyaddr
#	define mtgethostbyname gethostbyname
#	define mtgethostname gethostname
#	define mthtons htons
#	define mtinet_addr inet_addr
#	define mtinet_ntoa inet_ntoa
#	define mtioctlsocket fcntl
#	define mtsetsockopt setsockopt
#	define mtlisten listen
#	define mtrecv recv
#	define mtsend send
#	define mtrecvfrom recvfrom
#	define mtsendto sendto
#	define mtshutdown shutdown
#	define mtsocket socket
#	define wsgetlasterror() errno;
#endif
#include "MTSystem.h"
//---------------------------------------------------------------------------
class MTSocket{
public:
	MTSocket(bool datagram = false);
	MTSocket(MTServer *cserver,SOCKET cs,sockaddr_in *caddr);
	~MTSocket();
	bool isdatagram;
	virtual bool MTCT isconnected();
	virtual void MTCT disconnect();
	virtual int MTCT getlasterror();
	virtual int MTCT read(void *buffer,int size,sockaddr *from = 0,mt_uint32 *fromsize = 0);
	virtual int MTCT write(const void *buffer,mt_uint32 size);
	virtual void MTCT setblocking(bool b);
	virtual const char* MTCT getname();
	virtual int MTCT getip();
	virtual int MTCT getendip();
protected:
	MTServer *server;
	bool connected;
	int ip;
	int port;
	int lasterror;
	SOCKET s;
	sockaddr_in addr;
	int MTCT makesocket();
};

class MTServer : public MTSocket{
public:
	MTServer(int p,int max,bool datagram = false);
	~MTServer();
	virtual int MTCT write(const void *buffer,int size);
	virtual MTSocket* MTCT accept();
	virtual void MTCT disconnectclient(MTSocket *s);
	virtual int MTCT getnumclients();
	virtual MTSocket* MTCT getclient(int id);
private:
	int maxconn;
	int nclients;
	MTSocket **clients;
};

class MTClient : public MTSocket{
public:
	MTClient(const char *a,int p,bool datagram = false);
	MTClient(int ip,int p,bool datagram = false);
	virtual bool MTCT reconnect();
};
//---------------------------------------------------------------------------
extern "C"
{
void initSocket();
void uninitSocket();
}
//---------------------------------------------------------------------------
#ifdef _WIN32
	extern int (WSAAPI *wsgetlasterror)();
	extern SOCKET (WSAAPI *mtaccept)(SOCKET,struct sockaddr*,mt_uint32*);
	extern int (WSAAPI *mtbind)(SOCKET,const struct sockaddr*,int);
	extern int (WSAAPI *mtclosesocket)(SOCKET);
	extern int (WSAAPI *mtconnect)(SOCKET,const struct sockaddr*,int);
	extern struct hostent* (WSAAPI *mtgethostbyaddr)(const char*,int,int);
	extern struct hostent* (WSAAPI *mtgethostbyname)(const char*);
	extern int (WSAAPI *mtgethostname)(char*,int);
	extern u_short (WSAAPI *mthtons)(u_short);
	extern unsigned long (WSAAPI *mtinet_addr)(const char*);
	extern char* (WSAAPI *mtinet_ntoa)(struct in_addr);
	extern int (WSAAPI *mtioctlsocket)(SOCKET,long,u_long*);
	extern int (WSAAPI *mtsetsockopt)(SOCKET,int,int,const char*,int);
	extern int (WSAAPI *mtlisten)(SOCKET,int);
	extern int (WSAAPI *mtrecv)(SOCKET,char*,int,mt_uint32);
	extern int (WSAAPI *mtsend)(SOCKET,const char*,int,mt_uint32);
	extern int (WSAAPI *mtrecvfrom)(SOCKET s, char*,int,int,struct sockaddr*,mt_uint32*);
	extern int (WSAAPI *mtsendto)(SOCKET,const char*,int,int,const struct sockaddr*,mt_uint32);
	extern int (WSAAPI *mtshutdown)(SOCKET,int);
	extern int (WSAAPI *mtsocket)(int,int,int);
#endif
//---------------------------------------------------------------------------
#endif
