#pragma once
#include "Typedefs.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>


class Client : public boost::enable_shared_from_this<Client>
{
public:
	Client();

	void run(const std::string& ip, const std::string& port);

	boost::asio::io_service* getIOService() {
		return ioService;
	}

	void stop();

	~Client();

private:
	void messageHandler(client_ptr, resp_ptr);
	void disconnectHandler(client_ptr);

	void resolveHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator epIter);
	boost::asio::io_service* ioService;
	client_ptr httpClient;
	boost::asio::ip::tcp::resolver* resolver;
};

