#pragma once

#include "IChaosModule.h"

#include "Glacier/ZMath.h"

class ZExplodeRandomActorModule : public IChaosModule
{
    void OnEngineInitialized() override;
    void Trigger() override;
    std::string GetName() override;

    void OnDrawDebugUI() override;

private:
    ZActor* m_pLastTarget = nullptr;
    SMatrix m_LastTargetPos;
};