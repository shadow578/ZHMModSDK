#pragma once

#include "IChaosEffect.h"

class ZPlayerRagdollImpulseEffect : public IChaosEffect
{
    void Start() override;
    void Stop() override;

    bool Available() override;

    std::string GetDisplayName() override
    {
        return "Super Stunt";
    }
};