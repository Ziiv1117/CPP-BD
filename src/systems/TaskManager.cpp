#include "systems/TaskManager.h"

#include <utility>

CoopTask::CoopTask(TaskId id, std::string name, std::string objective)
    : id_(id), name_(std::move(name)), objective_(std::move(objective)) {}

TaskId CoopTask::Id() const {
    return id_;
}

bool CoopTask::Complete() const {
    return complete_;
}

void CoopTask::MarkComplete() {
    complete_ = true;
}

const std::string& CoopTask::Name() const {
    return name_;
}

const std::string& CoopTask::Objective() const {
    return objective_;
}

TaskManager::TaskManager() {
    tasks_.push_back({TaskId::ThreeLights, "三灯校准", "同时点亮并协作上层房间"});
    tasks_.push_back({TaskId::DualGate, "双闸供能", "电闸房和镜光室同时协作"});
    tasks_.push_back({TaskId::CoreEscort, "运送光核", "把光核从中庭光井送到封印大厅"});
    tasks_.push_back({TaskId::FinalPurification, "最终净化", "所有存活人类在封印大厅协作"});
}

bool TaskManager::Complete(TaskId id) const {
    for (const auto& task : tasks_) {
        if (task.Id() == id) {
            return task.Complete();
        }
    }
    return false;
}

void TaskManager::Mark(TaskId id) {
    for (auto& task : tasks_) {
        if (task.Id() == id) {
            task.MarkComplete();
        }
    }
}

const std::vector<CoopTask>& TaskManager::Tasks() const {
    return tasks_;
}
