#include <iostream>
#include "kv_store.hpp"
#include "colors.hpp"
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>

void start_cli(KVStore& db) {
    std::string line;
    std::cout << "Welcome to KVDB! Type 'help' to see commands.\n";

    while (true) {
        char* input = readline("kvdb> ");
        if (!input) break; // Handles Ctrl+D

        line = input;
        free(input);

        if (line.empty()) continue; // Ignore empty input

        add_history(line.c_str());

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        // Implement compaction if the size limit is reached
        if (db.get_current_size() >= db.get_size_limit()) { // Example condition for compaction
            db.compact();
            std::cout << COLOR_YELLOW << "-------------------\n";
            std::cout << "Compaction done.\n";
            std::cout << "-------------------\n" << COLOR_RESET;
        }

        
        // TODO: Handle multiple word inputs for key and value
        // For simplicity, we will assume single word inputs for now
        if (cmd == "put") {
            std::string key, value;
            iss >> key;
            std::getline(iss, value);
            value = value.substr(value.find_first_not_of(' ')); // Trim leading spaces
            db.put(key, value);
            std::cout << COLOR_GREEN << "OK\n" << COLOR_RESET;
        } else if (cmd == "get") {
            std::string key;
            iss >> key;
            std::string value;
            if (db.get(key) != "")
                std::cout << db.get(key) << "\n";
            else
                std::cout << COLOR_RED << "(nil)\n" << COLOR_RESET;
        } else if (cmd == "del") {
            std::string key;
            iss >> key;
            db.del(key);
            std::cout << COLOR_CYAN << "Deleted\n" << COLOR_RESET;
        } else if (cmd == "scan") {
            std::string prefix;
            iss >> prefix;
            auto results = db.scan_prefix(prefix);
            for (const auto& [key, value] : db.scan_prefix(prefix)) {
                std::cout << COLOR_MAGENTA << key.substr(0, prefix.size()) << COLOR_RESET << key.substr(prefix.size()) << ": " << value << "\n";
            }
        } else if (cmd == "compact") {
            db.compact();
            std::cout << COLOR_YELLOW << "Compaction done.\n" << COLOR_RESET;
        } else if (cmd == "stats") {
            db.print_stats();
        } else if (cmd == "snapshot") {
            if (db.save_snapshot()) {
                std::cout << COLOR_GREEN << "Snapshot saved.\n" << COLOR_RESET;
            } else {
                std::cout << COLOR_RED << "Failed to save snapshot.\n" << COLOR_RESET;
            }
        } else if (cmd == "loadsnapshot") {
            db.restore_from_snapshot();
            std::cout << COLOR_CYAN << "Snapshot loaded.\n" << COLOR_RESET;
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
            std::cout << "  snapshot\n";
            std::cout << "  loadsnapshot\n";
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
