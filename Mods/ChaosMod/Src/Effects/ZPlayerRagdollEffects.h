#pragma once
#include "Base/ZPlayerRagdollEffectBase.h"

#include "Helpers/ZVelocityTracker.h"

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
    void Stop() override;
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining) override;
    void OnDrawDebugUI() override;

    std::string GetDisplayName() override
    {
        return "Super Stunt";
    }

private:
    bool m_bActive = false;
    ZVelocityTracker m_VelocityTracker;
};
