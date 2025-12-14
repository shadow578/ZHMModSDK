#pragma once

#include "IChaosEffect.h"

class ZFakeCrashEffect : public IChaosEffect
{
    void Start() override;

    std::string GetDisplayName() override
    {
        return "Fake Crash";
    }

private:

    void SuspendAllThreads();
};