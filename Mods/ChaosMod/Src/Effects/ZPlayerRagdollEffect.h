#pragma once

#include "IChaosEffect.h"

class ZPlayerRagdollEffect : public IChaosEffect
{
    void Start() override;
    void Stop() override;

    bool Available() override;

    std::string GetDisplayName() override
    {
        return "Ragdoll Player";
    }
};