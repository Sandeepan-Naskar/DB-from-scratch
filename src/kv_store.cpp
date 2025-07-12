#include "kv_store.hpp"
#include <fstream>
#include <iostream>
#include <string>

KVStore::KVStore(const std::string& db_path) {
    engine_ = std::make_unique<StorageEngine>(db_path);

    auto entries = engine_->load_all();
    for (const auto& [k, v] : entries) {
        store_[k] = v;
        index_.insert(k);
    }
}

void KVStore::put(const std::string& key, const std::string& value) {
    store_[key] = value; // Update in-memory state
    engine_->append_put(key, value); // Append to log
    index_.insert(key);
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
    index_.remove(key);
}

void KVStore::compact() {
    // Compact the log file
    engine_->compact(store_);
}

std::vector<std::pair<std::string, std::string>> KVStore::scan_prefix(const std::string& prefix) const {
    std::vector<std::pair<std::string, std::string>> results;
    auto keys = index_.scan_prefix(prefix);
    for (const auto& key : keys) {
        if (store_.count(key)) {
            results.emplace_back(key, store_.at(key));
        }
    }

    return results;
}

void KVStore::print_stats() const {
    std::cout << "Database Stats:\n";
    std::cout << "------------------------\n";
    std::cout << "Keys stored   : " << store_.size() << "\n";
    std::cout << "Log file size : ";

    std::ifstream file(engine_->db_path(), std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::cout << file.tellg() << " bytes\n";
    } else {
        std::cout << "Unknown (couldn't open file)\n";
    }

    std::cout << "Snapshot file : ";
    std::ifstream snap(engine_->snapshot_path(), std::ios::binary | std::ios::ate);
    if (snap.is_open()) {
        std::cout << snap.tellg() << " bytes\n";
    } else {
        std::cout << "Not available\n";
    }

    std::cout << "------------------------\n";
}

bool KVStore::save_snapshot() {
    return engine_->write_snapshot(store_);
}

bool KVStore::restore_from_snapshot() {
    store_ = engine_->load_snapshot();
    for (const auto& [key, _] : store_) {
        index_.insert(key);
    }
    return true;
}