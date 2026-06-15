#pragma once

#include "entities/HumanPlayer.h"
#include "entities/ShadowPlayer.h"
#include "map/LightSource.h"
#include "map/Map.h"
#include "systems/TaskManager.h"
#include "ui/UIManager.h"

#include <vector>

enum class ScreenState { MainMenu, RoleSelect, Playing, Paused, Help, Result };
enum class EntryMode { LocalDemo, LightTeam, ShadowTeam };

class Game {
public:
    Game();

    const std::vector<HumanPlayer>& Humans() const;
    const ShadowPlayer& Shadow() const;
    const std::vector<LightSource>& Lamps() const;
    const TaskManager& Tasks() const;
    int Turn() const;
    int SelectedHuman() const;
    GridPos CorePos() const;
    bool CoreDelivered() const;

    void Update();
    void Draw();
    bool LitByAnyHuman(GridPos target) const;
    bool LitByAnyLight(GridPos target) const;
    bool GameOver() const;

private:
    void ResetRun();
    void UpdateMenu();
    void UpdateRoleSelect();
    void UpdatePlaying();
    void UpdatePaused();
    void UpdateHelp();
    void UpdateResult();
    void UpdateVisuals(float dt);

    int AliveHumanCount() const;
    bool AllHumansAssimilated() const;
    int CompletedTaskCount() const;
    int AssimilatedHumanCount() const;
    std::string ResultText() const;
    bool LampLitInRoom(int room) const;
    bool HumanChannelingInRoom(int room) const;
    int ChannelingHumanCountInRoom(int room) const;
    int ChannelingLitRooms(const std::vector<int>& rooms) const;
    bool AllMajorTasksComplete() const;

    void AdvanceTurn();
    void UpdateTaskProgress();
    bool CanChannelCurrentObjective(const HumanPlayer& human) const;
    void HandleChannel(HumanPlayer& human);
    void HandleCore(HumanPlayer& human);
    void RescueNearby(int activeHuman);
    void ResolveAssimilation();
    void PushShadowFromLight();

    char ObjectiveMarker(GridPos p) const;
    void DrawObjectiveMarkers() const;
    void DrawCore() const;
    void DrawDarknessOverlay() const;
    void DrawSelectionRing() const;
    void DrawPanel() const;
    void DrawMainMenu();
    void DrawRoleSelect();
    void DrawPauseMenu();
    void DrawHelpScreen() const;
    void DrawResultScreen();
    void DrawGameScene() const;

    Map map_;
    std::vector<HumanPlayer> humans_;
    std::vector<LightSource> lamps_;
    TaskManager tasks_;
    ShadowPlayer shadow_;
    GridPos corePos_{RoomCenter(4)};
    int selectedHuman_ = 0;
    int turn_ = 1;
    float resolveTimer_ = 0.0f;
    bool shadowVision_ = false;
    bool coreDelivered_ = false;
    EntryMode entryMode_ = EntryMode::LocalDemo;
    bool lightReady_ = false;
    bool shadowReady_ = false;
    ScreenState screen_ = ScreenState::MainMenu;
    ScreenState previousScreen_ = ScreenState::MainMenu;
    UIManager ui_;
    std::string toast_;
};
