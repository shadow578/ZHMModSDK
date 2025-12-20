#pragma once

#include "ZCameraFOVEffectBase.h"

class ZCameraZoomFOVEffect : public ZCameraFOVEffectBase
{
public:
    ZCameraZoomFOVEffect() : ZCameraFOVEffectBase(10.0f)
    {
    }

    std::string GetDisplayName() override
    {
        return "Binoculars";
    }
};

class ZCameraWideFOVEffect : public ZCameraFOVEffectBase
{
public:
    ZCameraWideFOVEffect() : ZCameraFOVEffectBase(120.0f)
    {
    }

    std::string GetDisplayName() override
    {
        return "Quake PRO";
    }
};
