#pragma once
#include "Typedefs.h"
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <queue>

namespace bcli {
	class EventManager;

	class EventHandler
	{
	public:
		EventHandler(boost::shared_ptr<EventManager> evtManager, const std::string& resource, evt_handler handler);

		std::string getResource() {
			return resource;
		}

		evt_handler getHandler() {
			return handler;
		}

		~EventHandler();

	private:
		boost::shared_ptr<EventManager> evtManager;
		std::string resource;
		evt_handler handler;
	};

	class EventManager
	{
	public:
		EventManager();

		void runConnect(client_ptr);
		void runMessage(const std::string& target, client_ptr, resp_ptr);
		void runDisconnect(client_ptr);

		void runEvent(const std::string& resource, client_ptr, resp_ptr);

		void addEventHandler(EventHandler* evtHandler);

		void removeEventHandler(EventHandler* evtHandler);

		std::queue<std::pair<std::string, resp_ptr>> unHandledMessages;

		~EventManager();

	private:
		void onMessageHandlerNotFound(const std::string& target, client_ptr, resp_ptr);

		std::unordered_map<std::string, std::unordered_set<EventHandler*>> eventHandlers;
	};
}