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

#include "Modules/ZExplodeRandomActorModule.h"
#include "Modules/ZSwapPlayerWithActorModule.h"
#include "Modules/ZTeleportUp.h"

#include <array>

#define TAG "[ChaosMod] "

ChaosMod::ChaosMod() :
    m_bMenuActive(false),
    m_bDebugMenuActive(false),
    m_pDebugSelectedModule(nullptr),
    m_pLastTriggeredModule(nullptr),
    m_ChaosTriggerTimer(std::bind(&ChaosMod::TriggerRandomChaosModule, this), 30.0
    )
{
    m_aChaosModules = std::vector<IChaosModule*>{    
        new ZExplodeRandomActorModule(),
        new ZSwapPlayerWithActorModule(),
        new ZTeleportUpModule(),
    };
}

ChaosMod::~ChaosMod()
{
    for (auto* module : m_aChaosModules)
    {
        delete module;
    }
    m_aChaosModules.clear();
}

void ChaosMod::Init()
{
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &ChaosMod::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &ChaosMod::OnClearScene);

    for (auto s_Module : m_aChaosModules)
    {
        if (s_Module)
        {
            Logger::Debug(TAG "Forwarding OnModInitialized to '{}'", s_Module->GetName());
            s_Module->OnModInitialized();
        }
    }
}

void ChaosMod::OnEngineInitialized()
{
    m_ChaosTriggerTimer.Initialize();

    const ZMemberDelegate<ChaosMod, void(const SGameUpdateEvent&)> s_Delegate(this, &ChaosMod::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    for (auto s_Module : m_aChaosModules)
    {
        if (s_Module)
        {
            Logger::Debug(TAG "Forwarding OnEngineInitialized to '{}'", s_Module->GetName());
            s_Module->OnEngineInitialized();
        }
    }
}

void ChaosMod::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    for (auto s_Module : m_aChaosModules)
    {
        if (s_Module)
        {
            s_Module->OnFrameUpdate(p_UpdateEvent);
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
    for (auto s_Module : m_aChaosModules)
    {
        if (s_Module)
        {
            s_Module->OnDrawUI(p_HasFocus);
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
        float32 s_Elapsed = m_ChaosTriggerTimer.GetElapsedSeconds();
        ImGui::SliderFloat(
            "Chaos Timer",
            &s_Elapsed,
            0.0,
            m_ChaosTriggerTimer.m_fIntervalSeconds
        );
        ImGui::TextUnformatted(fmt::format("Last Triggered: {}", m_pLastTriggeredModule ? m_pLastTriggeredModule->GetName() : "<null>").c_str());

        if (!p_HasFocus)
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();
            return;
        }
        
        ImGui::Separator();

        ImGui::Checkbox("Enable Chaos", &m_ChaosTriggerTimer.m_bEnable);
        ImGui::SliderFloat(
            "Chaos Interval (Seconds)",
            &m_ChaosTriggerTimer.m_fIntervalSeconds,
            5.0,
            120.0
        );
        ImGui::Checkbox("Show Debug Menu", &m_bDebugMenuActive);
        ImGui::TextUnformatted(fmt::format("Modules Loaded: {}", m_aChaosModules.size()).c_str());
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

        for (auto s_Module : m_aChaosModules)
        {
            if (s_Module)
            {
                if (ImGui::Selectable(
                    s_Module->GetName().c_str(),
                    m_pDebugSelectedModule == s_Module
                ))
                {
                    m_pDebugSelectedModule = s_Module;
                    Logger::Debug(TAG "Selected '{}' for debug", s_Module->GetName());
                }
            }
        }

        ImGui::EndChild();

        if (!m_pDebugSelectedModule)
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();
            return;
        }

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::BeginChild("module debug view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        
        if (ImGui::Button("Trigger Now"))
        {
            Logger::Info(TAG "Manually triggering '{}'", m_pDebugSelectedModule->GetName());
            m_pDebugSelectedModule->Trigger();
        }

        if (ImGui::Button("Signal Next Triggered"))
        {
            Logger::Info(TAG "Signaling OnNextTriggered to '{}'", m_pDebugSelectedModule->GetName());
            m_pDebugSelectedModule->OnNextTriggered();
        }

        ImGui::Separator();

        m_pDebugSelectedModule->OnDrawDebugUI();

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void ChaosMod::TriggerRandomChaosModule()
{
    // notify previous module the next will be triggered now
    // so that it can stop any ongoing effects if needed
    if (m_pLastTriggeredModule)
    {
        m_pLastTriggeredModule->OnNextTriggered();
    }

    // trigger the next module
    if (m_aChaosModules.size() == 0)
    {
        Logger::Error(TAG "No chaos modules loaded to trigger.");
        return;
    }
    
    const size_t s_ModuleIndex = GetRandomNumber<size_t>(0, m_aChaosModules.size());
    const auto s_Module = m_aChaosModules[rand() % m_aChaosModules.size()];
    if (!s_Module)
    {
        return;
    }

    Logger::Info(TAG "Triggering '{}'", s_Module->GetName());
    s_Module->Trigger();

    m_pLastTriggeredModule = s_Module;
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters&)
{
    m_ChaosTriggerTimer.m_bEnable = false;
    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(ChaosMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    m_ChaosTriggerTimer.m_bEnable = false;
    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(ChaosMod);
