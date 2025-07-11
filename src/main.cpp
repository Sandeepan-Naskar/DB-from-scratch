#include <iostream>
#include "kv_store.hpp"

int main() {
    KVStore db("data/db.log");
    int choice;
    std::string key, value;

    while (true) {
        std::cout << "\n1. Put\n2. Get\n3. Delete\n4. Exit\nChoice: ";
        std::cin >> choice;

        if (std::cin.fail()) { // Check if input is invalid
            std::cin.clear(); // Clear the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number.\n";
            continue; // Restart the loop
        }
        
        switch (choice) {
            case 1:
                std::cout << "Key: "; std::cin >> key;
                std::cout << "Value: "; std::cin >> value;
                db.put(key, value);
                break;
            case 2:
                std::cout << "Key: "; std::cin >> key;
                std::cout << "Value: " << db.get(key) << "\n";
                break;
            case 3:
                std::cout << "Key: "; std::cin >> key;
                db.del(key);
                break;
            case 4: return 0;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
}
