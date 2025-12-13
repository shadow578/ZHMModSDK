#pragma once

#include "IPluginInterface.h"

#include <string>

class IChaosModule
{
public:
    virtual void OnModInitialized() {};
    virtual void OnEngineInitialized() {};
    virtual void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {};
    virtual void OnDrawUI(const bool p_HasFocus) {};
    virtual void OnDrawDebugUI() {};
    virtual void OnNextTriggered() {};

    virtual void Trigger() = 0;
    virtual std::string GetName() = 0;
};
