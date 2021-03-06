#pragma once
#include "Typedefs.h"
#include "Condvar.h"
#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace bcli {
	class TCPConnector : public boost::enable_shared_from_this<TCPConnector>
	{
	public:
		TCPConnector(boost::asio::io_service* ioService, boost::shared_ptr<boost::asio::ssl::context> sslContext, connect_handler, std::chrono::milliseconds retryDelay = std::chrono::milliseconds(0), int numRetries = 0);

		ssl_socket run(const std::string& host, const std::string& port);
		ssl_socket run(const std::string& host, const std::string& port, std::chrono::milliseconds retryDelay, int numRetries);

		void setRetries(std::chrono::milliseconds retryDelay, int numRetries);
		void setConnectHandler(connect_handler handler) {
			this->connectHandler = handler;
		}
		void cancel();

		~TCPConnector();

	private:
		void connect();
		void handleResolve(const boost::system::error_code & err, boost::asio::ip::tcp::resolver::iterator iterator);
		void handleConnect(const boost::system::error_code & err, boost::asio::ip::tcp::resolver::iterator iterator);

		boost::shared_ptr<boost::asio::ssl::context> sslContext;
		connect_handler connectHandler;
		bool running;

		std::thread* retryRunner;
		Condvar retryCondvar;
		void runRetry();
		std::chrono::milliseconds retryDelay;
		int maxRetries;
		int retryCount;

		ssl_socket socket;
		boost::asio::io_service* ioService;
		boost::asio::ip::tcp::resolver* resolver;
		boost::asio::ip::tcp::resolver::query* endpoint;
	};
}