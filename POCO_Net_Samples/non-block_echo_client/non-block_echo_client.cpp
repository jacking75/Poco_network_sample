
// Timespan은 마이크로세컨드, Timespan(long seconds, long microseconds); 사용하기
// 서버에는 클라이언트에서 데이터 받으면 일부러 3초 이후에 보낸다.
// 클라이언트는 poll에서 2초로 잡아서 반환 값이 어떻게 나오는지 본다. 나는 진짜 받을 때만 true가 나올 듯
// 위의 상황에 맞게 서버도 수정해야 한다.

//
//EchoServer echoServer;
//StreamSocket ss;
//ss.connect(SocketAddress("localhost", echoServer.port()));
//ss.setBlocking(false);
//
//Timespan timeout(1000000);
//assert(ss.poll(timeout, Socket::SELECT_WRITE));
//int n = ss.sendBytes("hello", 5);
//assert(n == 5);
//
//char buffer[256];
//assert(ss.poll(timeout, Socket::SELECT_READ));
//n = ss.receiveBytes(buffer, sizeof(buffer));
//assert(n == 5);
//assert(std::string(buffer, n) == "hello");
//ss.close();
#include "Poco/Net/Net.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"

const Poco::UInt16 PORT = 32452;

int main()
{
	std::cout << "서버에 연결 시도..." << std::endl;
	Poco::Net::StreamSocket ss;

	try
	{
		ss.connectNB(Poco::Net::SocketAddress("localhost", PORT));

		char szMessage[] = "Send Message From Client";
		auto nMsgLen = (int)strnlen_s(szMessage, 128 - 1);

		ss.sendBytes(szMessage, nMsgLen);
		std::cout << "서버에 보낸 메시지: " << szMessage << std::endl;


		Poco::DateTime now1;
		std::cout << "poll 시작: " << now1.second() << std::endl;
		Poco::Timespan timeout(2, 0); // 2초
		auto pollResult = ss.poll(timeout, Poco::Net::Socket::SELECT_READ);
		Poco::DateTime now2;
		std::cout << "poll 완료: " << now2.second() << std::endl;
		
		if (pollResult)
		{
			char buffer[256] = { 0, };
			auto len = ss.receiveBytes(buffer, sizeof(buffer));
			std::cout << "서버로부터 받은 메시지: " << buffer << std::endl;			
		}
		else
		{
			std::cout << "poll 결과: false" << std::endl;
			std::cout << "poll 다시 시작: " << &Poco::DateTime::second << std::endl;

			pollResult = ss.poll(timeout, Poco::Net::Socket::SELECT_READ);

			if (pollResult)
			{
				char buffer[256] = { 0, };
				auto len = ss.receiveBytes(buffer, sizeof(buffer));
				std::cout << "서버로부터 받은 메시지: " << buffer << std::endl;
			}
		}

		ss.close();
	}
	catch (Poco::Exception& exc)
	{
		std::cout << "서버 접속 실패: " << exc.displayText() << std::endl;
	}
	
	getchar();
	return 0;
}