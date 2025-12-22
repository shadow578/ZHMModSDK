#pragma once
#include "IChaosEffect.h"

// TODO wrong name
class ZFollowHitmanBehaviourEffectBase : public IChaosEffect
{
public:
    void OnEngineInitialized() override;
    bool Available() override;

    void Start() override;
    void Stop() override;
    void OnClearScene() override;

private:
    ZEntityRef m_BehaviourEntity;

    bool EnsureBehaviourEntity();
};
