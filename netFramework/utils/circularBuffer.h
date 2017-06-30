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
	bool biggerCache();//����
	int DataReadableCount();//���ж������ݿɶ�
	int DataWriteableCount();//���ж������ݿ�д
	int m_maxLength;
	unsigned char *m_data;
	int m_dataSize;
	int m_begin;
	int m_end;//end û�����ݣ�ʵ������-1;end=0ʱ�����������
	std::mutex m_mux;

};