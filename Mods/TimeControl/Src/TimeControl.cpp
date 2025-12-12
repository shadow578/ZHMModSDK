#include "TimeControl.h"

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZInputActionManager.h>
#include <Glacier/ZGameLoopManager.h>

#include <IconsMaterialDesign.h>
#include <imgui_internal.h>

#define g_fTimeScale (Globals::GameTimeManager->m_fTimeMultiplier1)
#define g_bPaused (Globals::GameTimeManager->m_bPaused)

TimeControl::TimeControl() :
    m_bMenuActive(false),
    m_bOverlayActive(false),
    m_fMinTimeScale(0.0f),
    m_fMaxTimeScale(10.0f),
    m_SpeedUp("SpeedUp"),
    m_SpeedDown("SpeedDown"),
    m_PauseGame("PauseGame")
{
    m_PcControls = {
        {"J", "Increase time scale"},
        {"N", "Decrease time scale"},
        {"P", "Pause/Resume game"},
    };
}

void TimeControl::Init()
{
    Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &TimeControl::OnLoadScene);
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &TimeControl::OnClearScene);
}

void TimeControl::OnEngineInitialized()
{
    const ZMemberDelegate<TimeControl, void(const SGameUpdateEvent&)> s_Delegate(this, &TimeControl::OnFrameUpdate);
    Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

    const char* binds = "TimeControlInput={"
        "SpeedUp=tap(kb,j);"
        "SpeedDown=tap(kb,n);"
        "PauseGame=tap(kb,p);"
        "}";

    if (ZInputActionManager::AddBindings(binds))
    {
        Logger::Debug("Successfully added bindings.");
    }
    else
    {
        Logger::Debug("Failed to add bindings.");
    }
}

void TimeControl::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    if (Functions::ZInputAction_Digital->Call(&m_SpeedUp, -1))
    {
        g_fTimeScale = Clampf(g_fTimeScale + 0.1f, m_fMinTimeScale, m_fMaxTimeScale);
    }

    if (Functions::ZInputAction_Digital->Call(&m_SpeedDown, -1))
    {
        g_fTimeScale = Clampf(g_fTimeScale - 0.1f, m_fMinTimeScale, m_fMaxTimeScale);
    }

    if (Functions::ZInputAction_Digital->Call(&m_PauseGame, -1))
    {
        g_bPaused = !g_bPaused;
    }
}

void TimeControl::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_TIMER " TIMESCALE"))
    {
        m_bMenuActive = !m_bMenuActive;
    }
}

void TimeControl::OnDrawUI(const bool p_HasFocus)
{
    if (p_HasFocus && m_bMenuActive)
    {
        ImGui::PushFont(SDK()->GetImGuiBlackFont());
        const auto s_Showing = ImGui::Begin("TIMESCALE", &m_bMenuActive);
        ImGui::PushFont(SDK()->GetImGuiRegularFont());

        if (s_Showing)
        {
            ImGui::Checkbox("Paused", &g_bPaused);
            ImGui::SliderFloat(
                "Time Scale",
                &g_fTimeScale,
                m_fMinTimeScale,
                m_fMaxTimeScale
            );

            ImGui::Checkbox("Show Overlay", &m_bOverlayActive);

            ImGui::Separator();
            ImGui::TextUnformatted("PC Controls");

            ImGui::BeginTable("TimeControlsPc", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);
            for (auto& [s_Key, s_Description] : m_PcControls)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(s_Key.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(s_Description.c_str());
            }
            ImGui::EndTable();
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if (m_bMenuActive && m_bOverlayActive)
    {
        ImGui::PushFont(SDK()->GetImGuiBlackFont());
        const auto s_Showing = ImGui::Begin("TIMESCALE OVERLAY", &m_bMenuActive);
        ImGui::PushFont(SDK()->GetImGuiRegularFont());

        if (s_Showing)
        {
            ImGui::TextUnformatted(fmt::format("Current Speed: {}%", g_fTimeScale * 100).c_str());
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

DEFINE_PLUGIN_DETOUR(TimeControl, void, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters&)
{
    g_fTimeScale = 1.0f;

    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(TimeControl, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    g_fTimeScale = 1.0f;

    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(TimeControl);
