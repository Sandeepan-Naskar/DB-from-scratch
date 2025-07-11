#pragma once
#include "storage_engine.hpp"
#include <memory>

class KVStore {
public:
    KVStore(const std::string& path);
    void put(const std::string& key, const std::string& value); // For create and update
    std::string get(const std::string& key); // For read
    void del(const std::string& key); // For delete

    void compact(); // Compact the log file to remove tombstones

private:
    std::unordered_map<std::string, std::string> store_;
    std::unique_ptr<StorageEngine> engine_;
};
