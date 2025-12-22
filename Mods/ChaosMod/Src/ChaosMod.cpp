#include "ChaosMod.h"

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZActor.h>
#include <Glacier/ZSpatialEntity.h>

#include "Helpers/ZTimer.h"
#include "Helpers/Utils.h"

#include "EffectRegistry.h"

#define TAG "[ChaosMod] "

ChaosMod::ChaosMod() : 
    m_fFullEffectDuration(60.0f),
    m_nVoteOptions(4),
    m_EffectTimer(std::bind(&ChaosMod::OnEffectTimerTrigger, this), 30.0)
{

}

ChaosMod::~ChaosMod()
{
    Hooks::ZEntitySceneContext_LoadScene->RemoveDetour(&ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->RemoveDetour(&ChaosMod::OnClearScene);

    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            Logger::Debug(TAG "Forwarding OnModUnload to '{}'", s_Effect->GetName());
            s_Effect->OnModUnload();
        }
    }
}

void ChaosMod::Init()
{
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &ChaosMod::OnClearScene);

    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            Logger::Debug(TAG "Forwarding OnModInitialized to '{}'", s_Effect->GetName());
            s_Effect->OnModInitialized();
        }
    }
}

void ChaosMod::OnEngineInitialized()
{
    m_EffectTimer.Initialize();

    const ZMemberDelegate<ChaosMod, void(const SGameUpdateEvent&)> s_Delegate(this, &ChaosMod::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            Logger::Debug(TAG "Forwarding OnEngineInitialized to '{}'", s_Effect->GetName());
            s_Effect->OnEngineInitialized();

            if (!s_Effect->Available())
            {
                Logger::Warn(
                    TAG "'{}' reported as unavailable during OnEngineInitialized, it will not be used.",
                    s_Effect->GetName());
            }
        }
    }
}

void ChaosMod::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            auto s_fEffectRemainingTime = 0.0f;
            for (auto &s_ActiveEffect : m_aActiveEffects)
            {
                if (s_ActiveEffect.m_pEffect == s_Effect.get())
                {
                    s_fEffectRemainingTime = s_ActiveEffect.m_fTimeRemaining;
                    break;
                }
            }

            // debug takes precedence
            if (s_Effect.get() == m_pEffectForDebug)
            {
                s_fEffectRemainingTime = m_fDebugEffectRemainingTime;
            }

            s_Effect->OnFrameUpdate(p_UpdateEvent, s_fEffectRemainingTime);
        }
    }

    UpdateEffectExpiration(p_UpdateEvent.m_GameTimeDelta.ToSeconds());
}

void ChaosMod::OnLoadOrClearScene()
{
    m_EffectTimer.m_bEnable = false;

    m_aCurrentVote.clear();
    m_aActiveEffects.clear();
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters&)
{
    OnLoadOrClearScene();
    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    OnLoadOrClearScene();

    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            Logger::Debug(TAG "Forwarding OnClearScene to '{}'", s_Effect->GetName());
            s_Effect->OnClearScene();
        }
    }

    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(ChaosMod);
