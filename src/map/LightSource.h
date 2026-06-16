#pragma once

#include "core/Common.h"

enum class LightState { Unlit, Lit, Damaged };

class LightSource {
public:
    LightSource(int room, GridPos pos, bool lit);

    int Room() const;
    GridPos Position() const;
    LightState State() const;
    bool IsLit() const;
    bool IsDamaged() const;
    const char* StateName() const;

    bool TryActivate(GridPos humanPos);
    bool TryRepair(GridPos humanPos);
    bool TryDamage(GridPos shadowPos);
    bool Lights(GridPos target) const;
    void Draw() const;

private:
    int room_ = 0;
    GridPos pos_;
    LightState state_ = LightState::Unlit;
};
