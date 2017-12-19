#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/make_shared.hpp>
#include <functional>

namespace bcli {
	class HttpConnection;
	class Client;

	typedef boost::shared_ptr <boost::beast::http::request<boost::beast::http::string_body>> req_ptr;
	typedef boost::shared_ptr <boost::beast::http::response<boost::beast::http::string_body>> resp_ptr;
	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

	static req_ptr CreateRequest() {
		return req_ptr(new boost::beast::http::request<boost::beast::http::string_body>());
	}

	static resp_ptr CreateResponse() {
		return resp_ptr(new boost::beast::http::response<boost::beast::http::string_body>());
	}

	typedef boost::shared_ptr<Client> client_ptr;
	typedef boost::shared_ptr<HttpConnection> connection_ptr;

	typedef std::function<void(const boost::system::error_code & err, socket_ptr)> connect_handler;
	typedef std::function<void(connection_ptr, const std::string&, resp_ptr)> msg_handler;
	typedef std::function<void(connection_ptr)> disconnect_handler;
	typedef std::function<void(client_ptr, const std::string&, resp_ptr)> evt_handler;
}