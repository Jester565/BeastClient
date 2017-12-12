#include "Client.h"
#include "HttpClient.h"
#include "EventManager.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/make_shared.hpp>

using namespace boost::asio::ip;

Client::Client()
	:ioService(nullptr), resolver(nullptr), connected(false), syncStore(nullptr)
{
	evtManager = new EventManager();
}

void Client::run(const std::string & ip, const std::string & port)
{
	ioService = new boost::asio::io_service();
	resolver = new tcp::resolver(*ioService);
	tcp::resolver::query endpoint(ip, port);
	auto resolveResult = resolver->resolve(endpoint);
	boost::shared_ptr<tcp::socket> socket = boost::make_shared<tcp::socket>(*ioService);
	boost::asio::connect(*socket, resolveResult.begin(), resolveResult.end());
	connected = true;
	httpClient = boost::shared_ptr<HttpClient>(new HttpClient(socket,
		(msg_handler)std::bind(&Client::messageHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
		(disconnect_handler)std::bind(&Client::disconnectHandler, this, std::placeholders::_1)));
	httpClient->start();
	evtManager->runConnect(httpClient);
}

resp_ptr Client::makeRequest(req_ptr req)
{
	while (connected && syncStore == nullptr) {
		httpClient->send(req, std::bind(&Client::syncHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ioService->run_one();
	}
	resp_ptr result = syncStore;
	syncStore = nullptr;
	return result;
}

void Client::stop() {
	resolver->cancel();
}

Client::~Client()
{
	delete ioService;
	delete resolver;
}

void Client::syncHandler(client_ptr, resp_ptr resp, const std::string & target)
{
	syncStore = resp;
}

void Client::messageHandler(client_ptr client, resp_ptr resp, const std::string& target)
{
	evtManager->runMessage(client, resp, target);
}

void Client::disconnectHandler(client_ptr client)
{
	std::cout << "DISCONNECTED" << std::endl;
	connected = false;
	evtManager->runDisconnect(client);
}

/*
void Client::resolveHandler(const boost::system::error_code & ec, boost::asio::ip::tcp::resolver::iterator epIter)
{
	if (ec) {
		std::cerr << "Resolver error" << std::endl;
	}
	tcp::resolver::iterator epIterEnd;
	boost::system::error_code connectEC;
	while (epIter != epIterEnd) {
		if (!connectEC) {
			std::cout << "Connected" << std::endl;
			httpClient = boost::shared_ptr<HttpClient>(new HttpClient(socket,
				(msg_handler)std::bind(&Client::messageHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
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
*/