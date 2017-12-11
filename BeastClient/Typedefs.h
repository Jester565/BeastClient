#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <functional>

class HttpClient;

typedef boost::shared_ptr <boost::beast::http::request<boost::beast::http::string_body>> req_ptr;
typedef boost::shared_ptr <boost::beast::http::response<boost::beast::http::string_body>> resp_ptr;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

static req_ptr CreateRequest() {
	return req_ptr(new boost::beast::http::request<boost::beast::http::string_body>());
}

static resp_ptr CreateResponse() {
	return resp_ptr(new boost::beast::http::response<boost::beast::http::string_body>());
}

typedef boost::shared_ptr<HttpClient> client_ptr;

typedef std::function<void(socket_ptr)> raw_connect_handler;
typedef std::function<void(client_ptr)> connect_handler;
typedef std::function<void(client_ptr, resp_ptr)> msg_handler;
typedef std::function<void(client_ptr)> disconnect_handler;
