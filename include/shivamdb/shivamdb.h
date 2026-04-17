#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include "shivamdb/wal.h"

namespace shivamdb {

    class ShivamDB {
        public:

			// Creates a new ShivamDB instance with an optional WAL file path. If no path is provided, it defaults to "data/wal.log". The constructor initializes the WAL instance and replays any existing WAL records to restore the database state.
            explicit ShivamDB(const std::string& wal_path = "data/wal.log");

            // Sets the value for the given key. If the key already exists, it will overwrite the existing value.
            void set(const std::string& key, const std::string& value);

            // Returns the value associated with the given key, or std::nullopt if the key does not exist.
            std::optional<std::string> get(const std::string& key) const;

            // Deletes the key-value pair associated with the given key. Returns true if the key was found and deleted, false otherwise.
            bool del(const std::string& key);

            // Checks if the given key exists in the database.
            bool exists(const std::string& key) const;

            // Returns the number of key-value pairs currently stored in the database.
            size_t size() const;

            // Clears all key-value pairs from the database.
            void clear();

        private:
            // Internal storage for key-value pairs. Using an unordered_map for efficient lookups.
            std::unordered_map<std::string, std::string> store_;
			std::unique_ptr<WAL> wal_;
    };

}