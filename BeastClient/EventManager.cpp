#include "EventManager.h"
#include "HttpClient.h"
#include <iostream>

using namespace boost::beast;

EventManager::EventManager()
{
}

void EventManager::runConnect(client_ptr client)
{
	runEvent("connect", client, nullptr);
}

void EventManager::runMessage(client_ptr client, resp_ptr resp, const std::string& target)
{
	runEvent(target, client, resp);
}

void EventManager::runDisconnect(client_ptr client)
{
	runEvent("disconnect", client, nullptr);
}

void EventManager::runEvent(const std::string & resource, client_ptr client, resp_ptr resp)
{
	auto it = eventHandlers.find(resource);
	if (it != eventHandlers.end()) {
		for (auto subIt = it->second.begin(); subIt != it->second.end(); it++) {
			(*subIt)->getHandler()(client, resp);
		}
	}
	else
	{
		onMessageHandlerNotFound(client, resp, resource);
	}
}

void EventManager::addEventHandler(EventHandler* evtHandler)
{
	auto it = eventHandlers.find(evtHandler->getResource());
	if (it == eventHandlers.end()) {
		std::unordered_set<EventHandler*> set;
		set.insert(evtHandler);
		eventHandlers.insert(std::make_pair(evtHandler->getResource(), set));
	}
	else {
		it->second.insert(evtHandler);
	}
}

void EventManager::removeEventHandler(EventHandler* evtHandler) {
	auto it = eventHandlers.find(evtHandler->getResource());
	if (it != eventHandlers.end()) {
		it->second.erase(evtHandler);
	}
}

EventManager::~EventManager()
{
}


void EventManager::onMessageHandlerNotFound(client_ptr client, resp_ptr resp, const std::string& target)
{
	unHandledMessages.push(std::make_pair(target, resp));
}
