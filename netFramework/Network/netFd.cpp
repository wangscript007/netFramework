#include "netFd.h"

NetFd::NetFd(fdType type, socketFd fd)
{
	this->fd = fd;
	this->type = type;
}

fdType NetFd::getType() {
	return type;
}

int NetFd::setReadTimeout(LPTIMEVal time)
{
	fdMutex.RLock();
	auto ret = setSocketReadTimeout(fd, static_cast<int>(time->tvSec), time->tvUsec);
	fdMutex.RUnlock();
	return ret;
}

int NetFd::setWriteTimeout(LPTIMEVal time)
{
	fdMutex.RLock();
	auto ret = setSocketWriteTimeout(fd, static_cast<int>(time->tvSec), time->tvUsec);
	fdMutex.RUnlock();
	return ret;
}

int NetFd::read(void * buf, int len, int * errCode)
{
	auto ret = -1;
	switch (type)
	{
	case fd_udp:
		break;
	case fd_tcpClient:
		mutexRead.lock();
		ret = tcpRead(fd, buf, len, errCode);
		mutexRead.unlock();
		break;
	case fd_tcpSvrClient:
		break;
	default:
		break;
	}
	return ret;
}

int NetFd::write(const void * buf, int len, int * errCode)
{
	auto ret = -1;
	switch (type)
	{
	case fd_udp:
		break;
	case fd_tcpClient:
		mutexWrite.lock();
		ret = tcpWrite(fd, buf, len, errCode);
		mutexWrite.unlock();
		break;
	case fd_tcpSvrClient:
		break;
	default:
		break;
	}
	return ret;
}

int NetFd::setBlock(bool block)
{
	return setSocketBlocked(fd, block);
}

void NetFd::close()
{
	fdMutex.WLock();
	closeSocketFd(fd);
	fdMutex.WUnlock();
}

bool NetFd::valid()
{
	fdMutex.RLock();
	auto valid = fd != SOCKET_INVALID;
	fdMutex.RUnlock();
	return valid;
}

NetFd::~NetFd()
{
	this->close();
}