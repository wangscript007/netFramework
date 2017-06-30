#include "Log/log4z.h"
#include "Network/TcpConn.h"
#include "utils/circularBuffer.h"

#include <iostream>
using namespace zsummer::log4z;

void badCallback(std::shared_ptr<Conn> conn) {

}

int main(int argc, char **argv) {
	ILog4zManager::getInstance()->setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "./logs");
	ILog4zManager::getInstance()->start();
	networkInit();
	auto addr = Addr::ResolveAddr(TCP, "127.0.0.1", 9004);
	auto conn = Conn::Dial(TCP, addr);
	auto cir = new cirBuffer(8);
	char *testData = "12345678";
	char cache[8];
	auto ret = cir->write(testData, 5);
	auto redret = cir->read(cache, 6);
	ret = cir->write(testData, 5);
	redret = cir->read(cache, 6);
	if (nullptr!=conn)
	{
		conn->write("ahahaha", strlen("ahahaha"), nullptr);
		char buf[100];
		conn->read(buf, 100);
		LOGD(buf);
	}

	auto server = TcpServer::CreateServer(addr, badCallback);
	redret = server->Start();
	std::cin >> ret;
	return 1;
}