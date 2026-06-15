#include "core/Game.h"

#include "core/Text.h"

#include <cmath>

namespace {

constexpr float kHumanMoveSpeed = 4.8f;
constexpr float kShadowMoveSpeed = 5.6f;
constexpr float kResolveInterval = 0.35f;

Vector2 HeldDirection(int up, int down, int left, int right) {
    Vector2 dir{0.0f, 0.0f};
    if (IsKeyDown(up)) {
        dir.y -= 1.0f;
    }
    if (IsKeyDown(down)) {
        dir.y += 1.0f;
    }
    if (IsKeyDown(left)) {
        dir.x -= 1.0f;
    }
    if (IsKeyDown(right)) {
        dir.x += 1.0f;
    }
    return dir;
}

bool HasDirection(Vector2 dir) {
    return std::abs(dir.x) > 0.001f || std::abs(dir.y) > 0.001f;
}

}  // namespace

Game::Game() {
    ResetRun();
}

void Game::ResetRun() {
    humans_.clear();
    lamps_.clear();
    tasks_ = TaskManager();
    shadow_ = ShadowPlayer();
    corePos_ = RoomCenter(4);
    selectedHuman_ = 0;
    turn_ = 1;
    resolveTimer_ = kResolveInterval;
    shadowVision_ = false;
    coreDelivered_ = false;
    toast_.clear();
    lightReady_ = false;
    shadowReady_ = false;

    humans_.push_back({"人类1 守灯人", {3, 3}, Color{92, 192, 128, 255}});
    humans_.push_back({"人类2 工程师", {6, 3}, Color{82, 154, 222, 255}});
    humans_.push_back({"人类3 观测员", {9, 3}, Color{230, 184, 82, 255}});
    for (int room = 0; room < 6; ++room) {
        lamps_.push_back({room, RoomCenter(room), room == 4});
    }
}

const std::vector<HumanPlayer>& Game::Humans() const {
    return humans_;
}

const ShadowPlayer& Game::Shadow() const {
    return shadow_;
}

const std::vector<LightSource>& Game::Lamps() const {
    return lamps_;
}

const TaskManager& Game::Tasks() const {
    return tasks_;
}

int Game::Turn() const {
    return turn_;
}

int Game::SelectedHuman() const {
    return selectedHuman_;
}

GridPos Game::CorePos() const {
    return corePos_;
}

bool Game::CoreDelivered() const {
    return coreDelivered_;
}

void Game::Update() {
    switch (screen_) {
        case ScreenState::MainMenu:
            UpdateMenu();
            return;
        case ScreenState::RoleSelect:
            UpdateRoleSelect();
            return;
        case ScreenState::Playing:
            UpdatePlaying();
            return;
        case ScreenState::Paused:
            UpdatePaused();
            return;
        case ScreenState::Help:
            UpdateHelp();
            return;
        case ScreenState::Result:
            UpdateResult();
            return;
    }
}

void Game::UpdateMenu() {
    if (IsKeyPressed(KEY_ENTER)) {
        screen_ = ScreenState::RoleSelect;
        return;
    }
    if (IsKeyPressed(KEY_H)) {
        previousScreen_ = ScreenState::MainMenu;
        screen_ = ScreenState::Help;
    }
}

void Game::UpdateRoleSelect() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = ScreenState::MainMenu;
        return;
    }
    if (IsKeyPressed(KEY_ONE)) {
        entryMode_ = EntryMode::LocalDemo;
        lightReady_ = true;
        shadowReady_ = true;
    }
    if (IsKeyPressed(KEY_TWO)) {
        entryMode_ = EntryMode::LightTeam;
        lightReady_ = true;
        shadowReady_ = true;
    }
    if (IsKeyPressed(KEY_THREE)) {
        entryMode_ = EntryMode::ShadowTeam;
        lightReady_ = true;
        shadowReady_ = true;
    }
    if (IsKeyPressed(KEY_SPACE)) {
        lightReady_ = true;
        shadowReady_ = true;
    }
    if (IsKeyPressed(KEY_ENTER) && lightReady_ && shadowReady_) {
        ResetRun();
        screen_ = ScreenState::Playing;
    }
}

void Game::UpdatePlaying() {
    const float dt = GetFrameTime();
    resolveTimer_ -= dt;

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = ScreenState::Paused;
        return;
    }
    if (IsKeyPressed(KEY_Q)) {
        selectedHuman_ = (selectedHuman_ + static_cast<int>(humans_.size()) - 1) % static_cast<int>(humans_.size());
    }
    if (IsKeyPressed(KEY_E)) {
        selectedHuman_ = (selectedHuman_ + 1) % static_cast<int>(humans_.size());
    }
    if (IsKeyPressed(KEY_V)) shadowVision_ = !shadowVision_;

    bool immediateAction = false;
    HumanPlayer& human = humans_[static_cast<std::size_t>(selectedHuman_)];

    const Vector2 humanDir = HeldDirection(KEY_W, KEY_S, KEY_A, KEY_D);
    const Vector2 shadowDir = HeldDirection(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
    if (HasDirection(humanDir)) {
        human.MoveContinuous(humanDir, dt, kHumanMoveSpeed, map_);
    }
    if (HasDirection(shadowDir)) {
        shadow_.MoveContinuous(shadowDir, dt, kShadowMoveSpeed);
    }

    if (IsKeyPressed(KEY_H)) {
        for (auto& lamp : lamps_) {
            lamp.TryActivate(human.Position());
        }
        immediateAction = true;
    } else if (IsKeyPressed(KEY_J)) {
        HandleChannel(human);
        immediateAction = true;
    } else if (IsKeyPressed(KEY_K)) {
        HandleCore(human);
        immediateAction = true;
    } else if (IsKeyPressed(KEY_L)) {
        RescueNearby(selectedHuman_);
        immediateAction = true;
    }

    for (int i = 0; i < 6; ++i) {
        if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i)) {
            shadow_.TeleportToRoom(i);
            immediateAction = true;
        }
    }

    if ((immediateAction || resolveTimer_ <= 0.0f) && !GameOver()) {
        AdvanceTurn();
        resolveTimer_ = kResolveInterval;
        if (GameOver()) {
            screen_ = ScreenState::Result;
        }
    }

    UpdateVisuals(dt);
}

void Game::UpdatePaused() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = ScreenState::Playing;
        return;
    }
    if (IsKeyPressed(KEY_H)) {
        previousScreen_ = ScreenState::Paused;
        screen_ = ScreenState::Help;
    }
}

void Game::UpdateHelp() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = previousScreen_;
    }
}

void Game::UpdateResult() {
    if (IsKeyPressed(KEY_ENTER)) {
        ResetRun();
        screen_ = ScreenState::Playing;
    }
    if (IsKeyPressed(KEY_M)) {
        screen_ = ScreenState::MainMenu;
    }
}

void Game::UpdateVisuals(float dt) {
    for (auto& human : humans_) {
        human.UpdateVisual(dt);
    }
    shadow_.UpdateVisual(dt);
}

void Game::Draw() {
    switch (screen_) {
        case ScreenState::MainMenu:
            DrawMainMenu();
            return;
        case ScreenState::RoleSelect:
            DrawRoleSelect();
            return;
        case ScreenState::Playing:
            DrawGameScene();
            ui_.DrawToast(toast_);
            return;
        case ScreenState::Paused:
            DrawGameScene();
            DrawPauseMenu();
            ui_.DrawToast(toast_);
            return;
        case ScreenState::Help:
            DrawHelpScreen();
            return;
        case ScreenState::Result:
            DrawGameScene();
            DrawResultScreen();
            ui_.DrawToast(toast_);
            return;
    }
}

void Game::DrawGameScene() const {
    ClearBackground(Color{14, 15, 22, 255});
    DrawTextCN("《余光未熄》第二版单局演示", 36, 24, 24, Color{236, 230, 216, 255});
    DrawTextCN("Q/E 切换人类 | WASD 移动人类 | H 点灯 | J 协作 | K 光核 | L 救援 | 方向键移动影 | 1-6/小键盘 影传送",
               36, 52, 16, Color{210, 212, 224, 255});

    map_.DrawBase();
    DrawObjectiveMarkers();
    for (const auto& human : humans_) {
        human.DrawFlashlight();
    }
    for (const auto& lamp : lamps_) {
        lamp.Draw();
    }
    DrawDarknessOverlay();
    for (const auto& human : humans_) {
        human.Draw();
    }
    shadow_.Draw();
    DrawCore();
    DrawSelectionRing();
    DrawPanel();
}

bool Game::LitByAnyHuman(GridPos target) const {
    for (const auto& human : humans_) {
        if (human.InFlashlight(target)) {
            return true;
        }
    }
    return false;
}

bool Game::LitByAnyLight(GridPos target) const {
    if (LitByAnyHuman(target)) {
        return true;
    }
    for (const auto& lamp : lamps_) {
        if (lamp.Lights(target)) {
            return true;
        }
    }
    return false;
}

bool Game::GameOver() const {
    return AllHumansAssimilated() || tasks_.Complete(TaskId::FinalPurification);
}

int Game::AliveHumanCount() const {
    int count = 0;
    for (const auto& human : humans_) {
        if (!human.IsAssimilated()) {
            ++count;
        }
    }
    return count;
}

bool Game::AllHumansAssimilated() const {
    return AliveHumanCount() == 0;
}

int Game::CompletedTaskCount() const {
    int count = 0;
    for (const auto& task : tasks_.Tasks()) {
        if (task.Complete()) {
            ++count;
        }
    }
    return count;
}

int Game::AssimilatedHumanCount() const {
    int count = 0;
    for (const auto& human : humans_) {
        if (human.IsAssimilated()) {
            ++count;
        }
    }
    return count;
}

std::string Game::ResultText() const {
    if (tasks_.Complete(TaskId::FinalPurification)) {
        return "光阵营胜利：最终净化完成";
    }
    if (AllHumansAssimilated()) {
        return "影阵营胜利：所有人类被同化";
    }
    return "对局仍在进行";
}

bool Game::LampLitInRoom(int room) const {
    for (const auto& lamp : lamps_) {
        if (lamp.Room() == room) {
            return lamp.IsLit();
        }
    }
    return false;
}

bool Game::HumanChannelingInRoom(int room) const {
    for (const auto& human : humans_) {
        if (!human.IsAssimilated() && human.IsChanneling() && RoomIndex(human.Position()) == room) {
            return true;
        }
    }
    return false;
}

int Game::ChannelingHumanCountInRoom(int room) const {
    int count = 0;
    for (const auto& human : humans_) {
        if (!human.IsAssimilated() && human.IsChanneling() && RoomIndex(human.Position()) == room) {
            ++count;
        }
    }
    return count;
}

int Game::ChannelingLitRooms(const std::vector<int>& rooms) const {
    int count = 0;
    for (int room : rooms) {
        if (HumanChannelingInRoom(room) && LampLitInRoom(room)) {
            ++count;
        }
    }
    return count;
}

bool Game::AllMajorTasksComplete() const {
    return tasks_.Complete(TaskId::ThreeLights) &&
           tasks_.Complete(TaskId::DualGate) &&
           tasks_.Complete(TaskId::CoreEscort);
}

void Game::AdvanceTurn() {
    ++turn_;
    ResolveAssimilation();
    UpdateTaskProgress();
    for (auto& human : humans_) {
        if (human.IsCarryingCore()) {
            corePos_ = human.Position();
        }
    }
}

void Game::UpdateTaskProgress() {
    if (!tasks_.Complete(TaskId::ThreeLights) &&
        ChannelingLitRooms({0, 1, 2}) >= AliveHumanCount()) {
        tasks_.Mark(TaskId::ThreeLights);
    }
    if (tasks_.Complete(TaskId::ThreeLights) &&
        !tasks_.Complete(TaskId::DualGate) &&
        HumanChannelingInRoom(3) &&
        HumanChannelingInRoom(2) &&
        LampLitInRoom(3) &&
        LampLitInRoom(2)) {
        tasks_.Mark(TaskId::DualGate);
    }
    if (coreDelivered_ && !tasks_.Complete(TaskId::CoreEscort)) {
        tasks_.Mark(TaskId::CoreEscort);
    }
    if (AllMajorTasksComplete() &&
        !tasks_.Complete(TaskId::FinalPurification) &&
        ChannelingHumanCountInRoom(5) == AliveHumanCount() &&
        LitByAnyHuman(RoomCenter(5))) {
        tasks_.Mark(TaskId::FinalPurification);
    }
}

bool Game::CanChannelCurrentObjective(const HumanPlayer& human) const {
    if (human.IsAssimilated()) {
        return false;
    }
    const int room = RoomIndex(human.Position());
    if (!tasks_.Complete(TaskId::ThreeLights)) {
        return (room == 0 || room == 1 || room == 2) && LampLitInRoom(room);
    }
    if (!tasks_.Complete(TaskId::DualGate)) {
        return (room == 2 || room == 3) && LampLitInRoom(room);
    }
    if (AllMajorTasksComplete() && !tasks_.Complete(TaskId::FinalPurification)) {
        return room == 5 && Distance(human.Position(), RoomCenter(5)) <= 5.0f;
    }
    return false;
}

void Game::HandleChannel(HumanPlayer& human) {
    human.SetChanneling(CanChannelCurrentObjective(human));
}

void Game::HandleCore(HumanPlayer& human) {
    if (!tasks_.Complete(TaskId::DualGate) || tasks_.Complete(TaskId::CoreEscort) || human.IsAssimilated()) {
        return;
    }
    if (!human.IsCarryingCore() && !coreDelivered_ && Distance(human.Position(), corePos_) <= 2.0f) {
        human.SetCarryingCore(true);
        return;
    }
    if (human.IsCarryingCore() && RoomIndex(human.Position()) == 5 &&
        Distance(human.Position(), RoomCenter(5)) <= 3.0f) {
        human.SetCarryingCore(false);
        coreDelivered_ = true;
    }
}

void Game::RescueNearby(int activeHuman) {
    HumanPlayer& rescuer = humans_[static_cast<std::size_t>(activeHuman)];
    if (rescuer.IsAssimilated()) {
        return;
    }
    for (std::size_t i = 0; i < humans_.size(); ++i) {
        if (static_cast<int>(i) == activeHuman || humans_[i].IsAssimilated()) {
            continue;
        }
        if (Distance(rescuer.Position(), humans_[i].Position()) <= 2.0f &&
            rescuer.InFlashlight(humans_[i].Position())) {
            humans_[i].AddAssimilation(-25);
        }
    }
}

void Game::ResolveAssimilation() {
    const bool shadowLit = LitByAnyLight(shadow_.Position());
    if (shadowLit) {
        shadow_.Weaken(2);
        PushShadowFromLight();
    }

    for (auto& human : humans_) {
        if (human.IsAssimilated()) {
            continue;
        }
        if (Distance(human.Position(), shadow_.Position()) <= 1.5f) {
            human.SetChanneling(false);
            human.AddAssimilation((shadowLit || shadow_.WeakTurns() > 0) ? -10 : 15);
        } else if (LitByAnyLight(human.Position())) {
            human.AddAssimilation(-4);
        }
    }
    shadow_.TickWeakness();
}

void Game::PushShadowFromLight() {
    const HumanPlayer* closest = nullptr;
    float best = 10000.0f;
    for (const auto& human : humans_) {
        if (!human.IsAssimilated() && human.InFlashlight(shadow_.Position())) {
            const float d = Distance(human.Position(), shadow_.Position());
            if (d < best) {
                best = d;
                closest = &human;
            }
        }
    }
    if (closest == nullptr) {
        return;
    }
    GridPos delta{shadow_.Position().x - closest->Position().x, shadow_.Position().y - closest->Position().y};
    if (std::abs(delta.x) >= std::abs(delta.y)) {
        delta = {delta.x >= 0 ? 1 : -1, 0};
    } else {
        delta = {0, delta.y >= 0 ? 1 : -1};
    }
    shadow_.MoveThroughWalls(delta);
}

char Game::ObjectiveMarker(GridPos p) const {
    if (!tasks_.Complete(TaskId::ThreeLights)) {
        if (p.x == RoomCenter(0).x && p.y == RoomCenter(0).y - 2) return 'A';
        if (p.x == RoomCenter(1).x && p.y == RoomCenter(1).y - 2) return 'B';
        if (p.x == RoomCenter(2).x && p.y == RoomCenter(2).y - 2) return 'C';
    } else if (!tasks_.Complete(TaskId::DualGate)) {
        if (p.x == RoomCenter(3).x && p.y == RoomCenter(3).y - 2) return 'D';
        if (p.x == RoomCenter(2).x && p.y == RoomCenter(2).y - 2) return 'M';
    } else if (!tasks_.Complete(TaskId::CoreEscort)) {
        if (p.x == RoomCenter(5).x && p.y == RoomCenter(5).y) return 'O';
    } else if (!tasks_.Complete(TaskId::FinalPurification)) {
        if (p.x == RoomCenter(5).x && p.y == RoomCenter(5).y) return 'P';
    }
    return '\0';
}

void Game::DrawObjectiveMarkers() const {
    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            const char marker = ObjectiveMarker({x, y});
            if (marker == '\0') {
                continue;
            }
            const Vector2 center = ToScreen({x, y});
            DrawCircleV(center, 10, Color{92, 188, 210, 255});
            DrawTextCN(TextFormat("%c", marker), static_cast<int>(center.x - 5), static_cast<int>(center.y - 8), 16,
                       Color{12, 25, 32, 255});
        }
    }
}

void Game::DrawCore() const {
    if (tasks_.Complete(TaskId::CoreEscort)) {
        return;
    }
    const Vector2 center = ToScreen(corePos_);
    DrawCircleV(center, 9, Color{255, 244, 148, 255});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 15, Color{255, 245, 180, 220});
}

void Game::DrawDarknessOverlay() const {
    if (shadowVision_) {
        return;
    }
    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            GridPos p{x, y};
            if (!LitByAnyLight(p)) {
                DrawRectangleRec(CellRect(p), Color{4, 5, 9, 126});
            }
        }
    }
}

void Game::DrawSelectionRing() const {
    const Vector2 center = humans_[static_cast<std::size_t>(selectedHuman_)].VisualCenter();
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 20, Color{255, 255, 255, 255});
}

void Game::DrawPanel() const {
    DrawRectangle(kPanelX, 78, 300, 622, Color{26, 28, 38, 255});
    DrawRectangleLines(kPanelX, 78, 300, 622, Color{75, 78, 96, 255});
    DrawTextCN(TextFormat("当前人类：%d", selectedHuman_ + 1), kPanelX + 18, 96, 18, Color{230, 224, 190, 255});
    DrawTextCN(TextFormat("影所在：%s%s", RoomName(RoomIndex(shadow_.Position())).c_str(),
                          shadow_.WeakTurns() > 0 ? "（虚弱）" : ""),
               kPanelX + 18, 122, 15, Color{210, 194, 238, 255});
    const char* mode = "本地演示";
    if (entryMode_ == EntryMode::LightTeam) {
        mode = "光阵营视角";
    } else if (entryMode_ == EntryMode::ShadowTeam) {
        mode = "影阵营视角";
    }
    DrawTextCN(TextFormat("当前模式：%s", mode), kPanelX + 18, 144, 14, Color{190, 194, 208, 255});

    int y = 184;
    for (std::size_t i = 0; i < humans_.size(); ++i) {
        const auto& h = humans_[i];
        DrawTextCN(h.Label(), kPanelX + 18, y, 16, RAYWHITE);
        DrawRectangle(kPanelX + 18, y + 20, 170, 12, Color{48, 48, 58, 255});
        DrawRectangle(kPanelX + 18, y + 20, static_cast<int>(170.0f * h.Assimilation() / 100.0f), 12,
                      StageColor(h.Assimilation()));
        DrawTextCN(AssimilationStage(h.Assimilation(), h.IsAssimilated()), kPanelX + 198, y + 15, 14,
                   Color{218, 220, 230, 255});
        y += 54;
    }

    y += 10;
    DrawTextCN("任务", kPanelX + 18, y, 18, Color{238, 226, 180, 255});
    y += 30;
    for (const auto& task : tasks_.Tasks()) {
        DrawTextCN(task.Complete() ? "[x]" : "[ ]", kPanelX + 18, y, 15,
                   task.Complete() ? Color{106, 206, 140, 255} : Color{210, 210, 210, 255});
        DrawTextCN(task.Name(), kPanelX + 50, y, 15, RAYWHITE);
        DrawTextCN(task.Objective(), kPanelX + 50, y + 18, 13, Color{198, 202, 216, 255});
        y += 52;
    }

    DrawTextCN("目标：完成净化或阻止净化", kPanelX + 18, 664, 14, Color{190, 194, 208, 255});
}

void Game::DrawMainMenu() {
    ui_.DrawScreenBackdrop("余光未熄", "第二版入口流程与完整 UI");
    DrawTextCN("光只能照亮局部，影却掌控全局。", 100, 204, 20,
               Color{226, 218, 196, 255});
    DrawRectangleRounded({840, 118, 450, 420}, 0.04f, 10, Color{22, 24, 34, 222});
    DrawRectangleRoundedLines({840, 118, 450, 420}, 0.04f, 10, Color{104, 108, 132, 255});
    DrawTextCN("第二版目标", 880, 154, 26, Color{246, 232, 180, 255});
    DrawTextCN("完整入口：菜单、阵营选择、准备确认、进入对局", 880, 204, 18, Color{224, 226, 236, 255});
    DrawTextCN("完整 UI：状态、任务、暂停、说明、结算闭环", 880, 242, 18, Color{224, 226, 236, 255});
    DrawTextCN("联机预留：先用本地演示跑通一整局", 880, 280, 18, Color{224, 226, 236, 255});
    DrawTextCN("正式玩法面向联机单局，结算后重开", 880, 318, 18, Color{224, 226, 236, 255});

    const Rectangle start{108, 282, 300, 56};
    const Rectangle help{108, 354, 300, 56};
    const Rectangle quit{108, 426, 300, 56};
    if (ui_.DrawButton(start, "开始游戏") || IsKeyPressed(KEY_ENTER)) {
        screen_ = ScreenState::RoleSelect;
    }
    if (ui_.DrawButton(help, "操作说明")) {
        previousScreen_ = ScreenState::MainMenu;
        screen_ = ScreenState::Help;
    }
    if (ui_.DrawButton(quit, "退出游戏")) {
        CloseWindow();
    }

    DrawTextCN("Enter：开始   H：操作说明", 112, 510, 18, Color{168, 172, 188, 255});
}

void Game::DrawRoleSelect() {
    ui_.DrawScreenBackdrop("对局准备", "选择本局视角并确认双方准备状态");

    const Rectangle local{96, 238, 360, 244};
    const Rectangle light{550, 238, 360, 244};
    const Rectangle shadow{1004, 238, 360, 244};
    const Rectangle cards[3] = {local, light, shadow};
    const char* titles[3] = {"本地联机演示", "光阵营", "影阵营"};
    const char* descA[3] = {"同一台电脑控制双方", "完成任务、点灯、救援", "追击、同化、拖延时间"};
    const char* descB[3] = {"用于课堂展示完整单局", "适合先验证任务闭环", "适合验证对抗压力"};
    const EntryMode modes[3] = {EntryMode::LocalDemo, EntryMode::LightTeam, EntryMode::ShadowTeam};

    for (int i = 0; i < 3; ++i) {
        const bool selected = entryMode_ == modes[i];
        const Color fill = selected ? Color{52, 58, 82, 248} : Color{24, 26, 38, 238};
        const Color border = selected ? Color{238, 220, 148, 255} : Color{104, 108, 132, 255};
        DrawRectangleRounded(cards[i], 0.04f, 10, fill);
        DrawRectangleRoundedLines(cards[i], 0.04f, 10, border);
        DrawTextCN(TextFormat("%d", i + 1), static_cast<int>(cards[i].x + 26), static_cast<int>(cards[i].y + 24),
                   32, selected ? Color{238, 220, 148, 255} : Color{168, 172, 188, 255});
        DrawTextCN(titles[i], static_cast<int>(cards[i].x + 76), static_cast<int>(cards[i].y + 30), 26,
                   Color{246, 232, 180, 255});
        DrawTextCN(descA[i], static_cast<int>(cards[i].x + 32), static_cast<int>(cards[i].y + 92), 18, RAYWHITE);
        DrawTextCN(descB[i], static_cast<int>(cards[i].x + 32), static_cast<int>(cards[i].y + 128), 18,
                   Color{180, 184, 198, 255});
        const Rectangle choose{cards[i].x + 32, cards[i].y + 172, cards[i].width - 64, 46};
        if (ui_.DrawButton(choose, selected ? "已选择" : "选择")) {
            entryMode_ = modes[i];
            lightReady_ = true;
            shadowReady_ = true;
        }
    }

    DrawRectangleRounded({196, 530, 1068, 84}, 0.04f, 10, Color{20, 22, 32, 232});
    DrawRectangleRoundedLines({196, 530, 1068, 84}, 0.04f, 10, Color{104, 108, 132, 255});
    DrawTextCN(lightReady_ ? "光阵营：已准备" : "光阵营：等待准备", 240, 558, 22,
               lightReady_ ? Color{112, 216, 148, 255} : Color{190, 192, 210, 255});
    DrawTextCN(shadowReady_ ? "影阵营：已准备" : "影阵营：等待准备", 620, 558, 22,
               shadowReady_ ? Color{112, 216, 148, 255} : Color{190, 192, 210, 255});

    const Rectangle ready{968, 548, 246, 48};
    if (ui_.DrawButton(ready, lightReady_ && shadowReady_ ? "进入对局" : "切换准备")) {
        if (lightReady_ && shadowReady_) {
            ResetRun();
            screen_ = ScreenState::Playing;
        } else {
            lightReady_ = true;
            shadowReady_ = true;
        }
    }

    DrawTextCN("1/2/3 选择模式   Space 切换准备   Enter 进入对局   Esc 返回主菜单",
               286, 652, 18, Color{168, 172, 188, 255});
}

void Game::DrawPauseMenu() {
    ui_.DrawPausePanel();
    const Rectangle resume{kScreenW / 2.0f - 150, 284, 300, 52};
    const Rectangle restart{kScreenW / 2.0f - 150, 352, 300, 52};
    const Rectangle help{kScreenW / 2.0f - 150, 420, 300, 52};
    const Rectangle menu{kScreenW / 2.0f - 150, 488, 300, 52};
    if (ui_.DrawButton(resume, "继续游戏")) {
        screen_ = ScreenState::Playing;
    }
    if (ui_.DrawButton(restart, "重新开始本局")) {
        ResetRun();
        screen_ = ScreenState::Playing;
    }
    if (ui_.DrawButton(help, "操作说明")) {
        previousScreen_ = ScreenState::Paused;
        screen_ = ScreenState::Help;
    }
    if (ui_.DrawButton(menu, "返回主菜单")) {
        screen_ = ScreenState::MainMenu;
    }
    DrawTextCN("Esc：继续   H：说明", kScreenW / 2 - 78, 558, 16, Color{168, 172, 188, 255});
}

void Game::DrawHelpScreen() const {
    ui_.DrawHelpPanel();
}

void Game::DrawResultScreen() {
    ui_.DrawResultPanel(ResultText(), turn_, CompletedTaskCount(), AssimilatedHumanCount());
    const Rectangle restart{kScreenW / 2.0f - 230, 462, 210, 52};
    const Rectangle role{kScreenW / 2.0f + 20, 462, 210, 52};
    const Rectangle menu{kScreenW / 2.0f - 105, 532, 210, 52};
    if (ui_.DrawButton(restart, "重新开始")) {
        ResetRun();
        screen_ = ScreenState::Playing;
    }
    if (ui_.DrawButton(role, "重新选阵营")) {
        screen_ = ScreenState::RoleSelect;
    }
    if (ui_.DrawButton(menu, "主菜单")) {
        screen_ = ScreenState::MainMenu;
    }
    DrawTextCN("Enter：重新开始   M：主菜单", kScreenW / 2 - 106, 616, 16,
               Color{168, 172, 188, 255});
}
