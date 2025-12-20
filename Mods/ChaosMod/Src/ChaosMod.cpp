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

ChaosMod::ChaosMod() : m_EffectTimer(std::bind(&ChaosMod::TriggerRandomChaosModule, this), 30.0)
{

}

ChaosMod::~ChaosMod()
{
    Hooks::ZEntitySceneContext_LoadScene->RemoveDetour(&ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->RemoveDetour(&ChaosMod::OnClearScene);
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
    const auto s_fTimeRemaining = m_EffectTimer.m_fIntervalSeconds - m_EffectTimer.GetElapsedSeconds();
    for (auto& s_Effect : EffectRegistry::GetInstance().GetEffects())
    {
        if (s_Effect && s_Effect->Available())
        {
            auto s_fEffectRemainingTime = 0.0f;

            // debug takes precedence
            if (s_Effect.get() == m_pEffectForDebug)
            {
                s_fEffectRemainingTime = m_fDebugEffectRemainingTime;
            }
            else if (s_Effect.get() == m_pLastEffect)
            {
                s_fEffectRemainingTime = s_fTimeRemaining;
            }

            s_Effect->OnFrameUpdate(p_UpdateEvent, s_fEffectRemainingTime);
        }
    }
}

void ChaosMod::TriggerRandomChaosModule()
{
    // stop previous effect
    if (m_pLastEffect)
    {
        m_pLastEffect->Stop();
    }

    // get and trigger the next effect
    auto& s_aEffects = EffectRegistry::GetInstance().GetEffects();
    if (EffectRegistry::GetInstance().GetEffects().size() == 0)
    {
        Logger::Error(TAG "No effects loaded to trigger.");
        return;
    }

    for (int t = 0; t < 10; t++)
    {
        const size_t s_nEffectIndex = Utils::GetRandomNumber<size_t>(0, s_aEffects.size() - 1);
        m_pLastEffect = s_aEffects[s_nEffectIndex].get();
        if (m_pLastEffect && m_pLastEffect->Available())
        {
            break;
        }
    }

    if (!m_pLastEffect)
    {
        return;
    }

    Logger::Info(TAG "Triggering '{}'", m_pLastEffect->GetName());
    m_pLastEffect->Start();
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters&)
{
    m_EffectTimer.m_bEnable = false;
    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    m_EffectTimer.m_bEnable = false;
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
