#include "ChaosMod.h"

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZActor.h>
#include <Glacier/ZSpatialEntity.h>

#include <IconsMaterialDesign.h>
#include <imgui_internal.h>

#include "Helpers/ZTimer.h"
#include "Helpers/Utils.h"

#include <array>

#include "Effects/ZExplodeRandomActorEffect.h"
#include "Effects/ZFakeCrashEffect.h"
#include "Effects/ZLagEffect.h"
#include "Effects/ZCameraEffect.h"
#include "Effects/ZPlayerPoweredRagdollEffect.h"
#include "Effects/ZPlayerRagdollEffect.h"
#include "Effects/ZPlayerRagdollImpulseEffect.h"
#include "Effects/ZSlowTimeScaleEffect.h"
#include "Effects/ZSwapPlayerWithActorEffect.h"
#include "Effects/ZTeleportEffect.h"


#define TAG "[ChaosMod] "

ChaosMod::ChaosMod() : m_bMenuActive(false),
                       m_bDebugMenuActive(false),
                       m_pEffectForDebug(nullptr),
                       m_pLastEffect(nullptr),
                       m_EffectTimer(std::bind(&ChaosMod::TriggerRandomChaosModule, this), 30.0)
{
    m_aEffects = std::vector<IChaosEffect*>{
        new ZCameraEffect(ZCameraEffect::ECameraType::Flipped),
        new ZCameraEffect(ZCameraEffect::ECameraType::Overhead),
        new ZExplodeRandomActorEffect(),
        new ZFakeCrashEffect(),
        new ZLagEffect(),
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
    for (auto *s_pEffect : m_aEffects)
    {
        delete s_pEffect;
    }
    m_aEffects.clear();
}

void ChaosMod::Init()
{
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &ChaosMod::OnClearScene);

    for (auto *s_pEffect : m_aEffects)
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

    const ZMemberDelegate<ChaosMod, void(const SGameUpdateEvent &)> s_Delegate(this, &ChaosMod::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    for (auto *s_pEffect : m_aEffects)
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

void ChaosMod::OnFrameUpdate(const SGameUpdateEvent &p_UpdateEvent)
{
    for (auto *s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            s_pEffect->OnFrameUpdate(p_UpdateEvent);
        }
    }
}

void ChaosMod::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_QUESTION_MARK " CHAOS MOD"))
    {
        m_bMenuActive = !m_bMenuActive;
    }
}

void ChaosMod::OnDrawUI(const bool p_HasFocus)
{
    for (auto *s_pEffect : m_aEffects)
    {
        if (s_pEffect && s_pEffect->Available())
        {
            s_pEffect->OnDrawUI(p_HasFocus);
        }
    }

    if (!m_bMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("CHAOS MOD", &m_bMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing)
    {
        // this is shown even without focus to give feedback on the timer
        float32 s_Elapsed = m_EffectTimer.GetElapsedSeconds();
        ImGui::SliderFloat(
            "Chaos Timer",
            &s_Elapsed,
            0.0,
            m_EffectTimer.m_fIntervalSeconds);
        ImGui::TextUnformatted(fmt::format("Last Effect: {}", (m_pLastEffect != nullptr) ? m_pLastEffect->GetDisplayName() : "None").c_str());

        if (!p_HasFocus)
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();
            return;
        }

        ImGui::Separator();

        ImGui::Checkbox("Enable Chaos", &m_EffectTimer.m_bEnable);
        ImGui::SliderFloat(
            "Chaos Interval (Seconds)",
            &m_EffectTimer.m_fIntervalSeconds,
            5.0,
            120.0);
        ImGui::Checkbox("Show Debug Menu", &m_bDebugMenuActive);
        ImGui::TextUnformatted(fmt::format("Effects Loaded: {}", m_aEffects.size()).c_str());
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();

    if (!m_bDebugMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_DebugShowing = ImGui::Begin("CHAOS DEBUG", &m_bDebugMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_DebugShowing)
    {
        ImGui::BeginChild("chaos left pane", ImVec2(300, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto *s_pEffect : m_aEffects)
        {
            if (s_pEffect)
            {
                if (ImGui::Selectable(
                        s_pEffect->GetName().c_str(),
                        m_pEffectForDebug == s_pEffect))
                {
                    m_pEffectForDebug = s_pEffect;
                    Logger::Debug(TAG "Selected '{}' for debug", s_pEffect->GetName());
                }
            }
        }

        ImGui::EndChild();

        if (!m_pEffectForDebug)
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();
            return;
        }

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::BeginChild("module debug view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        ImGui::TextUnformatted(fmt::format("Internal Name: {}", m_pEffectForDebug->GetName()).c_str());
        ImGui::TextUnformatted(fmt::format("Display Name:  {}", m_pEffectForDebug->GetDisplayName()).c_str());

        if (m_pEffectForDebug->Available())
        {
            if (ImGui::Button("Start() now"))
            {
                Logger::Info(TAG "Calling Start() for '{}'", m_pEffectForDebug->GetName());
                m_pEffectForDebug->Start();
            }

            if (ImGui::Button("Stop() now"))
            {
                Logger::Info(TAG "Calling Stop() for '{}'", m_pEffectForDebug->GetName());
                m_pEffectForDebug->Stop();
            }

            ImGui::Separator();

            m_pEffectForDebug->OnDrawDebugUI();
        }
        else
        {
            ImGui::TextUnformatted("This effect reports as unavailable.");
        }

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
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

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnLoadScene, ZEntitySceneContext *th, SSceneInitParameters &)
{
    m_EffectTimer.m_bEnable = false;
    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnClearScene, ZEntitySceneContext *th, bool p_FullyUnloadScene)
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
