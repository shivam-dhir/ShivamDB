// ShivamDB.cpp : Defines the entry point for the application.

#include "shivamdb/shivamdB.h"

namespace shivamdb {
	void ShivamDB::set(const std::string& key, const std::string& value) {
		store_[key] = value;
	}
	std::optional<std::string> ShivamDB::get(const std::string& key) const {
		auto it = store_.find(key);
		if (it != store_.end()) {
			return it->second;
		}
		return std::nullopt;
	}
	bool ShivamDB::del(const std::string& key) {
		return store_.erase(key) > 0;
	}
	bool ShivamDB::exists(const std::string& key) const {
		return store_.find(key) != store_.end();
	}
	size_t ShivamDB::size() const {
		return store_.size();
	}
	void ShivamDB::clear() {
		store_.clear();
	}
}