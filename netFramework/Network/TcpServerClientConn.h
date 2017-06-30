#pragma once
#include "TcpConn.h"
#include "netFd.h"
#include "../utils/circularBuffer.h"

class TcpServerClientConn:public TcpConn {
public:
	TcpServerClientConn(std::shared_ptr<NetFd> pNetfd, std::shared_ptr<Addr> remoteAddr);
	~TcpServerClientConn();
	virtual int read(char *b, int size, int *error)override;
	virtual int write(char *b, int size, int *error)override;
	virtual bool recved(char *b, int size);
	virtual int close()override;
private:
	cirBuffer m_cirBuffer;
};