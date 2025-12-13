#pragma once

#include "Globals.h"

#include "Glacier/ZActor.h"

template <typename T>
T GetRandomNumber(const T p_Min, const T p_Max)
{
    static_assert(std::is_arithmetic<T>::value, "GetRandomNumber only supports arithmetic types.");

    if constexpr (std::is_integral<T>::value)
    {
        const auto s_Range = p_Max - p_Min;
        const auto s_RandomValue = rand() % s_Range;
        return p_Min + s_RandomValue;
    }
    else
    {
        const auto s_RandomValue = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
        return p_Min + s_RandomValue * (p_Max - p_Min);
    }
}

static ZActor* GetRandomActor(const bool p_bRequireAlive)
{
    // limit tries
    for (int t = 0; t < 10; t++)
    {
        // random actor id between 0 and NextActorId
        const int s_ActorId = GetRandomNumber<int>(0, *Globals::NextActorId);
        auto s_Actor = Globals::ActorManager->m_aActiveActors[s_ActorId].m_pInterfaceRef;

        auto s_bIsAlive = !s_Actor->IsDead() && !s_Actor->IsPacified();

        if (!p_bRequireAlive || s_bIsAlive)
        {
            return s_Actor;
        }
    }

    return nullptr;
}