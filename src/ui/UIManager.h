#pragma once

#include "core/Common.h"

#include <string>
#include <vector>

class UIManager {
public:
    bool DrawButton(Rectangle rect, const std::string& text) const;
    void DrawScreenBackdrop(const std::string& title, const std::string& subtitle) const;
    void DrawHelpPanel() const;
    void DrawPausePanel() const;
    void DrawResultPanel(const std::string& result,
                         int turn,
                         int completedTasks,
                         int assimilatedHumans) const;
    void DrawToast(const std::string& message) const;
};
