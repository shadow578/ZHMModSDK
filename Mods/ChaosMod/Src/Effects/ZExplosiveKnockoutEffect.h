#pragma once
#include "Base/ZActorWellbeignChangeEffectBase.h"
#include "Base/ZExplosionEffectBase.h"

class ZExplosiveKnockoutEffect : public ZActorWellbeignChangeEffectBase, public ZExplosionEffectBase
{
public:
    std::string GetDisplayName() override
    {
        return "Explosive Knockout";
    }

protected:
    void OnActorWellbeingChanged(ZActor* p_pActor, const SActorState& p_OldState, const SActorState& p_NewState) override;

private:
    void SpawnExplosionAtActor(ZActor* p_pActor, const float32 p_fStrength);
};
