#include "ZTeleportUp.h"

#include "Glacier/ZSpatialEntity.h"

#include "Helpers/Utils.h"

void ZTeleportUpModule::Trigger()
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

    auto s_Pos = s_PlayerSpartialEntity->GetWorldMatrix();

    s_Pos.Trans.z += GetRandomNumber(1.0f, 100.0f);

    s_PlayerSpartialEntity->SetWorldMatrix(s_Pos);
}

std::string ZTeleportUpModule::GetName()
{
    return "ZTeleportUp";
}