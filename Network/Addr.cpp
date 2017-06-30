#include "Addr.h"
#include "TCPAddr.h"

using namespace std;


shared_ptr<Addr> Addr::ResolveAddr(string protocol, string hostName, int port)
{
	if (protocol.compare(TCP)==0)
	{
		return TCPAddr::ResolveAddr(hostName, port);
	}
	return shared_ptr<Addr>();
}
