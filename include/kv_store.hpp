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

    std::vector<std::pair<std::string, std::string>> scan_prefix(const std::string& prefix) const;
    void print_stats() const;

    bool save_snapshot();
    bool restore_from_snapshot();

    int get_size_limit() const { return engine_->size_limit(); } // Get size limit for compaction
    int get_current_size() const { return engine_->current_size(); } // Get current size of the log file

private:
    std::unordered_map<std::string, std::string> store_;
    std::unique_ptr<StorageEngine> engine_;
};
