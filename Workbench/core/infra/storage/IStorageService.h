#pragma once
//
// IStorageService — Read/write the data directory (one record per file, for easy git merging and conflict avoidance).
// Skeleton stage: stub implementation reads/writes files on the local filesystem under dataDir.
//
#include <string>
#include <vector>
#include <optional>

namespace wb::services {

class IStorageService {
public:
    virtual ~IStorageService() = default;

    /// Data root directory (= git repository root).
    [[nodiscard]] virtual std::string data_dir() const = 0;
    virtual void set_data_dir(std::string dir) = 0;

    /// Ensure a subdirectory exists (notes/ calendar/ vault/ config/ etc.).
    virtual void ensure_dir(const std::string& relative) = 0;

    /// Read/write text relative to dataDir.
    [[nodiscard]] virtual std::optional<std::string> read_text(const std::string& relPath) = 0;
    virtual bool write_text(const std::string& relPath, const std::string& content) = 0;
    virtual bool remove(const std::string& relPath) = 0;

    /// List filenames in a subdirectory (relative paths).
    [[nodiscard]] virtual std::vector<std::string> list(const std::string& relDir) = 0;
};

}  // namespace wb::services
