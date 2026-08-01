#include "services/CalendarService.h"

#include "infra/log/Log.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <sstream>

namespace wb::calendar {

namespace {

constexpr std::string_view kSubsDir  = "calendar/subs";
constexpr std::string_view kCacheDir = "calendar/cache";

std::int64_t now_seconds() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

bool valid_id(std::string_view id) {
    if (id.empty()) return false;
    return std::all_of(id.begin(), id.end(), [](unsigned char c) {
        return std::isalnum(c) || c == '-' || c == '_';
    });
}

SubId make_sub_id() {
    static constexpr char kAlphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    const auto stamp = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    std::string s = "sub-";
    auto v = stamp;
    std::string b36;
    while (v > 0) { b36.push_back(kAlphabet[v % 36]); v /= 36; }
    s.append(b36.rbegin(), b36.rend());
    std::random_device rd; std::mt19937 g(rd());
    std::uniform_int_distribution<int> d(0, 35);
    s.push_back('-');
    for (int i = 0; i < 5; ++i) s.push_back(kAlphabet[d(g)]);
    return s;
}

std::string host_of(const std::string& url) {
    auto p = url.find("://");
    auto start = (p == std::string::npos) ? 0 : p + 3;
    auto end = url.find_first_of("/?", start);
    return url.substr(start, end == std::string::npos ? std::string::npos : end - start);
}

// RFC5545 行展开：以空格/制表符开头的续行拼接到上一行。返回逻辑行序列。
std::vector<std::string> unfold_lines(std::string_view text) {
    std::vector<std::string> lines;
    std::string cur;
    std::size_t i = 0;
    auto flush = [&] { if (!cur.empty()) { lines.push_back(cur); cur.clear(); } };
    while (i < text.size()) {
        // 读一物理行（到 \r\n / \n）。
        std::size_t j = i;
        while (j < text.size() && text[j] != '\n' && text[j] != '\r') ++j;
        std::string_view phys = text.substr(i, j - i);
        // 跳过换行符
        std::size_t next = j;
        if (next < text.size() && text[next] == '\r') ++next;
        if (next < text.size() && text[next] == '\n') ++next;
        if (!phys.empty() && (phys.front() == ' ' || phys.front() == '\t')) {
            cur.append(phys.substr(1));  // 续行：去掉前导折叠空白
        } else {
            flush();
            cur.assign(phys);
        }
        i = next;
    }
    flush();
    return lines;
}

// 把 "20260728" 或 "20260728T130000Z" 解析进 CalendarEvent 的日期/时间字段。
bool parse_dt(std::string_view value, int& y, int& mo, int& d,
              bool& allDay, std::string& hhmm) {
    // 去掉可能的参数已在调用方处理，这里 value 是纯值。
    if (value.size() < 8) return false;
    auto to_int = [](std::string_view s) {
        int n = 0;
        for (char c : s) { if (c < '0' || c > '9') return -1; n = n * 10 + (c - '0'); }
        return n;
    };
    y  = to_int(value.substr(0, 4));
    mo = to_int(value.substr(4, 2));
    d  = to_int(value.substr(6, 2));
    if (y < 0 || mo < 1 || mo > 12 || d < 1 || d > 31) return false;
    if (value.size() >= 15 && value[8] == 'T') {
        const int hh = to_int(value.substr(9, 2));
        const int mi = to_int(value.substr(11, 2));
        if (hh >= 0 && mi >= 0) {
            char buf[6];
            std::snprintf(buf, sizeof(buf), "%02d:%02d", hh, mi);
            hhmm = buf;
            allDay = false;
            return true;
        }
    }
    allDay = true;
    hhmm.clear();
    return true;
}

// 拆 "KEY;PARAM=..:VALUE" → (name_without_params, value)。
void split_prop(const std::string& line, std::string& name, std::string& value) {
    const auto colon = line.find(':');
    if (colon == std::string::npos) { name = line; value.clear(); return; }
    const std::string left = line.substr(0, colon);
    value = line.substr(colon + 1);
    const auto semi = left.find(';');
    name = (semi == std::string::npos) ? left : left.substr(0, semi);
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c) { return std::toupper(c); });
}

}  // namespace

CalendarResult<EventList> CalendarService::parse_ics(std::string_view text,
                                                     const SubId& source) {
    EventList events;
    const auto lines = unfold_lines(text);

    bool inEvent = false;
    CalendarEvent cur;
    for (const auto& raw : lines) {
        std::string name, value;
        split_prop(raw, name, value);
        if (name == "BEGIN" && value == "VEVENT") {
            inEvent = true; cur = CalendarEvent{}; cur.source = source; cur.allDay = true;
            continue;
        }
        if (name == "END" && value == "VEVENT") {
            if (inEvent && cur.startYear != 0) {
                if (cur.endYear == 0) {  // 无 DTEND → 与起始同日
                    cur.endYear = cur.startYear; cur.endMonth = cur.startMonth; cur.endDay = cur.startDay;
                }
                events.push_back(std::move(cur));
            }
            inEvent = false;
            continue;
        }
        if (!inEvent) continue;
        if (name == "UID") cur.uid = value;
        else if (name == "SUMMARY") cur.summary = value;
        else if (name == "DTSTART")
            parse_dt(value, cur.startYear, cur.startMonth, cur.startDay, cur.allDay, cur.startTime);
        else if (name == "DTEND") {
            bool tmpAll; std::string tmpHH;
            parse_dt(value, cur.endYear, cur.endMonth, cur.endDay, tmpAll, tmpHH);
        }
    }
    return CalendarResult<EventList>::success(std::move(events));
}

CalendarResult<SubscriptionList> CalendarService::load_subscriptions() {
    storage_.ensure_dir(std::string(kSubsDir));
    SubscriptionList subs;
    for (const auto& file : storage_.list(std::string(kSubsDir))) {
        if (!file.ends_with(".ini")) continue;
        auto content = storage_.read_text(std::string(kSubsDir) + "/" + file);
        if (!content) continue;
        Subscription s;
        std::istringstream in(*content);
        std::string line;
        while (std::getline(in, line)) {
            const auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            const std::string k = line.substr(0, eq);
            const std::string v = line.substr(eq + 1);
            if (k == "id") s.id = v;
            else if (k == "url") s.url = v;
            else if (k == "name") s.name = v;
            else if (k == "lastFetched") s.lastFetched = std::strtoll(v.c_str(), nullptr, 10);
        }
        if (valid_id(s.id) && !s.url.empty()) subs.push_back(std::move(s));
    }
    std::sort(subs.begin(), subs.end(),
              [](const Subscription& a, const Subscription& b) { return a.id < b.id; });
    log_info << "loaded " << subs.size() << " calendar subscriptions";
    return CalendarResult<SubscriptionList>::success(std::move(subs));
}

CalendarResult<void> CalendarService::save_subscription_(const Subscription& sub) {
    storage_.ensure_dir(std::string(kSubsDir));
    std::ostringstream out;
    out << "id=" << sub.id << '\n'
        << "url=" << sub.url << '\n'
        << "name=" << sub.name << '\n'
        << "lastFetched=" << sub.lastFetched << '\n';
    const std::string path = std::string(kSubsDir) + "/" + sub.id + ".ini";
    if (!storage_.write_text(path, out.str())) {
        return CalendarResult<void>::failure(CalendarError::WriteFailed, "failed to save subscription");
    }
    return CalendarResult<void>::success();
}

CalendarResult<Subscription> CalendarService::add_subscription(const std::string& url,
                                                               const std::string& name) {
    if (url.empty() || url.find("http") != 0) {
        return CalendarResult<Subscription>::failure(
            CalendarError::InvalidData, "subscription url must be http(s)");
    }
    Subscription s;
    s.id = make_sub_id();
    s.url = url;
    s.name = name.empty() ? host_of(url) : name;
    s.lastFetched = 0;
    if (auto r = save_subscription_(s); !r.ok()) {
        return CalendarResult<Subscription>::failure(r.error, r.message);
    }
    log_info << "added subscription " << s.id << " (" << s.url << ")";
    return CalendarResult<Subscription>::success(std::move(s));
}

CalendarResult<void> CalendarService::remove_subscription(const SubId& id) {
    if (!valid_id(id)) {
        return CalendarResult<void>::failure(CalendarError::InvalidData, "unsafe subscription id");
    }
    storage_.remove(std::string(kSubsDir) + "/" + id + ".ini");
    storage_.remove(std::string(kCacheDir) + "/" + id + ".ics");
    log_info << "removed subscription " << id;
    return CalendarResult<void>::success();
}

CalendarResult<EventList> CalendarService::load_cached(const SubId& id) {
    if (!valid_id(id)) {
        return CalendarResult<EventList>::failure(CalendarError::InvalidData, "unsafe id");
    }
    auto content = storage_.read_text(std::string(kCacheDir) + "/" + id + ".ics");
    if (!content) {
        return CalendarResult<EventList>::failure(CalendarError::NotFound, "no cached ics");
    }
    return parse_ics(*content, id);
}

CalendarResult<EventList> CalendarService::refresh(const Subscription& sub) {
    if (sub.url.empty()) {
        return CalendarResult<EventList>::failure(CalendarError::InvalidData, "empty url");
    }
    log_info << "fetching ics: " << sub.url;
    auto resp = http_.get(sub.url);
    if (!resp.ok) {
        log_warn << "fetch failed (" << resp.status << "): " << resp.error;
        return CalendarResult<EventList>::failure(
            CalendarError::FetchFailed,
            resp.error.empty() ? ("http status " + std::to_string(resp.status)) : resp.error);
    }

    // 写缓存（失败不致命，仍继续解析内存内容）。
    storage_.ensure_dir(std::string(kCacheDir));
    storage_.write_text(std::string(kCacheDir) + "/" + sub.id + ".ics", resp.body);

    auto parsed = parse_ics(resp.body, sub.id);
    if (!parsed.ok()) return parsed;

    // 更新 lastFetched。
    Subscription updated = sub;
    updated.lastFetched = now_seconds();
    (void)save_subscription_(updated);

    log_info << "parsed " << parsed.value.size() << " events from " << sub.id;
    return parsed;
}

}  // namespace wb::calendar
