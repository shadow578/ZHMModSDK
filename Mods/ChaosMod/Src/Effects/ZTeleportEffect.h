#pragma once

#include "IChaosEffect.h"

#include "Glacier/ZMath.h"

class ZTeleportEffect : public IChaosEffect
{
    void OnDrawDebugUI() override;
    void Start() override;

private:
    enum class ETeleportDirection
    {
        Up,
        Forward,
        NUMBER_OF_DIRECTIONS
    };

    void DoTeleport(const ETeleportDirection p_eDirection);
};