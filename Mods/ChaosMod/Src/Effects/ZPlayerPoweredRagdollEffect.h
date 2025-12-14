#pragma once

#include "IChaosEffect.h"

class ZPlayerPoweredRagdollEffect : public IChaosEffect
{
    void Start() override;
    void Stop() override;

    bool Available() override;

    std::string GetDisplayName() override
    {
        return "Sleepwalk Mode";
    }
};