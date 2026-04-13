#include "shivamdb/wal.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace shivamdb {
    namespace {

        void write_uint8(std::ofstream& out, std::uint8_t value) {
            out.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }

        void write_uint32(std::ofstream& out, std::uint32_t value) {
            out.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }

        bool read_uint8(std::ifstream& in, std::uint8_t& value) {
            return static_cast<bool>(in.read(reinterpret_cast<char*>(&value), sizeof(value)));
        }

        bool read_uint32(std::ifstream& in, std::uint32_t& value) {
            return static_cast<bool>(in.read(reinterpret_cast<char*>(&value), sizeof(value)));
        }

    } // namespace

    WAL::WAL(const std::filesystem::path& path) : path_(path) {
        const auto parent = path_.parent_path();

        if (!parent.empty()) {
            std::filesystem::create_directories(parent);
        }

        out_.open(path_, std::ios::binary | std::ios::app);

        if (!out_) {
            throw std::runtime_error("Failed to open WAL file for append: " + path_.string());
        }
    }

    WAL::~WAL() {
        if (out_.is_open()) {
            out_.flush();
            out_.close();
        }
    }

    void WAL::append_set(const std::string& key, const std::string& value) {
        append_record(WalOp::Set, key, value);
    }

    void WAL::append_delete(const std::string& key) {
        append_record(WalOp::Delete, key, "");
    }

    void WAL::append_record(WalOp op, const std::string& key, const std::string& value) {
        const auto key_size = static_cast<std::uint32_t>(key.size());
        const auto value_size = static_cast<std::uint32_t>(value.size());
        const auto op_byte = static_cast<std::uint8_t>(op);

        write_uint8(out_, op_byte);
        write_uint32(out_, key_size);
        write_uint32(out_, value_size);
        out_.write(key.data(), static_cast<std::streamsize>(key.size()));
        out_.write(value.data(), static_cast<std::streamsize>(value.size()));

        if (!out_) {
            throw std::runtime_error("Failed to write WAL record.");
        }

        out_.flush();
    }

    void WAL::replay(const std::function<void(WalOp, const std::string&, const std::string&)>& apply_callback) {
        if (out_.is_open()) {
            out_.flush();
        }

        std::ifstream in(path_, std::ios::binary);

        if (!in) {
            return;
        }

        while (true) {
            std::uint8_t op_raw = 0;
            std::uint32_t key_size = 0;
            std::uint32_t value_size = 0;

            if (!read_uint8(in, op_raw)) {
                break;
            }

            if (!read_uint32(in, key_size) || !read_uint32(in, value_size)) {
                break;
            }

            std::string key(key_size, '\0');
            std::string value(value_size, '\0');

            if (!in.read(key.data(), static_cast<std::streamsize>(key_size))) {
                break;
            }

            if (!in.read(value.data(), static_cast<std::streamsize>(value_size))) {
                break;
            }

            const auto op = static_cast<WalOp>(op_raw);
            apply_callback(op, key, value);
        }
    }

} // namespace shivamdb