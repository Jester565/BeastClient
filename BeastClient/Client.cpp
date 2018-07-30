#include "Client.h"
#include "HttpConnection.h"
#include "EventManager.h"
#include "TCPConnector.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <iostream>

using namespace boost::asio::ip;
namespace bcli {
	Client::Client(boost::asio::io_service* ioService, const std::string& certFile, const std::string& keyFile, const std::string& verifyPath)
		:ioService(ioService), syncStore(nullptr), connected(boost::logic::indeterminate)
	{
		if (ioService == nullptr) {
			ioService = new boost::asio::io_service();
		}
		sslContext = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23_client);
		sslContext->set_options(boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::single_dh_use);
		try {
			sslContext->use_certificate_chain_file(certFile);
			sslContext->use_private_key_file(keyFile, boost::asio::ssl::context::pem);
			sslContext->set_verify_mode(boost::asio::ssl::verify_peer);
			sslContext->load_verify_file(verifyPath);
		}
		catch (std::exception& ex) {
			std::cerr << "ERROR WHEN CREATING SSL CONTEXT: " << ex.what() << std::endl;
		}

		tcpConnector = boost::make_shared<TCPConnector>(ioService, sslContext, std::bind(&Client::connectHandler, this, std::placeholders::_1, std::placeholders::_2));
		evtManager = boost::make_shared<EventManager>();
	}

	bool Client::connect(const std::string & ip, const std::string & port, int retryDelayMillis, int maxRetries)
	{
		if (asyncConnect(ip, port, retryDelayMillis, maxRetries)) {
			while (true) {
				if (connected || !connected) {
					return connected;
				}
				ioService->run_one();
			}
		}
		return false;
	}

	ssl_socket Client::asyncConnect(const std::string & ip, const std::string & port, int retryDelayMillis, int maxRetries)
	{
		if (connected) {
			std::cerr << "Cannot connect client when already connected" << std::endl;
			return nullptr;
		}
		connected = boost::logic::indeterminate;
		return tcpConnector->run(ip, port, std::chrono::milliseconds(retryDelayMillis), maxRetries);
	}

	resp_ptr Client::send(req_ptr req)
	{
		if (asyncSend(req, std::bind(&Client::syncHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))) {
			while (connected && syncStore == nullptr) {
				ioService->run_one();
			}
			resp_ptr result = syncStore;
			syncStore = nullptr;
			return result;
		}
		return nullptr;
	}

	bool Client::asyncSend(req_ptr req, evt_handler handler)
	{
		if (connected) {
			perMsgHandlers.push(handler);
			httpConnection->send(req);
			return true;
		}
		return false;
	}

	void Client::stop() {
		connected = false;
		while (!perMsgHandlers.empty()) {
			perMsgHandlers.pop();
		}
		tcpConnector->cancel();
		if (httpConnection != nullptr) {
			httpConnection->stop();
			httpConnection = nullptr;
		}
	}

	Client::~Client()
	{
		delete ioService;
		ioService = nullptr;
	}

	void Client::syncHandler(client_ptr, const std::string& target, resp_ptr resp)
	{
		syncStore = resp;
	}

	void Client::connectHandler(const boost::system::error_code & ec, ssl_socket socket)
	{
		if (ec) {
			connected = false;
			std::cerr << "Connect ERROR: " << ec << std::endl;
		}
		else {
			connected = true;
			httpConnection = boost::make_shared<HttpConnection>(socket,
				(msg_handler)std::bind(&Client::messageHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
				(disconnect_handler)std::bind(&Client::disconnectHandler, shared_from_this(), std::placeholders::_1));
			httpConnection->run();
		}
		evtManager->runConnect(shared_from_this());
	}

	void Client::messageHandler(connection_ptr connection, const std::string& target, resp_ptr resp)
	{
		evt_handler handler = perMsgHandlers.front();
		perMsgHandlers.pop();
		if (handler != nullptr) {
			handler(shared_from_this(), target, resp);
		}
		evtManager->runMessage(target, shared_from_this(), resp);
	}

	void Client::disconnectHandler(connection_ptr connection)
	{
		std::cout << "D Handler" << std::endl;
		stop();
		evtManager->runDisconnect(shared_from_this());
	}

	void Client::shutdown() {
		stop();
		tcpConnector->setConnectHandler(nullptr);
	}
}