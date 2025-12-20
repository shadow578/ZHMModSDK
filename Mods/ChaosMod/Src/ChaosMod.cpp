#include "ChaosMod.h"

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZActor.h>
#include <Glacier/ZSpatialEntity.h>

#include "Helpers/ZTimer.h"
#include "Helpers/Utils.h"

#include "Effects/ZCameraFOVEffects.h"
#include "Effects/ZExplodeRandomActorEffect.h"
#include "Effects/ZFakeCrashEffect.h"
#include "Effects/ZFlipCameraEffect.h"
#include "Effects/ZLagEffect.h"
#include "Effects/ZOverheadCameraEffect.h"
#include "Effects/ZPlayerPoweredRagdollEffect.h"
#include "Effects/ZPlayerRagdollEffect.h"
#include "Effects/ZPlayerRagdollImpulseEffect.h"
#include "Effects/ZSlowTimeScaleEffect.h"
#include "Effects/ZSwapPlayerWithActorEffect.h"
#include "Effects/ZTeleportEffect.h"


#define TAG "[ChaosMod] "

ChaosMod::ChaosMod() : m_EffectTimer(std::bind(&ChaosMod::TriggerRandomChaosModule, this), 30.0)
{
    m_aEffects = std::vector<IChaosEffect*>{
        new ZCameraZoomFOVEffect(),
        new ZCameraWideFOVEffect(),
        //FIXME: slow load new ZExplodeRandomActorEffect(),
        new ZFakeCrashEffect(),
        new ZFlipCameraEffect(),
        new ZLagEffect(),
        new ZOverheadCameraEffect(),
        new ZPlayerPoweredRagdollEffect(),
        new ZPlayerRagdollEffect(),
        new ZPlayerRagdollImpulseEffect(),
        new ZSlowTimeScaleEffect(),
        new ZSwapPlayerWithActorEffect(),
        new ZTeleportEffect(),
    };
}

ChaosMod::~ChaosMod()
{
    for (auto* s_pEffect : m_aEffects)
    {
        delete s_pEffect;
    }
    m_aEffects.clear();
}

void ChaosMod::Init()
{
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &ChaosMod::OnClearScene);

    for (auto* s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            Logger::Debug(TAG "Forwarding OnModInitialized to '{}'", s_pEffect->GetName());
            s_pEffect->OnModInitialized();
        }
    }
}

void ChaosMod::OnEngineInitialized()
{
    m_EffectTimer.Initialize();

    const ZMemberDelegate<ChaosMod, void(const SGameUpdateEvent&)> s_Delegate(this, &ChaosMod::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    for (auto* s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            Logger::Debug(TAG "Forwarding OnEngineInitialized to '{}'", s_pEffect->GetName());
            s_pEffect->OnEngineInitialized();

            if (!s_pEffect->Available())
            {
                Logger::Warn(
                    TAG "'{}' reported as unavailable during OnEngineInitialized, it will not be used.",
                    s_pEffect->GetName());
            }
        }
    }
}

void ChaosMod::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    const auto s_fTimeRemaining = m_EffectTimer.m_fIntervalSeconds - m_EffectTimer.GetElapsedSeconds();
    for (auto* s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            auto s_fEffectRemainingTime = 0.0f;

            // debug takes precedence
            if (s_pEffect == m_pEffectForDebug)
            {
                s_fEffectRemainingTime = m_fDebugEffectRemainingTime;
            }
            else if (s_pEffect == m_pLastEffect)
            {
                s_fEffectRemainingTime = s_fTimeRemaining;
            }

            s_pEffect->OnFrameUpdate(p_UpdateEvent, s_fEffectRemainingTime);
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
    if (m_aEffects.size() == 0)
    {
        Logger::Error(TAG "No effects loaded to trigger.");
        return;
    }

    for (int t = 0; t < 10; t++)
    {
        const size_t s_nEffectIndex = Utils::GetRandomNumber<size_t>(0, m_aEffects.size() - 1);
        m_pLastEffect = m_aEffects[s_nEffectIndex];
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
    for (auto* s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            Logger::Debug(TAG "Forwarding OnClearScene to '{}'", s_pEffect->GetName());
            s_pEffect->OnClearScene();
        }
    }

    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(ChaosMod);
