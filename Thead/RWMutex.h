#pragma once
#include <mutex>
#include <condition_variable>
class RWMutex
{
public:
	RWMutex();
	virtual ~RWMutex();
	void RLock();
	void RUnlock();
	void WLock();
	void WUnlock();
private:
	int readerCount;
	std::mutex mutexReaderCount;
	std::mutex mutexWriter;
	std::condition_variable_any cvReaderConut;
};

