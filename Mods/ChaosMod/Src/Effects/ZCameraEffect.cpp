#include "ZCameraEffect.h"

#include <Globals.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZCameraEntity.h>
#include <Glacier/ZScene.h>
#include <Glacier/SExternalReferences.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/ZCollision.h>

#include "Logging.h"

#include "Helpers/Utils.h"

#define TAG "[ZCameraEffect] "

constexpr float32 c_LerpDuration = 2.0f; // seconds

constexpr float32 c_fOverheadMinDistance = 2.0f; // at least 47's height
constexpr float32 c_fOverheadMaxDistance = 15.0f;

constexpr float32 c_ZoomFOV = 10.0f;
constexpr float32 c_WideFOV = 120.0f;
const std::string c_CameraFOVPropertyName = "m_fFovYDeg";

static std::string ECameraTypeToString(ZCameraEffect::ECameraType p_eCameraType)
{
    switch (p_eCameraType)
    {
    case ZCameraEffect::ECameraType::Overhead:
        return "Overhead";
    case ZCameraEffect::ECameraType::Flipped:
        return "Flipped";
    case ZCameraEffect::ECameraType::ZoomFOV:
        return "ZoomFOV";
    case ZCameraEffect::ECameraType::WideFOV:
        return "WideFOV";
    default:
        return "<" + std::to_string(static_cast<int>(p_eCameraType)) + ">";
    }
}

std::string ZCameraEffect::GetName()
{
    return "ZCameraEffect_" + ECameraTypeToString(m_eType);
}

std::string ZCameraEffect::GetDisplayName()
{
    switch (m_eType)
    {
    case ECameraType::Overhead:
        return "GTA1 Mode";
    case ECameraType::Flipped:
        return "Turn Turtle";
    case ECameraType::ZoomFOV:
        return "Binoculars";
    case ECameraType::WideFOV:
        return "Quake Pro FOV";
    default:
        return GetName();
    }
}

void ZCameraEffect::Start()
{
    if (m_eState != ECameraState::Disabled)
    {
        Logger::Debug(TAG "Effect already running, aborting start.");
        return;
    }

    if (!SpawnCameraEntity())
    {
        Logger::Debug(TAG "Could not spawn overhead camera entity, aborting effect.");
        m_bIsAvailable = false;
        m_eState = ECameraState::Disabled;
        return;
    }

    SetActiveCamera(m_OverheadCameraEntity, m_OriginalCameraEntity);
    m_eState = ECameraState::LerpIn;
    m_fLerpPoint = 0.0f;
}

void ZCameraEffect::Stop()
{
    ZEntityRef s_Dummy;
    SetActiveCamera(m_OriginalCameraEntity, s_Dummy);
    m_eState = ECameraState::Disabled;
}

void ZCameraEffect::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent, const float32 p_fEffectTimeRemaining)
{
    // disable state skips frame updates,
    // all other states apply transform
    if (m_eState == ECameraState::Disabled)
    {
        return;
    }

    if (!m_OverheadCameraEntity || !m_OriginalCameraEntity)
    {
        m_eState = ECameraState::Disabled;
        return;
    }

    // get references to all involved spatial entities
    auto s_OriginalCameraSpatialEntity = m_OriginalCameraEntity.QueryInterface<ZSpatialEntity>();
    auto s_CameraSpatialEntity = m_OverheadCameraEntity.QueryInterface<ZSpatialEntity>();
    if (!s_OriginalCameraSpatialEntity || !s_CameraSpatialEntity)
    {
        m_eState = ECameraState::Disabled;
        return;
    }

    auto s_OriginalWM = s_OriginalCameraSpatialEntity->GetWorldMatrix();
    auto s_TargetWM = s_CameraSpatialEntity->GetWorldMatrix();
    float32 s_fTargetFOV = -1.0f, s_fOriginalFOV = -1.0f;

    // set target based on camera type
    switch (m_eType)
    {
    case ECameraType::Overhead:
    {
        // copy player translation
        auto s_Player = SDK()->GetLocalPlayer();
        auto s_PlayerSpatialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
        auto s_PlayerWM = s_PlayerSpatialEntity->GetWorldMatrix();

        s_TargetWM.Trans = s_PlayerWM.Trans;
        s_TargetWM.Trans.z += c_fOverheadMinDistance;
        s_TargetWM.Trans.z = GetFreeHeightFor(
            s_TargetWM,
            c_fOverheadMaxDistance
        ); // elevate

        // rotate facing downwards
        s_TargetWM.XAxis = float4(0.0f, 1.0f, 0.0f, 0.0f);
        s_TargetWM.YAxis = float4(-1.0f, 0.0f, -0.0f, 0.0f);
        s_TargetWM.ZAxis = float4(-0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    case ECameraType::Flipped:
    {
        // copy stock player camera
        s_TargetWM = s_OriginalWM;

        // rotate around forward axis by 180 degrees to flip the screen
        const auto s_RotationMatrix = SMatrix::RotationAxisAngle(
            (-s_TargetWM.Backward).Normalized(),
            3.14159f // 180 degrees
        );
        s_TargetWM = s_RotationMatrix * s_TargetWM;
        break;
    }
    case ECameraType::ZoomFOV:
    case ECameraType::WideFOV:
    {
        // copy stock player camera
        s_TargetWM = s_OriginalWM;

        s_fTargetFOV = (m_eType == ECameraType::ZoomFOV) ? c_ZoomFOV : c_WideFOV;
        s_fOriginalFOV = *m_OriginalCameraEntity.GetProperty<float32>(c_CameraFOVPropertyName).As<float32>();
        break;
    }
    default:
        break;
    }

    // lerp towards end transform and fov
    switch (m_eState)
    {
    case ECameraState::LerpIn:
        s_TargetWM = Utils::LerpAffine(s_OriginalWM, s_TargetWM, m_fLerpPoint);

        if (s_fTargetFOV > 0.0f)
        {
            s_fTargetFOV = Utils::Lerp(
                s_fOriginalFOV,
                s_fTargetFOV,
                m_fLerpPoint
            );
        }
        break;
    case ECameraState::LerpOut:
        s_TargetWM = Utils::LerpAffine(s_TargetWM, s_OriginalWM, m_fLerpPoint);

        if (s_fTargetFOV > 0.0f)
        {
            s_fTargetFOV = Utils::Lerp(
                s_fTargetFOV,
                s_fOriginalFOV,
                m_fLerpPoint
            );
        }
        break;
    default:
        break;
    }
    m_fLerpPoint += p_UpdateEvent.m_GameTimeDelta.ToSeconds() / c_LerpDuration;

    s_CameraSpatialEntity->SetWorldMatrix(s_TargetWM);

    if (s_fTargetFOV > 0.0f)
    {
        m_OverheadCameraEntity.SetProperty(c_CameraFOVPropertyName, s_fTargetFOV);
    }

    // advance state machine
    switch (m_eState)
    {
    case ECameraState::LerpIn:
    {
        if (m_fLerpPoint >= 1.0f)
        {
            m_eState = ECameraState::Stable;
        }
        break;
    }
    case ECameraState::Stable:
        // start lerp out early
        if (p_fEffectTimeRemaining != 0.0f && p_fEffectTimeRemaining <= c_LerpDuration + 0.1f)
        {
            m_eState = ECameraState::LerpOut;
            m_fLerpPoint = 0.0f;
        }
        break;
    case ECameraState::LerpOut:
    {
        if (m_fLerpPoint >= 1.0f)
        {
            m_eState = ECameraState::Disabled;
        }
        break;
    }
    default:
        break;
    }
}

void ZCameraEffect::OnClearScene()
{
    m_eState = ECameraState::Disabled;
    m_OverheadCameraEntity = {};
    m_OriginalCameraEntity = {};
}

void ZCameraEffect::OnDrawDebugUI()
{
    ImGui::TextUnformatted(fmt::format("Current State: {}", static_cast<int>(m_eState)).c_str());
    ImGui::TextUnformatted(fmt::format("Lerp Point:   {:.3f}", m_fLerpPoint).c_str());

    if (ImGui::BeginCombo("Camera Type", ECameraTypeToString(m_eType).c_str()))
    {
        for (int i = 0; i < static_cast<int>(ECameraType::NUMBER_OF_TYPES); i++)
        {
            auto s_eType = static_cast<ECameraType>(i);
            if (ImGui::Selectable(
                ECameraTypeToString(static_cast<ECameraType>(i)).c_str(),
                s_eType == m_eType
            ))
            {
                m_eType = s_eType;
            }
        }

        ImGui::EndCombo();
    }

    switch (m_eType)
    {
    case ECameraType::Overhead:
    {
        ImGui::DragFloat("Max Overhead Distance", &m_fOverheadDistance, 0.1f, 1.0f, 100.0f);
        break;
    }
    default:
        break;
    }
}

float32 ZCameraEffect::GetFreeHeightFor(const SMatrix& p_Position, const float32 p_fMaxHeight)
{
    if (!*Globals::CollisionManager)
    {
        return p_fMaxHeight;
    }

    ZRayQueryInput s_RayInput{
        .m_vFrom = p_Position.Trans,
        .m_vTo = p_Position.Trans + (p_Position.Up * p_fMaxHeight),
    };
    ZRayQueryOutput s_RayOutput {};

    if (!(*Globals::CollisionManager)->RayCastClosestHit(s_RayInput, &s_RayOutput))
    {
        // ray didn't hit anything
        return p_fMaxHeight;
    }

    return s_RayOutput.m_vPosition.z;
}

bool ZCameraEffect::SpawnCameraEntity()
{
    // only once
    if (m_OverheadCameraEntity.m_pEntity)
    {
        return true;
    }

    const auto s_Scene = Globals::Hitman5Module->m_pEntitySceneContext->m_pScene;
    if (!s_Scene)
    {
        Logger::Debug(TAG "Scene not loaded.");
        return false;
    }

    // spawning "[modules:/zcameraentity.class].pc_entitytype" directly didn't work, so 
    // use a template that contains a camera entity and "steal" it from there
    const auto s_RuntimeResourceId = ResId<"[assembly:/templates/core/hm5camera.template?/compositeentity_norenderdestination.entitytemplate].pc_entitytype">;

    TResourcePtr<ZTemplateEntityFactory> s_Resource;
    Globals::ResourceManager->GetResourcePtr(s_Resource, s_RuntimeResourceId, 0);
    if (!s_Resource)
    {
        Logger::Debug(TAG "Resource is not loaded.");
        return false;
    }

    ZEntityRef m_CameraHolderEntity;
    SExternalReferences s_DummyExternalRefs;

    Functions::ZEntityManager_NewEntity->Call(
        Globals::EntityManager,
        m_CameraHolderEntity,
        "",
        s_Resource,
        s_Scene.m_ref,
        s_DummyExternalRefs,
        -1
    );
    if (!m_CameraHolderEntity)
    {
        Logger::Debug(TAG "Could not spawn camera entity.");
        return false;
    }

    // inside the template, grab camera entity "HM5MainCamera01"
    auto s_CameraBpFactory = m_CameraHolderEntity.GetBlueprintFactory();
    if (const auto idx = s_CameraBpFactory->GetSubEntityIndex(0x151b9072f9d0dc77); idx != -1)
    {
        if (const auto s_Ent = s_CameraBpFactory->GetSubEntity(m_CameraHolderEntity.m_pEntity, idx))
        {
            m_OverheadCameraEntity = s_Ent;
        }
    }

    // ensure the overhead camera entity implements
    auto s_CameraInterfaceRef = m_OverheadCameraEntity.QueryInterface<ICameraEntity>();
    if (!s_CameraInterfaceRef)
    {
        Logger::Debug(TAG "Spawned camera entity does not implement ICameraEntity.");
        return false;
    }

    return true;
}

void ZCameraEffect::SetActiveCamera(ZEntityRef& p_pNewCameraEntity, ZEntityRef& p_pPreviousCameraEntity)
{
    if (!p_pNewCameraEntity)
    {
        return;
    }

    TEntityRef<IRenderDestinationEntity> s_RenderDestination;
    Functions::ZCameraManager_GetActiveRenderDestinationEntity->Call(
        Globals::CameraManager,
        &s_RenderDestination
    );

    if (!s_RenderDestination)
    {
        Logger::Debug(TAG "No active render destination entity.");
        return;
    }

    p_pPreviousCameraEntity = *s_RenderDestination.m_pInterfaceRef->GetSource();
    s_RenderDestination.m_pInterfaceRef->SetSource(&p_pNewCameraEntity);
}
