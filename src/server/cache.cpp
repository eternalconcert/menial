#include "cache.h"

std::string ACTION_GET = "GET";
std::string ACTION_SET = "SET";
std::string DELIMITER = ":";

std::string Cache::handleMessage(std::string message) {
    std::string action = message.substr(0, 3);
    std::string payload = message.substr(4, message.size() - 5);

    std::string reply;

    if (action == ACTION_GET) {
        reply = this->get(payload);

    } else if (action == ACTION_SET) {
        std::string key = payload.substr(0, payload.find(DELIMITER));
        std::string value = payload.substr(payload.find(DELIMITER) + 1);
        this->set(key, value);
        reply = "OK";
    }

    return reply;
}

Cache::Cache() {};

std::string Cache::get(std::string key) {
    return this->storage[key];
}

void Cache::set(std::string key, std::string value) {
    this->storage[key] = value;
};
