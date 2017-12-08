#include "Client.h"
#include "TCPConnection.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/make_shared.hpp>

using namespace boost::asio::ip;

Client::Client()
	:ioServiceRunner(nullptr), ioService(nullptr), resolver(nullptr)
{
}

void Client::run(const std::string & ip, const std::string & port)
{
	running = true;
	ioService = new boost::asio::io_service();
	resolver = new tcp::resolver(*ioService);
	tcp::resolver::query endpoint(ip, port);
	resolver->async_resolve(endpoint, boost::bind(&Client::resolveHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	ioServiceRunner = new std::thread(std::bind(&Client::ioServiceHandler, shared_from_this()));
	ioServiceRunner->detach();
}

void Client::stop() {
	running = false;
	resolver->cancel();
}

Client::~Client()
{
	delete ioService;
	delete ioServiceRunner;
	delete resolver;
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
			tcpConnection = boost::make_shared<TCPConnection>(socket);
			tcpConnection->start();
			/*
			//TEST SEND
			auto req = boost::shared_ptr<http::response<http::string_body>>(new http::response<http::string_body>());
			resp->result(200);
			resp->keep_alive(false);
			resp->set(http::field::server, "Beast");
			resp->set(http::field::content_type, "text/plain");
			resp->body() = "dog";
			tcpConnection->send(resp);
			return;
			*/
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

void Client::ioServiceHandler()
{
	while (true) {
		if (!running) {
			return;
		}
		ioService->run();
	}
}
