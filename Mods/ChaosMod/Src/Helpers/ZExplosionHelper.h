#pragma once

#include "IPluginInterface.h"

#include "Glacier/ZResource.h"

class ZExplosionHelper
{
public:
    static bool PreloadResources();

    ZExplosionHelper();

    void SetPosition(const SMatrix& p_Position);
    void SetFuseTime(const float32 p_TimeMin, const float32 p_TimeMax);
    void SetDeathContext(const EDeathContext p_eDeathContext);
    void SetDeathType(const EDeathType p_eDeathType);
    void SetTargetStrength(const float32 p_fTargetStrength);
    void Trigger();

private:
    bool IsValid();

    static bool SpawnExplosionProp(ZEntityRef& p_EntityRef, ZEntityRef& p_ExplosionEntity, ZEntityRef& p_TriggerEntityRef);
    static bool GetExplosionPropFactory(TResourcePtr<ZTemplateEntityFactory>& p_PropRTFactory, TResourcePtr<ZTemplateEntityBlueprintFactory>& p_PropRTBpFactory);

private:
    ZEntityRef m_RootEntityRef;
    ZEntityRef m_ExplosionEntityRef;
    ZEntityRef m_TriggerEntityRef;
};