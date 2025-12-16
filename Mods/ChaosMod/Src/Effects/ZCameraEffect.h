#pragma once

#include "IChaosEffect.h"

#include <Glacier/ZEntity.h>

class ZCameraEffect : public IChaosEffect
{
public:
    enum class ECameraType
    {
        Overhead,
        Flipped,
        NUMBER_OF_TYPES
    };

    ZCameraEffect(const ECameraType p_eCameraType) :
        m_eType(p_eCameraType)
    {
    }

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) override;
    void OnDrawDebugUI() override;
    void Start() override;
    void Stop() override;
    void OnClearScene() override;

    std::string GetName() override;
    std::string GetDisplayName() override;

private:
    enum class ECameraState
    {
        LerpIn,
        Stable,
        LerpOut,
        Disabling,
        Disabled
    };

    bool SpawnCameraEntity();
    void SetActiveCamera(ZEntityRef& p_pNewCameraEntity, ZEntityRef& p_pPreviousCameraEntity);

    ECameraType m_eType;

    ECameraState m_eState = ECameraState::Disabled;
    float32 m_fLerpPoint = 0.0f;


    float32 m_fOverheadDistance = 5.0f;

    ZEntityRef m_OverheadCameraEntity;
    ZEntityRef m_OriginalCameraEntity;
};
