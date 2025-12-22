#pragma once
#include "IChaosEffect.h"

class ZNoHUDEffect : public IChaosEffect
{
public:
    void Start() override;
    void Stop() override;

private:
    void SetHUDVisibility(const bool p_bVisible);
};
