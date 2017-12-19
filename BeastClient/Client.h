#pragma once
#include "Typedefs.h"
#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>
#include <queue>

namespace bcli {
	class EventManager;
	class TCPConnector;

	class Client : public boost::enable_shared_from_this<Client>
	{
	public:
		Client(boost::asio::io_service* ioService = nullptr);

		bool connect(const std::string& ip, const std::string& port, int retryDelayMillis = 0, int maxRetries = 0);

		socket_ptr asyncConnect(const std::string& ip, const std::string& port, int retryDelayMillis = 0, int maxRetries = 0);

		bool isConnected() {
			return connected;
		}

		resp_ptr send(req_ptr req);

		bool asyncSend(req_ptr req, evt_handler handler = nullptr);

		boost::asio::io_service* getIOService() {
			return ioService;
		}

		boost::shared_ptr<EventManager> getEventManager() {
			return evtManager;
		}

		void stop();
		void shutdown();

		~Client();

	private:
		boost::shared_ptr<TCPConnector> tcpConnector;
		boost::tribool connected;

		void connectHandler(const boost::system::error_code& ec, socket_ptr socket);
		void syncHandler(client_ptr, const std::string& target, resp_ptr);
		resp_ptr syncStore;
		void messageHandler(connection_ptr, const std::string& target, resp_ptr);
		void disconnectHandler(connection_ptr);
		boost::shared_ptr<EventManager> evtManager;

		boost::asio::io_service* ioService;
		connection_ptr httpConnection;

		std::queue<evt_handler> perMsgHandlers;
	};
}