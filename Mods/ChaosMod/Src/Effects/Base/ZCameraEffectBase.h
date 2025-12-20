#pragma once

#include "IChaosEffect.h"

#include <Glacier/ZEntity.h>

class ZCameraEffectBase : public virtual IChaosEffect
{
public:
    void Start() override;
    void Stop() override;
    void OnClearScene() override;
    void OnDrawDebugUI() override;

    bool Available() override;

protected:
    inline ZEntityRef GetEffectCameraEntity() const
    {
        return m_EffectCameraEntity;
    }

    inline ZEntityRef GetOriginalCameraEntity() const
    {
        return m_OriginalCameraEntity;
    }

    inline bool IsEffectCameraActive() const
    {
        return m_bEffectCameraActive && m_EffectCameraEntity && m_OriginalCameraEntity;
    }

private:
    bool EnsureCameraEntity();
    bool SetActiveCamera(ZEntityRef& p_NewCameraEntity, ZEntityRef& p_PreviousCameraEntity);

    bool m_bEffectCameraActive = false;
    ZEntityRef m_EffectCameraEntity;
    ZEntityRef m_OriginalCameraEntity;

private: // single active enforcement
    bool TakeCameraControl();
    void ReleaseCameraControl();
    bool CanTakeCameraControl();
    bool HasCameraControl();
};
