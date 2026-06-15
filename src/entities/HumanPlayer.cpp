#include "entities/HumanPlayer.h"

#include "core/Text.h"

#include <cmath>
#include <utility>

HumanPlayer::HumanPlayer(std::string label, GridPos pos, Color color)
    : Character(std::move(label), pos, color) {}

bool HumanPlayer::InFlashlight(GridPos target) const {
    if (assimilated_ || RoomIndex(pos_) != RoomIndex(target)) {
        return false;
    }
    const int dx = target.x - pos_.x;
    const int dy = target.y - pos_.y;
    const int forward = dx * dir_.x + dy * dir_.y;
    if (forward <= 0 || forward > 7) {
        return false;
    }
    const int side = std::abs(dx * dir_.y - dy * dir_.x);
    return side <= forward / 2 + 1;
}

void HumanPlayer::DrawFlashlight() const {
    if (assimilated_) {
        return;
    }
    const Vector2 origin = VisualCenter();
    const float length = 7.0f * kTile;
    const float width = 3.4f * kTile;
    Vector2 forward{static_cast<float>(dir_.x), static_cast<float>(dir_.y)};
    Vector2 side{-forward.y, forward.x};
    Vector2 tip{origin.x + forward.x * length, origin.y + forward.y * length};
    Vector2 left{tip.x + side.x * width, tip.y + side.y * width};
    Vector2 right{tip.x - side.x * width, tip.y - side.y * width};
    DrawTriangle(origin, left, right, Color{245, 220, 96, 70});
}

void HumanPlayer::Draw() const {
    const Vector2 center = VisualCenter();
    DrawCircleV(center, channeling_ ? 14.0f : 11.0f, color_);
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), channeling_ ? 16 : 13,
                    channeling_ ? Color{255, 230, 110, 255} : Color{245, 238, 218, 210});
    DrawTextCN(label_, static_cast<int>(center.x - 28), static_cast<int>(center.y - 29), 12, RAYWHITE);
    if (carryingCore_) {
        DrawCircle(static_cast<int>(center.x + 13), static_cast<int>(center.y - 13), 5, Color{255, 218, 72, 255});
    }
}
