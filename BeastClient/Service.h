#pragma once
#define RAPIDJSON_HAS_STRING 1
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/prettywriter.h>

using namespace rapidjson;

struct Check {
	void setName(const std::string& name) {
		this->Name = name;
	}
	std::string getName() {
		return Name;
	}

	void setID(const std::string& id) {
		this->ID = id;
	}
	boost::optional<std::string> getID() {
		return ID;
	}

	void setDeregisterCriticalServiceAfter(const std::string& val) {
		DeregisterCriticalServiceAfter = val;
	}
	void setDeregisterCriticalServiceAfter(int mins, int secs) {
		secs += mins * 60;
		Interval = std::to_string(secs) + "s";
	}
	boost::optional<std::string> getDeregisterCriticalServiceAfter() {
		return DeregisterCriticalServiceAfter;
	}

	void setHTTP(const std::string& url) {
		this->HTTP = url;
	}
	boost::optional<std::string> getHTTP() {
		return HTTP;
	}

	void setInterval(const std::string& val) {
		Interval = val;
	}
	void setInterval(int mins, int secs) {
		secs += mins * 60;
		Interval = std::to_string(secs) + "s";
	}
	boost::optional<std::string> getIntervalStr() {
		return Interval;
	}

	void setArgs(const std::vector<std::string>& args) {
		this->args = args;
	}
	void addArg(const std::string& arg) {
		args.push_back(arg);
	}
	std::vector<std::string> getArgs() {
		return args;
	}

	void parse(Value& v);

	void serialize(Writer<StringBuffer>& writer);

private:
	std::string Name;
	boost::optional<std::string> ID;
	boost::optional<std::string> DeregisterCriticalServiceAfter;
	boost::optional<std::string> HTTP;
	boost::optional<std::string> Interval;
	//std::string TTL;
	std::vector<std::string> args;
};

class Service
{
public:
	Service();
	
	void setName(const std::string& name) {
		this->name = name;
	}
	std::string getName() {
		return name;
	}
	void setID(const std::string& id) {
		this->id = id;
	}
	boost::optional<std::string> getID() {
		return id;
	}
	void setAddress(const std::string& address) {
		this->address = address;
	}
	boost::optional<std::string> getAddress() {
		return address;
	}
	void setPort(const std::string& port) {
		this->port = std::stoi(port);
	}
	boost::optional<std::string> getPortStr() {
		boost::optional<std::string> str;
		if (port) {
			str = std::to_string(*port);
		}
		return str;
	}
	boost::optional<int> getPortInt() {
		return port;
	}

	void parse(Value& v);

	void serialize(Writer<StringBuffer>& writer);

	std::vector<std::string> tags;

	std::vector<Check> checks;

	~Service();

private:
	std::string name;
	boost::optional<std::string> id;
	boost::optional<std::string> address;
	boost::optional<int> port;
};

