#pragma once

#include "IPluginInterface.h"

#include <string>

/**
 * Interface for all chaos effects.
 * Effects must - at minimum - implement Start() for one-shot effects.
 */
class IChaosEffect
{
public:
    /**
     * Called when the mod itself is initialized. 
     * Forwards @see IPluginInterface::Init
     */
    virtual void OnModInitialized() {};

    /**
     * Called when the engine is fully initialized.
     * Forwards @see IPluginInterface::OnEngineInitialized
     */
    virtual void OnEngineInitialized() {};

    /**
     * Called every frame during play mode (not when paused).
     */
    virtual void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {};

    /**
     * Called for drawing effect-specific UI in the ZHMModSDK menu, allowing 
     * effects to create custom windows.
     * Forwards @see IPluginInterface::OnDrawUI
     */
    virtual void OnDrawUI(const bool p_HasFocus) {};

    /**
     * Called within the rendering pass of the ChaosMod debug menu, allowing
     * effects to display debug information (when selected for debug).
     * since a window is already created, effects only need to use ImGui controls.
     */
    virtual void OnDrawDebugUI() {};

    /**
     * Called to start the effect.
     * One-Shot effects should do their activities here.
     */
    virtual void Start() = 0;

    /**
     * Called shortly before the next effect.
     * Persistent effects should stop their activities here.
     */
    virtual void Stop() {};

    /**
     * Check whether the effect is available to be used.
     * Unavailable effects will not be selected by the ChaosMod.
     * No functions except for @see IChaosEffect::Available, 
     * @see IChaosEffect::GetName and @see IChaosEffect::GetDisplayName may be called.
     */
    virtual bool Available() { return m_bIsAvailable; }

    /**
     * Get the internal name of the effect, e.g. for debug menu.
     * Default implementation returns the C++ type name.
     */
    virtual std::string GetName();

    /**
     * Get the display name of the effect, e.g. for enduser facing UI.
     * Default implementation returns @see IChaosEffect::GetName.
     */
    virtual std::string GetDisplayName();

protected:
    bool m_bIsAvailable = true;
};
