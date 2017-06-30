#include "utilsNet.h"
#ifdef WINDOWS_SYSTEM
#include "utilsWin.h"
#endif

using namespace std;

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}

std::shared_ptr<TcpServer> TcpServer::CreateServer(std::shared_ptr<Addr> addr, SocketEventCallback callback, SocketDataCallback dataCallback )
{
#ifdef WINDOWS_SYSTEM
	return shared_ptr<TcpServer>(new TcpServerWin(addr, callback,dataCallback));
#else
#endif
	return 0;
}

