#pragma once
#include "Typedefs.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <queue>
#include <string>

namespace bcli {
	class HttpConnection : public boost::enable_shared_from_this<HttpConnection>
	{
	public:
		HttpConnection(socket_ptr socket, msg_handler msgHandler = nullptr, disconnect_handler disHandler = nullptr);

		void run();
		void send(req_ptr req);
		void stop();

		~HttpConnection();

	private:
		void sendNoQueue(req_ptr req);

		void readResponse();
		void asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes);
		void asyncSendHandler(const boost::system::error_code& error, unsigned int nBytes, bool close);
		socket_ptr socket;

		boost::beast::flat_buffer buffer;
		resp_ptr resp;

		msg_handler msgHandler;
		disconnect_handler disHandler;
		std::queue<req_ptr> reqStore;
		std::queue<std::string> sentTargets;
		boost::mutex queueMutex;
	};
}