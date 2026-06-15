#pragma once

#include "entities/Character.h"

class ShadowPlayer : public Character {
public:
    ShadowPlayer();

    int WeakTurns() const;
    void Weaken(int turns);
    void TickWeakness();
    void MoveThroughWalls(GridPos delta);
    bool MoveContinuous(Vector2 dir, float dt, float speed);
    void TeleportToRoom(int room);
    void Draw() const override;

private:
    int weakTurns_ = 0;
};
