#pragma once
#include "IUnlocker.h"

class ZFixPlayerPositionUnlocker : public IUnlocker
{
public:
    void Start() override;

    std::string GetDisplayName() const override
    {
        return "Fix Player Position";
    }
};
