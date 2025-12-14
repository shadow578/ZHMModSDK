#pragma once

#include "IChaosEffect.h"

#include "Glacier/ZMath.h"

class ZSwapPlayerWithActorEffect : public IChaosEffect
{
    void OnDrawDebugUI() override;

    void Start() override;

private:
    SMatrix m_PreviousPlayerPosition;
};