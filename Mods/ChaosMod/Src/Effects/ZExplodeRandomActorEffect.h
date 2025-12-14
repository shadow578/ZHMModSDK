#pragma once

#include "IChaosEffect.h"

#include "Glacier/ZMath.h"

class ZExplodeRandomActorEffect : public IChaosEffect
{
    void OnEngineInitialized() override;
    void OnDrawDebugUI() override;

    void Start() override;

    std::string GetDisplayName() override
    {
        return "Random Actor Goes Boom";
    }

private:
    ZActor* m_pLastTarget = nullptr;
    SMatrix m_LastTargetPos;
};