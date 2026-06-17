#include "map/Map.h"

#include "core/Text.h"

namespace {

constexpr int kAssetTile = 16;
constexpr int kAssetCols = 12;

Texture2D gDungeonTiles{};
bool gTriedLoadDungeonTiles = false;

bool HasDungeonTiles() {
    if (!gTriedLoadDungeonTiles) {
        gTriedLoadDungeonTiles = true;
        const char* candidates[] = {
            "/assets/maps/kenney_tiny_dungeon/Tilemap/tilemap_packed.png",
            "assets/maps/kenney_tiny_dungeon/Tilemap/tilemap_packed.png",
            "../assets/maps/kenney_tiny_dungeon/Tilemap/tilemap_packed.png"};
        for (const char* path : candidates) {
            if (FileExists(path)) {
                gDungeonTiles = LoadTexture(path);
                if (gDungeonTiles.id != 0) {
                    SetTextureFilter(gDungeonTiles, TEXTURE_FILTER_POINT);
                    break;
                }
            }
        }
    }
    return gDungeonTiles.id != 0;
}

Rectangle TileSource(int tileIndex) {
    const int col = tileIndex % kAssetCols;
    const int row = tileIndex / kAssetCols;
    return {
        static_cast<float>(col * kAssetTile),
        static_cast<float>(row * kAssetTile),
        static_cast<float>(kAssetTile),
        static_cast<float>(kAssetTile)};
}

void DrawAssetTile(int tileIndex, Rectangle dest, Color tint) {
    DrawTexturePro(gDungeonTiles, TileSource(tileIndex), dest, {0.0f, 0.0f}, 0.0f, tint);
}

Color RoomTint(int room, bool wall) {
    static const Color floorTints[] = {
        Color{120, 112, 132, 255},
        Color{108, 118, 142, 255},
        Color{130, 130, 154, 255},
        Color{116, 108, 126, 255},
        Color{126, 118, 102, 255},
        Color{136, 118, 148, 255}};
    static const Color wallTints[] = {
        Color{94, 88, 108, 255},
        Color{86, 94, 116, 255},
        Color{102, 102, 126, 255},
        Color{92, 84, 104, 255},
        Color{104, 96, 84, 255},
        Color{108, 92, 124, 255}};
    return wall ? wallTints[room % 6] : floorTints[room % 6];
}

int FloorTileFor(GridPos p) {
    static const int roomFloors[] = {0, 2, 14, 24, 25, 26};
    const int room = RoomIndex(p);
    const int variant = (p.x * 3 + p.y * 5 + room) % 11;
    if (variant == 0) {
        return 3;
    }
    if (variant == 1) {
        return 12;
    }
    return roomFloors[room % 6];
}

int WallTileFor(GridPos p) {
    const bool vertical = p.x > 0 && p.x % kRoomW == 0;
    const bool horizontal = p.y > 0 && p.y % kRoomH == 0;
    if (vertical && horizontal) {
        return 39;
    }
    if (vertical) {
        return 15;
    }
    if (horizontal) {
        return 27;
    }
    return 1;
}

void DrawRoomDecor(int room) {
    const GridPos center = RoomCenter(room);
    const int decorByRoom[] = {49, 56, 55, 32, 45, 41};
    const int accentByRoom[] = {36, 31, 44, 33, 57, 54};

    DrawAssetTile(decorByRoom[room], CellRect({center.x - 1, center.y}), WHITE);
    DrawAssetTile(accentByRoom[room], CellRect({center.x + 1, center.y}), Color{218, 210, 228, 235});

    if (room == 5) {
        DrawAssetTile(35, CellRect({center.x, center.y - 1}), Color{255, 235, 172, 255});
        DrawCircleLines(static_cast<int>(ToScreen(center).x), static_cast<int>(ToScreen(center).y), 34,
                        Color{255, 225, 120, 120});
    }
}

}  // namespace

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

    const bool useAssets = HasDungeonTiles();
    for (int y = 0; y < kMapH; ++y) {
        for (int x = 0; x < kMapW; ++x) {
            const GridPos p{x, y};
            const Rectangle r = CellRect(p);
            const int room = RoomIndex(p);
            const Color floor = (room % 2 == 0) ? Color{44, 42, 54, 255} : Color{39, 45, 56, 255};
            if (useAssets) {
                const bool wall = IsWall(p);
                DrawAssetTile(wall ? WallTileFor(p) : FloorTileFor(p), r, RoomTint(room, wall));
                if (wall) {
                    DrawRectangleRec(r, Color{12, 10, 18, 58});
                }
            } else {
                DrawRectangleRec(r, IsWall(p) ? Color{84, 76, 92, 255} : floor);
            }
        }
    }

    for (int room = 0; room < 6; ++room) {
        const int col = room % kCols;
        const int row = room / kCols;
        if (useAssets) {
            DrawRoomDecor(room);
        }
        DrawTextCN(RoomName(room),
                   kMapOffsetX + col * kRoomW * kTile + 10,
                   kMapOffsetY + row * kRoomH * kTile + 8,
                   13,
                   Color{178, 176, 192, 255});
    }
}
