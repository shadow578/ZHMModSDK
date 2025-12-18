#include "ZCameraFOVEffect.h"

#include <Glacier/ZEntity.h>
#include <Glacier/ZSpatialEntity.h>

#include "Helpers/Utils.h"

constexpr float32 c_ZoomFOV = 10.0f;
constexpr float32 c_WideFOV = 120.0f;
const std::string c_CameraFOVPropertyName = "m_fFovYDeg";

void ZCameraFOVEffect::Start()
{
    ZCameraEffectBase::Start();
    ZInterpolatingEffectBase::Start();

    m_fOriginalFOV = -1.0f;
}

void ZCameraFOVEffect::Stop()
{
    ZInterpolatingEffectBase::Stop();
    ZCameraEffectBase::Stop();
}

void ZCameraFOVEffect::OnClearScene()
{
    ZCameraEffectBase::OnClearScene();
    ZInterpolatingEffectBase::OnClearScene();
}

void ZCameraFOVEffect::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining)
{
    ZInterpolatingEffectBase::OnFrameUpdate(p_UpdateEvent, p_fEffectTimeRemaining);

    if (!IsEffectCameraActive())
    {
        return;
    }

    // get references to all involved spatial entities
    auto s_OriginalCameraSpatialEntity = GetOriginalCameraEntity().QueryInterface<ZSpatialEntity>();
    auto s_CameraSpatialEntity = GetEffectCameraEntity().QueryInterface<ZSpatialEntity>();
    if (!s_OriginalCameraSpatialEntity || !s_CameraSpatialEntity)
    {
        Stop();
        return;
    }

    // copy stock player camera
    const auto s_OriginalWM = s_OriginalCameraSpatialEntity->GetWorldMatrix();
    s_CameraSpatialEntity->SetWorldMatrix(s_OriginalWM);

    // get original FOV once for interpolation
    if (m_fOriginalFOV <= 0.0f)
    {
        m_fOriginalFOV = *GetOriginalCameraEntity().GetProperty<float32>(c_CameraFOVPropertyName).As<float32>();
    }

    auto s_fTargetFOV = (m_eType == EType::WideFOV) ? c_WideFOV : c_ZoomFOV;

    // interpolation
    s_fTargetFOV = Utils::Interpolate(m_fOriginalFOV, s_fTargetFOV, GetInterpolationPoint());

    GetEffectCameraEntity().SetProperty(c_CameraFOVPropertyName, s_fTargetFOV);
}
