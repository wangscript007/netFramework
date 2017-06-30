#pragma once
#include <string>
#include <memory>
const std::string TCP = "tcp";
const std::string UDP = "udp";

class Addr
{
public:
	static std::shared_ptr<Addr> ResolveAddr(std::string protocol , std::string hostName, int port);
	virtual std::string Network() = 0;
	virtual std::string String() = 0;
	virtual std::string IP() = 0;
	virtual int Port() = 0;

	virtual ~Addr()
	{
	}
};

