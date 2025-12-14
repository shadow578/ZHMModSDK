#include "Utils.h"

#include "IPluginInterface.h"

#include "Glacier/ZSpatialEntity.h"

ZActor *Utils::GetRandomActor(const bool p_bRequireAlive)
{
    // limit tries
    for (int t = 0; t < 10; t++)
    {
        // random actor id between 0 and NextActorId
        const int s_ActorId = GetRandomNumber<int>(0, *Globals::NextActorId);
        auto s_Actor = Globals::ActorManager->m_aActiveActors[s_ActorId].m_pInterfaceRef;

        auto s_bIsAlive = !s_Actor->IsDead() && !s_Actor->IsPacified();

        if (!p_bRequireAlive || s_bIsAlive)
        {
            return s_Actor;
        }
    }

    return nullptr;
}

bool Utils::TeleportPlayerTo(const SMatrix p_Position)
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_ref)
    {
        return false;
    }

    auto s_PlayerSpatialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
    if (!s_PlayerSpatialEntity)
    {
        return false;
    }

    s_PlayerSpatialEntity->SetWorldMatrix(p_Position);
    return true;
}