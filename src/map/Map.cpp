#include "map/Map.h"

#include "core/Text.h"

bool Map::IsWall(GridPos p) const {
    if (p.x < 0 || p.x >= kMapW || p.y < 0 || p.y >= kMapH) {
        return true;
    }

    const bool vertical = p.x > 0 && p.x % kRoomW == 0;
    const bool horizontal = p.y > 0 && p.y % kRoomH == 0;
    if (!vertical && !horizontal) {
        return false;
    }

    if (vertical) {
        const int localY = p.y % kRoomH;
        return localY < 2 || localY > 4;
    }
    if (horizontal) {
        const int localX = p.x % kRoomW;
        return localX < 6 || localX > 9;
    }
    return true;
}

bool Map::CanHumanMove(GridPos p) const {
    return !IsWall(p);
}

void Map::DrawBase() const {
    DrawRectangle(kMapOffsetX - 8, kMapOffsetY - 8, kMapW * kTile + 16, kMapH * kTile + 16,
                  Color{20, 21, 30, 255});

    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            const GridPos p{x, y};
            const Rectangle r = CellRect(p);
            const int room = RoomIndex(p);
            const Color floor = (room % 2 == 0) ? Color{44, 42, 54, 255} : Color{39, 45, 56, 255};
            DrawRectangleRec(r, IsWall(p) ? Color{84, 76, 92, 255} : floor);
        }
    }

    for (int room = 0; room < 6; ++room) {
        const int col = room % kCols;
        const int row = room / kCols;
        DrawTextCN(RoomName(room),
                   kMapOffsetX + col * kRoomW * kTile + 10,
                   kMapOffsetY + row * kRoomH * kTile + 8,
                   13,
                   Color{178, 176, 192, 255});
    }
}
