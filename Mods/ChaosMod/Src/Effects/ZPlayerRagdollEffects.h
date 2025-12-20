#pragma once
#include "Base/ZPlayerRagdollEffectBase.h"

class ZPlayerSimpleRagdollEffect : public ZPlayerRagdollEffectBase
{
public:
    ZPlayerSimpleRagdollEffect() : ZPlayerRagdollEffectBase(false) {}

    std::string GetDisplayName() override
    {
        return "Ragdoll Player";
    }
};

class ZPlayerPoweredRagdollEffect : public ZPlayerRagdollEffectBase
{
public:
    ZPlayerPoweredRagdollEffect() : ZPlayerRagdollEffectBase(true) {}

    std::string GetDisplayName() override
    {
        return "Floppy 47";
    }
};

class ZPlayerRagdollWithImpulseEffect : public ZPlayerRagdollEffectBase
{
public:
    ZPlayerRagdollWithImpulseEffect() : ZPlayerRagdollEffectBase(false) {}

    void Start() override;

    std::string GetDisplayName() override
    {
        return "Super Stunt";
    }
};
