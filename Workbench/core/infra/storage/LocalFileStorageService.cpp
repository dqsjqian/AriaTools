//
// LocalFileStorageService — Skeleton-stage storage stub implementation (reads/writes the local filesystem).
// Uses std::filesystem. Can be used directly in the production phase, or enhanced with caching / indexing.
//
#include "infra/storage/IStorageService.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace wb::services {

class LocalFileStorageService final : public IStorageService {
public:
    explicit LocalFileStorageService(std::string dataDir)
        : dataDir_(std::move(dataDir)) {}

    std::string data_dir() const override { return dataDir_; }
    void set_data_dir(std::string dir) override { dataDir_ = std::move(dir); }

    void ensure_dir(const std::string& relative) override {
        std::error_code ec;
        fs::create_directories(fs::path(dataDir_) / relative, ec);
    }

    std::optional<std::string> read_text(const std::string& relPath) override {
        std::ifstream in(fs::path(dataDir_) / relPath, std::ios::binary);
        if (!in) return std::nullopt;
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    bool write_text(const std::string& relPath, const std::string& content) override {
        fs::path full = fs::path(dataDir_) / relPath;
        std::error_code ec;
        fs::create_directories(full.parent_path(), ec);
        std::ofstream out(full, std::ios::binary | std::ios::trunc);
        if (!out) return false;
        out << content;
        return static_cast<bool>(out);
    }

    bool remove(const std::string& relPath) override {
        std::error_code ec;
        return fs::remove(fs::path(dataDir_) / relPath, ec);
    }

    std::vector<std::string> list(const std::string& relDir) override {
        std::vector<std::string> out;
        std::error_code ec;
        fs::path dir = fs::path(dataDir_) / relDir;
        if (!fs::exists(dir, ec)) return out;
        for (auto& e : fs::directory_iterator(dir, ec)) {
            if (e.is_regular_file()) out.push_back(e.path().filename().string());
        }
        return out;
    }

private:
    std::string dataDir_;
};

IStorageService* make_local_file_storage_service(std::string dataDir) {
    return new LocalFileStorageService(std::move(dataDir));
}

}  // namespace wb::services
