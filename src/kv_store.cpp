#include "kv_store.hpp"
#include <fstream>
#include <iostream>
#include <string>

KVStore::KVStore(const std::string& db_path) {
    engine_ = std::make_unique<StorageEngine>(db_path);

    auto entries = engine_->load_all();
    for (const auto& [k, v] : entries) {
        store_[k] = v;
    }
}

void KVStore::put(const std::string& key, const std::string& value) {
    store_[key] = value; // Update in-memory state
    engine_->append_put(key, value); // Append to log
}

std::string KVStore::get(const std::string& key) {
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second; // Return value if found
    }
    return ""; // Return empty string if key not found
}

void KVStore::del(const std::string& key) {
    store_.erase(key); // Remove from in-memory state
    engine_->append_del(key); // Append tombstone to log
}

void KVStore::compact() {
    // Compact the log file
    engine_->compact(store_);
}
