#pragma once

//默认只传递指针，不释放数据
class DataPacket {
public:
	DataPacket(void *data, int len);
	virtual ~DataPacket();
	void *Data(int &len);
private:
	void *m_data;
	int m_dataLen;
};