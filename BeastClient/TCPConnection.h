#pragma once
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <shared_mutex>
#include <queue>

using namespace boost::beast;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:
	TCPConnection(socket_ptr socket);

	void start();

	void send(boost::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>> resp);

	~TCPConnection();

private:
	void close();

	void readRequest();
	void asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes);
	void asyncSendHandler(const boost::system::error_code& error, unsigned int nBytes, bool close);
	socket_ptr socket;

	boost::beast::flat_buffer buffer;
	boost::beast::http::request<http::string_body> req;
	boost::shared_ptr<boost::beast::http::response<http::string_body>> respStore;
	//std::queue<boost::shared_ptr<boost::beast::http::response<http::string_body>>> respStore;
	//boost::shared_mutex queueMutex;
};

