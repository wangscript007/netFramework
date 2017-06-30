
#pragma once
#include "utilsNet.h"
#include "../Thead/RWMutex.h"
#include <thread>
#include <mutex>
#include <map>
#ifdef WINDOWS_SYSTEM

class TcpServerWin :public TcpServer {
public:
	TcpServerWin(std::shared_ptr<Addr> addr, SocketEventCallback callback, SocketDataCallback dataCallback = nullptr);
	~TcpServerWin();
	int Start() override;
	void Close() override;
private:
	void init(std::shared_ptr<Addr> addr);
	void ThreadLoop();
	void threadWork(LPVOID lparam);
	void AddConn(std::shared_ptr<Conn> conn, socketFd fd);
	void DelConn(socketFd fd);
private:
	socketFd m_fd;
	SocketEventCallback m_callback;
	SocketDataCallback m_dataCallback;
	HANDLE m_hCompletion;
	std::thread m_threadLoop;
	std::map<socketFd, std::shared_ptr<Conn>> m_mapConns;
	RWMutex m_muxConns;
};

const int MAX_IOCP_BUF_SIZE = 2048;

struct IOCP_DATA
{
	OVERLAPPED overlapped;
	WSABUF dataBuf;
	char buf[MAX_IOCP_BUF_SIZE];
	int bufSize;
	socketFd fd;
	sockaddr_in remoteAddr;
};

#endif