#pragma once
//
// IStorageService — 读写数据目录（一条记录一个文件，便于 git 合并、免冲突）。
// 骨架期：桩实现读写本地文件系统 dataDir 下的文件。
//
#include <string>
#include <vector>
#include <optional>

namespace wb::services {

class IStorageService {
public:
    virtual ~IStorageService() = default;

    /// 数据根目录（= git 仓库根）。
    [[nodiscard]] virtual std::string data_dir() const = 0;
    virtual void set_data_dir(std::string dir) = 0;

    /// 确保子目录存在（notes/ calendar/ vault/ config/ 等）。
    virtual void ensure_dir(const std::string& relative) = 0;

    /// 相对 dataDir 的路径读写文本。
    [[nodiscard]] virtual std::optional<std::string> read_text(const std::string& relPath) = 0;
    virtual bool write_text(const std::string& relPath, const std::string& content) = 0;
    virtual bool remove(const std::string& relPath) = 0;

    /// 列出某子目录下的文件名（相对）。
    [[nodiscard]] virtual std::vector<std::string> list(const std::string& relDir) = 0;
};

}  // namespace wb::services
