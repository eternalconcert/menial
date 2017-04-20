#include "cache.h"

const std::string ACTION_GET = "GET";
const std::string ACTION_SET = "SET";
const std::string DELIMITER = ":";
const int ACTION_KEYWORD_LEN = ACTION_GET.length();

std::string Cache::handleMessage(std::string message) {
    std::string action = message.substr(0, ACTION_KEYWORD_LEN);

    int payloadEnd = message.size() - ACTION_KEYWORD_LEN + 1;
    std::string payload = message.substr(ACTION_KEYWORD_LEN + 1, payloadEnd);

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
