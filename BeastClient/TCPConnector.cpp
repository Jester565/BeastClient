#include "TCPConnector.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost::asio::ip;

namespace bcli {
	TCPConnector::TCPConnector(boost::asio::io_service * ioService, boost::shared_ptr<boost::asio::ssl::context> sslContext, connect_handler conHandler, std::chrono::milliseconds retryDelay, int numRetries)
		:connectHandler(conHandler), sslContext(sslContext), ioService(ioService), running(false), retryRunner(nullptr)
	{
		resolver = new tcp::resolver(*ioService);
		setRetries(retryDelay, numRetries);
	}

	ssl_socket TCPConnector::run(const std::string & host, const std::string & port)
	{
		if (!running) {
			running = true;
			socket = boost::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*ioService, *sslContext);
			if (endpoint != nullptr) {
				delete endpoint;
				endpoint = nullptr;
			}
			endpoint = new tcp::resolver::query(host, port);
			connect();
			return socket;
		}
		std::cerr << "Cannot run while TCPConnector already running!" << std::endl;
		return nullptr;
	}
	ssl_socket TCPConnector::run(const std::string & host, const std::string & port, std::chrono::milliseconds retryDelay, int numRetries)
	{
		setRetries(retryDelay, numRetries);
		return run(host, port);
	}

	void TCPConnector::setRetries(std::chrono::milliseconds retryDelay, int numRetries)
	{
		if (!running) {
			this->retryDelay = retryDelay;
			this->maxRetries = numRetries;
		}
		else {
			std::cerr << "Cannot change retries while TCPConnector is already running" << std::endl;
		}
	}

	void TCPConnector::cancel()
	{
		running = false;
		retryCondvar.set();
		resolver->cancel();
		if (running && socket != nullptr) {
			boost::system::error_code ec;
			socket->lowest_layer().shutdown(tcp::socket::shutdown_both, ec);
			std::cout << "TCP Connect close " << ec << std::endl;
			socket->lowest_layer().close();
		}
		//socket->cancel();
	}

	TCPConnector::~TCPConnector()
	{
		delete resolver;
		resolver = nullptr;
		if (retryRunner != nullptr) {
			delete retryRunner;
			retryRunner = nullptr;
		}
		if (endpoint != nullptr) {
			delete endpoint;
			endpoint = nullptr;
		}
	}

	void TCPConnector::connect()
	{
		resolver->async_resolve(*endpoint, boost::bind(&TCPConnector::handleResolve, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	}

	void TCPConnector::handleResolve(const boost::system::error_code & err, boost::asio::ip::tcp::resolver::iterator iterator)
	{
		//If resolve error, then fail instantly
		if (err) {
			if (err == boost::asio::error::operation_aborted) {
				return;
			}
			running = false;
			if (connectHandler != nullptr) {
				connectHandler(err, nullptr);
			}
			return;
		}
		socket->lowest_layer().async_connect(*iterator, boost::bind(&TCPConnector::handleConnect, shared_from_this(), boost::asio::placeholders::error, ++iterator));
	}

	void TCPConnector::handleConnect(const boost::system::error_code & err, boost::asio::ip::tcp::resolver::iterator iterator)
	{
		if (err) {
			if (err == boost::asio::error::operation_aborted) {
				return;
			}
			//Try the next possible endpoint
			if (iterator != tcp::resolver::iterator()) {
				socket->lowest_layer().close();
				socket->lowest_layer().async_connect(*iterator, boost::bind(&TCPConnector::handleConnect, shared_from_this(), boost::asio::placeholders::error, ++iterator));
				return;
			}
			//We've tried all connections wait to retry
			if (retryCount < maxRetries) {
				retryCount++;
				std::cout << "detatched" << std::endl;
				if (retryRunner != nullptr) {
					delete retryRunner;
					retryRunner = nullptr;
				}
				retryRunner = new std::thread(std::bind(&TCPConnector::runRetry, this));
				retryRunner->detach();
				return;
			}
			//Out of retries
			running = false;
			if (connectHandler != nullptr) {
				connectHandler(err, nullptr);
			}
			return;
		}
		//We have found a connection!
		running = false;
		if (connectHandler != nullptr) {
			connectHandler(err, socket);
		}
	}

	void TCPConnector::runRetry()
	{
		//wait for the retryDelay
		retryCondvar.wait(retryDelay);
		if (!running) {
			return;
		}
		connect();
	}
}