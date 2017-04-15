#include "cache.h"


Cache::Cache() {};

std::string Cache::get(std::string key) {
    return this->storage[key];
}

void Cache::set(std::string key, std::string value) {
    this->storage[key] = value;
};
