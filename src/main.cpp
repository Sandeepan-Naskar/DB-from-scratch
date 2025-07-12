#include <iostream>
#include "document_store.hpp"
#include "colors.hpp"
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void start_cli(DocumentStore& db) {
    std::string line;
    std::cout << "Welcome to NoSQL Document DB! Type 'help' to see commands.\n";

    while (true) {
        // Implement compaction if the size limit is reached
        if (db.get_current_size() >= db.get_size_limit()) { // Example condition for compaction
            db.compact();
            std::cout << COLOR_YELLOW << "-------------------\n";
            std::cout << "Compaction done.\n";
            std::cout << "-------------------\n" << COLOR_RESET;
        }
        
        char* input = readline("docDB> ");
        if (!input) break; // Handles Ctrl+D

        line = input;
        free(input);

        if (line.empty()) continue; // Ignore empty input

        add_history(line.c_str());

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "put") {
            std::string json_str;
            std::getline(iss, json_str);
            json_str = json_str.substr(json_str.find_first_not_of(' '));

            try {
                json doc = json::parse(json_str);  // Validate JSON
                std::string id = db.insert(doc);  // Insert document and generate _id
                std::cout << COLOR_GREEN << "Inserted with _id: " << id << "\n" << COLOR_RESET;
            } catch (const std::exception& e) {
                std::cout << COLOR_RED << "Invalid JSON: " << e.what() << "\n" << COLOR_RESET;
            }
        } else if (cmd == "get") {
            std::string id;
            iss >> id;

            json doc = db.get(id);
            if (!doc.is_null()) {
                std::cout << COLOR_CYAN << doc.dump(4) << "\n" << COLOR_RESET;  // Pretty print
            } else {
                std::cout << COLOR_RED << "(nil)\n" << COLOR_RESET;
            }
        } else if (cmd == "update") {
            std::string id;
            iss >> id;

            std::string json_str;
            std::getline(iss, json_str);
            json_str = json_str.substr(json_str.find_first_not_of(' '));

            try {
                json update_doc = json::parse(json_str);  // Validate JSON
                if (db.update(id, update_doc)) {
                    std::cout << COLOR_GREEN << "Updated document with _id: " << id << "\n" << COLOR_RESET;
                } else {
                    std::cout << COLOR_RED << "Document with _id " << id << " not found.\n" << COLOR_RESET;
                }
            } catch (const std::exception& e) {
                std::cout << COLOR_RED << "Invalid JSON: " << e.what() << "\n" << COLOR_RESET;
            }
        } else if (cmd == "del") {
            std::string id;
            iss >> id;

            if (db.remove(id)) {
                std::cout << COLOR_CYAN << "Deleted\n" << COLOR_RESET;
            } else {
                std::cout << COLOR_RED << "Document not found\n" << COLOR_RESET;
            }
        } else if (cmd == "scan") {
            std::string prefix;
            iss >> prefix;

            auto results = db.find_by_prefix(prefix);
            for (const auto& doc : results) {
                std::cout << COLOR_MAGENTA << doc.dump(4) << "\n" << COLOR_RESET; // Pretty print
                std::cout << "-----------------\n";
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
            std::cout << "  put <JSON document>\n";
            std::cout << "    - Inserts a new document into the database.\n";
            std::cout << "    - A unique _id is automatically generated for the document.\n";
            std::cout << "    - Example: put {\"name\": \"Alice\", \"age\": 30}\n";
            std::cout << "    - [Note]: Ensure the JSON document is valid; otherwise, an error will be shown.\n\n";

            std::cout << "  get <_id>\n";
            std::cout << "    - Retrieves a document by its _id.\n";
            std::cout << "    - Example: get 123e4567-e89b-12d3-a456-426614174000\n";
            std::cout << "    - [Note]: If the _id does not exist, it will return (nil).\n\n";

            std::cout << "  update <_id> <JSON document>\n";
            std::cout << "    - Updates an existing document identified by _id with the provided JSON document.\n";
            std::cout << "    - Example: update 123e4567-e89b-12d3-a456-426614174000 {\"name\": \"Alice\", \"age\": 31}\n";
            std::cout << "    - [Note]: If the _id does not exist, an error message will be displayed.\n";
            std::cout << "    - [Note]: Ensure the JSON document is valid; otherwise, an error will be shown.\n\n";

            std::cout << "  del <_id>\n";
            std::cout << "    - Deletes a document by its _id.\n";
            std::cout << "    - Example: del 123e4567-e89b-12d3-a456-426614174000\n";
            std::cout << "    - [Note]: If the _id does not exist, an error message will be displayed.\n\n";

            std::cout << "  scan <prefix>\n";
            std::cout << "    - Finds all documents whose _id starts with the given prefix.\n";
            std::cout << "    - Example: scan 123e\n";
            std::cout << "    - [Note]: If no documents match the prefix, no results will be displayed.\n\n";
            std::cout << "    - [Note]: If no prefix is entered, all the documents in store are displayed.\n\n";

            std::cout << "  stats\n";
            std::cout << "    - Displays database statistics, such as the number of documents stored and the size of the log and snapshot files.\n\n";

            std::cout << "  compact\n";
            std::cout << "    - Triggers log compaction to clean up the log file and reduce its size.\n";
            std::cout << "    - [Note]: Compaction will occur automatically if the log file exceeds the size limit.\n\n";

            std::cout << "  snapshot\n";
            std::cout << "    - Saves the current state of the database to a snapshot file.\n\n";

            std::cout << "  loadsnapshot\n";
            std::cout << "    - Restores the database state from the most recent snapshot file.\n";
            std::cout << "    - [Note]: If no snapshot file exists, an error message will be displayed.\n\n";

            std::cout << "  exit or quit\n";
            std::cout << "    - Exits the CLI and terminates the program.\n\n";

            std::cout << "  help\n";
            std::cout << "    - Displays this help menu with descriptions for all commands.\n\n";
        } else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
}

int main() {
    DocumentStore db("data/db.log");
    start_cli(db);
    return 0;
}
