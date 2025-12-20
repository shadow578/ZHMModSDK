#pragma once

#include <memory>
#include <vector>

#include "IChaosEffect.h"
#include "Logging.h"


class EffectRegistry
{
private:
    EffectRegistry() = default;
    std::vector<std::unique_ptr<IChaosEffect>> m_aEffects;

public:
    static EffectRegistry& GetInstance()
    {
        // Note: g_Registry is *intentionally* leaked to avoid static deinitialization order issues.
        static EffectRegistry* g_Registry = new EffectRegistry();
        return *g_Registry;
    }

    void Register(std::unique_ptr<IChaosEffect> p_Effect)
    {
        Logger::Debug("[EffectRegistry] Registered effect '{}'", p_Effect->GetName());
        m_aEffects.push_back(std::move(p_Effect));
    }

    const std::vector<std::unique_ptr<IChaosEffect>>& GetEffects() const
    {
        return m_aEffects;
    }
};

struct EffectRegistrar
{
    explicit EffectRegistrar(std::unique_ptr<IChaosEffect> p_Effect)
    {
        EffectRegistry::GetInstance().Register(std::move(p_Effect));
    }
};

/**
 * Register an effect class with the EffectRegistry.
 * Effecs should be registered in the global scope of the Effect's .cpp file.
 * Examples:
 * REGISTER_EFFECT(MySimpleEffect);
 * REGISTER_EFFECT(MyParameterizedEffect, constructor_arg1, constructor_arg2);
 */
#define REGISTER_CHAOS_EFFECT(EFFECT_CLASS, ...)                   \
    static EffectRegistrar g_EffectRegistrar_##EFFECT_CLASS( \
        std::make_unique<EFFECT_CLASS>(__VA_ARGS__)          \
    );
