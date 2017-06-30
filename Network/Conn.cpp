#include "utilsNet.h"
#include "Conn.h"
#include "TcpConn.h"
#include "../Log/log4z.h"
using namespace std;

shared_ptr<Conn> Conn::Dial(string protocol, shared_ptr<Addr> addr)
{
	if (addr==nullptr)
	{
		return nullptr;
	}
	if (protocol.compare(TCP)==0)
	{
		auto fd = connectTcp(addr->IP().data(), addr->Port());
		if (fd==SOCKET_INVALID)
		{
			return nullptr;
		}

		shared_ptr<NetFd> ptrNetFd(new NetFd(fd_tcpClient, fd));
		shared_ptr<TcpConn> tcpConn(new TcpConn(ptrNetFd, addr));
		return tcpConn;
	}
	else
	{
		LOGW(protocol << " dial not processed");
		return nullptr;
	}
	return shared_ptr<Conn>();
}
