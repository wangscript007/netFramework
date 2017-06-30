#include "RWMutex.h"



RWMutex::RWMutex():readerCount(0)
{
}


RWMutex::~RWMutex()
{
}

void RWMutex::RLock()
{
	std::lock(mutexWriter, mutexReaderCount);
	readerCount++;
	mutexWriter.unlock();
	mutexReaderCount.unlock();
}

void RWMutex::RUnlock()
{
	std::unique_lock<std::mutex> _(mutexReaderCount);
	if (--readerCount==0)
	{
		cvReaderConut.notify_one();
	}
}

void RWMutex::WLock()
{
	std::lock(mutexWriter, mutexReaderCount);
	cvReaderConut.wait(mutexReaderCount, [&] {return readerCount == 0; });
}

void RWMutex::WUnlock()
{
	cvReaderConut.notify_one();
	mutexWriter.unlock();
	mutexReaderCount.unlock();
}
