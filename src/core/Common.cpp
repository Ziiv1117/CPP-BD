#include "core/Common.h"

#include <algorithm>
#include <array>
#include <cmath>

int ClampInt(int value, int low, int high) {
    return std::max(low, std::min(high, value));
}

float Distance(GridPos a, GridPos b) {
    const int dx = a.x - b.x;
    const int dy = a.y - b.y;
    return std::sqrt(static_cast<float>(dx * dx + dy * dy));
}

Vector2 ToScreen(GridPos p) {
    return {
        static_cast<float>(kMapOffsetX + p.x * kTile + kTile / 2),
        static_cast<float>(kMapOffsetY + p.y * kTile + kTile / 2)};
}

Rectangle CellRect(GridPos p) {
    return {
        static_cast<float>(kMapOffsetX + p.x * kTile),
        static_cast<float>(kMapOffsetY + p.y * kTile),
        static_cast<float>(kTile),
        static_cast<float>(kTile)};
}

int RoomIndex(GridPos p) {
    const int col = ClampInt(p.x / kRoomW, 0, kCols - 1);
    const int row = ClampInt(p.y / kRoomH, 0, kRows - 1);
    return row * kCols + col;
}

GridPos RoomCenter(int room) {
    const int col = room % kCols;
    const int row = room / kCols;
    return {col * kRoomW + kRoomW / 2, row * kRoomH + kRoomH / 2};
}

std::string RoomName(int index) {
    static const std::array<std::string, 6> names = {
        "旧钟楼", "上层廊桥", "镜光室",
        "电闸房", "中庭光井", "封印大厅"};
    return names[static_cast<std::size_t>(index)];
}

Color StageColor(int assimilation) {
    if (assimilation >= 70) {
        return Color{204, 78, 150, 255};
    }
    if (assimilation >= 35) {
        return Color{230, 142, 62, 255};
    }
    return Color{74, 168, 116, 255};
}

std::string AssimilationStage(int assimilation, bool assimilated) {
    if (assimilated || assimilation >= 100) {
        return "完全同化";
    }
    if (assimilation >= 70) {
        return "半影";
    }
    if (assimilation >= 35) {
        return "受蚀";
    }
    return "正常";
}
