#pragma once

#include "core/Common.h"

class LightSource {
public:
    LightSource(int room, GridPos pos, bool lit);

    int Room() const;
    GridPos Position() const;
    bool IsLit() const;

    void TryActivate(GridPos humanPos);
    bool Lights(GridPos target) const;
    void Draw() const;

private:
    int room_ = 0;
    GridPos pos_;
    bool lit_ = false;
};
