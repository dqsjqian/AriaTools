#include "models/CalendarModel.h"

#include "infra/log/Log.h"
#include "utils/Platform.h"

#include <array>
#include <chrono>
#include <ctime>

namespace wb::calendar {

namespace {
void local_today(int& y, int& m, int& d) {
    const auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm{};
#if WB_OS_WINDOWS
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    y = tm.tm_year + 1900; m = tm.tm_mon + 1; d = tm.tm_mday;
}
}  // namespace

CalendarModel::CalendarModel(std::shared_ptr<CalendarService> service)
    : service_(std::move(service)) {}

int CalendarModel::days_in_month_(int y, int m) {
    static constexpr std::array<int, 12> base{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2) {
        const bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        return leap ? 29 : 28;
    }
    return base[static_cast<std::size_t>(m - 1)];
}

// 基姆拉尔森公式变体：返回 0=周一 .. 6=周日。
int CalendarModel::weekday_of_(int y, int m, int d) {
    if (m < 3) { m += 12; y -= 1; }
    const int k = y % 100, j = y / 100;
    // Zeller: 0=Saturday..6=Friday
    int h = (d + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    // 转为 0=Monday..6=Sunday
    int iso = (h + 5) % 7;
    return iso;
}

void CalendarModel::initialize() {
    int y, m, d; local_today(y, m, d);
    year.set(y);
    month.set(m);

    auto subs = service_->load_subscriptions();
    subscriptions.clear();
    if (subs.ok()) {
        for (auto& s : subs.value) subscriptions.push_back(std::make_shared<Subscription>(s));
    }
    // 载入各订阅缓存事件（离线可见），联网刷新由 refresh_all 触发。
    events_.clear();
    for (auto& s : subscriptions.snapshot()) {
        if (auto cached = service_->load_cached(s->id); cached.ok()) {
            for (auto& e : cached.value) events_.push_back(std::move(e));
        }
    }
    rebuild_days_();
}

void CalendarModel::prev_month() {
    int y = year.get(), m = month.get();
    if (--m < 1) { m = 12; --y; }
    year.set(y); month.set(m);
    rebuild_days_();
}

void CalendarModel::next_month() {
    int y = year.get(), m = month.get();
    if (++m > 12) { m = 1; ++y; }
    year.set(y); month.set(m);
    rebuild_days_();
}

void CalendarModel::go_today() {
    int y, m, d; local_today(y, m, d);
    year.set(y); month.set(m);
    rebuild_days_();
}

bool CalendarModel::add_subscription(const std::string& url, const std::string& name) {
    auto r = service_->add_subscription(url, name);
    if (!r.ok()) { set_error_(r.error, r.message); return false; }
    subscriptions.push_back(std::make_shared<Subscription>(r.value));
    set_error_(CalendarError::None, {});
    return true;
}

bool CalendarModel::remove_subscription(const SubId& id) {
    auto r = service_->remove_subscription(id);
    if (!r.ok()) { set_error_(r.error, r.message); return false; }
    for (std::size_t i = 0; i < subscriptions.size(); ++i) {
        auto s = subscriptions.at(i);
        if (s && s->id == id) { subscriptions.remove_at(i); break; }
    }
    // 移除该订阅的事件并重排。
    EventList kept;
    for (auto& e : events_) if (e.source != id) kept.push_back(std::move(e));
    events_.swap(kept);
    rebuild_days_();
    set_error_(CalendarError::None, {});
    return true;
}

bool CalendarModel::refresh_all() {
    events_.clear();
    bool anyFail = false;
    std::string firstErr;
    for (auto& s : subscriptions.snapshot()) {
        auto r = service_->refresh(*s);
        if (r.ok()) {
            for (auto& e : r.value) events_.push_back(std::move(e));
            s->lastFetched = 1;  // 标记已抓取（展示用，精确值以磁盘为准）
        } else {
            anyFail = true;
            if (firstErr.empty()) firstErr = r.message;
            // 回退到缓存，避免刷新失败清空显示。
            if (auto cached = service_->load_cached(s->id); cached.ok()) {
                for (auto& e : cached.value) events_.push_back(std::move(e));
            }
        }
    }
    rebuild_days_();
    if (anyFail) { set_error_(CalendarError::FetchFailed, firstErr); return false; }
    set_error_(CalendarError::None, {});
    return true;
}

void CalendarModel::rebuild_days_() {
    const int y = year.get(), m = month.get();
    int ty, tm, td; local_today(ty, tm, td);

    const int firstWd = weekday_of_(y, m, 1);          // 0=周一
    const int dim = days_in_month_(y, m);
    int pm = m - 1, py = y; if (pm < 1) { pm = 12; --py; }
    const int dimPrev = days_in_month_(py, pm);

    days.clear();
    for (int cell = 0; cell < 42; ++cell) {
        DayCell c;
        int dayNum, cy, cm; bool inMonth;
        if (cell < firstWd) {                           // 前月补齐
            dayNum = dimPrev - firstWd + 1 + cell;
            cy = py; cm = pm; inMonth = false;
        } else if (cell < firstWd + dim) {              // 当月
            dayNum = cell - firstWd + 1;
            cy = y; cm = m; inMonth = true;
        } else {                                        // 次月补齐
            dayNum = cell - firstWd - dim + 1;
            cm = m + 1; cy = y; if (cm > 12) { cm = 1; ++cy; }
            inMonth = false;
        }
        c.year = cy; c.month = cm; c.day = dayNum;
        c.inCurrentMonth = inMonth;
        c.isToday = (cy == ty && cm == tm && dayNum == td);
        c.label = std::to_string(dayNum);
        // 归类事件（按起始日；仅当月格子展示，减少视觉噪声）。
        if (inMonth) {
            for (const auto& e : events_) {
                if (e.startYear == cy && e.startMonth == cm && e.startDay == dayNum) {
                    c.eventTitles.push_back(
                        e.allDay ? e.summary : (e.startTime + " " + e.summary));
                }
            }
        }
        days.push_back(std::make_shared<DayCell>(std::move(c)));
    }

    int shown = 0;
    for (const auto& e : events_)
        if (e.startYear == y && e.startMonth == m) ++shown;
    status.set(std::to_string(shown) + " events");
    log_info << "rebuilt " << y << "-" << m << " grid, " << shown << " events this month";
}

void CalendarModel::set_error_(CalendarError e, std::string msg) {
    lastError.set(e == CalendarError::None ? std::string{} : std::move(msg));
}

}  // namespace wb::calendar
