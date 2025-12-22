#include "ZActorWellbeignChangeEffectBase.h"

#include "Helpers/Utils.h"

void ZActorWellbeignChangeEffectBase::Start()
{
    // note: next frame update will populate the initial states
    m_mLastActorStates.clear();
    m_bMonitorChanges = true;
}

void ZActorWellbeignChangeEffectBase::Stop()
{
    m_mLastActorStates.clear();
    m_bMonitorChanges = false;
}

void ZActorWellbeignChangeEffectBase::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining)
{
    if (!m_bMonitorChanges)
    {
        return;
    }

    for (auto* s_pActor : Utils::GetActors(true, true))
    {
        const SActorState s_CurrentState{
            .m_bDead = s_pActor->IsDead(),
            .m_bPacified = s_pActor->IsPacified()
        };

        auto s_it = m_mLastActorStates.find(s_pActor);
        if (s_it == m_mLastActorStates.end())
        {
            m_mLastActorStates[s_pActor] = s_CurrentState;
            continue;
        }

        const auto s_LastState = s_it->second;
        if (s_CurrentState == s_LastState)
        {
            continue;
        }

        m_mLastActorStates[s_pActor] = s_CurrentState;

        OnActorWellbeingChanged(s_pActor, s_LastState, s_CurrentState);
    }
}
