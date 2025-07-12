#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

class StorageEngine {
public:
    StorageEngine(const std::string& db_path);
    
    void append_put(const std::string& key, const std::string& value);
    void append_del(const std::string& key);
    
    std::vector<std::pair<std::string, std::string>> load_all();
    void compact(std::unordered_map<std::string, std::string>& latest);

    // Getters for size limit and current size
    int size_limit() const { return size_limit_; }
    int current_size() const { return current_size_; }

private:
    std::string db_path_;
    int size_limit_; // Size limit for compaction
    int current_size_; // Current size of the log file
};
