// http://www.cachet.co.jp/?p=80

#include <iostream>

#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Timestamp.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/Exception.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/BasicEvent.h>
#include <Poco/Delegate.h>

const Poco::UInt16 PORT = 32452;



static Poco::BasicEvent<int> push_event;

class PushRunnable : public Poco::Runnable
{
public:
	PushRunnable() : _stopped(false)
	{
	}

	void run()
	{
		int tmp = 0;
		while (!_stopped)
		{
			++tmp;
			push_event.notify(this, tmp);
			Poco::Thread::sleep(1000);
		}
	}

	void notify()
	{
		_stopped = true;
	}

private:
	bool _stopped;
};


class TimeServerConnection : public Poco::Net::TCPServerConnection
	/// This class handles all client connections.
	///
	/// A string with the current date and time is sent back to the client.
{
public:
	TimeServerConnection(const Poco::Net::StreamSocket& s, const std::string& format)
		: TCPServerConnection(s)
		, _format(format)
		, _stopped(false)
	{
	}

	void run()
	{
		auto& app = Poco::Util::ServerApplication::Application::instance();
		app.logger().information("Request from " + this->socket().peerAddress().toString());
		try
		{
			push_event += Poco::delegate(this, &TimeServerConnection::onPush);
			while (!_stopped)
			{
				Poco::Thread::sleep(1);
			}
		}
		catch (Poco::Exception& exc)
		{
			app.logger().log(exc);
		}
	}

	void onPush(const void* pSender, int& i)
	{
		try
		{
			Poco::Timestamp now;
			std::string dt(Poco::DateTimeFormatter::format(now, _format));
			dt.append("\r\n");
			socket().sendBytes(dt.data(), (int)dt.length());
		}
		catch (Poco::Exception& exc)
		{
			push_event -= Poco::delegate(this, &TimeServerConnection::onPush);
			_stopped = true;
			Poco::Util::ServerApplication::Application::instance().logger().log(exc);;
		}
	}

private:
	std::string _format;
	bool _stopped;
};


class TimeServerConnectionFactory : public Poco::Net::TCPServerConnectionFactory
	/// A factory for TimeServerConnection.
{
public:
	TimeServerConnectionFactory(const std::string& format) :
		_format(format)
	{
	}

	Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket)
	{
		return new TimeServerConnection(socket, _format);
	}

private:
	std::string _format;
};


class TimeServer : public Poco::Util::ServerApplication
	/// The main application class.
	///
	/// This class handles command-line arguments and
	/// configuration files.
	/// Start the TimeServer executable with the help
	/// option (/help on Windows, --help on Unix) for
	/// the available command line options.
	///
	/// To use the sample configuration file (TimeServer.properties),
	/// copy the file to the directory where the TimeServer executable
	/// resides. If you start the debug version of the TimeServer
	/// (TimeServerd[.exe]), you must also create a copy of the configuration
	/// file named TimeServerd.properties. In the configuration file, you
	/// can specify the port on which the server is listening (default
	/// 9911) and the format of the date/time string sent back to the client.
	///
	/// To test the TimeServer you can use any telnet client (telnet localhost 9911).
{
public:
	TimeServer() : _helpRequested(false)
	{
	}

	~TimeServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
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
		helpFormatter.setHeader("A server application that serves the current date and time.");
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
			// get parameters from configuration file
			unsigned short port = (unsigned short)config().getInt("TimeServer.port", 9911);
			std::string format(config().getString("TimeServer.format", Poco::DateTimeFormat::ISO8601_FORMAT));

			// set-up a server socket
			Poco::Net::ServerSocket svs(port);
			// set-up a TCPServer instance
			Poco::Net::TCPServer srv(new TimeServerConnectionFactory(format), svs);
			// start the TCPServer
			srv.start();

			// start push thread
			Poco::Thread thread;
			PushRunnable runnable;
			thread.start(runnable);

			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the TCPServer
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};


int main(int argc, char** argv)
{
	TimeServer app;
	return app.run(argc, argv);
}