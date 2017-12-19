#pragma once
#include "Typedefs.h"
#include <string>
#include <functional>
#include <rapidjson/document.h>
#include "Service.h"
#include <vector>
#include <boost/asio/io_service.hpp>

using namespace bcli;

typedef std::function<req_ptr(rapidjson::Document&)> consul_handler;

struct SendReqStruct {
	SendReqStruct();
	client_ptr client;
	std::string target;
	boost::beast::http::verb method;
	int consulIndex;
	evt_handler handler;
};

class Discoverer
{
public:
	Discoverer(const std::string& consulHost, const std::string& consulPort);

	void getService(const std::string& serviceID, consul_handler handler);

	void putHttpService(Service& service);

	void sendRequest(const SendReqStruct& srs);

	client_ptr createConsulClient();

	boost::asio::io_service* getIOService() {
		return ioService;
	}

	~Discoverer();

private:
	boost::asio::io_service* ioService;
	void onGetServiceResponse(client_ptr client, const std::string& target, resp_ptr resp, consul_handler);
	std::string consulHost;
	std::string consulPort;
};

