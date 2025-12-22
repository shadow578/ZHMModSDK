#pragma once
#include "IChaosEffect.h"

#include <Glacier/ZEntity.h>
#include <Glacier/ZActor.h>

#include <map>

class ZActorWellbeignChangeEffectBase : public virtual IChaosEffect
{
public:
    void Start() override;
    void Stop() override;
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining) override;

protected:
    struct SActorState
    {
        bool m_bDead;
        bool m_bPacified;

        bool operator==(const SActorState& p_Other) const
        {
            return m_bDead == p_Other.m_bDead &&
                   m_bPacified == p_Other.m_bPacified;
        }
    };

    virtual void OnActorWellbeingChanged(ZActor* p_pActor, const SActorState& p_OldState, const SActorState& p_NewState) = 0;

private:
    bool m_bMonitorChanges = false;
    std::map<ZActor*, SActorState> m_mLastActorStates;
};
