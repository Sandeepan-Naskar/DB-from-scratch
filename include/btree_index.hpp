#pragma once
#include <map>
#include <string>
#include <vector>

class BTreeIndex {
public:
    void insert(const std::string& key);
    void remove(const std::string& key);
    std::vector<std::string> scan_prefix(const std::string& prefix) const;

private:
    std::map<std::string, bool> tree_;
};