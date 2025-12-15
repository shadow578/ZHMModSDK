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
        m_eCameraType(p_eCameraType)
    {
    }

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) override;
    void OnDrawDebugUI() override;
    void Start() override;
    void Stop() override;

    std::string GetName() override;
    std::string GetDisplayName() override;

private:
    bool SpawnCameraEntity();
    void SetActiveCamera(ZEntityRef& p_pNewCameraEntity, ZEntityRef& p_pPreviousCameraEntity);

    ECameraType m_eCameraType;

    bool m_bApplyCameraTransform = false;
    float32 m_fOverheadDistance = 5.0f;

    ZEntityRef m_OverheadCameraEntity;
    ZEntityRef m_OriginalCameraEntity;
};
