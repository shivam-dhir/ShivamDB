#include "shivamdb/shivamdb.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

static void assert_true(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

static void assert_false(bool condition, const std::string& message) {
    assert_true(!condition, message);
}

static void assert_equals(std::size_t actual, std::size_t expected, const std::string& message) {
    if (actual != expected) {
        throw std::runtime_error(
            "Assertion failed: " + message +
            " | expected=" + std::to_string(expected) +
            " actual=" + std::to_string(actual));
    }
}

static void assert_equals(const std::optional<std::string>& actual,
    const std::optional<std::string>& expected,
    const std::string& message) {
    if (actual != expected) {
        const std::string actual_str = actual.has_value() ? *actual : "nullopt";
        const std::string expected_str = expected.has_value() ? *expected : "nullopt";

        throw std::runtime_error(
            "Assertion failed: " + message +
            " | expected=" + expected_str +
            " actual=" + actual_str);
    }
}

static void remove_if_exists(const fs::path& path) {
    std::error_code ec;
    fs::remove(path, ec);
}

static void remove_dir_if_exists(const fs::path& path) {
    std::error_code ec;
    fs::remove_all(path, ec);
}

static void test_basic_set_get() {
    const fs::path dir = "test_data/basic_set_get";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());
    db.set("name", "Shivam");

    assert_equals(db.get("name"), std::make_optional<std::string>("Shivam"),
        "GET should return value after SET");
    assert_true(db.exists("name"), "EXISTS should be true for inserted key");
    assert_equals(db.size(), 1, "SIZE should be 1 after one insert");
}

static void test_missing_key() {
    const fs::path dir = "test_data/missing_key";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());

    assert_equals(db.get("missing"), std::nullopt, "GET on missing key should return nullopt");
    assert_false(db.exists("missing"), "EXISTS on missing key should be false");
    assert_equals(db.size(), 0, "SIZE should be 0 for empty DB");
}

static void test_overwrite_value() {
    const fs::path dir = "test_data/overwrite_value";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());
    db.set("lang", "C++");
    db.set("lang", "Rust");

    assert_equals(db.get("lang"), std::make_optional<std::string>("Rust"),
        "Latest SET should overwrite previous value");
    assert_equals(db.size(), 1, "Overwriting same key should not increase size");
}

static void test_delete_key() {
    const fs::path dir = "test_data/delete_key";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());
    db.set("x", "10");

    assert_true(db.del("x"), "DEL should return true for existing key");
    assert_equals(db.get("x"), std::nullopt, "Deleted key should not be found");
    assert_false(db.exists("x"), "Deleted key should not exist");
    assert_equals(db.size(), 0, "SIZE should be 0 after delete");

    assert_false(db.del("x"), "DEL should return false for missing key");
}

static void test_clear() {
    const fs::path dir = "test_data/clear";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());
    db.set("a", "1");
    db.set("b", "2");
    db.set("c", "3");

    db.clear();

    assert_equals(db.size(), 0, "CLEAR should remove all keys");
    assert_equals(db.get("a"), std::nullopt, "CLEAR should remove key a");
    assert_equals(db.get("b"), std::nullopt, "CLEAR should remove key b");
    assert_equals(db.get("c"), std::nullopt, "CLEAR should remove key c");
}

static void test_wal_recovery_after_restart() {
    const fs::path dir = "test_data/wal_recovery";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    {
        shivamdb::ShivamDB db(wal_path.string());
        db.set("name", "Shivam");
        db.set("role", "Engineer");
        db.set("city", "Sunnyvale");
        db.del("city");
    }

    {
        shivamdb::ShivamDB recovered_db(wal_path.string());

        assert_equals(recovered_db.get("name"), std::make_optional<std::string>("Shivam"),
            "Recovered DB should contain name");
        assert_equals(recovered_db.get("role"), std::make_optional<std::string>("Engineer"),
            "Recovered DB should contain role");
        assert_equals(recovered_db.get("city"), std::nullopt,
            "Recovered DB should preserve deletes");
        assert_equals(recovered_db.size(), 2, "Recovered DB should have correct size");
    }
}

static void test_value_with_spaces() {
    const fs::path dir = "test_data/value_with_spaces";
    const fs::path wal_path = dir / "wal.log";

    remove_dir_if_exists(dir);

    shivamdb::ShivamDB db(wal_path.string());
    db.set("intro", "I love storage engines");

    assert_equals(db.get("intro"), std::make_optional<std::string>("I love storage engines"),
        "Value with spaces should be stored correctly");
}

int main() {
    try {
        test_basic_set_get();
        test_missing_key();
        test_overwrite_value();
        test_delete_key();
        test_clear();
        test_wal_recovery_after_restart();
        test_value_with_spaces();

        std::cout << "All ShivamDB tests passed.\n";
        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}