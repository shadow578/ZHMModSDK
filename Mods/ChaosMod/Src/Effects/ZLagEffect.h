#pragma once

#include "IChaosEffect.h"

class ZLagEffect : public IChaosEffect
{
    void Start() override;
    void Stop() override;
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining) override;

    std::string GetDisplayName() override
    {
        return "Console Experience";
    }

private:
    bool m_bEnabled = false;
};