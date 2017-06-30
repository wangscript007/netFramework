#include "utilsNet.h"
#include "utilsWin.h"
#include "../Log/log4z.h"
#include "TcpServerClientConn.h"

#include <thread>
#ifdef WINDOWS_SYSTEM
int networkInit() {
	WORD wVer;
	WSADATA wsaData;
	wVer = MAKEWORD(2, 2);
	return WSAStartup(wVer, &wsaData);
}
int networkShutdown() {
	return WSACleanup();
}
socketFd connectTcp(const char * hostname, int port)
{
	socketFd fd = SOCKET_INVALID;
	int ret = 0;
	do
	{
		sockaddr_in svrAddr;
		memset(&svrAddr, 0, sizeof(svrAddr));
		svrAddr.sin_family = AF_INET;
		svrAddr.sin_port = htons(0);
		svrAddr.sin_addr.s_addr = htons(INADDR_ANY);
		fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd==SOCKET_INVALID)
		{
			break;
		}
		struct addrinfo hints;
		struct addrinfo *result = nullptr;
		struct addrinfo *ptr = nullptr;
		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		char portStr[8];

		_itoa_s(port, portStr, 10);

		ret = getaddrinfo(hostname, portStr, &hints, &result);
		if (ret!=0)
		{
			break;
		}
		for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)
			{
				memcpy(&svrAddr, ptr->ai_addr, ptr->ai_addrlen);
				break;
			}
		}
		freeaddrinfo(result);

		ret = connect(fd, (const sockaddr*)&svrAddr, sizeof(svrAddr));
		if (0!=ret)
		{
			break;
		}
	} while (0);
	if (0!=ret)
	{
		closeSocketFd(fd);
	}
	return fd;
}
int closeSocketFd(socketFd &fd) {
	if (fd==SOCKET_INVALID)
	{
		return 0;
	}
	auto ret = closesocket(fd);
	fd = SOCKET_INVALID;
	return ret;
}

int setSocketReuseAddr(socketFd fd) {
	int opt = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
}
int setSocketReusePort(socketFd fd) {
	return 0;
}

int setSocketBlocked(socketFd fd, bool blocked)
{
	u_long argp = blocked ? 0 : 1;
	return ioctlsocket(fd, FIONBIO, &argp);
}

int setSocketReadTimeout(socketFd fd,int sec, long long usec) {
	int optval = sec * 1000 + static_cast<int>(usec / 1000);
	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, sizeof(optval));
}
int setSocketWriteTimeout(socketFd fd, int sec, long long usec) {
	int optval = sec * 1000 + static_cast<int>(usec / 1000);
	return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&optval, sizeof(optval));
}
int tcpRead(socketFd fd, void * buf, int len, int* errCode)
{
	auto ret = recv(fd, (char*)buf, len, 0);
	if (ret==SOCKET_ERROR&&nullptr!=errCode)
	{
		*errCode = WSAGetLastError();
	}
	return ret;
}
int tcpWrite(socketFd fd, const void * buf, int len, int* errCode)
{
	auto ret = send(fd, (const char*)buf, len, 0);
	if (ret==SOCKET_ERROR&&nullptr!=errCode)
	{
		*errCode = WSAGetLastError();
	}
	return ret;
}




TcpServerWin::TcpServerWin(std::shared_ptr<Addr> addr, SocketEventCallback callback, SocketDataCallback dataCallback):
	m_fd(SOCKET_INVALID),m_callback(callback),m_dataCallback(dataCallback)
{
	this->init(addr);
}

TcpServerWin::~TcpServerWin()
{
	this->Close();
}

int TcpServerWin::Start()
{
	if (SOCKET_INVALID==m_fd||m_callback==nullptr)
	{
		return -1;
	}

	if (0 != listen(m_fd, SOMAXCONN)) {
		LOGE("listen failed");
		return -1;
	}
	LOGD("listen backlog:" << SOMAXCONN);

	m_hCompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL==m_hCompletion)
	{
		LOGE("create io completion port failed");
		return -1;
	}

	std::thread threadLoop(std::mem_fun(&TcpServerWin::ThreadLoop), this);
	m_threadLoop = std::move(threadLoop);

	return 0;
}

void TcpServerWin::Close()
{
	closeSocketFd(m_fd);
	CloseHandle(m_hCompletion);
	if (m_threadLoop.joinable())
	{
		m_threadLoop.join();
	}
}

void TcpServerWin::init(std::shared_ptr<Addr> addr)
{
	if (nullptr==addr)
	{
		LOGE("nullptr init TcpServer");
		return;
	}

	sockaddr_in svrAddr;
	memset((void*)&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(static_cast<u_short>(addr->Port()));
	svrAddr.sin_addr.s_addr = htons(INADDR_ANY);

	//m_fd = socket(AF_INET, SOCK_STREAM, 0);
	m_fd = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (m_fd<0)
	{
		LOGE("create server socket failed");
		return;
	}

	setSocketReuseAddr(m_fd);

	if (0!=bind(m_fd,(const sockaddr*)&svrAddr,sizeof(svrAddr)))
	{
		LOGE("bind failed,port:" << addr->Port());
		this->Close();
		return;
	}

	//setSocketBlocked(m_fd, false);

	return;
}

void TcpServerWin::ThreadLoop()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	for (auto i = 0; i < static_cast<int>(sysInfo.dwNumberOfProcessors*2); i++)
	{
		std::thread worker(std::mem_fun(&TcpServerWin::threadWork), this, m_hCompletion);
		worker.detach();
	}

	sockaddr_in remoteAddr;
	int addrLen = sizeof(remoteAddr);
	for (;;)
	{
		auto fd = WSAAccept(m_fd, (sockaddr*)(&remoteAddr), &addrLen, nullptr, 0);
		if (fd<0)
		{
			LOGD("WSAAccept failed,errcode" << WSAGetLastError());
			return;
		}
		auto ip = inet_ntoa(remoteAddr.sin_addr);
		auto port = remoteAddr.sin_port;
		auto ioData= new IOCP_DATA;
		memcpy(&ioData->remoteAddr, &remoteAddr, addrLen);
		ioData->fd = fd;
		CreateIoCompletionPort(HANDLE(fd), this->m_hCompletion, ULONG_PTR(ioData), 0);

		memset(&(ioData->overlapped), 0, sizeof(OVERLAPPED));
		ioData->dataBuf.len = MAX_IOCP_BUF_SIZE;
		ioData->dataBuf.buf = ioData->buf;

		DWORD recvBytes;
		DWORD flags = 0;
		WSARecv(ioData->fd, &(ioData->dataBuf), 1, &recvBytes, &flags, &(ioData->overlapped), nullptr);
	}
}

void TcpServerWin::threadWork(LPVOID lparam)
{
	auto completionPort = static_cast<HANDLE>(lparam);
	DWORD bytesTransferred = 0;
	LPOVERLAPPED lpOverlapped = nullptr;
	IOCP_DATA *iodata = nullptr;
	DWORD recvBytes = 0, flags = 0;

	for (;;)
	{
		auto ret=GetQueuedCompletionStatus(completionPort,&bytesTransferred,
			(PULONG_PTR)&iodata, (LPOVERLAPPED*)&lpOverlapped, INFINITE);
		//失败
		if (0==ret)
		{
			LOGE("GetQueuedCompletionStatus failed:" << GetLastError());
			/*this->DelConn(iodata->fd);
			delete iodata;*/
			if (iodata)
			{

			}
			continue;
		}
		if (0==bytesTransferred)
		{
			this->DelConn(iodata->fd);
			delete iodata;
			continue;
		}
		//添加到接收缓存

		this->m_muxConns.RLock();
		auto conn = static_cast<TcpServerClientConn*>(m_mapConns[iodata->fd].get());
		if (nullptr==m_dataCallback)
		{
			if (false == conn->recved(iodata->dataBuf.buf, iodata->dataBuf.len)) {
				LOGE("append to recved buff failed");
			}
		}
		else
		{
			std::shared_ptr<DataPacket> pkt(new DataPacket(iodata->dataBuf.buf, bytesTransferred));
			m_dataCallback(m_mapConns[iodata->fd], pkt);
		}
		this->m_muxConns.RUnlock();

		//再次读
		memset(&(iodata->overlapped), 0, sizeof(OVERLAPPED));
		iodata->dataBuf.len = MAX_IOCP_BUF_SIZE;
		iodata->dataBuf.buf = iodata->buf;

		DWORD recvBytes;
		DWORD flags = 0;
		WSARecv(iodata->fd, &(iodata->dataBuf), 1, &recvBytes, &flags, &(iodata->overlapped), nullptr);
	}
}

void TcpServerWin::AddConn(std::shared_ptr<Conn> conn, socketFd fd)
{
	m_muxConns.WLock();
	auto it = m_mapConns.find(fd);
	if (it!=m_mapConns.end())
	{
		LOGE("conn not removed:" << fd);
		it->second->close();
		m_mapConns.erase(it);
	}
	m_mapConns[fd] = conn;
	m_muxConns.WUnlock();
}

void TcpServerWin::DelConn( socketFd fd)
{
	m_muxConns.WLock();
	auto it = m_mapConns.find(fd);
	if (it!=m_mapConns.end())
	{
		it->second->close();
		m_mapConns.erase(it);
	}
	m_muxConns.WUnlock();
}



#endif