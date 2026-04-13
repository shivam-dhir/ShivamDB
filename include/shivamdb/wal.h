#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

namespace shivamdb {

    enum class WalOp : std::uint8_t {
        Set = 1,
        Delete = 2
    };

    class WAL {
    public:
		// Creates a new WAL instance for the given file path. If the file does not exist, it will be created.
		// explicitly deletes copy and move constructors and assignment operators to prevent copying or moving of the WAL instance, ensuring that only one instance manages the WAL file at a time.
		// explicit forces the caller to provide a file path when creating a WAL instance, preventing accidental creation of a WAL without a specified file path.
        explicit WAL(const std::filesystem::path& path);
        ~WAL();

		// Delete copy and move constructors and assignment operators to prevent copying or moving of the WAL instance
        WAL(const WAL&) = delete;
        WAL& operator=(const WAL&) = delete;

		// Delete move constructor and move assignment operator to prevent moving of the WAL instance
        WAL(WAL&&) = delete;
        WAL& operator=(WAL&&) = delete;

		// Appends a SET operation to the WAL with the specified key and value.
        void append_set(const std::string& key, const std::string& value);
		// Appends a DELETE operation to the WAL for the specified key.
        void append_delete(const std::string& key);

		// Replays the WAL by reading all records from the file and invoking the provided callback for each record.
        void replay(const std::function<void(WalOp, const std::string&, const std::string&)>& apply_callback);

    private:
		// Appends a record to the WAL with the specified operation, key, and value. This is a helper function used by append_set and append_delete.
        void append_record(WalOp op, const std::string& key, const std::string& value);

		// The file path of the WAL file.
        std::filesystem::path path_;
		// The output file stream used to write records to the WAL file. It is opened in binary mode and append mode.
        std::ofstream out_;
    };

} // namespace shivamdb