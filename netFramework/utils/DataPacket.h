#pragma once

//Ĭ��ֻ����ָ�룬���ͷ�����
class DataPacket {
public:
	DataPacket(void *data, int len);
	virtual ~DataPacket();
	void *Data(int &len);
private:
	void *m_data;
	int m_dataLen;
};