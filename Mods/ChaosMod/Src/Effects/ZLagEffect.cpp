#include "ZLagEffect.h"


void ZLagEffect::Start()
{
    m_bEnabled = true;
}

void ZLagEffect::Stop()
{
    m_bEnabled = false;
}

void ZLagEffect::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    if (m_bEnabled)
    {
        volatile double dummy = 0.0;
        for (int i = 0; i < 5000000; ++i)
        {
            dummy += std::sin(i) * std::cos(i);
        }
    }
}