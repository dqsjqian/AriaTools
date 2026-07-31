#include "infra/i18n/XmlI18nService.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace wb::services {

XmlI18nService::XmlI18nService(std::string baseDir,
                               std::string defaultLang,
                               std::string initialLang)
    : baseDir_(std::move(baseDir)),
      defaultLang_(std::move(defaultLang)),
      language_(std::move(initialLang))
{
    // 不做全量预加载：模块按需懒加载。仅确保缓存桶存在。
}

// ── 语种自适应：扫描各模块目录里的 strings[_xx].xml，推断可用语种 ──────────
std::vector<std::string> XmlI18nService::available_languages() const {
    std::unordered_set<std::string> langs;
    langs.insert(defaultLang_);  // strings.xml 一定对应默认语种。

    std::error_code ec;
    if (fs::exists(baseDir_, ec)) {
        for (auto& moduleDir : fs::directory_iterator(baseDir_, ec)) {
            if (!moduleDir.is_directory()) continue;
            for (auto& f : fs::directory_iterator(moduleDir.path(), ec)) {
                if (!f.is_regular_file()) continue;
                const std::string name = f.path().filename().string();
                // 匹配 strings_<lang>.xml
                constexpr std::string_view pre = "strings_";
                constexpr std::string_view suf = ".xml";
                if (name.size() > pre.size() + suf.size() &&
                    name.compare(0, pre.size(), pre) == 0 &&
                    name.compare(name.size() - suf.size(), suf.size(), suf) == 0) {
                    langs.insert(name.substr(pre.size(),
                                             name.size() - pre.size() - suf.size()));
                }
            }
        }
    }
    return {langs.begin(), langs.end()};
}

void XmlI18nService::set_language(const std::string& lang) {
    if (lang == language_.get()) return;
    language_.set(lang);  // 懒加载会在后续 tr() 时按需读入该语种。
}

std::string XmlI18nService::file_for_(const std::string& lang,
                                      const std::string& module) const {
    const std::string fname =
        (lang == defaultLang_) ? "strings.xml" : ("strings_" + lang + ".xml");
    return (fs::path(baseDir_) / module / fname).string();
}

// ── 极简 XML 解析：只认 <string name="KEY">TEXT</string> ──────────────────
namespace {

std::string unescape_xml(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size();) {
        if (in[i] == '&') {
            auto semi = in.find(';', i);
            if (semi != std::string::npos) {
                std::string ent = in.substr(i + 1, semi - i - 1);
                if      (ent == "amp")  { out += '&';  i = semi + 1; continue; }
                else if (ent == "lt")   { out += '<';  i = semi + 1; continue; }
                else if (ent == "gt")   { out += '>';  i = semi + 1; continue; }
                else if (ent == "quot") { out += '"';  i = semi + 1; continue; }
                else if (ent == "apos") { out += '\''; i = semi + 1; continue; }
                else if (!ent.empty() && ent[0] == '#') {
                    try {
                        int code = (ent.size() > 1 && (ent[1] == 'x' || ent[1] == 'X'))
                                       ? std::stoi(ent.substr(2), nullptr, 16)
                                       : std::stoi(ent.substr(1));
                        if (code < 0x80) out += static_cast<char>(code);
                        else if (code < 0x800) {
                            out += static_cast<char>(0xC0 | (code >> 6));
                            out += static_cast<char>(0x80 | (code & 0x3F));
                        } else {
                            out += static_cast<char>(0xE0 | (code >> 12));
                            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                            out += static_cast<char>(0x80 | (code & 0x3F));
                        }
                        i = semi + 1; continue;
                    } catch (...) { /* fallthrough */ }
                }
            }
        }
        out += in[i++];
    }
    return out;
}

std::string attr_value(const std::string& tag, const std::string& attr) {
    auto p = tag.find(attr);
    if (p == std::string::npos) return {};
    p = tag.find('=', p);
    if (p == std::string::npos) return {};
    p = tag.find_first_of("\"'", p);
    if (p == std::string::npos) return {};
    char q = tag[p];
    auto end = tag.find(q, p + 1);
    if (end == std::string::npos) return {};
    return tag.substr(p + 1, end - p - 1);
}

}  // namespace

XmlI18nService::Table XmlI18nService::parse_file_(const std::string& path) {
    Table table;
    std::ifstream in(path, std::ios::binary);
    if (!in) return table;
    std::ostringstream ss;
    ss << in.rdbuf();
    const std::string xml = ss.str();

    std::size_t pos = 0;
    while (true) {
        auto open = xml.find("<string", pos);
        if (open == std::string::npos) break;
        auto gt = xml.find('>', open);
        if (gt == std::string::npos) break;
        const std::string opentag = xml.substr(open, gt - open + 1);
        const std::string key = attr_value(opentag, "name");
        auto close = xml.find("</string>", gt + 1);
        if (close == std::string::npos) break;
        const std::string raw = xml.substr(gt + 1, close - gt - 1);
        if (!key.empty()) table[key] = unescape_xml(raw);
        pos = close + 9;
    }
    return table;
}

void XmlI18nService::ensure_module_loaded_(const std::string& lang,
                                           const std::string& module) const {
    auto& langBucket = cache_[lang];
    if (langBucket.count(module)) return;               // 已加载
    langBucket[module] = parse_file_(file_for_(lang, module));  // 缺文件→空表，不再重试
}

const std::string* XmlI18nService::lookup_(const std::string& lang,
                                           std::string_view module,
                                           std::string_view key) const {
    const std::string m(module);
    ensure_module_loaded_(lang, m);
    auto li = cache_.find(lang);
    if (li == cache_.end()) return nullptr;
    auto mi = li->second.find(m);
    if (mi == li->second.end()) return nullptr;
    auto ki = mi->second.find(std::string(key));
    if (ki == mi->second.end()) return nullptr;
    return &ki->second;
}

std::string XmlI18nService::tr(std::string_view module, std::string_view key) const {
    std::lock_guard<std::mutex> lk(mutex_);
    const std::string cur = language_.get();
    if (auto* v = lookup_(cur, module, key)) return *v;
    if (cur != defaultLang_) {
        if (auto* v = lookup_(defaultLang_, module, key)) return *v;
    }
    return "[" + std::string(module) + "/" + std::string(key) + "]";
}

II18nService* make_xml_i18n_service(std::string baseDir, std::string initialLang) {
    return new XmlI18nService(std::move(baseDir), "zh-CN", std::move(initialLang));
}

}  // namespace wb::services
