#pragma once

#include "ZCameraEffectBase.h"
#include "ZInterpolatingEffectBase.h"

class ZCameraFOVEffect : public ZCameraEffectBase, public ZInterpolatingEffectBase
{
public:
    enum class EType
    {
        WideFOV,
        ZoomFOV
    };

    ZCameraFOVEffect(const EType p_eType) : ZInterpolatingEffectBase(1.0f), m_eType(p_eType)
    {
    }

    void Start() override;
    void Stop() override;
    void OnClearScene() override;

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining) override;

private:
    EType m_eType;
    float32 m_fOriginalFOV = -1.0f;
};
