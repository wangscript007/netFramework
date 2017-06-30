#include "TcpConn.h"
#include "TcpServerClientConn.h"
using namespace std;

TcpServerClientConn::TcpServerClientConn(std::shared_ptr<NetFd> pNetfd, std::shared_ptr<Addr> remoteAddr):
	TcpConn(pNetfd,remoteAddr)
{
}

TcpServerClientConn::~TcpServerClientConn()
{
}

int TcpServerClientConn::read(char * b, int size, int * error)
{
	auto ret = m_cirBuffer.read(b, size);
	if (ret==0)
	{
		if (nullptr!=error)
		{
			if (m_netFd->valid())
			{
				*error = NETFD_ERR_NO_DATA;
			}
			else {
				*error = NETFD_ERR_CLOSED;
				ret = -1;
			}
		}
	}
	return ret;
}

int TcpServerClientConn::write(char * b, int size, int * error)
{
	auto ret = 0;
	//Ìí¼Óµ½»º´æ
	if (m_netFd->valid())
	{
		auto appended = m_cirBuffer.write(b, size);
		if (false==appended)
		{
			ret = -1;
			*error = NETFD_ERR_CACHE_FULL;
		}
	}
	else {
		if (error!=nullptr)
		{
			*error = NETFD_ERR_CLOSED;
		}
		ret = -1;
	}
	return ret;
}

bool TcpServerClientConn::recved(char * b, int size)
{
	return m_cirBuffer.write(b, size);
}

int TcpServerClientConn::close()
{
	return 0;
}
