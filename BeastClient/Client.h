#pragma once
#include "Typedefs.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

class EventManager;

class Client : public boost::enable_shared_from_this<Client>
{
public:
	Client();

	void run(const std::string& ip, const std::string& port);

	boost::asio::io_service* getIOService() {
		return ioService;
	}

	EventManager* getEventManager() {
		return evtManager;
	}

	resp_ptr makeRequest(req_ptr req);

	client_ptr getHttpClient() {
		return httpClient;
	}

	void stop();

	~Client();

private:
	resp_ptr syncStore;
	void syncHandler(client_ptr, resp_ptr, const std::string& target);
	bool connected;
	EventManager* evtManager;
	void messageHandler(client_ptr, resp_ptr, const std::string& target);
	void disconnectHandler(client_ptr);

	//void resolveHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator epIter);
	boost::asio::io_service* ioService;
	client_ptr httpClient;
	boost::asio::ip::tcp::resolver* resolver;
};

