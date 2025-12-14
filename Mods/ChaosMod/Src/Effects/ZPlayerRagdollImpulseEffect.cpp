#include "ZPlayerRagdollImpulseEffect.h"

#include "IPluginInterface.h"
#include "Functions.h"

#include <Glacier/ZHitman5.h>
#include <Glacier/ZSpatialEntity.h>

void ZPlayerRagdollImpulseEffect::Start()
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_pInterfaceRef)
    {
        return;
    }

    Functions::ZHM5BaseCharacter_ActivateRagdoll->Call(s_Player.m_pInterfaceRef, false);

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

void ZPlayerRagdollImpulseEffect::Stop()
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_pInterfaceRef)
    {
        return;
    }

    Functions::ZHM5BaseCharacter_DeactivateRagdoll->Call(s_Player.m_pInterfaceRef);

    // play animation to get up
    auto s_pAnimator = s_Player.m_pInterfaceRef->m_Animator.QueryInterface<ZHM5Animator>();
    auto s_fTime = 1.0f;

    if (s_pAnimator && Functions::ZHM5Animator_ActivateRagdollToAnimationBlend->Exists())
    {
        Functions::ZHM5Animator_ActivateRagdollToAnimationBlend->Call(s_pAnimator, &s_fTime);
    }
}

bool ZPlayerRagdollImpulseEffect::Available()
{
    return Functions::ZHM5BaseCharacter_ActivateRagdoll->Exists() &&
        Functions::ZHM5BaseCharacter_DeactivateRagdoll->Exists();
}
