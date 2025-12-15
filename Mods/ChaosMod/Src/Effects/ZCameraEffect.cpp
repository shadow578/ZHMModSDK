#include "ZCameraEffect.h"

#include <Globals.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZCameraEntity.h>
#include <Glacier/ZScene.h>
#include <Glacier/SExternalReferences.h>
#include <Glacier/SGameUpdateEvent.h>

#include "Logging.h"

#include "Helpers/Utils.h"

#define TAG "[ZCameraEffect] "

constexpr float32 c_LerpFactor = 1.0f / 2.0f; // 2 seconds to fully interpolate

static std::string ECameraTypeToString(ZCameraEffect::ECameraType p_eCameraType)
{
    switch (p_eCameraType)
    {
    case ZCameraEffect::ECameraType::Overhead:
        return "Overhead";
    case ZCameraEffect::ECameraType::Flipped:
        return "Flipped";
    default:
        return "<" + std::to_string(static_cast<int>(p_eCameraType)) + ">";
    }
}

std::string ZCameraEffect::GetName()
{
    return "ZCameraEffect_" + ECameraTypeToString(m_eCameraType);
}

std::string ZCameraEffect::GetDisplayName()
{
    switch (m_eCameraType)
    {
    case ECameraType::Overhead:
        return "GTA1 Mode";
    case ECameraType::Flipped:
        return "Turn Turtle";
    default:
        return GetName();
    }
}

void ZCameraEffect::Start()
{
    if (!SpawnCameraEntity())
    {
        Logger::Debug(TAG "Could not spawn overhead camera entity, aborting effect.");
        m_bIsAvailable = false;
        return;
    }

    SetActiveCamera(m_OverheadCameraEntity, m_OriginalCameraEntity);
    m_bApplyCameraTransform = true;
    m_fLerpPoint = 0.0f;
}

void ZCameraEffect::Stop()
{
    ZEntityRef s_Dummy;
    SetActiveCamera(m_OriginalCameraEntity, s_Dummy);
    m_bApplyCameraTransform = false;
}

void ZCameraEffect::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    if (!m_bApplyCameraTransform)
    {
        return;
    }

    auto s_CameraSpatialEntity = m_OverheadCameraEntity.QueryInterface<ZSpatialEntity>();
    auto s_TargetWM = s_CameraSpatialEntity->GetWorldMatrix();

    switch (m_eCameraType)
    {
    case ECameraType::Overhead:
    {
        // copy player translation
        auto s_Player = SDK()->GetLocalPlayer();
        auto s_PlayerSpatialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
        auto s_PlayerWM = s_PlayerSpatialEntity->GetWorldMatrix();

        s_TargetWM.Trans = s_PlayerWM.Trans;
        s_TargetWM.Trans.z += m_fOverheadDistance; // elevate

        // lerp to target transform
        s_TargetWM.Trans = Utils::Lerp(
            s_PlayerWM.Trans,
            s_TargetWM.Trans,
            m_fLerpPoint
        );

        // rotate facing downwards
        s_TargetWM.XAxis = float4(0.0f, 1.0f, 0.0f, 0.0f);
        s_TargetWM.YAxis = float4(-1.0f, 0.0f, 0.0f, 0.0f);
        s_TargetWM.ZAxis = float4(1.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    case ECameraType::Flipped:
    {
        // copy stock player camera
        auto s_OriginalCameraSpatialEntity = m_OriginalCameraEntity.QueryInterface<ZSpatialEntity>();
        s_TargetWM = s_OriginalCameraSpatialEntity->GetWorldMatrix();

        // rotate around forward axis by 180 degrees to flip the screen
        constexpr auto s_TargetRotation = 3.14159f; // 180 degrees
        const auto s_RotationMatrix = SMatrix::RotationAxisAngle(
            (-s_TargetWM.Backward).Normalized(),
            Utils::Lerp(0.0f, s_TargetRotation, m_fLerpPoint)
        );
        s_TargetWM = s_RotationMatrix * s_TargetWM;
        break;
    }
    default:
        break;
    }

    s_CameraSpatialEntity->SetWorldMatrix(s_TargetWM);

    m_fLerpPoint += c_LerpFactor * p_UpdateEvent.m_GameTimeDelta.ToSeconds();
}

void ZCameraEffect::OnDrawDebugUI()
{
    if (ImGui::BeginCombo("Camera Type", ECameraTypeToString(m_eCameraType).c_str()))
    {
        for (int i = 0; i < static_cast<int>(ECameraType::NUMBER_OF_TYPES); i++)
        {
            auto s_eType = static_cast<ECameraType>(i);
            if (ImGui::Selectable(
                ECameraTypeToString(static_cast<ECameraType>(i)).c_str(),
                s_eType == m_eCameraType
            ))
            {
                m_eCameraType = s_eType;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Checkbox("Update Camera World Matrix", &m_bApplyCameraTransform);

    switch (m_eCameraType)
    {
        case ECameraType::Overhead:
            ImGui::DragFloat("Overhead Distance", &m_fOverheadDistance, 0.1f, 1.0f, 100.0f);
            break;
        case ECameraType::Flipped:
            break;
    }
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
