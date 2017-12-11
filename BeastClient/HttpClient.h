#pragma once
#include "Typedefs.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>

using namespace boost::beast;

class HttpClient : public boost::enable_shared_from_this<HttpClient>
{
public:
	HttpClient(socket_ptr socket, msg_handler msgHandler = nullptr, disconnect_handler disHandler = nullptr);

	void start();

	void send(req_ptr req);

	~HttpClient();

private:
	void close();

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
	boost::mutex queueMutex;
};
