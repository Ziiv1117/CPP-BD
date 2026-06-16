#pragma once

#include "entities/Character.h"

class ShadowPlayer : public Character {
public:
    ShadowPlayer();

    int WeakTurns() const;
    bool IsStunned() const;
    void Weaken(int turns);
    void Stun(float seconds);
    void TickWeakness();
    void UpdateTimers(float dt);
    void MoveThroughWalls(GridPos delta);
    bool MoveContinuous(Vector2 dir, float dt, float speed);
    void TeleportToRoom(int room);
    void Draw() const override;

private:
    int weakTurns_ = 0;
    float stunTimer_ = 0.0f;
};
