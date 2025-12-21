#pragma once

#include "Base/ZExplosionEffectBase.h"

class ZExplodeRandomActorEffect : public ZExplosionEffectBase
{
public:
    void Start() override;
    void OnDrawDebugUI() override;

    std::string GetDisplayName() override
    {
        return "Random Actor Goes Boom";
    }

private:
    bool m_bPreloadStarted = false;
    ZActor* m_pLastTarget = nullptr;
    SMatrix m_LastTargetPos;
};