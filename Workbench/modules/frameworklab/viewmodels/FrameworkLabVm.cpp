#include "viewmodels/FrameworkLabVm.h"

#include <algorithm>
#include <utility>

namespace wb::frameworklab {

FrameworkLabVm::FrameworkLabVm()
    : tasks(std::make_shared<aria::ObservableList<LabTask>>()),
      active(tasks, [](const LabTask& task) { return !task.completed; }),
      completed(tasks, [](const LabTask& task) { return task.completed; }),
      totalCount([this] {
          (void)revision_.get();
          return tasks->size();
      }),
      activeCount([this] {
          (void)revision_.get();
          return active.size();
      }),
      completedCount([this] {
          (void)revision_.get();
          return completed.size();
      }),
      summaryText([this] {
          return allLabel.get() + ": " + std::to_string(totalCount.get()) +
                 "  |  " + activeLabel.get() + ": " +
                 std::to_string(activeCount.get()) + "  |  " +
                 completedLabel.get() + ": " +
                 std::to_string(completedCount.get());
      }),
      selectedText([this] {
          const auto selected = selection.selected().get();
          return selected ? selected->title : noneLabel.get();
      }),
      addTask([this] {
          auto value = draft.get();
          if (value.empty()) return;
          tasks->push_back(std::make_shared<LabTask>(LabTask{std::move(value), false}));
          selectedIndex.set(tasks->size() - 1);
          selection.select(tasks->at(tasks->size() - 1));
          draft.set({});
      }, [this] { return !draft.get().empty(); }),
      toggleSelected([this] {
          auto selected = selection.value();
          if (!selected) return;
          const auto snapshot = tasks->snapshot();
          const auto it = std::find(snapshot.begin(), snapshot.end(), selected);
          if (it == snapshot.end()) return;
          const auto index = static_cast<std::size_t>(std::distance(snapshot.begin(), it));
          tasks->replace_at(index, std::make_shared<LabTask>(
              LabTask{selected->title, !selected->completed}));
          selection.select(tasks->at(index));
      }, [this] { return static_cast<bool>(selection.selected().get()); }),
      removeSelected([this] {
          auto selected = selection.value();
          if (!selected) return;
          const auto snapshot = tasks->snapshot();
          const auto it = std::find(snapshot.begin(), snapshot.end(), selected);
          if (it != snapshot.end()) {
              tasks->remove_at(static_cast<std::size_t>(std::distance(snapshot.begin(), it)));
          }
      }, [this] { return static_cast<bool>(selection.selected().get()); }),
      clearCompleted([this] {
          const auto snapshot = tasks->snapshot();
          for (std::size_t i = snapshot.size(); i-- > 0;) {
              if (snapshot[i]->completed) tasks->remove_at(i);
          }
      }, [this] { return completedCount.get() > 0; }),
      refreshGraph([this] { refresh_graph(); }) {
    text(title, "title");
    text(desc, "desc");
    text(draft, "default_task");
    text(draftLabel, "draft");
    text(addLabel, "add");
    text(toggleLabel, "toggle");
    text(removeLabel, "remove");
    text(clearLabel, "clear_completed");
    text(refreshLabel, "refresh_graph");
    text(allLabel, "all");
    text(activeLabel, "active");
    text(completedLabel, "completed");
    text(graphLabel, "graph");
    text(noneLabel, "none_selected");
    text(seedGraphText, "seed_graph");
    text(seedFilterText, "seed_filter");
    text(seedSelectionText, "seed_selection");

    revision_.set_debug_name("frameworklab.tasks_revision");
    totalCount.set_debug_name("frameworklab.total_count");
    activeCount.set_debug_name("frameworklab.active_count");
    completedCount.set_debug_name("frameworklab.completed_count");
    summaryText.set_debug_name("frameworklab.summary_text");

    selection.bind_to(*tasks);
    tasksSub_ = tasks->on_any_change([this] {
        revision_.set(revision_.get() + 1);
        if (tasks->empty()) selectedIndex.set(0);
    });

    tasks->push_back(std::make_shared<LabTask>(LabTask{seedGraphText.get(), false}));
    tasks->push_back(std::make_shared<LabTask>(LabTask{seedFilterText.get(), false}));
    tasks->push_back(std::make_shared<LabTask>(LabTask{seedSelectionText.get(), true}));
    selection.select(tasks->at(0));
    refresh_graph();
}

void FrameworkLabVm::select_index(const std::size_t index) {
    if (index >= tasks->size()) {
        selection.clear();
        return;
    }
    selectedIndex.set(index);
    selection.select(tasks->at(index));
}

void FrameworkLabVm::refresh_graph() {
    (void)summaryText.get();
    graphSnapshot.set(aria::reactive::GraphInspector::to_text(
        {static_cast<const aria::reactive::Node*>(&summaryText)}));
}

}  // namespace wb::frameworklab
