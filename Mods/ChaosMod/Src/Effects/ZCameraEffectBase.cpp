#include "ZCameraEffectBase.h"

#include <Globals.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZCameraEntity.h>
#include <Glacier/ZScene.h>
#include <Glacier/SExternalReferences.h>

#include "Logging.h"


#define TAG "[ZCameraEffectBase] "

void ZCameraEffectBase::Start()
{
    if (!EnsureCameraEntity())
    {
        Logger::Debug(TAG "Could not spawn camera entity.");
        m_bIsAvailable = false;
        return;
    }

    m_bEffectCameraActive = SetActiveCamera(m_EffectCameraEntity, m_OriginalCameraEntity);
}

void ZCameraEffectBase::Stop()
{
    ZEntityRef dummy;
    SetActiveCamera(m_OriginalCameraEntity, dummy);

    m_bEffectCameraActive = false;
    m_OriginalCameraEntity = {};
}

void ZCameraEffectBase::OnClearScene()
{
    m_bEffectCameraActive = false;
    m_EffectCameraEntity = {};
    m_OriginalCameraEntity = {};
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

bool ZCameraEffectBase::SetActiveCamera(ZEntityRef& p_pNewCameraEntity, ZEntityRef& p_pPreviousCameraEntity)
{
    if (!p_pNewCameraEntity)
    {
        return false;
    }

    TEntityRef<IRenderDestinationEntity> s_RenderDestination;
    Functions::ZCameraManager_GetActiveRenderDestinationEntity->Call(
        Globals::CameraManager,
        &s_RenderDestination
    );

    if (!s_RenderDestination)
    {
        Logger::Debug(TAG "No active render destination entity.");
        return false;
    }

    p_pPreviousCameraEntity = *s_RenderDestination.m_pInterfaceRef->GetSource();
    s_RenderDestination.m_pInterfaceRef->SetSource(&p_pNewCameraEntity);
    return true;
}
