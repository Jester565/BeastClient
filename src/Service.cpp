#include "Service.h"
#define RAPIDJSON_HAS_STRING 1
using namespace rapidjson;

static void SerializeString(Document& json, Document::AllocatorType& allocator, const char* name, const char* val) {
	Value nameVal;
	nameVal.SetString(name, allocator);
	Value strVal;
	strVal.SetString(val, allocator);
	json.AddMember(nameVal, strVal, allocator);
}

static void SerializeString(Document& json, Document::AllocatorType& allocator, const char* name, const std::string& val) {
	SerializeString(json, allocator, name, val.c_str());
}

static void SerializeInt(Document& json, Document::AllocatorType& allocator, const char* name, int val) {
	Value nameVal;
	nameVal.SetString(name, allocator);
	Value intVal;
	intVal.SetInt(val);
	json.AddMember(nameVal, intVal, allocator);
}

Service::Service()
{
}

void Service::parse(Value & json)
{
	json.SetObject();
	json["Name"].GetString();
	if (json.HasMember("ID")) {
		id = json["ID"].GetString();
	}
	if (json.HasMember("Address")) {
		address = json["Address"].GetString();
	}
	if (json.HasMember("Port")) {
		port = json["port"].GetInt();
	}
	if (json.HasMember("Check")) {
		auto checkArr = json.GetArray();
		for (SizeType i = 0; i < checkArr.Size(); i++) {
			Check check;
			check.parse(checkArr[i]);
			checks.push_back(check);
		}
	}
}

void Service::serialize(Writer<StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("Name");
	writer.String(name.c_str());
	if (id) {
		writer.String("ID");
		writer.String(id->c_str());
	}
	if (address) {
		writer.String("Address");
		writer.String(address->c_str());
	}
	if (port) {
		writer.String("Port");
		writer.Int(*port);
	}
	if (!checks.empty()) {
		writer.String("Check");
		writer.StartArray();
		for (auto it = checks.begin(); it != checks.end(); it++) {
			it->serialize(writer);
		}
		writer.EndArray();
	}
	writer.EndObject();
}

Service::~Service()
{
}

void Check::parse(Value& json) {
	json.SetObject();
	Name = json["Name"].GetString();
	if (json.HasMember("ID")) {
		ID = json["ID"].GetString();
	}
	if (json.HasMember("DeregisterCriticalServiceAfter")) {
		DeregisterCriticalServiceAfter = json["DeregisterCriticalServiceAfter"].GetString();
	}
	if (json.HasMember("HTTP")) {
		HTTP = json["HTTP"].GetString();
	}
	if (json.HasMember("Interval")) {
		Interval = json["Interval"].GetString();
	}
	if (json.HasMember("Args")) {
		auto argsArr = json["Args"].GetArray();
		for (SizeType i = 0; i < argsArr.Size(); i++) {
			args.push_back(argsArr[i].GetString());
		}
	}
}

void Check::serialize(Writer<StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("Name");
	writer.String(Name.c_str());
	if (ID) {
		writer.String("ID");
		writer.String(ID->c_str());
	}
	if (DeregisterCriticalServiceAfter) {
		writer.String("DeregsiterCriticalServiceAfter");
		writer.String(DeregisterCriticalServiceAfter->c_str());
	}
	if (HTTP) {
		writer.String("HTTP");
		writer.String(HTTP->c_str());
	}
	if (Interval) {
		writer.String("Interval");
		writer.String(Interval->c_str());
	}
	writer.String("Args");
	writer.StartArray();
	for (auto it = args.begin(); it != args.end(); it++) {
		writer.String(it->c_str());
	}
	writer.EndArray();
	writer.EndObject();
}
