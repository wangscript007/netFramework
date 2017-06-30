#pragma once
#ifdef WIN32
#define WINDOWS_SYSTEM
#endif
//linux
//#define LINUX_SYSTEM
//mac
//#define MAC_SYSTEM
#include <memory>
#include "Conn.h"
#include "../utils/DataPacket.h"
#ifdef WINDOWS_SYSTEM
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#define  socketFd SOCKET
#else
#ifdef LINUX_SYSTEM
#define socketFd int
#else
#endif
#endif
#define SOCKET_INVALID (~0)
enum NETFD_ERR
{
	NETFD_ERR_NONE,
	NETFD_ERR_CLOSED,
	NETFD_ERR_NO_DATA,
	NETFD_ERR_CACHE_FULL
};

int networkInit();
int networkShutdown();
socketFd connectTcp(const char *hostname, int port);
int closeSocketFd(socketFd& fd);
int setSocketReuseAddr(socketFd fd);
int setSocketReusePort(socketFd fd);
int setSocketBlocked(socketFd fd, bool blocked);
int setSocketReadTimeout(socketFd fd, int sec, long long usec);
int setSocketWriteTimeout(socketFd fd, int sec, long long usec);
int tcpRead(socketFd fd, void *buf, int len, int* errCode);
int tcpWrite(socketFd fd, const void *buf, int len, int* errCode);


typedef void(*SocketEventCallback)(std::shared_ptr<Conn> conn);
typedef void(*SocketDataCallback)(std::shared_ptr<Conn> conn,std::shared_ptr<DataPacket> data);

class TcpServer
{
public:
	TcpServer();
	virtual ~TcpServer();
	static std::shared_ptr<TcpServer> CreateServer(std::shared_ptr<Addr> addr, SocketEventCallback callback,SocketDataCallback dataCallback=nullptr);
	virtual int Start() = 0;
	virtual void Close() = 0;
};
