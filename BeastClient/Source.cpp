#include <iostream>
#include <boost/make_shared.hpp>
#include "Client.h"

int main() {
	boost::shared_ptr<Client> client = boost::make_shared<Client>();
	client->run("localhost", "5650");

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}