#include "map/LightSource.h"

LightSource::LightSource(int room, GridPos pos, bool lit)
    : room_(room), pos_(pos), state_(lit ? LightState::Lit : LightState::Unlit) {}

int LightSource::Room() const {
    return room_;
}

GridPos LightSource::Position() const {
    return pos_;
}

bool LightSource::IsLit() const {
    return state_ == LightState::Lit;
}

LightState LightSource::State() const {
    return state_;
}

bool LightSource::IsDamaged() const {
    return state_ == LightState::Damaged;
}

const char* LightSource::StateName() const {
    switch (state_) {
        case LightState::Unlit:
            return "未点亮";
        case LightState::Lit:
            return "已点亮";
        case LightState::Damaged:
            return "损坏";
    }
    return "未知";
}

bool LightSource::TryActivate(GridPos humanPos) {
    if (RoomIndex(humanPos) == room_ && Distance(humanPos, pos_) <= 2.0f) {
        if (state_ == LightState::Unlit) {
            state_ = LightState::Lit;
            return true;
        }
    }
    return false;
}

bool LightSource::TryRepair(GridPos humanPos) {
    if (RoomIndex(humanPos) == room_ && Distance(humanPos, pos_) <= 2.0f && state_ == LightState::Damaged) {
        state_ = LightState::Lit;
        return true;
    }
    return false;
}

bool LightSource::TryDamage(GridPos shadowPos) {
    if (RoomIndex(shadowPos) == room_ && Distance(shadowPos, pos_) <= 2.0f && state_ == LightState::Lit) {
        state_ = LightState::Damaged;
        return true;
    }
    return false;
}

bool LightSource::Lights(GridPos target) const {
    return state_ == LightState::Lit && RoomIndex(target) == room_ && Distance(pos_, target) <= 3.0f;
}

void LightSource::Draw() const {
    const Vector2 center = ToScreen(pos_);
    if (state_ == LightState::Lit) {
        DrawCircleV(center, 64, Color{250, 220, 110, 36});
        DrawCircleV(center, 9, Color{255, 222, 100, 255});
    } else if (state_ == LightState::Damaged) {
        DrawCircleV(center, 11, Color{136, 54, 82, 255});
        DrawLine(static_cast<int>(center.x - 8), static_cast<int>(center.y - 8),
                 static_cast<int>(center.x + 8), static_cast<int>(center.y + 8), Color{255, 160, 170, 255});
        DrawLine(static_cast<int>(center.x + 8), static_cast<int>(center.y - 8),
                 static_cast<int>(center.x - 8), static_cast<int>(center.y + 8), Color{255, 160, 170, 255});
    } else {
        DrawCircleV(center, 8, Color{92, 88, 102, 255});
    }
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 10, Color{240, 220, 150, 190});
}
