#include "DataPacket.h"


DataPacket::DataPacket(void * data, int len):m_data(data),m_dataLen(len)
{
}

DataPacket::~DataPacket()
{
}

void * DataPacket::Data(int & len)
{
	len = m_dataLen;
	return m_data;
}