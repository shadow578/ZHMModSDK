#pragma once

#include "ZCameraEffectBase.h"
#include "ZInterpolatingEffectBase.h"

class ZFlipCameraEffect : public ZCameraEffectBase, public ZInterpolatingEffectBase
{
public:
    ZFlipCameraEffect() : ZInterpolatingEffectBase(1.0f)
    {
    }

    void Start() override;
    void Stop() override;
    void OnClearScene() override;

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining) override;

    std::string GetDisplayName() override
    {
        return "Turn Turtle";
    }
};
