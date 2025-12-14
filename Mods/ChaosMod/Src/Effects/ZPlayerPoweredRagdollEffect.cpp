#include "ZPlayerPoweredRagdollEffect.h"

#include "IPluginInterface.h"
#include "Functions.h"

#include <Glacier/ZHitman5.h>

void ZPlayerPoweredRagdollEffect::Start()
{
    auto s_Player = SDK()->GetLocalPlayer();
    if (!s_Player.m_pInterfaceRef)
    {
        return;
    }

    // 47 goes floppy, but can still be controlled
    Functions::ZHM5BaseCharacter_ActivatePoweredRagdoll->Call(
        s_Player.m_pInterfaceRef,
        0,
        false,
        true,
        0,
        true
    );
}

void ZPlayerPoweredRagdollEffect::Stop()
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

bool ZPlayerPoweredRagdollEffect::Available()
{
    return Functions::ZHM5BaseCharacter_ActivatePoweredRagdoll->Exists() &&
        Functions::ZHM5BaseCharacter_DeactivateRagdoll->Exists();
}
