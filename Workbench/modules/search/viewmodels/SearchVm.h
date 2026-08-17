#pragma once
//
// SearchVm — Tab 5: search box
// input → debounce(300ms) → distinct_until_changed → hit history list
//

#include "aria/aria.hpp"
#include "aria/observable_list.hpp"
#include "aria/property_ops.hpp"
#include "module_api/BaseVm.h"

#include <memory>
#include <string>

namespace wb::search {

struct SearchHit {
    std::string q;
    int         seq = 0;
};

class SearchVm final : public wb::core::BaseVm {
public:
    explicit SearchVm(aria::IDelayedScheduler& timer);

    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    aria::Property<std::string> placeholder;
    aria::Property<std::string> searchesLabel;
    aria::Property<std::string>                  query{""};
    aria::ObservableList<SearchHit>              hits;

    std::shared_ptr<aria::Property<std::string>> debounced;
    std::shared_ptr<aria::Property<std::string>> distinct;

private:
    int seq_ = 0;
};

}  // namespace wb::search
