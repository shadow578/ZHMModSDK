#pragma once

#include "IPluginInterface.h"

#include <Glacier/ZScene.h>

#include "Helpers/ZTimer.h"
#include "IChaosEffect.h"

#include <vector>


class ChaosMod : public IPluginInterface
{
public:
    ChaosMod();
    ~ChaosMod();

    void Init() override;
    void OnEngineInitialized() override;
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

private:
    DECLARE_PLUGIN_DETOUR(ChaosMod, void, OnLoadScene, ZEntitySceneContext*, SSceneInitParameters&);
    DECLARE_PLUGIN_DETOUR(ChaosMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene);

    void TriggerRandomChaosModule();

private:
    bool m_bMenuActive = false;

    ZTimer m_EffectTimer;
    IChaosEffect* m_pLastEffect = nullptr;

private: // Debug
    void DrawDebugWindow();
    void DrawEffectDebugPane();

    bool m_bDebugMenuActive = false;
    float32 m_fDebugEffectRemainingTime = 30.0f;
    IChaosEffect* m_pEffectForDebug = nullptr;
};

DECLARE_ZHM_PLUGIN(ChaosMod)
