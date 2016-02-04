#include <iostream>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/NObserver.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/Timespan.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

const Poco::UInt16 PORT = 32452;


class Session {
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_socket(socket),
		m_reactor(reactor)
	{
		m_peerAddress = socket.peerAddress().toString();
		//std::cout << "connection from " << m_peerAddress << " ..." << std::endl;
		auto& app = Poco::Util::Application::instance();
		app.logger().information("Connection from " + socket.peerAddress().toString());

		m_reactor.addEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));

		m_reactor.addEventHandler(m_socket, Poco::Observer<Session, Poco::Net::ShutdownNotification>(*this, &Session::onShutdown)); // Event Handler when Socket was shutdown on the remote/peer side
		m_reactor.addEventHandler(m_socket, Poco::Observer<Session, Poco::Net::ErrorNotification>(*this, &Session::onError)); //Event Handler when Socket throws an error
		m_reactor.addEventHandler(m_socket, Poco::Observer<Session, Poco::Net::IdleNotification>(*this, &Session::onIdle)); //select 호출 때
		m_reactor.addEventHandler(m_socket, Poco::Observer<Session, Poco::Net::TimeoutNotification>(*this, &Session::onTimeout)); //select 호출 후 지정 시간까지 이벤트가 없을 때

	}

	~Session()
	{
		std::cout << m_peerAddress << " disconnected ..." << std::endl;
		m_reactor.removeEventHandler(m_socket,
			Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable)
			);

		m_reactor.removeEventHandler(m_socket, Poco::Observer<Session, Poco::Net::ShutdownNotification>(*this, &Session::onShutdown));
		m_reactor.removeEventHandler(m_socket, Poco::Observer<Session, Poco::Net::ErrorNotification>(*this, &Session::onError));
		m_reactor.removeEventHandler(m_socket, Poco::Observer<Session, Poco::Net::IdleNotification>(*this, &Session::onIdle));
		m_reactor.removeEventHandler(m_socket, Poco::Observer<Session, Poco::Net::TimeoutNotification>(*this, &Session::onTimeout));
	}

	void onReadable(Poco::Net::ReadableNotification* pNf)
	{
		pNf->release();

		char buffer[256] = { 0, };
		int n = m_socket.receiveBytes(buffer, sizeof(buffer));
		if (n > 0)
		{
			std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;

			char szSendMessage[256] = { 0, };
			sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);
			int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

			m_socket.sendBytes(szSendMessage, nMsgLen);
		}
		else
		{
			m_socket.shutdownSend();
			delete this;
		}
	}

	void onShutdown(Poco::Net::ShutdownNotification* pNf)
	{
		pNf->release();
	}

	void onError(Poco::Net::ErrorNotification* pNf)
	{
		pNf->release();

		Poco::Util::Application::instance().logger().information("onError called");
	}

	void onTimeout(Poco::Net::TimeoutNotification* pNf)
	{
		pNf->release();
		//Application::instance().logger().information("onTimeout called");
	}

	void onIdle(Poco::Net::IdleNotification* pNf)
	{
		pNf->release();

		//Application::instance().logger().information("onIdle called");
	}

private:
	Poco::Net::StreamSocket   m_socket;
	std::string m_peerAddress;
	Poco::Net::SocketReactor& m_reactor;
};

class EchoServer : public Poco::Util::ServerApplication
{
public:
	EchoServer() : _helpRequested(false)
	{
	}

	~EchoServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // 만약 설정 파일이 있다면 로딩한다.
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(Poco::Util::OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Poco::Util::Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		Poco::Util::HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("An echo server implemented using the Reactor and Acceptor patterns.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (_helpRequested)
		{
			displayHelp();
		}
		else
		{
			auto port = (unsigned short)config().getInt("EchoServer.port", PORT);

			Poco::Net::ServerSocket svs(port);

			Poco::Timespan timeout(10,0);
			Poco::Net::SocketReactor reactor(timeout);
			Poco::Net::SocketAcceptor<Session> acceptor(svs, reactor);

			Poco::Thread thread;
			thread.start(reactor);

			// wait for CTRL-C or kill
			Poco::Util::ServerApplication::waitForTerminationRequest();

			// Stop the SocketReactor
			reactor.stop();
			thread.join();
		}

		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};


int main(int argc, char** argv)
{
	EchoServer app;
	return app.run(argc, argv);
}