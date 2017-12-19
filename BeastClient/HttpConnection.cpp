#include "HttpConnection.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost::beast;

namespace bcli {
	HttpConnection::HttpConnection(socket_ptr socket, msg_handler msgHandler, disconnect_handler disHandler)
		:socket(socket), msgHandler(msgHandler), disHandler(disHandler)
	{
	}

	void HttpConnection::run()
	{
		readResponse();
	}

	void HttpConnection::send(req_ptr req)
	{
		sentTargets.push(std::string(req->target()));
		boost::unique_lock<boost::mutex> lock(queueMutex);
		reqStore.push(req);
		if (reqStore.size() == 1) {
			sendNoQueue(req);
		}
	}

	void HttpConnection::stop()
	{
		boost::system::error_code ec;
		socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		std::cout << "Stop err: " << ec << std::endl;
		socket->close();
	}

	HttpConnection::~HttpConnection()
	{
		std::cout << "Http connection destroyed" << std::endl;
	}

	void HttpConnection::sendNoQueue(req_ptr req)
	{
		req->prepare_payload();
		http::async_write(*socket, *req,
			boost::bind(&HttpConnection::asyncSendHandler,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				resp->need_eof()));
	}

	void HttpConnection::readResponse()
	{
		resp = CreateResponse();
		http::async_read(*socket, buffer, *resp,
			boost::bind(&HttpConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void HttpConnection::asyncReceiveHandler(const boost::system::error_code & error, unsigned int nBytes)
	{
		if (error == http::error::end_of_stream) {
			disHandler(shared_from_this());
			return;
		}

		if (error) {
			std::cerr << "Receive Error: " << error.message() << std::endl;
			disHandler(shared_from_this());
			return;
		}

		std::string target = sentTargets.front();
		sentTargets.pop();
		msgHandler(shared_from_this(), target, resp);
		readResponse();
	}

	void HttpConnection::asyncSendHandler(const boost::system::error_code & error, unsigned int nBytes, bool close)
	{
		if (error) {
			std::cerr << "Send Error: " << error.message() << std::endl;
		}
		boost::unique_lock<boost::mutex> lock(queueMutex);
		reqStore.pop();
		if (!reqStore.empty()) {
			sendNoQueue(reqStore.front());
		}
	}
}