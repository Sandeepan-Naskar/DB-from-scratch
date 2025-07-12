#pragma once
#include "storage_engine.hpp"
#include "btree_index.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

class DocumentStore {
public:
    DocumentStore(const std::string& path);

    // Document-level operations
    std::string insert(json doc);                          // Generates _id, inserts document
    json get(const std::string& id);                       // Get document by _id
    bool update(const std::string& id, const json& update);// Update full document
    bool remove(const std::string& id);                    // Delete by _id

    // Utility
    std::vector<json> find_by_prefix(const std::string& prefix) const;
    void print_stats() const;

    bool save_snapshot();
    bool restore_from_snapshot();

    int get_size_limit() const { return engine_->size_limit(); }
    int get_current_size() const { return engine_->current_size(); }

    void compact();

private:
    std::string generate_uuid() const;

    std::unordered_map<std::string, json> store_;  // JSON objects mapped by _id
    std::unique_ptr<StorageEngine> engine_;
    BTreeIndex index_;
};
