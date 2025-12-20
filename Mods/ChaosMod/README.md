# Hitman 3 Chaos Mod

the Hitman 3 chaos mod injects random effects into your gameplay to create unpredictable and entertaining scenarios.
heavily inspired by the [GTA V chaos mod](https://github.com/gta-chaos-mod/ChaosModV).

## Installation Instructions

no additional resources are required to run this mod.

## Usage Instructions

once loaded, enable "chaos mode" from the mod menu.
good luck!

## Development Instructions

to develop new effects, you'll need to write a new class implementing `IChaosEffect` and - specifically - the `Start` method.
from there, use the `REGISTER_CHAOS_EFFECT` macro to register your effect with the chaos mod.
reusable effect bases (like for interpolating in/out) are also available, and are located in the `Effects/Base` folder.

minimal mod example:

```cpp
// MyChaosEffect.h
#pragma once
#include "IChaosEffect.h"

class MyChaosEffect : public IChaosEffect {
public:
    void Start() override;

    std::string GetDisplayName() override
    {
      return "My Chaos Effect";
    }
};

// MyChaosEffect.cpp
#include "MyChaosEffect.h"

void MyChaosEffect::Start() {
    // Your effect logic here
}

REGISTER_CHAOS_EFFECT(MyChaosEffect);
```

refer to existing effects in the `Effects` folder for more complex examples.
