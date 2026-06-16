#include "ui/UIManager.h"

#include "core/Text.h"

namespace {

Color PanelColor() {
    return Color{24, 26, 38, 238};
}

Color BorderColor() {
    return Color{104, 108, 132, 255};
}

}  // namespace

bool UIManager::DrawButton(Rectangle rect, const std::string& text) const {
    const Vector2 mouse = LogicalMousePosition();
    const bool hovered = CheckCollisionPointRec(mouse, rect);
    const Color fill = hovered ? Color{78, 88, 118, 255} : Color{44, 48, 68, 255};
    const Color border = hovered ? Color{238, 220, 148, 255} : BorderColor();
    DrawRectangleRounded(rect, 0.16f, 8, fill);
    DrawRectangleRoundedLines(rect, 0.16f, 8, border);
    DrawTextCN(text,
               static_cast<int>(rect.x + rect.width / 2 - MeasureTextCN(text, 20) / 2),
               static_cast<int>(rect.y + rect.height / 2 - 10),
               20,
               RAYWHITE);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UIManager::DrawScreenBackdrop(const std::string& title, const std::string& subtitle) const {
    ClearBackground(Color{10, 11, 18, 255});
    for (int i = 0; i < 12; ++i) {
        DrawCircle(140 + i * 120, 120 + (i % 3) * 170, 88, Color{40, 34, 60, 90});
    }
    DrawRectangle(0, 0, kScreenW, kScreenH, Color{0, 0, 0, 70});
    DrawTextCN(title, 92, 96, 52, Color{246, 232, 180, 255});
    DrawTextCN(subtitle, 98, 166, 22, Color{190, 192, 210, 255});
}

void UIManager::DrawHelpPanel() const {
    DrawScreenBackdrop("操作说明", "第二版单局演示键盘操作");
    DrawRectangleRounded({82, 224, 900, 400}, 0.04f, 10, PanelColor());
    DrawRectangleRoundedLines({82, 224, 900, 400}, 0.04f, 10, BorderColor());

    const std::vector<std::string> lines = {
        "入口界面      选择模式、确认准备、进入对局",
        "Q / E         切换当前人类角色",
        "W A S D       移动当前人类",
        "H             靠近灯源时点亮灯源",
        "J             在当前任务点进行协作",
        "K             拾取或交付光核",
        "L             救援并净化附近队友",
        "方向键        控制影穿墙移动",
        "1 - 6         让影传送到指定房间，支持小键盘",
        "V             切换人类有限视野 / 影全图视野",
        "Esc           游戏中暂停 / 从帮助返回"};

    int y = 250;
    for (const auto& line : lines) {
        DrawTextCN(line, 120, y, 20, Color{232, 232, 238, 255});
        y += 29;
    }
    DrawTextCN("按 Esc 返回", 120, 584, 18, Color{238, 220, 148, 255});
}

void UIManager::DrawPausePanel() const {
    DrawRectangle(0, 0, kScreenW, kScreenH, Color{0, 0, 0, 138});
    DrawRectangleRounded({kScreenW / 2.0f - 230, 150, 460, 430}, 0.05f, 10, PanelColor());
    DrawRectangleRoundedLines({kScreenW / 2.0f - 230, 150, 460, 430}, 0.05f, 10, BorderColor());
    DrawTextCN("游戏暂停", kScreenW / 2 - MeasureTextCN("游戏暂停", 40) / 2, 190, 40,
               Color{246, 232, 180, 255});
}

void UIManager::DrawResultPanel(const std::string& result,
                                int turn,
                                int completedTasks,
                                int assimilatedHumans) const {
    (void)turn;
    DrawRectangle(0, 0, kScreenW, kScreenH, Color{0, 0, 0, 160});
    DrawRectangleRounded({kScreenW / 2.0f - 310, 132, 620, 468}, 0.05f, 10, PanelColor());
    DrawRectangleRoundedLines({kScreenW / 2.0f - 310, 132, 620, 468}, 0.05f, 10, BorderColor());
    DrawTextCN(result, kScreenW / 2 - MeasureTextCN(result, 28) / 2, 178, 28,
               Color{246, 232, 180, 255});
    DrawTextCN(TextFormat("完成任务：%d / 4", completedTasks), kScreenW / 2 - 190, 252, 22, RAYWHITE);
    DrawTextCN(TextFormat("被同化人类：%d / 3", assimilatedHumans), kScreenW / 2 - 190, 300, 22, RAYWHITE);
    DrawTextCN("可重新开始、重新选择阵营或返回主菜单。", kScreenW / 2 - 265, 370, 18,
               Color{180, 184, 198, 255});
}

void UIManager::DrawToast(const std::string& message) const {
    if (message.empty()) {
        return;
    }
    const int width = MeasureTextCN(message, 18) + 36;
    DrawRectangleRounded({kScreenW / 2.0f - width / 2.0f, 18, static_cast<float>(width), 38}, 0.2f, 8,
                         Color{36, 40, 58, 238});
    DrawRectangleRoundedLines({kScreenW / 2.0f - width / 2.0f, 18, static_cast<float>(width), 38}, 0.2f, 8,
                              Color{238, 220, 148, 255});
    DrawTextCN(message, kScreenW / 2 - MeasureTextCN(message, 18) / 2, 28, 18, RAYWHITE);
}
