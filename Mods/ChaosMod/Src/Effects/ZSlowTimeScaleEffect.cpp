#include "ZSlowTimeScaleEffect.h"

#include "Globals.h"

#include "Glacier/ZGameTime.h"

constexpr float32 g_fSlowTimeScale = 0.2f;

void ZSlowTimeScaleEffect::Start()
{
    m_fPreviousTimeScale = Globals::GameTimeManager->m_fTimeMultiplier1;
    Globals::GameTimeManager->m_fTimeMultiplier1 = g_fSlowTimeScale;
}

void ZSlowTimeScaleEffect::Stop()
{
    Globals::GameTimeManager->m_fTimeMultiplier1 = m_fPreviousTimeScale;
}

std::string ZSlowTimeScaleEffect::GetDisplayName()
{
    return fmt::format("{:.1f}x Game Speed", g_fSlowTimeScale);
}
