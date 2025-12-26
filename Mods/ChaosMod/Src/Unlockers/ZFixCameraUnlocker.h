#pragma once
#include "IUnlocker.h"

class ZFixCameraUnlocker : public IUnlocker
{
public:
    void Start() override;

    std::string GetDisplayName() const override
    {
        return "Fix Player Camera";
    }
};
