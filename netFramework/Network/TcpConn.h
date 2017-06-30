#pragma once
#include "Conn.h"
#include "netFd.h"

class TcpConn:public Conn {
public:
	TcpConn(std::shared_ptr<NetFd> pNetfd, std::shared_ptr<Addr> remoteAddr);
	~TcpConn();
	virtual int read(char *b, int size, int *error)override;
	virtual int write(char *b, int size, int *error)override;
	virtual std::shared_ptr<Addr> remoteAddr()override;
	virtual int close()override;
	virtual int setBlock(bool block)override;
	virtual int setReadTimeout(LPTIMEVal time)override;
	virtual int setWriteTimeout(LPTIMEVal time)override;
protected:
	std::shared_ptr<NetFd> m_netFd;
	std::shared_ptr<Addr> m_remoteAddr;
};