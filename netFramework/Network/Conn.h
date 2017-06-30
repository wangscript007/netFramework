#pragma once
#include <memory>
#include "Addr.h"
#include "../Clock/Timer.h"
class Conn
{
public:
	static std::shared_ptr<Conn> Dial(std::string protocol, std::shared_ptr<Addr> addr);
	virtual int read(char *b, int size, int *error=nullptr) = 0;
	virtual int write(char *b, int size, int *error = nullptr) = 0;
	virtual int close() = 0;
	virtual std::shared_ptr<Addr> remoteAddr() = 0;
	virtual int setBlock(bool block) = 0;
	virtual int setReadTimeout(LPTIMEVal time) = 0;
	virtual int setWriteTimeout(LPTIMEVal time) = 0;
};

