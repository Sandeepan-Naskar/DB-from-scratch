#include "document_store.hpp"
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

DocumentStore::DocumentStore(const std::string& path) {
    engine_ = std::make_unique<StorageEngine>(path);

    auto entries = engine_->load_all();
    for (const auto& [id, doc] : entries) {
        store_[id] = json::parse(doc); // Deserialize JSON
        index_.insert(id);
    }
}

std::string DocumentStore::insert(json doc) {
    std::string id = generate_uuid(); // Generate a unique ID
    store_[id] = doc;                // Store the document
    engine_->append_put(id, doc.dump()); // Serialize and append to log
    index_.insert(id);               // Add to index
    return id;
}

json DocumentStore::get(const std::string& id) {
    auto it = store_.find(id);
    if (it != store_.end()) {
        return it->second; // Return the document if found
    }
    return nullptr; // Return null if not found
}

bool DocumentStore::update(const std::string& id, const json& update) {
    auto it = store_.find(id);
    if (it != store_.end()) {
        store_[id] = update;            // Update the document
        engine_->append_put(id, update.dump()); // Serialize and append to log
        return true;
    }
    return false; // Document not found
}

bool DocumentStore::remove(const std::string& id) {
    auto it = store_.find(id);
    if (it != store_.end()) {
        store_.erase(id);               // Remove from in-memory store
        engine_->append_del(id);        // Append tombstone to log
        index_.remove(id);              // Remove from index
        return true;
    }
    return false; // Document not found
}

std::vector<json> DocumentStore::find_by_prefix(const std::string& prefix) const {
    std::vector<json> results;
    auto keys = index_.scan_prefix(prefix);
    for (const auto& key : keys) {
        if (store_.count(key)) {
            results.push_back(store_.at(key));
        }
    }
    return results;
}

void DocumentStore::print_stats() const {
    std::cout << "Document Store Stats:\n";
    std::cout << "------------------------\n";
    std::cout << "Documents stored: " << store_.size() << "\n";
    std::cout << "Log file size   : ";

    std::ifstream file(engine_->db_path(), std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::cout << file.tellg() << " bytes\n";
    } else {
        std::cout << "Unknown (couldn't open file)\n";
    }

    std::cout << "Snapshot file   : ";
    std::ifstream snap(engine_->snapshot_path(), std::ios::binary | std::ios::ate);
    if (snap.is_open()) {
        std::cout << snap.tellg() << " bytes\n";
    } else {
        std::cout << "Not available\n";
    }

    std::cout << "------------------------\n";
}

bool DocumentStore::save_snapshot() {
    std::unordered_map<std::string, std::string> serialized_store;
    for (const auto& [id, doc] : store_) {
        serialized_store[id] = doc.dump(); // Serialize JSON
    }
    return engine_->write_snapshot(serialized_store);
}

bool DocumentStore::restore_from_snapshot() {
    auto snapshot = engine_->load_snapshot();
    store_.clear();
    for (const auto& [id, doc] : snapshot) {
        store_[id] = json::parse(doc); // Deserialize JSON
        index_.insert(id);
    }
    return true;
}

void DocumentStore::compact() {
    std::unordered_map<std::string, std::string> serialized_store;
    for (const auto& [id, doc] : store_) {
        serialized_store[id] = doc.dump(); // Serialize JSON
    }
    engine_->compact(serialized_store);
}

std::string DocumentStore::generate_uuid() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 8; ++i) ss << std::hex << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; ++i) ss << std::hex << dis(gen);
    ss << "-4"; // UUID version 4
    for (int i = 0; i < 3; ++i) ss << std::hex << dis(gen);
    ss << "-";
    ss << std::hex << ((dis(gen) & 0x3) | 0x8); // UUID variant
    for (int i = 0; i < 3; ++i) ss << std::hex << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; ++i) ss << std::hex << dis(gen);

    return ss.str();
}