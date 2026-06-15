#include "entities/Character.h"

#include <cmath>
#include <utility>

Character::Character(std::string label, GridPos pos, Color color)
    : Entity(std::move(label), pos, color) {}

GridPos Character::Direction() const {
    return dir_;
}

bool Character::IsAssimilated() const {
    return assimilated_;
}

bool Character::IsChanneling() const {
    return channeling_;
}

bool Character::IsCarryingCore() const {
    return carryingCore_;
}

int Character::Assimilation() const {
    return assimilation_;
}

void Character::SetDirection(GridPos dir) {
    dir_ = dir;
}

void Character::SetChanneling(bool value) {
    channeling_ = value;
}

void Character::SetCarryingCore(bool value) {
    carryingCore_ = value;
}

void Character::SetAssimilated(bool value) {
    assimilated_ = value;
}

void Character::AddAssimilation(int delta) {
    assimilation_ = ClampInt(assimilation_ + delta, 0, 100);
    if (assimilation_ >= 100) {
        assimilated_ = true;
        channeling_ = false;
        carryingCore_ = false;
    }
}

void Character::Move(GridPos delta, const Map& map) {
    if (assimilated_) {
        return;
    }
    GridPos target{pos_.x + delta.x, pos_.y + delta.y};
    if (map.CanHumanMove(target)) {
        pos_ = target;
        channeling_ = false;
    }
    if (delta.x != 0 || delta.y != 0) {
        dir_ = delta;
    }
}

bool Character::MoveContinuous(Vector2 dir, float dt, float speed, const Map& map) {
    if (assimilated_) {
        return false;
    }

    const float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length <= 0.001f) {
        return false;
    }
    dir.x /= length;
    dir.y /= length;

    bool moved = false;
    Vector2 next = worldPos_;
    const float step = speed * dt;

    next.x += dir.x * step;
    GridPos probeX{
        ClampInt(static_cast<int>(std::round(next.x)), 0, kMapW - 1),
        ClampInt(static_cast<int>(std::round(worldPos_.y)), 0, kMapH - 1)};
    if (map.CanHumanMove(probeX)) {
        SetWorldPosition({next.x, worldPos_.y});
        moved = true;
    }

    next = worldPos_;
    next.y += dir.y * step;
    GridPos probeY{
        ClampInt(static_cast<int>(std::round(worldPos_.x)), 0, kMapW - 1),
        ClampInt(static_cast<int>(std::round(next.y)), 0, kMapH - 1)};
    if (map.CanHumanMove(probeY)) {
        SetWorldPosition({worldPos_.x, next.y});
        moved = true;
    }

    if (moved) {
        channeling_ = false;
        if (std::abs(dir.x) >= std::abs(dir.y)) {
            dir_ = {dir.x >= 0.0f ? 1 : -1, 0};
        } else {
            dir_ = {0, dir.y >= 0.0f ? 1 : -1};
        }
    }
    return moved;
}
