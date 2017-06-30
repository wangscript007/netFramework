#include "circularBuffer.h"
#include "../Log/log4z.h"
#include <stdio.h>
#include <stdlib.h>

cirBuffer::cirBuffer(int maxLength):m_maxLength(maxLength),m_data(nullptr),m_dataSize(0),
m_begin(0),m_end(0)
{
	if (maxLength>0)
	{
		auto initSize = maxLength > 1024 ? 1024 : maxLength;
		m_data = (unsigned char*)malloc(initSize);
		if (nullptr==m_data)
		{
			LOGE("out of memory");
			return;
		}
		m_dataSize = initSize;
	}
}

cirBuffer::~cirBuffer()
{
	if (m_data!=nullptr)
	{
		free(m_data);
		m_data = nullptr;
	}
}

bool cirBuffer::write(void * data, int len)
{
	std::lock_guard<std::mutex> guard(m_mux);
	while (len>this->DataWriteableCount())
	{
		if (false==this->biggerCache())
		{
			LOGE("cirBuffer write failed:cache out of max size");
			return false;
		}
	}
	//是否换行

	auto spaceAfterEnd = m_dataSize - m_end;
	auto twoline = spaceAfterEnd >= len ? false : true;

	auto transSize = min(spaceAfterEnd, len);
	memmove(m_data + m_end, data, transSize);
	m_end += transSize;
	m_end %= m_dataSize;
	//第二行数据
	if (twoline)
	{
		transSize = len - transSize;
		memmove(m_data + m_end, (unsigned char*)data + transSize, transSize);
		m_end += transSize;
	}

	return true;
}

int cirBuffer::read(void * data, int len)
{
	std::lock_guard<std::mutex> guard(m_mux);
	auto readSize = len < this->DataReadableCount() ? len : this->DataReadableCount();
	if (readSize==0)
	{
		return 0;
	}
	//同样判断是否需要换行
	auto dataLeftBegin = m_begin < m_end ? m_end - m_begin : m_dataSize - m_begin;
	auto twoLine = dataLeftBegin <= readSize ? false : true;
	auto transSize = min(dataLeftBegin, readSize);
	memmove(data, m_data + m_begin, transSize);
	m_begin += transSize;
	m_begin %= m_dataSize;
	//第二行
	if (twoLine)
	{
		transSize = readSize - transSize;
		memmove((unsigned char*)data + transSize, m_data + m_begin, transSize);
		m_begin += transSize;
	}
	return readSize;
}

bool cirBuffer::biggerCache()
{
	auto nextSize = m_dataSize << 1;
	if (nextSize>m_maxLength)
	{
		LOGE("out of cache size");
		return false;
	}
	m_data = (unsigned char*)realloc(m_data, nextSize);
	if (m_data==nullptr)
	{
		LOGE("out of memory");
		return false;
	}
	//move data
	if (m_end<m_begin)
	{
		memmove(m_data + m_dataSize, m_data, m_end);
		m_end += m_dataSize;
	}
	//!move data
	m_dataSize = nextSize;
	return true;
}

int cirBuffer::DataReadableCount()
{
	auto tmpEnd = m_begin <= m_end ? m_end : m_end + m_dataSize;
	return tmpEnd - m_begin;
}

int cirBuffer::DataWriteableCount()
{
	return m_dataSize - DataReadableCount() - 1;
}

