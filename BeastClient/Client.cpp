#include "Client.h"
#include "HttpClient.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/make_shared.hpp>

using namespace boost::asio::ip;

Client::Client()
	:ioService(nullptr), resolver(nullptr)
{
}

void Client::run(const std::string & ip, const std::string & port)
{
	ioService = new boost::asio::io_service();
	resolver = new tcp::resolver(*ioService);
	tcp::resolver::query endpoint(ip, port);
	resolver->async_resolve(endpoint, boost::bind(&Client::resolveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void Client::stop() {
	resolver->cancel();
}

Client::~Client()
{
	delete ioService;
	delete resolver;
}

void Client::messageHandler(client_ptr client, resp_ptr resp)
{
	std::cout << "RESPONSE" << std::endl;
	std::cout << *resp << std::endl;
}

void Client::disconnectHandler(client_ptr)
{
	std::cout << "DISCONNECTED" << std::endl;
}

void Client::resolveHandler(const boost::system::error_code & ec, boost::asio::ip::tcp::resolver::iterator epIter)
{
	if (ec) {
		std::cerr << "Resolver error" << std::endl;
	}
	boost::shared_ptr<tcp::socket> socket = boost::make_shared<tcp::socket>(*ioService);
	tcp::resolver::iterator epIterEnd;
	boost::system::error_code connectEC;
	while (epIter != epIterEnd) {
		socket->connect(*epIter, connectEC);
		if (!connectEC) {
			std::cout << "Connected" << std::endl;
			httpClient = boost::shared_ptr<HttpClient>(new HttpClient(socket,
				(msg_handler)std::bind(&Client::messageHandler, this, std::placeholders::_1, std::placeholders::_2),
				(disconnect_handler)std::bind(&Client::disconnectHandler, this, std::placeholders::_1)));
			httpClient->start();
			//TEST SEND
			for (int i = 0; i < 50; i++) {
				req_ptr req = CreateRequest();
				req->target("test");
				req->method(http::verb::post);
				req->set(http::field::server, "Beast");
				req->set(http::field::content_type, "text/plain");
				req->body() = std::to_string(i);
				httpClient->send(req);
			}
			//TEST SEND DONE
			return;
		}
		if (connectEC.value() != boost::asio::error::host_not_found) {
			std::cerr << "Resolve Error: " << connectEC.message();
		}
		socket->shutdown(tcp::socket::shutdown_both);
		socket->close();
		connectEC.clear();
		epIter++;
	}
	std::cerr << "Could not connect" << std::endl;
}
