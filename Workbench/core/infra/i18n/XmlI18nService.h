#pragma once
//
// XmlI18nService — II18nService 的 XML 实现（core 层，无 Qt）。
//
// 自适应设计（无需为新模块/新语种改动本类）：
//   * 模块自适应：i18n/ 下每个子目录名即一个「模块」；tr() 首次访问某模块时
//     懒加载该模块的 xml，故新增模块只要新建目录 + xml，底层零改动。
//   * 语种自适应：available_languages() 扫描各模块目录内的 strings[_xx].xml 文件名
//     推断可用语种；strings.xml=默认语种(defaultLang)，strings_<lang>.xml=<lang>。
//     新增语种只要放一份 strings_<lang>.xml，底层零改动。
//
#include "infra/i18n/II18nService.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wb::services {

class XmlI18nService final : public II18nService {
public:
    explicit XmlI18nService(std::string baseDir,
                            std::string defaultLang = "zh-CN",
                            std::string initialLang = "zh-CN");

    aria::Property<std::string>& language() override { return language_; }
    std::vector<std::string> available_languages() const override;
    void set_language(const std::string& lang) override;
    std::string tr(std::string_view module, std::string_view key) const override;

private:
    using Table        = std::unordered_map<std::string, std::string>;  // key→text
    using ModuleTables = std::unordered_map<std::string, Table>;        // module→Table

    std::string baseDir_;
    std::string defaultLang_;
    aria::Property<std::string> language_;

    // lang → (module → (key → text))。懒加载填充。mutable 供 const tr() 缓存。
    mutable std::unordered_map<std::string, ModuleTables> cache_;
    mutable std::mutex mutex_;

    // 懒加载：确保 (lang, module) 已从磁盘读入缓存。
    void ensure_module_loaded_(const std::string& lang, const std::string& module) const;
    static Table parse_file_(const std::string& path);
    std::string file_for_(const std::string& lang, const std::string& module) const;

    const std::string* lookup_(const std::string& lang,
                               std::string_view module,
                               std::string_view key) const;
};

}  // namespace wb::services
