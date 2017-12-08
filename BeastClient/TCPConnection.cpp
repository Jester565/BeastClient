#include "TCPConnection.h"
#include <boost/bind.hpp>

using namespace boost::beast;
TCPConnection::TCPConnection(socket_ptr socket)
	:socket(socket)
{
}

void TCPConnection::start()
{
	readRequest();
}

void TCPConnection::send(boost::shared_ptr<http::response<http::string_body>> resp)
{
	resp->prepare_payload();
	http::async_write(*socket, *resp, boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, resp->need_eof()));
}

TCPConnection::~TCPConnection()
{
}

void TCPConnection::close()
{
	boost::system::error_code ec;
	socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}

void TCPConnection::readRequest()
{
	http::async_read(*socket, buffer, req,
		boost::bind(&TCPConnection::asyncReceiveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::asyncReceiveHandler(const boost::system::error_code & error, unsigned int nBytes)
{
	if (error == http::error::end_of_stream) {
		close();
	}


	if (error) {
		std::cerr << "Receive Error: " << error.message() << std::endl;
		return;
	}
	std::cout << "RECEIVED: " << nBytes << std::endl;
	std::cout << req << std::endl;
	//Test Loop
	for (int i = 0; i < 20; i++) {
		auto resp = boost::shared_ptr<http::response<http::string_body>>(new http::response<http::string_body>());
		resp->result(200);
		resp->keep_alive(false);
		resp->set(http::field::server, "Beast");
		resp->set(http::field::content_type, "text/plain");
		resp->body() = std::to_string(i);
		send(resp);
	}
}

void TCPConnection::asyncSendHandler(const boost::system::error_code & error, unsigned int nBytes, bool close)
{
	if (error) {
		std::cerr << "Send Error: " << error.message() << std::endl;
	}
	respStore = nullptr;
}
