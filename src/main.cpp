#include <iostream>
#include "kv_store.hpp"
#include <sstream>

void start_cli(KVStore& db) {
    std::string line;
    std::cout << "Welcome to KVDB! Type 'help' to see commands.\n";

    while (true) {
        std::cout << "kvdb> ";
        if (!std::getline(std::cin, line)) break;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        // Implement compaction if the size limit is reached
        if (db.get_current_size() >= db.get_size_limit()) { // Example condition for compaction
            db.compact();
            std::cout << "\n-------------------\n";
            std::cout << "Compaction done.\n";
            std::cout << "-------------------\n";
        }

        
        // TODO: Handle multiple word inputs for key and value
        // For simplicity, we will assume single word inputs for now
        if (cmd == "put") {
            std::string key, value;
            iss >> key;
            std::getline(iss, value);
            value = value.substr(value.find_first_not_of(' ')); // Trim leading spaces
            db.put(key, value);
            std::cout << "OK\n";
        } else if (cmd == "get") {
            std::string key;
            iss >> key;
            std::string value;
            if (db.get(key) != "")
                std::cout << value << "\n";
            else
                std::cout << "(nil)\n";
        } else if (cmd == "del") {
            std::string key;
            iss >> key;
            db.del(key);
            std::cout << "Deleted\n";
        } else if (cmd == "scan") {
            // std::string prefix;
            // iss >> prefix;
            // auto results = db.scan_prefix(prefix);  // you'll need to implement this
            // for (const auto& kv : results)
            //     std::cout << kv.first << ": " << kv.second << "\n";
        } else if (cmd == "compact") {
            db.compact();
            std::cout << "Compaction done.\n";
        } else if (cmd == "stats") {
            // db.print_stats();  // implement this to print internal stats
        } else if (cmd == "exit" || cmd == "quit") {
            std::cout << "Bye!\n";
            break;
        } else if (cmd == "help") {
            std::cout << "Commands:\n";
            std::cout << "  put <key> <value>\n";
            std::cout << "  get <key>\n";
            std::cout << "  del <key>\n";
            std::cout << "  scan <prefix>\n";
            std::cout << "  stats\n";
            std::cout << "  compact\n";
            std::cout << "  exit\n";
        } else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
}

int main() {
    KVStore db("data/db.log");
    start_cli(db);
    return 0;
}
