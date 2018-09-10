#include "EventManager.h"
#include "HttpConnection.h"
#include <iostream>

using namespace boost::beast;

namespace bcli {
	EventManager::EventManager()
	{
	}

	void EventManager::runConnect(client_ptr client)
	{
		runEvent("connect", client, nullptr);
	}

	void EventManager::runMessage(const std::string& target, client_ptr client, resp_ptr resp)
	{
		runEvent(target, client, resp);
	}

	void EventManager::runDisconnect(client_ptr client)
	{
		runEvent("disconnect", client, nullptr);
	}

	void EventManager::runEvent(const std::string & target, client_ptr client, resp_ptr resp)
	{
		auto it = eventHandlers.find(target);
		if (it != eventHandlers.end()) {
			for (auto subIt = it->second.begin(); subIt != it->second.end(); subIt++) {
				(*subIt)->getHandler()(client, target, resp);
			}
		}
		else
		{
			onMessageHandlerNotFound(target, client, resp);
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

	void EventManager::onMessageHandlerNotFound(const std::string& target, client_ptr client, resp_ptr resp)
	{
		unHandledMessages.push(std::make_pair(target, resp));
	}

	EventHandler::EventHandler(boost::shared_ptr<EventManager> evtManager, const std::string & resource, evt_handler handler)
		:evtManager(evtManager), resource(resource), handler(handler)
	{
		evtManager->addEventHandler(this);
	}

	EventHandler::~EventHandler()
	{
		evtManager->removeEventHandler(this);
	}
}