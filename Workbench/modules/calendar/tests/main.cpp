// calendar 模块业务测试：ICS 解析、月历排布、订阅持久化、以及用 fake
// IHttpClient 的 refresh 流程（不依赖真实网络）。
#include "models/CalendarModel.h"
#include "services/CalendarService.h"

#include "infra/storage/IStorageService.h"
#include "infra/http/IHttpClient.h"
#include "infra/log/Log.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;

namespace wb::services {
IStorageService* make_local_file_storage_service(std::string dataDir);
}

namespace {

int g_failures = 0;
void check(bool cond, const char* what) {
    if (!cond) { std::printf("[calendar-tests] FAIL: %s\n", what); ++g_failures; }
    else       { std::printf("[calendar-tests] ok: %s\n", what); }
}

fs::path make_temp_dir() {
    const auto stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto base = fs::temp_directory_path() / ("wb-cal-" + std::to_string(stamp));
    std::error_code ec; fs::remove_all(base, ec); fs::create_directories(base, ec);
    return base;
}

// 返回预置 .ics 的 fake http client。
class FakeHttp final : public wb::services::IHttpClient {
public:
    explicit FakeHttp(std::string body) : body_(std::move(body)) {}
    wb::services::HttpResponse send(const wb::services::HttpRequest& req) override {
        ++calls;
        lastUrl = req.url;
        wb::services::HttpResponse r;
        r.ok = ok_; r.status = ok_ ? 200 : 500;
        r.body = body_;
        if (!ok_) r.error = "simulated failure";
        return r;
    }
    void set_ok(bool v) { ok_ = v; }
    int calls = 0;
    std::string lastUrl;
private:
    std::string body_;
    bool ok_ = true;
};

const char* kIcs =
    "BEGIN:VCALENDAR\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:e1\r\n"
    "SUMMARY:All day mee\r\n ting\r\n"
    "DTSTART;VALUE=DATE:20260715\r\n"
    "DTEND;VALUE=DATE:20260716\r\n"
    "END:VEVENT\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:e2\r\n"
    "SUMMARY:Timed\r\n"
    "DTSTART:20260715T093000Z\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n";

}  // namespace

int main() {
    using namespace wb::calendar;
    wb::log::init_default_sink();

    // ── 1) 纯 ICS 解析 ────────────────────────────────────────────────────
    {
        auto r = CalendarService::parse_ics(kIcs, "sub-x");
        check(r.ok(), "parse ok");
        check(r.value.size() == 2, "two events parsed");
        if (r.value.size() == 2) {
            check(r.value[0].summary == "All day meeting", "line unfolding");
            check(r.value[0].allDay, "event1 all-day");
            check(r.value[0].startYear == 2026 && r.value[0].startMonth == 7 &&
                  r.value[0].startDay == 15, "event1 date");
            check(!r.value[1].allDay && r.value[1].startTime == "09:30", "event2 timed 09:30");
        }
    }

    // ── 2) 订阅持久化 + refresh（fake http）────────────────────────────────
    const fs::path dataDir = make_temp_dir();
    std::unique_ptr<wb::services::IStorageService> storage(
        wb::services::make_local_file_storage_service(dataDir.string()));
    FakeHttp http(kIcs);
    auto service = std::make_shared<CalendarService>(*storage, http);

    auto added = service->add_subscription("https://example.com/cal.ics", "Demo");
    check(added.ok(), "add subscription");
    const SubId id = added.value.id;

    auto listed = service->load_subscriptions();
    check(listed.ok() && listed.value.size() == 1, "one subscription persisted");
    check(listed.value.size() == 1 && listed.value[0].url == "https://example.com/cal.ics",
          "subscription url round-trips");

    auto refreshed = service->refresh(added.value);
    check(refreshed.ok(), "refresh ok");
    check(http.calls == 1 && http.lastUrl == "https://example.com/cal.ics", "http called with url");
    check(refreshed.value.size() == 2, "refresh parsed 2 events");
    check(fs::exists(dataDir / "calendar" / "cache" / (id + ".ics")), "ics cached to disk");

    // 离线读缓存
    auto cached = service->load_cached(id);
    check(cached.ok() && cached.value.size() == 2, "load cached events");

    // 抓取失败 → 返回错误
    http.set_ok(false);
    auto failed = service->refresh(added.value);
    check(!failed.ok() && failed.error == CalendarError::FetchFailed, "fetch failure surfaced");
    http.set_ok(true);

    // ── 3) 月历排布（CalendarModel）────────────────────────────────────────
    {
        auto model = std::make_shared<CalendarModel>(service);
        model->initialize();          // 定位今天所在月，载入缓存事件
        check(model->days.size() == 42, "42 day cells");

        // 计数今天格子
        int todayCells = 0;
        for (auto& c : model->days.snapshot()) if (c->isToday) ++todayCells;
        check(todayCells == 1, "exactly one today cell");

        // 导航到 2026-07 验证事件出现在 7/15，且首格星期正确
        // 2026-07-01 是周三 → ISO 0=周一，周三=2，所以前面有 2 个占位格。
        while (!(model->year.get() == 2026 && model->month.get() == 7)) {
            if (model->year.get() > 2026 ||
                (model->year.get() == 2026 && model->month.get() > 7)) model->prev_month();
            else model->next_month();
        }
        // 刷新拉取事件到当月
        (void)model->refresh_all();

        bool found = false;
        for (auto& c : model->days.snapshot()) {
            if (c->inCurrentMonth && c->day == 15 && !c->eventTitles.empty()) found = true;
        }
        check(found, "events land on 2026-07-15");

        // 首格（cell 0）应是 6 月末的占位（非当月），因为 7/1 是周三。
        auto first = model->days.at(0);
        check(first && !first->inCurrentMonth, "first cell is prev-month filler (Jul 1 is Wed)");
    }

    std::error_code ec; fs::remove_all(dataDir, ec);
    std::puts(g_failures == 0 ? "[calendar-tests] PASS" : "[calendar-tests] FAIL");
    return g_failures == 0 ? 0 : 1;
}
