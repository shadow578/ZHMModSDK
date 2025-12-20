#include "ZSlowTimeScaleEffect.h"

#include "Globals.h"

#include "Glacier/ZGameTime.h"

#include "EffectRegistry.h"

void ZSlowTimeScaleEffect::Start()
{
    m_fPreviousTimeScale = Globals::GameTimeManager->m_fTimeMultiplier1;
    Globals::GameTimeManager->m_fTimeMultiplier1 = m_fTargetTimeScale;
}

void ZSlowTimeScaleEffect::Stop()
{
    Globals::GameTimeManager->m_fTimeMultiplier1 = m_fPreviousTimeScale;
}

std::string ZSlowTimeScaleEffect::GetDisplayName()
{
    return fmt::format("{:.1f}x Game Speed", m_fTargetTimeScale);
}

REGISTER_CHAOS_EFFECT(ZSlowTimeScaleEffect, 0.2);
