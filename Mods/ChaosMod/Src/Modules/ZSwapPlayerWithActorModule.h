#pragma once

#include "IChaosModule.h"

#include "Glacier/ZMath.h"

class ZSwapPlayerWithActorModule : public IChaosModule
{
    void OnDrawDebugUI() override;

    void Trigger() override;
    std::string GetName() override;


private:
    SMatrix m_PreviousPlayerPosition;
};