#pragma once

#include "IChaosEffect.h"

class ZSlowTimeScaleEffect : public IChaosEffect
{
    void Start() override;
    void Stop() override;

    std::string GetDisplayName() override;

private:
    float32 m_fPreviousTimeScale = 1.0f;
};