#pragma once

#include "IPluginInterface.h"

#include <Glacier/ZScene.h>
#include <Glacier/ZInput.h>


class TimeControl : public IPluginInterface
{
public:
    TimeControl();

    void Init() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;


private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    static float Clampf(float value, float min, float max)
    {
        if (value < min)
            return min;
        
        if (value > max)
            return max;
        
        return value;
    }

private:
    DECLARE_PLUGIN_DETOUR(TimeControl, void, OnLoadScene, ZEntitySceneContext*, SSceneInitParameters&);
    DECLARE_PLUGIN_DETOUR(TimeControl, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene);

private:
    bool m_bMenuActive;
    bool m_bOverlayActive;

    float m_fMinTimeScale;
    float m_fMaxTimeScale;

    ZInputAction m_SpeedUp;
    ZInputAction m_SpeedDown;
    ZInputAction m_PauseGame;

    std::unordered_map<std::string, std::string> m_PcControls;
};

DECLARE_ZHM_PLUGIN(TimeControl)
