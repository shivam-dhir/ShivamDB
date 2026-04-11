#include <iostream>
#include <sstream>

#include "shivamdb/shivamdb.h"

using namespace shivamdb;

int main() {

    ShivamDB db;

    std::string line;

    std::cout << "ShivamDB Interactive CLI\n";

    while (true) {

        std::cout << "shivamdb> ";

        std::getline(std::cin, line);

        std::stringstream ss(line);

        std::string command;

        ss >> command;

        if (command == "SET" || command == "set") {

            std::string key, value;
            ss >> key >> value;

            db.set(key, value);

            std::cout << "OK\n";
        }

        else if (command == "GET" || command == "get") {

            std::string key;
            ss >> key;

            auto result = db.get(key);

            if (result)
                std::cout << *result << "\n";
            else
                std::cout << "NULL\n";
        }

        else if (command == "DEL" || command == "del") {

            std::string key;
            ss >> key;

            if (db.del(key))
                std::cout << "DELETED\n";
            else
                std::cout << "NOT FOUND\n";
        }

        else if (command == "SIZE" || command == "size") {

            std::cout << db.size() << "\n";
        }

        else if (command == "CLEAR" || command == "clear") {

            db.clear();

            std::cout << "CLEARED\n";
        }

        else if (command == "EXIT" || command == "exit") {

            break;
        }

        else {

            std::cout << "Unknown command\n";
        }
    }

}