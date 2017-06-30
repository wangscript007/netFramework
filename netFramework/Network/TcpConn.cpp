#include "TcpConn.h"
using namespace std;
TcpConn::TcpConn(shared_ptr<NetFd> pNetfd, shared_ptr<Addr> remoteAddr):m_netFd(pNetfd),m_remoteAddr(remoteAddr)
{
}

TcpConn::~TcpConn()
{
	close();
}

int TcpConn::read(char * b, int size, int * error)
{
	return m_netFd->read(static_cast<void*>(b), size, error);
}

int TcpConn::write(char * b, int size, int * error)
{
	return m_netFd->write(static_cast<const void*>(b), size, error);
}

shared_ptr<Addr> TcpConn::remoteAddr()
{
	return m_remoteAddr;
}

int TcpConn::close()
{
	m_netFd->close();
	return 0;
}

int TcpConn::setBlock(bool block)
{
	
	return 0;
}

int TcpConn::setReadTimeout(LPTIMEVal time)
{
	return 0;
}

int TcpConn::setWriteTimeout(LPTIMEVal time)
{
	return 0;
}
