#pragma once

#include "raylib.h"

#include <string>

constexpr int kRoomW = 16;
constexpr int kRoomH = 7;
constexpr int kCols = 3;
constexpr int kRows = 2;
constexpr int kMapW = kRoomW * kCols;
constexpr int kMapH = kRoomH * kRows;
constexpr int kTile = 24;
constexpr int kMapOffsetX = 36;
constexpr int kMapOffsetY = 78;
constexpr int kPanelX = kMapOffsetX + kMapW * kTile + 28;
constexpr int kScreenW = 1460;
constexpr int kScreenH = 760;

struct GridPos {
    int x = 0;
    int y = 0;
};

int ClampInt(int value, int low, int high);
float Distance(GridPos a, GridPos b);
Vector2 ToScreen(GridPos p);
Rectangle CellRect(GridPos p);
int RoomIndex(GridPos p);
GridPos RoomCenter(int room);
std::string RoomName(int index);
Color StageColor(int assimilation);
std::string AssimilationStage(int assimilation, bool assimilated);
void SetRenderScale(float scale);
float RenderScale();
Vector2 LogicalMousePosition();
