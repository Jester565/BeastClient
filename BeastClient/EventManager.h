#pragma once
#include "Typedefs.h"
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <queue>

class EventHandler
{
public:
	EventHandler(const std::string& resource, evt_handler handler)
		:resource(resource), handler(handler)
	{
	}

	std::string getResource() {
		return resource;
	}

	evt_handler getHandler() {
		return handler;
	}

private:
	std::string resource;
	evt_handler handler;
};

class EventManager
{
public:
	EventManager();

	void runConnect(client_ptr);
	void runMessage(client_ptr, resp_ptr, const std::string& target);
	void runDisconnect(client_ptr);

	void runEvent(const std::string& resource, client_ptr, resp_ptr);

	void addEventHandler(EventHandler* evtHandler);

	void removeEventHandler(EventHandler* evtHandler);

	std::queue<std::pair<std::string, resp_ptr>> unHandledMessages;

	~EventManager();

private:
	void onMessageHandlerNotFound(client_ptr, resp_ptr, const std::string& target);

	std::unordered_map<std::string, std::unordered_set<EventHandler*>> eventHandlers;
};

