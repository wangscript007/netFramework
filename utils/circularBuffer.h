#pragma once
#include <mutex>

const int cirBufferMaxSize = 1024 * 1024 << 3;

class cirBuffer
{
public:
	cirBuffer(int maxLength=cirBufferMaxSize);
	~cirBuffer();
	bool write(void *data, int len);
	int read(void *data, int len);
private:
	bool biggerCache();//扩容
	int DataReadableCount();//还有多少数据可读
	int DataWriteableCount();//还有多少数据可写
	int m_maxLength;
	unsigned char *m_data;
	int m_dataSize;
	int m_begin;
	int m_end;//end 没有数据，实际容量-1;end=0时，数据在最后
	std::mutex m_mux;

};