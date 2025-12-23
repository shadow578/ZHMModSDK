#pragma once
#include "Base/ZPoisonAOEDamageEffectBase.h"
#include "Base/ZSoundFXEffectBase.h"

class ZPlayerFlatulenceEffect : public ZPoisonAOEDamageEffectBase, public ZSoundFXEffectBase
{
    void LoadResources() override;
    void OnDrawDebugUI() override;
    bool Available() override;
    void Start() override;

    std::string GetDisplayName() override
    {
        return "Flatulence";
    }

    EDuration GetDuration() override
    {
        return EDuration::OneShot;
    }
};