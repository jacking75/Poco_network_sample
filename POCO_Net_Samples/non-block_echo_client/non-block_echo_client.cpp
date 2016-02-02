
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