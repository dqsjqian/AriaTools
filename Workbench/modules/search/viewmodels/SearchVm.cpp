#include "viewmodels/SearchVm.h"

namespace wb::search {

SearchVm::SearchVm(aria::IDelayedScheduler& timer) {
    text(title, "title");
    text(desc, "desc");
    text(placeholder, "placeholder");
    text(searchesLabel, "searches");

    debounced = aria::debounce(query, std::chrono::milliseconds(300), timer);
    distinct  = aria::distinct_until_changed(*debounced);
    bag() += distinct->on_changed([this](const std::string& q) {
        if (q.empty()) return;
        ++seq_;
        hits.push_back(std::make_shared<SearchHit>(SearchHit{q, seq_}));
    });
}

}  // namespace wb::search
