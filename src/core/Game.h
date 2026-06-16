#pragma once

#include "entities/HumanPlayer.h"
#include "entities/ShadowPlayer.h"
#include "map/LightSource.h"
#include "map/Map.h"
#include "systems/TaskManager.h"
#include "ui/UIManager.h"

#include <string>
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
    bool RunSecondVersionSelfTest(std::string& error);

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
    bool LampDamagedInRoom(int room) const;
    LightSource* NearestLamp(GridPos pos);
    const LightSource* NearestLamp(GridPos pos) const;
    bool AllHumansInRoom(int room) const;

    void AdvanceTurn();
    void UpdateTaskProgress();
    void HandleInteract(HumanPlayer& human);
    void HandleRoleSkill(HumanPlayer& human);
    void HandleShadowAction();
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
    float observerRevealTimer_ = 0.0f;
    bool shadowVision_ = false;
    bool coreDelivered_ = false;
    bool gateOpen_ = false;
    bool guardedGateOpen_ = false;
    bool lampCalibrated_ = false;
    bool routeObserved_ = false;
    bool routeOpened_ = false;
    bool trueCoreIdentified_ = false;
    bool sealDeviceStarted_ = false;
    EntryMode entryMode_ = EntryMode::LocalDemo;
    bool lightReady_ = false;
    bool shadowReady_ = false;
    ScreenState screen_ = ScreenState::MainMenu;
    ScreenState previousScreen_ = ScreenState::MainMenu;
    UIManager ui_;
    std::string toast_;
};
