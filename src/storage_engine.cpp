#include "storage_engine.hpp"
#include <fstream>
#include <iostream>

StorageEngine::StorageEngine(const std::string& db_path) : db_path_(db_path) {
    // Constructor implementation can be added if needed
}

void StorageEngine::append_put(const std::string& key, const std::string& value) {
    std::ofstream out(db_path_, std::ios::app);
    if (out.is_open()) {
        out << key << ":" << value << "\n";
    } else {
        std::cerr << "Failed to open DB file at " << db_path_ << "\n";
    }
}

void StorageEngine::append_del(const std::string& key) {
    std::ofstream out(db_path_, std::ios::app);
    if (out.is_open()) {
        out << key << ":<TOMBSTONE>\n"; // Using a special marker for deletion
    } else {
        std::cerr << "Failed to open DB file at " << db_path_ << "\n";
    }
}

void StorageEngine::compact(std::unordered_map<std::string, std::string>& latest) {
    
    // 1. Read old log
    std::ifstream in(db_path_);
    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find(":");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            if (val == "<TOMBSTONE>")
                latest.erase(key); // skip deleted keys
            else
                latest[key] = val;
        }
    }

    // 2. Write new compacted log
    std::ofstream out("data/db_compacted.log");
    for (const auto& [k, v] : latest)
        out << k << ":" << v << "\n";

    // 3. Replace old log (rename files)
    std::remove("data/db.log");
    std::rename("data/db_compacted.log", "data/db.log");
}

std::vector<std::pair<std::string, std::string>> StorageEngine::load_all() {
    std::ifstream in(db_path_);
    std::vector<std::pair<std::string, std::string>> result;
    std::unordered_map<std::string, std::string> latest;

    if (!in.is_open()) {
        std::cerr << "Failed to open DB file at " << db_path_ << "\n";
        return {};
    }

    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find(":");
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (value == "<TOMBSTONE>") {
            latest.erase(key);
        } else {
            latest[key] = value;
        }
    }

    for (const auto& [k, v] : latest)
        result.emplace_back(k, v);

    return result;
}
