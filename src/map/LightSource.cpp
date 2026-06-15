#include "map/LightSource.h"

LightSource::LightSource(int room, GridPos pos, bool lit) : room_(room), pos_(pos), lit_(lit) {}

int LightSource::Room() const {
    return room_;
}

GridPos LightSource::Position() const {
    return pos_;
}

bool LightSource::IsLit() const {
    return lit_;
}

void LightSource::TryActivate(GridPos humanPos) {
    if (RoomIndex(humanPos) == room_ && Distance(humanPos, pos_) <= 2.0f) {
        lit_ = true;
    }
}

bool LightSource::Lights(GridPos target) const {
    return lit_ && RoomIndex(target) == room_ && Distance(pos_, target) <= 3.0f;
}

void LightSource::Draw() const {
    const Vector2 center = ToScreen(pos_);
    if (lit_) {
        DrawCircleV(center, 64, Color{250, 220, 110, 36});
        DrawCircleV(center, 9, Color{255, 222, 100, 255});
    } else {
        DrawCircleV(center, 8, Color{92, 88, 102, 255});
    }
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 10, Color{240, 220, 150, 190});
}
