#include "btree_index.hpp"

void BTreeIndex::insert(const std::string& key) {
    tree_[key] = true;
}

void BTreeIndex::remove(const std::string& key) {
    tree_.erase(key);
}

std::vector<std::string> BTreeIndex::scan_prefix(const std::string& prefix) const {
    std::vector<std::string> results;
    for (auto it = tree_.lower_bound(prefix); it != tree_.end(); ++it) {
        if (it->first.compare(0, prefix.size(), prefix) == 0)
            results.push_back(it->first);
        else
            break;
    }
    return results;
}
