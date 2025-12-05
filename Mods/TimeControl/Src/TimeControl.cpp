#include "TimeControl.h"

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZInputActionManager.h>
#include <Glacier/ZGameLoopManager.h>

#include <IconsMaterialDesign.h>
#include <imgui_internal.h>

TimeControl::TimeControl() :
    m_MenuActive(false),
    m_fTimeScale(nullptr),
    m_fMinTimeScale(0.0f),
    m_fMaxTimeScale(10.0f),
    m_SpeedUp("SpeedUp"),
    m_SpeedDown("SpeedDown"),
    m_PauseGame("PauseGame")
{
    m_PcControls = {
        {"J", "Increase time scale"},
        {"M", "Decrease time scale"},
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
        "SpeedDown=tap(kb,m);"
        "PauseGame=tap(kb,p);";

    if (ZInputActionManager::AddBindings(binds))
    {
        Logger::Debug("Successfully added bindings.");
    }
    else
    {
        Logger::Debug("Failed to add bindings.");
    }

    m_fTimeScale = &Globals::GameTimeManager->m_fTimeMultiplier1;
}

void TimeControl::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    if (Functions::ZInputAction_Digital->Call(&m_SpeedUp, -1))
    {
        if (m_fTimeScale)
        {
            *m_fTimeScale = fClamp(*m_fTimeScale + 0.1f, m_fMinTimeScale, m_fMaxTimeScale);
        }
    }

    if (Functions::ZInputAction_Digital->Call(&m_SpeedDown, -1))
    {
        if (m_fTimeScale)
        {
            *m_fTimeScale = fClamp(*m_fTimeScale - 0.1f, m_fMinTimeScale, m_fMaxTimeScale);
        }
    }

    if (Functions::ZInputAction_Digital->Call(&m_PauseGame, -1))
    {
        Globals::GameTimeManager->m_bPaused = !Globals::GameTimeManager->m_bPaused;
    }
}

void TimeControl::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_TIMER " TIME CONTROL"))
    {
        m_MenuActive = !m_MenuActive;
    }
}

void TimeControl::OnDrawUI(const bool p_HasFocus)
{
    if (!p_HasFocus || !m_MenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("TIME CONTROL", &m_MenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing)
    {
        ImGui::Text("ZGameTimeManager multipliers");
        ImGui::Checkbox("Paused", &Globals::GameTimeManager->m_bPaused);
        ImGui::SliderFloat(
            "Time Scale",
            m_fTimeScale,
            m_fMinTimeScale,
            m_fMaxTimeScale
        );


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

DEFINE_PLUGIN_DETOUR(TimeControl, void, OnLoadScene, ZEntitySceneContext* th, SSceneInitParameters&)
{
    if (m_fTimeScale) 
    {
        *m_fTimeScale = 1.0f;
    }

    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(TimeControl, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    if (m_fTimeScale)
    {
        *m_fTimeScale = 1.0f;
    }

    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(TimeControl);
