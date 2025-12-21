#pragma once
#include "Base/ZPoisonAOEDamageEffectBase.h"
#include "Base/ZSoundFXEffectBase.h"

class ZPlayerFlatulenceEffect : public ZPoisonAOEDamageEffectBase, public ZSoundFXEffectBase
{
    void OnEngineInitialized() override;
    void OnDrawDebugUI() override;
    bool Available() override;
    void Start() override;

    std::string GetDisplayName() override
    {
        return "Flatulence";
    }
};