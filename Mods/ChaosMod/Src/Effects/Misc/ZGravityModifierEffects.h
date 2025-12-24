#pragma once
#include "Effects/Base/ZGravityModifierEffectBase.h"

class ZLowGravityEffect : public ZGravityModifierEffectBase
{
public:
    ZLowGravityEffect() : ZGravityModifierEffectBase(SVector3(0.f, 0.f, -1.625f)) {}

    std::string GetDisplayName() override
    {
        return "Moon Gravity";
    }
};

class ZHighGravityEffect : public ZGravityModifierEffectBase
{
public:
    ZHighGravityEffect() : ZGravityModifierEffectBase(SVector3(0.f, 0.f, -24.79f)) {}

    std::string GetDisplayName() override
    {
        return "Jupiter Gravity";
    }
};

class ZInverseGravityEffect : public ZGravityModifierEffectBase
{
public:
    ZInverseGravityEffect() : ZGravityModifierEffectBase(SVector3(0.f, 0.f, 9.81f)) {}

    std::string GetDisplayName() override
    {
        return "Inverse Gravity";
    }
};



