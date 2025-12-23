#include "ZCameraEffectBase.h"

#include <Globals.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZScene.h>
#include <Glacier/SExternalReferences.h>

#include "Logging.h"

#include "Helpers/Utils.h"

#define TAG "[ZCameraEffectBase] "

constexpr uint64_t c_nMainHitmanCameraEntityId = 1520968122659560567;
constexpr uint64_t c_nMainHitmanCameraOwningEntityId = 5884861939421351664;

void ZCameraEffectBase::Start()
{
    if (!EnsureCameraEntity())
    {
        Logger::Debug(TAG "Could not spawn camera entity.");
        m_bIsAvailable = false;
        return;
    }

    if (!GetActiveCamera(m_OriginalCameraEntity))
    {
        Logger::Error(TAG "Could not get active camera entity.");
        return;
    }

    if (!IsMainHitmanCamera(m_OriginalCameraEntity))
    {
        Logger::Error(TAG "Active camera entity is not the main Hitman camera, aborting");
        return;
    }

    m_bEffectCameraActive = SetActiveCamera(m_EffectCameraEntity);
}

void ZCameraEffectBase::Stop()
{
    if (!m_OriginalCameraEntity)
    {
        return;
    }

    if (!SetActiveCamera(m_OriginalCameraEntity))
    {
        Logger::Error(TAG "Could not restore original camera entity.");
    }

    m_bEffectCameraActive = false;
    m_OriginalCameraEntity = {};
}

void ZCameraEffectBase::OnClearScene()
{
    m_bEffectCameraActive = false;
    m_EffectCameraEntity = {};
    m_OriginalCameraEntity = {};
}

void ZCameraEffectBase::OnDrawDebugUI()
{
    ImGui::TextUnformatted(fmt::format("Effect Camera Active: {}", m_bEffectCameraActive ? "Yes" : "No").c_str());
    ImGui::TextUnformatted(fmt::format("Effect Camera Entity: {}", m_EffectCameraEntity ? "Valid" : "Invalid").c_str());
    ImGui::TextUnformatted(fmt::format("Original Camera Entity: {}", m_OriginalCameraEntity ? "Valid" : "Invalid").c_str());
}

bool ZCameraEffectBase::IsCompatibleWith(const IChaosEffect* p_pOther) const
{
    return IChaosEffect::IsCompatibleWith(p_pOther) 
        // all camera effects are incompatible with each other
        && !Utils::IsInstanceOf<ZCameraEffectBase>(p_pOther);
}

bool ZCameraEffectBase::EnsureCameraEntity()
{
    // only once
    if (m_EffectCameraEntity)
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
            m_EffectCameraEntity = s_Ent;
        }
    }

    // ensure the overhead camera entity implements
    auto s_CameraInterfaceRef = m_EffectCameraEntity.QueryInterface<ICameraEntity>();
    if (!s_CameraInterfaceRef)
    {
        Logger::Debug(TAG "Spawned camera entity does not implement ICameraEntity.");
        return false;
    }

    return true;
}

bool ZCameraEffectBase::GetActiveCamera(ZEntityRef& p_ActiveCameraEntity)
{
    TEntityRef<IRenderDestinationEntity> s_RenderDestination;
    if (!GetRenderDestinationEntity(s_RenderDestination))
    {
        return false;
    }

    auto s_pActiveCameraEntity = s_RenderDestination.m_pInterfaceRef->GetSource();
    if (!s_pActiveCameraEntity)
    {
        return false;
    }

    p_ActiveCameraEntity = *s_pActiveCameraEntity;
    return true;
}

bool ZCameraEffectBase::SetActiveCamera(ZEntityRef& p_NewCameraEntity)
{
    if (!p_NewCameraEntity)
    {
        return false;
    }

    TEntityRef<IRenderDestinationEntity> s_RenderDestination;
    if (!GetRenderDestinationEntity(s_RenderDestination))
    {
        return false;
    }

    s_RenderDestination.m_pInterfaceRef->SetSource(&p_NewCameraEntity);
    return true;
}

bool ZCameraEffectBase::IsMainHitmanCamera(ZEntityRef& p_CameraEntity)
{
    uint64_t s_nCameraEntityId = 0,
        s_nOwningEntityId = 0;

    // TODO: holy f this is ugly
    if (const auto s_pCameraEntity = p_CameraEntity.GetEntity())
    {
        if (const auto s_pCameraEntityType = s_pCameraEntity->GetType())
        {
            s_nCameraEntityId = s_pCameraEntityType->m_nEntityId;
        }

        if (const auto s_OwningEntity = p_CameraEntity.GetOwningEntity())
        {
            if (const auto s_pOwningEntity = s_OwningEntity.GetEntity())
            {
                if (const auto s_pOwningEntityType = s_pOwningEntity->GetType())
                {
                    s_nOwningEntityId = s_pOwningEntityType->m_nEntityId;
                }
            }
        }
    }

    return s_nCameraEntityId == c_nMainHitmanCameraEntityId && 
        s_nOwningEntityId == c_nMainHitmanCameraOwningEntityId;
}

bool ZCameraEffectBase::GetRenderDestinationEntity(TEntityRef<IRenderDestinationEntity>& p_RenderDestinationEntity)
{
    Functions::ZCameraManager_GetActiveRenderDestinationEntity->Call(
        Globals::CameraManager,
        &p_RenderDestinationEntity
    );

    if (!p_RenderDestinationEntity)
    {
        Logger::Debug(TAG "No active render destination entity.");
        return false;
    }

    return true;
}
