//fd maybe udp,tcp client,tcp server
#pragma once
#include "utilsNet.h"
#include "../Clock/Timer.h"
#include "../Thead/RWMutex.h"
#include <mutex>
enum fdType
{
	fd_udp,
	fd_tcpClient,
	fd_tcpSvrClient
};

class NetFd
{
public:
	NetFd(fdType type,socketFd fd);
	fdType getType();
	int setReadTimeout(LPTIMEVal time);
	int setWriteTimeout(LPTIMEVal time);
	int read(void *buf, int len, int *errCode);
	int write(const void *buf, int len, int *errCode);
	int setBlock(bool block);
	void close();
	bool valid();
	~NetFd();
private:
	RWMutex fdMutex;
	std::mutex mutexRead;
	std::mutex mutexWrite;
	socketFd  fd;
	fdType type;
};
