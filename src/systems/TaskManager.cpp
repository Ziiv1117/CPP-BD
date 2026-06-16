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
    tasks_.push_back({TaskId::RekindleLamp, "重燃残灯", "守灯人恢复关键灯源"});
    tasks_.push_back({TaskId::RepairGate, "维修机关门", "工程师打开机关门"});
    tasks_.push_back({TaskId::GuardedGate, "护光开门", "守灯人护灯，工程师开门"});
    tasks_.push_back({TaskId::CalibrateLamps, "灯源校准", "守灯人点灯，观测员识别目标"});
    tasks_.push_back({TaskId::TraceShadow, "影迹追踪", "观测员找影迹，工程师开路线"});
    tasks_.push_back({TaskId::FinalPurification, "最终净化", "三名人类各司其职完成封印"});
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
