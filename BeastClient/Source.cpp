#include <iostream>
#include <boost/make_shared.hpp>
#include "Client.h"
#include "EventManager.h"
#include "Discoverer.h"
#include <iostream>
#include <boost/beast.hpp>

using namespace bcli;

/*
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
	client_ptr client = boost::make_shared<Client>();
	EventHandler conHandler(client->getEventManager(), "connect", std::bind(&connectHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	EventHandler disHandler(client->getEventManager(), "disconnect", std::bind(&disconnectHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	EventHandler handler(client->getEventManager(), "alex", std::bind(&testHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	client->asyncConnect("localhost", "5650", 2000, 3);
	
	while (true) {
		client->getIOService()->run();
	}
}
*/

req_ptr serHandler(rapidjson::Document& json) {
	std::cout << "HANDLER" << std::endl;
	return nullptr;
}

int main() {
	Discoverer discoverer("localhost", "8500");
	Service service;
	service.setName("alex");
	service.setAddress("localhost");
	service.setPort("8500");
	discoverer.getService("alex", std::bind(&serHandler, std::placeholders::_1));
	while (true) {
		discoverer.getIOService()->run();
	}
}