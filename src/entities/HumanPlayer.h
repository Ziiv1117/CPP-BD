#pragma once

#include "entities/Character.h"

enum class HumanRole { Keeper, Engineer, Observer };

class HumanPlayer : public Character {
public:
    HumanPlayer(std::string label, HumanRole role, GridPos pos, Color color);

    HumanRole Role() const;
    const char* RoleName() const;
    bool InFlashlight(GridPos target) const;
    void DrawFlashlight() const;
    void Draw() const override;

private:
    HumanRole role_ = HumanRole::Keeper;
};
