#include <iostream>
#include <boost/make_shared.hpp>
#include "Client.h"
#include "EventManager.h"
#include <iostream>
#include <boost/beast.hpp>

using namespace bcli;


void connectHandler(client_ptr client, const std::string& target, resp_ptr resp) {
	if (client->isConnected()) {
		std::cout << "Connected" << std::endl;
		for (int i = 0; i < 50; i++) {
			req_ptr req = CreateRequest();
			req->target("alex");
			req->method(boost::beast::http::verb::post);
			req->body() = std::to_string(i);
			client->asyncSend(req);
		}
	}
	else {
		std::cout << "Failed to connect" << std::endl;
	}
}

void disconnectHandler(client_ptr client, const std::string& target, resp_ptr resp) {
	std::cout << "disconnected" << std::endl;
	client->asyncConnect("localhost", "5650", 2000, 5);
}

void testHandler(client_ptr client, const std::string& target, resp_ptr resp) {
	std::cout << *resp << std::endl;
}

int main() {
	boost::asio::io_service* ioService = new boost::asio::io_service();
	client_ptr client = boost::make_shared<Client>(ioService, 
		"C:/Users/ajcra/Desktop/north/ssl/client.crt",
		"C:/Users/ajcra/Desktop/north/ssl/client.key",
		"C:/Users/ajcra/Desktop/north/ssl/rootCA.crt");
	//EventHandler conHandler(client->getEventManager(), "connect", std::bind(&connectHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	EventHandler disHandler(client->getEventManager(), "disconnect", std::bind(&disconnectHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	EventHandler handler(client->getEventManager(), "alex", std::bind(&testHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	client->asyncConnect("localhost", "5600", 2000, 3);
	
	while (true) {
		client->getIOService()->run();
	}
}
