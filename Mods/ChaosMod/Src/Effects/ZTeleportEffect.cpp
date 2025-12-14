#include "ZTeleportEffect.h"

#include "Glacier/ZSpatialEntity.h"

#include "Helpers/Utils.h"

void ZTeleportEffect::Start()
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_ref)
    {
        return;
    }

    auto s_PlayerSpatialEntity = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
    if (!s_PlayerSpatialEntity)
    {
        return;
    }

    auto s_Pos = s_PlayerSpatialEntity->GetWorldMatrix();

    s_Pos.Trans.z += Utils::GetRandomNumber(1.0f, 100.0f);

    s_PlayerSpatialEntity->SetWorldMatrix(s_Pos);
}
