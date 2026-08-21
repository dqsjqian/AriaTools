#pragma once

#include "models/LabTask.h"
#include "module_api/BaseVm.h"

#include "aria/command.hpp"
#include "aria/derived/filtered_list.hpp"
#include "aria/observable_list.hpp"
#include "aria/reactive/inspector.hpp"
#include "aria/selection.hpp"

#include <cstddef>
#include <memory>
#include <string>

namespace wb::frameworklab {

class FrameworkLabVm final : public wb::core::BaseVm {
public:
    FrameworkLabVm();

    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    aria::Property<std::string> draft;
    aria::Property<std::size_t> selectedIndex{0};
    aria::Property<std::string> graphSnapshot;

    aria::Property<std::string> draftLabel;
    aria::Property<std::string> addLabel;
    aria::Property<std::string> toggleLabel;
    aria::Property<std::string> removeLabel;
    aria::Property<std::string> clearLabel;
    aria::Property<std::string> refreshLabel;
    aria::Property<std::string> allLabel;
    aria::Property<std::string> activeLabel;
    aria::Property<std::string> completedLabel;
    aria::Property<std::string> graphLabel;
    aria::Property<std::string> noneLabel;
    aria::Property<std::string> seedGraphText;
    aria::Property<std::string> seedFilterText;
    aria::Property<std::string> seedSelectionText;

    std::shared_ptr<aria::ObservableList<LabTask>> tasks;
    aria::FilteredList<LabTask> active;
    aria::FilteredList<LabTask> completed;
    aria::Selection<LabTask> selection;

private:
    aria::Property<std::size_t> revision_{0};

public:
    aria::Computed<std::size_t> totalCount;
    aria::Computed<std::size_t> activeCount;
    aria::Computed<std::size_t> completedCount;
    aria::Computed<std::string> summaryText;
    aria::Computed<std::string> selectedText;

    aria::Command<> addTask;
    aria::Command<> toggleSelected;
    aria::Command<> removeSelected;
    aria::Command<> clearCompleted;
    aria::Command<> refreshGraph;

    void select_index(std::size_t index);

private:
    void refresh_graph();
    aria::Subscription tasksSub_;
};

}  // namespace wb::frameworklab
