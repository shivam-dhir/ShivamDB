#include "shivamdb/shivamdb.h"

namespace shivamdb {

    ShivamDB::ShivamDB(const std::string& wal_path)
        : wal_(std::make_unique<WAL>(wal_path)) {
		// Replays the WAL to restore the database state. The replay function reads all records from the WAL file and applies them to the in-memory store using a callback function. This ensures that any changes made to the database before a crash or shutdown are preserved and can be recovered when the database is restarted.
		// replay function can take a function object (like a lambda) that will be called for each record in the WAL. The callback function takes three parameters: the operation type (Set or Delete), the key, and the value. Based on the operation type, the callback updates the in-memory store accordingly.
        wal_->replay([this](WalOp op, const std::string& key, const std::string& value) {
            if (op == WalOp::Set) {
                store_[key] = value;
            }
            else if (op == WalOp::Delete) {
                store_.erase(key);
            }
            });
    }

    void ShivamDB::set(const std::string& key, const std::string& value) {
        wal_->append_set(key, value);
        store_[key] = value;
    }

    std::optional<std::string> ShivamDB::get(const std::string& key) const {
        const auto it = store_.find(key);

        if (it == store_.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    bool ShivamDB::del(const std::string& key) {
        const auto it = store_.find(key);
        if (it == store_.end()) {
            return false;
        }

        wal_->append_delete(key);
        store_.erase(it);
        return true;
    }

    bool ShivamDB::exists(const std::string& key) const {
        return store_.find(key) != store_.end();
    }

    std::size_t ShivamDB::size() const {
        return store_.size();
    }

    void ShivamDB::clear() {
        for (const auto& [key, _] : store_) {
            wal_->append_delete(key);
        }
        store_.clear();
    }

} // namespace shivamdb