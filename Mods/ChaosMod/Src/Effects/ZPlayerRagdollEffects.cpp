#include "ZPlayerRagdollEffects.h"
#include "IPluginInterface.h"
#include "Functions.h"

#include <Glacier/ZHitman5.h>
#include <Glacier/ZSpatialEntity.h>

#include "EffectRegistry.h"

void ZPlayerRagdollWithImpulseEffect::Start()
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_pInterfaceRef)
    {
        return;
    }

    ZPlayerRagdollEffectBase::Start();

    const auto s_HitmanSpatial = s_Player.m_ref.QueryInterface<ZSpatialEntity>();
    auto s_WM = s_HitmanSpatial->GetWorldMatrix();

    Functions::ZRagdollHandler_ApplyImpulseOnRagdoll->Call(
        s_Player.m_pInterfaceRef->m_pRagdollHandler,
        {},
        (-s_WM.Backward * 220.f) + (s_WM.Up * 550.f),
        1,
        false
    );
}

REGISTER_CHAOS_EFFECT(ZPlayerSimpleRagdollEffect)
REGISTER_CHAOS_EFFECT(ZPlayerPoweredRagdollEffect)
REGISTER_CHAOS_EFFECT(ZPlayerRagdollWithImpulseEffect)
