#pragma once
#include "IChaosEffect.h"

#include <Glacier/ZMath.h>

class ZGravityModifierEffectBase : public IChaosEffect
{
public:
    ZGravityModifierEffectBase(const SVector3 p_vGravity) : m_vGravity(p_vGravity) {}
    void Start() override;
    void Stop() override;

private:
    const SVector3 m_vGravity;

    void SetGravity(const SVector3 p_vGravity);
};
