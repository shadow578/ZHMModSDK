#pragma once

#include "IChaosModule.h"

#include "Glacier/ZMath.h"

class ZTeleportUpModule : public IChaosModule
{
    void Trigger() override;
    std::string GetName() override;
};