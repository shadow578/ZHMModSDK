#include "ChaosMod.h"

#include "Logging.h"

#include <IconsMaterialDesign.h>
#include <imgui_internal.h>

#define TAG "[ChaosModUI] "


void ChaosMod::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_QUESTION_MARK " CHAOS MOD"))
    {
        m_bMenuActive = !m_bMenuActive;
    }
}

void ChaosMod::OnDrawUI(const bool p_HasFocus)
{
    for (auto* s_pEffect : m_aEffects)
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
        // this is shown even without focus to give user feedback at all times
        const float32 s_fRemainingToNext = m_EffectTimer.m_fIntervalSeconds - m_EffectTimer.GetElapsedSeconds();
        ImGui::ProgressBar(
            s_fRemainingToNext / m_EffectTimer.m_fIntervalSeconds,
            ImVec2(-1.0f, 0.0f),
            fmt::format("Next Effect In: {:.1f} Seconds", s_fRemainingToNext).c_str());
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

    DrawDebugWindow();
}

void ChaosMod::DrawDebugWindow()
{
    if (!m_bDebugMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("CHAOS DEBUG", &m_bDebugMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing)
    {
        ImGui::BeginChild("chaos left pane", ImVec2(300, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto* s_pEffect : m_aEffects)
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

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::BeginChild("effect debug pane", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        if (m_pEffectForDebug)
        {
            DrawEffectDebugPane();
        }
        else
        {
            ImGui::TextUnformatted("No effect selected for debug.");
        }

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void ChaosMod::DrawEffectDebugPane()
{
    ImGui::TextUnformatted(fmt::format("Name:         {}", m_pEffectForDebug->GetName()).c_str());
    ImGui::TextUnformatted(fmt::format("Display Name: {}", m_pEffectForDebug->GetDisplayName()).c_str());
    ImGui::TextUnformatted(fmt::format("Available:    {}", m_pEffectForDebug->Available() ? "Yes" : "No").c_str());

    ImGui::BeginDisabled(!m_pEffectForDebug->Available());
    if (ImGui::Button("Start() now"))
    {
        Logger::Info(TAG "Calling Start() for '{}'", m_pEffectForDebug->GetName());
        m_pEffectForDebug->Start();
        m_fDebugEffectRemainingTime = 30.0f;
    }

    if (ImGui::Button("Stop() now"))
    {
        Logger::Info(TAG "Calling Stop() for '{}'", m_pEffectForDebug->GetName());
        m_pEffectForDebug->Stop();
    }

    if (ImGui::Button("0.5 Seconds Remaining"))
    {
        m_fDebugEffectRemainingTime = 0.5f;
    }

    ImGui::SliderFloat(
        "Debug Effect Remaining Time (Seconds)",
        &m_fDebugEffectRemainingTime,
        0.0f,
        60.0f
    );

    ImGui::EndDisabled();

    ImGui::Separator();

    if (m_pEffectForDebug->Available())
    {
        m_pEffectForDebug->OnDrawDebugUI();
    }
    else
    {
        ImGui::TextUnformatted("Effect Debug UI not drawn, as effect is not available.");
    }
}