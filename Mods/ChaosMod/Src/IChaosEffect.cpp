#include "IChaosEffect.h"

#include <typeinfo>

std::string IChaosEffect::GetName()
{
    return typeid(*this).name();
}

std::string IChaosEffect::GetDisplayName()
{
    return GetName();
}