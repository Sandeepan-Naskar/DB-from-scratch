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
    
private:
    std::string db_path_;
};
