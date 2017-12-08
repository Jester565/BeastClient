#pragma once
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>


class TCPConnection;

class Client : public boost::enable_shared_from_this<Client>
{
public:
	Client();

	void run(const std::string& ip, const std::string& port);

	void stop();

	~Client();

private:
	void resolveHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator epIter);
	void ioServiceHandler();
	boost::asio::io_service* ioService;
	boost::shared_ptr<TCPConnection> tcpConnection;
	boost::asio::ip::tcp::resolver* resolver;

	std::atomic<bool> running;
	std::thread* ioServiceRunner;
};

