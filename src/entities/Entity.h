#pragma once

#include "core/Common.h"

#include <string>

class Entity {
public:
    Entity(std::string label, GridPos pos, Color color);
    virtual ~Entity() = default;

    GridPos Position() const;
    void SetPosition(GridPos pos);
    const std::string& Label() const;

    Vector2 VisualCenter() const;
    void UpdateVisual(float dt);
    virtual void Draw() const;

protected:
    void SnapVisualToGrid();
    void SetWorldPosition(Vector2 pos);

    std::string label_;
    GridPos pos_;
    Color color_;
    Vector2 worldPos_{};
    Vector2 visualCenter_{};
};
