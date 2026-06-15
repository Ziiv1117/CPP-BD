#include "entities/Entity.h"

#include <algorithm>
#include <cmath>
#include <utility>

Entity::Entity(std::string label, GridPos pos, Color color)
    : label_(std::move(label)),
      pos_(pos),
      color_(color),
      worldPos_{static_cast<float>(pos.x), static_cast<float>(pos.y)},
      visualCenter_(ToScreen(pos)) {}

GridPos Entity::Position() const {
    return pos_;
}

void Entity::SetPosition(GridPos pos) {
    pos_ = pos;
    worldPos_ = {static_cast<float>(pos.x), static_cast<float>(pos.y)};
    SnapVisualToGrid();
}

const std::string& Entity::Label() const {
    return label_;
}

Vector2 Entity::VisualCenter() const {
    return visualCenter_;
}

void Entity::UpdateVisual(float dt) {
    (void)dt;
    visualCenter_ = {
        static_cast<float>(kMapOffsetX) + worldPos_.x * kTile + kTile / 2.0f,
        static_cast<float>(kMapOffsetY) + worldPos_.y * kTile + kTile / 2.0f};
}

void Entity::SnapVisualToGrid() {
    visualCenter_ = ToScreen(pos_);
}

void Entity::SetWorldPosition(Vector2 pos) {
    worldPos_ = {
        std::max(0.0f, std::min(static_cast<float>(kMapW - 1), pos.x)),
        std::max(0.0f, std::min(static_cast<float>(kMapH - 1), pos.y))};
    pos_ = {
        ClampInt(static_cast<int>(std::round(worldPos_.x)), 0, kMapW - 1),
        ClampInt(static_cast<int>(std::round(worldPos_.y)), 0, kMapH - 1)};
    UpdateVisual(0.0f);
}

void Entity::Draw() const {
    const Vector2 center = VisualCenter();
    DrawCircleV(center, 9, color_);
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 10, Color{245, 238, 218, 200});
}
