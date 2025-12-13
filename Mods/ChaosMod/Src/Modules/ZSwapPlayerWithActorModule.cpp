#include "ZSwapPlayerWithActorModule.h"

#include "Glacier/ZSpatialEntity.h"

#include "Helpers/Utils.h"

#define TAG "[ZSwapPlayerWithActorModule] "

void ZSwapPlayerWithActorModule::Trigger()
{
    auto s_Actor = GetRandomActor(false);
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Actor || !s_Player.m_ref)
    {
        return;
    }

    ZEntityRef s_ActorRef;
    s_Actor->GetID(s_ActorRef);
    auto s_ActorSpartialEntity = s_ActorRef.QueryInterface<ZSpatialEntity>();

    auto s_PlayerSpartialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();

    if (!s_ActorSpartialEntity || !s_PlayerSpartialEntity)
    {
        return;
    }

    auto s_ActorWM = s_ActorSpartialEntity->GetWorldMatrix();
    auto s_PlayerWM = s_PlayerSpartialEntity->GetWorldMatrix();

    s_ActorSpartialEntity->SetWorldMatrix(s_PlayerWM);
    s_PlayerSpartialEntity->SetWorldMatrix(s_ActorWM);

    Logger::Info(TAG "Swapped player with actor '{}'", s_Actor->m_sActorName);

    m_PreviousPlayerPosition = s_PlayerWM;
}

std::string ZSwapPlayerWithActorModule::GetName()
{
    return "ZSwapPlayerWithActor";
}

void ZSwapPlayerWithActorModule::OnDrawDebugUI()
{
    if (ImGui::Button("Restore Last Player Position"))
    {
        auto s_Player = SDK()->GetLocalPlayer();
        if (!s_Player.m_ref)
        {
            return;
        }

        auto s_PlayerSpartialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
        if (!s_PlayerSpartialEntity)
        {
            return;
        }

        s_PlayerSpartialEntity->SetWorldMatrix(m_PreviousPlayerPosition);
    }
}