#include "entities/ShadowPlayer.h"

#include "core/Text.h"

#include <algorithm>
#include <cmath>

ShadowPlayer::ShadowPlayer() : Character("影", {kMapW - 4, kMapH - 4}, Color{122, 65, 178, 255}) {}

int ShadowPlayer::WeakTurns() const {
    return weakTurns_;
}

void ShadowPlayer::Weaken(int turns) {
    weakTurns_ = std::max(weakTurns_, turns);
}

void ShadowPlayer::TickWeakness() {
    if (weakTurns_ > 0) {
        --weakTurns_;
    }
}

void ShadowPlayer::MoveThroughWalls(GridPos delta) {
    SetWorldPosition({
        static_cast<float>(ClampInt(pos_.x + delta.x, 0, kMapW - 1)),
        static_cast<float>(ClampInt(pos_.y + delta.y, 0, kMapH - 1))});
}

bool ShadowPlayer::MoveContinuous(Vector2 dir, float dt, float speed) {
    const float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length <= 0.001f) {
        return false;
    }
    dir.x /= length;
    dir.y /= length;
    SetWorldPosition({
        worldPos_.x + dir.x * speed * dt,
        worldPos_.y + dir.y * speed * dt});
    return true;
}

void ShadowPlayer::TeleportToRoom(int room) {
    if (weakTurns_ <= 0 && room >= 0 && room < 6) {
        pos_ = RoomCenter(room);
        SnapVisualToGrid();
    }
}

void ShadowPlayer::Draw() const {
    const Vector2 center = VisualCenter();
    DrawCircleV(center, weakTurns_ > 0 ? 13.0f : 11.0f, color_);
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), weakTurns_ > 0 ? 17 : 13,
                    weakTurns_ > 0 ? Color{255, 230, 110, 255} : Color{178, 120, 220, 255});
    DrawTextCN("影", static_cast<int>(center.x - 8), static_cast<int>(center.y - 9), 16, RAYWHITE);
}
