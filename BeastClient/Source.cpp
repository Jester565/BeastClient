#include <iostream>
#include <boost/make_shared.hpp>
#include "Client.h"

int main() {
	boost::shared_ptr<Client> client = boost::make_shared<Client>();
	client->run("localhost", "5650");

	client->getIOService()->run();
}