#include "storage_engine.hpp"
#include <fstream>
#include <iostream>

StorageEngine::StorageEngine(const std::string& db_path) : db_path_(db_path) {
    size_limit_ = 10; // Example size limit for compaction
    current_size_ = 0; // Initialize current size
    snapshot_path_ = db_path + ".snapshot"; // Example snapshot path
}

void StorageEngine::append_put(const std::string& key, const std::string& value) {
    std::ofstream out(db_path_, std::ios::app);
    if (out.is_open()) {
        out << "PUT " << key << " " << value << "\n"; // Write in the desired format
    } else {
        std::cerr << "Failed to open DB file at " << db_path_ << "\n";
    }
    current_size_++; // Increment current size for each put operation
}

void StorageEngine::append_del(const std::string& key) {
    std::ofstream out(db_path_, std::ios::app);
    if (out.is_open()) {
        out << "DEL " << key << "\n"; // Write in the desired format
    } else {
        std::cerr << "Failed to open DB file at " << db_path_ << "\n";
    }
    current_size_++; // Increment current size for each delete operation
}

void StorageEngine::compact(std::unordered_map<std::string, std::string>& latest) {
    
    // 1. Read old log
    std::ifstream in(db_path_);
    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find(" ");
        if (pos != std::string::npos) {
            std::string operation = line.substr(0, pos);
            std::string rest = line.substr(pos + 1);

            if (operation == "PUT") {
                auto key_pos = rest.find(" ");
                if (key_pos != std::string::npos) {
                    std::string key = rest.substr(0, key_pos);
                    std::string value = rest.substr(key_pos + 1);
                    latest[key] = value; // Update the latest value
                }
            } else if (operation == "DEL") {
                latest.erase(rest); // Remove the key
            }
        }
    }

    // 2. Write new compacted log
    std::ofstream out("data/db_compacted.log");
    for (const auto& [k, v] : latest) {
        out << "PUT " << k << " " << v << "\n";
    }
    current_size_ = latest.size(); // Update current size after compaction

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
        auto pos = line.find(" ");
        if (pos != std::string::npos) {
            std::string operation = line.substr(0, pos);
            std::string rest = line.substr(pos + 1);

            if (operation == "PUT") {
                auto key_pos = rest.find(" ");
                if (key_pos != std::string::npos) {
                    std::string key = rest.substr(0, key_pos);
                    std::string value = rest.substr(key_pos + 1);
                    latest[key] = value; // Update the latest value
                }
            } else if (operation == "DEL") {
                latest.erase(rest); // Remove the key
            }
        }
        current_size_++; // Increment current size for each entry loaded
    }

    for (const auto& [k, v] : latest) {
        result.emplace_back(k, v);
    }

    return result;
}

bool StorageEngine::write_snapshot(const std::unordered_map<std::string, std::string>& store) {
    std::ofstream out(snapshot_path_, std::ios::trunc);
    if (!out.is_open()) return false;

    for (const auto& [key, value] : store) {
        out << key << '\t' << value << '\n';
    }
    current_size_ = store.size(); // Update current size after writing snapshot

    return true;
}

std::unordered_map<std::string, std::string> StorageEngine::load_snapshot() {
    std::unordered_map<std::string, std::string> store;
    std::ifstream in(snapshot_path_);
    if (!in.is_open()) return store;

    std::string line;
    while (std::getline(in, line)) {
        auto tab_pos = line.find('\t');
        if (tab_pos != std::string::npos) {
            std::string key = line.substr(0, tab_pos);
            std::string value = line.substr(tab_pos + 1);
            store[key] = value;
        }
    }

    return store;
}
