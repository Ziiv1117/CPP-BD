#pragma once

#include <string>
#include <vector>

enum class TaskId { ThreeLights, DualGate, CoreEscort, FinalPurification };

class CoopTask {
public:
    CoopTask(TaskId id, std::string name, std::string objective);

    TaskId Id() const;
    bool Complete() const;
    void MarkComplete();
    const std::string& Name() const;
    const std::string& Objective() const;

private:
    TaskId id_;
    std::string name_;
    std::string objective_;
    bool complete_ = false;
};

class TaskManager {
public:
    TaskManager();

    bool Complete(TaskId id) const;
    void Mark(TaskId id);
    const std::vector<CoopTask>& Tasks() const;

private:
    std::vector<CoopTask> tasks_;
};
