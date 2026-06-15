#pragma once

#include "entities/Character.h"

class HumanPlayer : public Character {
public:
    HumanPlayer(std::string label, GridPos pos, Color color);

    bool InFlashlight(GridPos target) const;
    void DrawFlashlight() const;
    void Draw() const override;
};
