#pragma once
#include <iostream>
#include <sstream>
#include <memory>
#include "Addr.h"

class TCPAddr :
	public Addr
{
public:
	static std::shared_ptr<TCPAddr>  ResolveAddr(std::string hostName, int port) {
		std::shared_ptr<TCPAddr> tcpAddr(new(TCPAddr));
		tcpAddr->port = port;
		tcpAddr->hostName = hostName;
		return tcpAddr;
	}
	std::string Network() override {
		return TCP;
	}
	std::string String() override {
		std::stringstream tmp;
		tmp << port;
		return hostName + ":" + tmp.str();
	}

	int Port() override{
		return this->port;
	}

	std::string IP()override {
		return hostName;
	}

	virtual ~TCPAddr()
	{
	}
private:
	TCPAddr();
	std::string hostName;
	int port;
};

