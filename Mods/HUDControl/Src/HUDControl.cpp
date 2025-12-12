#include "HUDControl.h"

#include <queue>

#include "Logging.h"

#include <Glacier/ZGameTime.h>
#include <Glacier/ZInputActionManager.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZModule.h>
#include <Glacier/SExternalReferences.h>
#include <Glacier/ZEntityManager.h>

#include <IconsMaterialDesign.h>
#include <imgui_internal.h>

/**
 * Static Entity ID for HUD MASTER_ROOT entity.
 * assembly:_PRO/scenes/bricks/gameessentials.brick, UI>MASTER_ROOT
 */
constexpr uint64_t HUD_MASTER_ROOT_ID = 0x77bed13890286553;

/**
 * Static Entity ID for Minimap entity.
 * assembly:_PRO/scenes/bricks/gameessentials.brick, UI>IngameMenu>MapMenu>InGameMapSetup>MiniMap
 */
constexpr uint64_t MINIMAP_ROOT_ID = 0xccda0a613b5d88cb;

HUDControl::HUDControl() :
    m_bMenuActive(false),
    m_bHUDVisible(true),
    m_fHUDAlpha(1.0f),
    m_bMiniMapShowsNpcs(true)
{

}

void HUDControl::Init()
{
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &HUDControl::OnClearScene);
}

void HUDControl::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_DASHBOARD " HUD"))
    {
        m_bMenuActive = !m_bMenuActive;
    }
}

void HUDControl::OnDrawUI(const bool p_HasFocus)
{
    if (!p_HasFocus || !m_bMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("HUD Control", &m_bMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing)
    {
        if (ImGui::Checkbox("HUD Visible", &m_bHUDVisible))
        {
            SetHUDVisibility(m_bHUDVisible);
        }

        if (ImGui::SliderFloat("HUD Alpha", &m_fHUDAlpha, 0.0f, 1.0f))
        {
            SetHUDAlpha(m_fHUDAlpha);
        }

        if (ImGui::Checkbox("Show NPCs on Minimap", &m_bMiniMapShowsNpcs))
        {
            SetMiniMapShowsNpcs(m_bMiniMapShowsNpcs);
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void HUDControl::SetHUDVisibility(const bool p_Visible)
{
    if (EnsureHUDRootEntity())
    {
        m_HUDRootEntity.SetProperty("m_bIsVisible", ZObjectRef::From(p_Visible));
    }
}

void HUDControl::SetHUDAlpha(const float p_Alpha)
{
    if (EnsureHUDRootEntity())
    {
        // m_fAlpha is 0-100, p_Alpha is 0.0-1.0
        m_HUDRootEntity.SetProperty("m_fAlpha", ZObjectRef::From(p_Alpha * 100.0f));
    }
}

void HUDControl::SetMiniMapShowsNpcs(const bool p_ShowNpcs)
{
    if (EnsureMinimapRootEntity())
    {
        // MenuMap (and MainMap) = no NPCs, MiniMap = with NPCs
        const auto s_eMapType = p_ShowNpcs ? EMapType::E_MAPTYPE_Minimap : EMapType::E_MAPTYPE_MenuMap;
        m_MinimapRootEntity.SetProperty("m_eMapType", ZObjectRef::From(s_eMapType));
    }
}

bool HUDControl::EnsureHUDRootEntity()
{
    if (m_HUDRootEntity)
    {
        return true;
    }

    m_HUDRootEntity = SDK()->GetEntityById(HUD_MASTER_ROOT_ID);
    return m_HUDRootEntity.m_pEntity;
}

bool HUDControl::EnsureMinimapRootEntity()
{
    if (m_MinimapRootEntity)
    {
        return true;
    }

    m_MinimapRootEntity = SDK()->GetEntityById(MINIMAP_ROOT_ID);
    return m_MinimapRootEntity.m_pEntity;
}


DEFINE_PLUGIN_DETOUR(HUDControl, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene)
{
    m_HUDRootEntity = {};
    m_MinimapRootEntity = {};

    return HookResult<void>(HookAction::Continue());
}

DEFINE_ZHM_PLUGIN(HUDControl);
