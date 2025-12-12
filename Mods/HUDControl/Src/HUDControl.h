#pragma once

#include "IPluginInterface.h"

#include <Glacier/ZScene.h>
#include <Glacier/ZInput.h>


class HUDControl : public IPluginInterface
{
public:
    HUDControl();

    void Init() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

private:
    void SetHUDVisibility(bool p_Visible);
    void SetHUDAlpha(float p_Alpha);
    void SetMiniMapShowsNpcs(bool p_ShowNpcs);

    bool EnsureHUDRootEntity();
    bool EnsureMinimapRootEntity();

private:
    DECLARE_PLUGIN_DETOUR(HUDControl, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene);

private:
    bool m_bMenuActive;

    bool m_bHUDVisible;
    float m_fHUDAlpha;
    bool m_bMiniMapShowsNpcs;

    ZEntityRef m_HUDRootEntity;
    ZEntityRef m_MinimapRootEntity;
};

DECLARE_ZHM_PLUGIN(HUDControl)
