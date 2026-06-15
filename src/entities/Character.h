#pragma once

#include "entities/Entity.h"
#include "map/Map.h"

class Character : public Entity {
public:
    Character(std::string label, GridPos pos, Color color);

    GridPos Direction() const;
    bool IsAssimilated() const;
    bool IsChanneling() const;
    bool IsCarryingCore() const;
    int Assimilation() const;

    void SetDirection(GridPos dir);
    void SetChanneling(bool value);
    void SetCarryingCore(bool value);
    void SetAssimilated(bool value);
    void AddAssimilation(int delta);
    void Move(GridPos delta, const Map& map);
    bool MoveContinuous(Vector2 dir, float dt, float speed, const Map& map);

protected:
    GridPos dir_{1, 0};
    int assimilation_ = 0;
    bool assimilated_ = false;
    bool channeling_ = false;
    bool carryingCore_ = false;
};
