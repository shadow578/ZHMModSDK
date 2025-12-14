#pragma once

#include "IChaosEffect.h"

class ZFakeCrashEffect : public IChaosEffect
{
    void Start() override;

    bool Available() override;

    std::string GetDisplayName() override
    {
        return "Fake Crash";
    }

private:

    void SuspendAllThreads();
};