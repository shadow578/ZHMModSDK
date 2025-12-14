#include "Utils.h"

#include "IPluginInterface.h"

#include "Glacier/ZSpatialEntity.h"

ZActor *Utils::GetRandomActor(const bool p_bRequireAlive)
{
    // Check if we have any actors available
    if (!Globals::NextActorId || *Globals::NextActorId <= 0)
    {
        return nullptr;
    }

    // limit tries to avoid infinite loops
    for (int t = 0; t < 10; t++)
    {
        // random actor id between 0 and NextActorId
        const int s_ActorId = GetRandomNumber<int>(0, *Globals::NextActorId);
        auto s_Actor = Globals::ActorManager->m_aActiveActors[s_ActorId].m_pInterfaceRef;

        if (!s_Actor)
        {
            continue;
        }

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