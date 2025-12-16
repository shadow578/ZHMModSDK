#pragma once

#include "Globals.h"

#include "Glacier/ZActor.h"
#include "Glacier/ZMath.h"

namespace Utils
{
    template <typename T>
    static T GetRandomNumber(const T p_Min, const T p_Max)
    {
        static_assert(std::is_arithmetic<T>::value, "GetRandomNumber only supports arithmetic types.");

        if constexpr (std::is_integral<T>::value)
        {
            const auto s_Range = p_Max - p_Min + 1;
            const auto s_RandomValue = rand() % s_Range;
            return p_Min + s_RandomValue;
        }
        else
        {
            const auto s_RandomValue = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
            return p_Min + s_RandomValue * (p_Max - p_Min);
        }
    }

    template <typename T>
    static T Lerp(const T& p_A, const T& p_B, const float32 p_fT)
    {
        if (p_fT <= 0.0f)
        {
            return p_A;
        }
        else if (p_fT >= 1.0f)
        {
            return p_B;
        }

        return p_A + (p_B - p_A) * p_fT;
    }

    SMatrix LerpAffine(const SMatrix& p_A, const SMatrix& p_B, const float32 p_fT);

    ZActor* GetRandomActor(const bool p_bRequireAlive);

    bool TeleportPlayerTo(const SMatrix p_Position);
};