#include "Discoverer.h"
#include "Client.h"
#include <iostream>
#include <boost/beast.hpp>
using namespace rapidjson;
using namespace boost::beast;

Discoverer::Discoverer(const std::string& consulHost, const std::string& consulPort)
	:consulHost(consulHost), consulPort(consulPort)
{
	ioService = new boost::asio::io_service();
}

void Discoverer::getService(const std::string & serviceID, consul_handler handler)
{
	SendReqStruct srs;
	srs.target = "/v1/catalog/service/" + serviceID;
	srs.client = createConsulClient();
	srs.handler = (evt_handler)std::bind(&Discoverer::onGetServiceResponse, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, handler);
	sendRequest(srs);
}

void Discoverer::putHttpService(Service& service)
{
	auto req = CreateRequest();
	req->target("/v1/agent/service/register");
	req->method(http::verb::put);
	req->set(http::field::host, "localhost");
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	service.serialize(writer);
	req->body() = sb.GetString();
	std::cout << req->body() << std::endl;
	client_ptr client = createConsulClient();
	std::cout << "PUTTING SERVIE" << std::endl;
	client->asyncSend(req);
}

Discoverer::~Discoverer()
{
}

void SerializeString(Document& json, Document::AllocatorType& allocator, const char* name, const char* val) {
	Value nameVal;
	nameVal.SetString(name, allocator);
	Value strVal;
	strVal.SetString(val, allocator);
	json.AddMember(nameVal, strVal, allocator);
}

void SerializeInt(Document& json, Document::AllocatorType& allocator, const char* name, int val) {
	Value nameVal;
	nameVal.SetString(name, allocator);
	Value intVal;
	intVal.SetInt(val);
	json.AddMember(nameVal, intVal, allocator);
}

void Discoverer::sendRequest(const SendReqStruct& srs) {
	auto req = CreateRequest();
	req->target(srs.target);
	req->set(http::field::host, consulHost);
	req->method(srs.method);
	req->set(http::field::accept, "application/json");
	if (srs.consulIndex >= 0) {
		req->set("X-Consul-Index", std::to_string(srs.consulIndex));
	}
	srs.client->asyncSend(req, srs.handler);
}

client_ptr Discoverer::createConsulClient()
{
	boost::shared_ptr<Client> client = boost::make_shared<Client>(ioService);
	client->connect(consulHost, consulPort);
	return client;
}

void Discoverer::onGetServiceResponse(client_ptr client, const std::string & target, resp_ptr resp, consul_handler handler)
{
	auto wrappedHandler = (evt_handler)std::bind(&Discoverer::onGetServiceResponse, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, handler);
	Document json;
	std::cout << resp->body().c_str() << std::endl;
	json.Parse(resp->body().c_str());
	auto arr = json.GetArray();
	if (arr.Size() == 0) {
		SendReqStruct srs;
		srs.target = target;
		srs.client = client;
		srs.consulIndex = std::stoi(resp->at("X-Consul-Index").to_string());
		srs.handler = wrappedHandler;
		sendRequest(srs);
		return;
	}
	req_ptr nextReq = handler(json);
	if (nextReq == nullptr) {
		client->stop();
		return;
	}
	client->asyncSend(nextReq, wrappedHandler);
}

SendReqStruct::SendReqStruct()
	:client(nullptr), method(http::verb::get), consulIndex(-1), handler(nullptr)
{
}
