#pragma once

#include "core/Common.h"

class Map {
public:
    bool IsWall(GridPos p) const;
    bool CanHumanMove(GridPos p) const;
    void DrawBase() const;
};
