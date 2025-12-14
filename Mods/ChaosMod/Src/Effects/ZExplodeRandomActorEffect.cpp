#include "ZExplodeRandomActorEffect.h"

#include "Glacier/ZMath.h"
#include "Glacier/ZSpatialEntity.h"

#include "Helpers/Utils.h"
#include "Helpers/ZExplosionHelper.h"

#define TAG "[ZExplodeRandomActorEffect] "

void ZExplodeRandomActorEffect::OnEngineInitialized()
{
    m_bIsAvailable = ZExplosionHelper::PreloadResources();
}

void ZExplodeRandomActorEffect::Start()
{
    ZActor* s_Actor = Utils::GetRandomActor(true);
    if (!s_Actor)
    {
        Logger::Warn(TAG "No actor found to explode!");
        return;
    }

    ZEntityRef s_EntityRef;
    s_Actor->GetID(s_EntityRef);

    ZSpatialEntity* s_ActorSpatialEntity = s_EntityRef.QueryInterface<ZSpatialEntity>();
    auto s_ActorPos = s_ActorSpatialEntity->GetWorldMatrix();

    // offset a bit upwards so it doesn't clip into the ground
    s_ActorPos.Trans.z += 0.25f;

    // FIXME: yes, this leaks memory. no, i don't care.
    auto s_Explosion = new ZExplosionHelper();
    s_Explosion->SetPosition(s_ActorPos);
    s_Explosion->SetDeathContext(EDeathContext::eDC_ACCIDENT);
    s_Explosion->SetDeathType(EDeathType::eDT_BLOODY_KILL);
    s_Explosion->Trigger();

    Logger::Info(TAG "Actor '{}' goes boom!", s_Actor->m_sActorName);
    m_pLastTarget = s_Actor;
    m_LastTargetPos = s_ActorPos;
}

void ZExplodeRandomActorEffect::OnDrawDebugUI()
{
    std::string s_LastTargetName = "<null>";
    if (m_pLastTarget)
    {
        s_LastTargetName = m_pLastTarget->m_sActorName;
    }

    ImGui::TextUnformatted(fmt::format("Last Target: {}", s_LastTargetName).c_str());

    if (ImGui::Button("Teleport to last target site"))
    {
        Utils::TeleportPlayerTo(m_LastTargetPos);
    }
}
