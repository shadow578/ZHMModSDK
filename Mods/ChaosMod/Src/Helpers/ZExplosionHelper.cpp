#include "ZExplosionHelper.h"

#include <Glacier/ZScene.h>
#include <Glacier/ZSpatialEntity.h>
#include <Glacier/ZModule.h>
#include <Glacier/SExternalReferences.h>

#include "Logging.h"

#define TAG "[ExplosionHelper] "

bool ZExplosionHelper::PreloadResources()
{
    TResourcePtr<ZTemplateEntityFactory> s_PropRTFactory;
    TResourcePtr<ZTemplateEntityBlueprintFactory> s_PropRTBpFactory;
    if (!GetExplosionPropFactory(s_PropRTFactory, s_PropRTBpFactory))
    {
        Logger::Debug(TAG "Resource preload failed.");
        return false;
    }

    return true;
}

ZExplosionHelper::ZExplosionHelper()
{
    SpawnExplosionProp(
        m_RootEntityRef, 
        m_ExplosionEntityRef,
        m_TriggerEntityRef
   );
}

void ZExplosionHelper::SetPosition(const SMatrix& p_Position)
{
    if (!IsValid())
    {
        return;
    }

    ZSpatialEntity* s_SpatialEntity = m_RootEntityRef.QueryInterface<ZSpatialEntity>();
    s_SpatialEntity->SetWorldMatrix(p_Position);
}

void ZExplosionHelper::SetFuseTime(float32 p_TimeMin, float32 p_TimeMax)
{
    if (!IsValid())
    {
        return;
    }

    m_TriggerEntityRef.SetProperty("m_fTimeMin", ZObjectRef::From(p_TimeMin));
    m_TriggerEntityRef.SetProperty("m_fTimeMax", ZObjectRef::From(p_TimeMax));
}

void ZExplosionHelper::SetDeathContext(const EDeathContext p_eDeathContext)
{
    if (!IsValid())
    {
        return;
    }

    m_RootEntityRef.SetProperty("m_eDeathContext", ZObjectRef::From(p_eDeathContext));
}

void ZExplosionHelper::SetDeathType(const EDeathType p_eDeathType)
{
    if (!IsValid())
    {
        return;
    }

    m_RootEntityRef.SetProperty("m_eDeathType", ZObjectRef::From(p_eDeathType));
}

void ZExplosionHelper::SetTargetStrength(const float32 p_fTargetStrength)
{
    if (!IsValid())
    {
        return;
    }

    m_RootEntityRef.SetProperty("m_fTargetStrength", ZObjectRef::From(p_fTargetStrength));
}

void ZExplosionHelper::Trigger()
{
    if (!IsValid())
    {
        return;
    }

    m_TriggerEntityRef.SignalInputPin("Start");
}

bool ZExplosionHelper::IsValid()
{
    return m_RootEntityRef && m_TriggerEntityRef;
}

bool ZExplosionHelper::SpawnExplosionProp(ZEntityRef& p_RootEntityRef, ZEntityRef& p_ExplosionEntity, ZEntityRef &p_TriggerEntityRef)
{
    const auto s_Scene = Globals::Hitman5Module->m_pEntitySceneContext->m_pScene;
    if (!s_Scene)
    {
        Logger::Debug(TAG "Cannot spawn prop, no scene loaded.");
        return false;
    }

    TResourcePtr<ZTemplateEntityFactory> s_PropRTFactory;
    TResourcePtr<ZTemplateEntityBlueprintFactory> s_PropRTBpFactory;
    if (!GetExplosionPropFactory(s_PropRTFactory, s_PropRTBpFactory))
    {
        Logger::Debug(TAG "Could not get factory.");
        return false;
    }

    SExternalReferences s_ExternalRefs;

    Functions::ZEntityManager_NewEntity->Call(
        Globals::EntityManager,
        p_RootEntityRef,
        "StandaloneExplosion_ROOT",
        s_PropRTFactory,
        s_Scene.m_ref,
        s_ExternalRefs,
        -1
    );

    if (!p_RootEntityRef)
    {
        Logger::Debug(TAG "Spawning explosion entity failed.");
        return false;
    }

    Logger::Debug(TAG "Spawned explosion entity successfully.");

    // "PropHelper_Explosive" does the explosion itself
    if (const auto idx = s_PropRTBpFactory.GetResource()->GetSubEntityIndex(0xfeedfeedfeedfeec); idx != -1)
    {
        if (const auto s_Ent = s_PropRTBpFactory.GetResource()->GetSubEntity(p_RootEntityRef.m_pEntity, idx))
        {
            p_ExplosionEntity = s_Ent;
        }
    }

    // "TimerRandom" acts as the trigger
    if (const auto idx = s_PropRTBpFactory.GetResource()->GetSubEntityIndex(0xfeedfeedfeedfee0); idx != -1)
    {
        if (const auto s_Ent = s_PropRTBpFactory.GetResource()->GetSubEntity(p_RootEntityRef.m_pEntity, idx))
        {
            p_TriggerEntityRef = s_Ent;
        }
    }

    if (!p_ExplosionEntity || !p_TriggerEntityRef)
    {
        Logger::Debug(TAG "Failed to get explosion trigger entity.");
        return false;
    }

    return true;
}

bool ZExplosionHelper::GetExplosionPropFactory(TResourcePtr<ZTemplateEntityFactory>& p_PropRTFactory, TResourcePtr<ZTemplateEntityBlueprintFactory>& p_PropRTBpFactory)
{
    static const std::string s_PropJson =
        R"(
{
	"tempHash": "00644fe9eb9feff5",
	"tbluHash": "005474211f99b411",
	"rootEntity": "feedfeedfeedfeed",
	"entities": {
		"feedfeedfeedfeed": {
			"parent": null,
			"name": "ZExplosionHeler_ROOT",
			"factory": "[modules:/zspatialentity.class].pc_entitytype",
			"blueprint": "[modules:/zspatialentity.class].pc_entityblueprint"
		},
		"feedfeedfeedfeec": {
			"parent": "feedfeedfeedfeed",
			"name": "PropHelper_Explosive",
			"factory": "[assembly:/_pro/design/items/prop_device_ica_modular_runtimes.template?/prophelper_ica_payload_explosion.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/items/prop_device_ica_modular_runtimes.template?/prophelper_ica_payload_explosion.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_mTransform": {
					"type": "SMatrix43",
					"value": {
						"rotation": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"position": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						}
					}
				},
				"m_eidParent": {
					"type": "SEntityTemplateReference",
					"value": "feedfeedfeedfeed"
				}
			}
		},
		"feedfeedfeedfeeb": {
			"parent": "feedfeedfeedfeed",
			"name": "FX_E_Explosion_C4_Large",
			"factory": "[assembly:/_pro/effects/templates/fire/fx_explosions.template?/fx_e_explosion_c4_large.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/effects/templates/fire/fx_explosions.template?/fx_e_explosion_c4_large.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_mTransform": {
					"type": "SMatrix43",
					"value": {
						"rotation": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"position": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						}
					}
				},
				"m_eRoomBehaviour": {
					"type": "ZSpatialEntity.ERoomBehaviour",
					"value": "ROOM_DYNAMIC"
				},
				"m_bForceVisible": {
					"type": "bool",
					"value": false
				},
				"m_eidParent": {
					"type": "SEntityTemplateReference",
					"value": "feedfeedfeedfeed"
				}
			}
		},
		"feedfeedfeedfeea": {
			"parent": "feedfeedfeedfeed",
			"name": "MOCKUP_SFX_DEVICE_AUDIO_EMITTER",
			"factory": "[assembly:/templates/sound/wwise/soundentities.template?/3daudioemitter.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/templates/sound/wwise/soundentities.template?/3daudioemitter.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_mTransform": {
					"type": "SMatrix43",
					"value": {
						"rotation": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"position": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						}
					}
				},
				"m_eidParent": {
					"type": "SEntityTemplateReference",
					"value": "feedfeedfeedfeed"
				},
				"m_bOneShotEmitter": {
					"type": "bool",
					"value": false
				},
				"m_bUseRaycastObstruction": {
					"type": "bool",
					"value": true
				}
			},
			"subsets": {
				"AudioEmitters": [
					"feedfeedfeedfeed"
				]
			}
		},
		"feedfeedfeedfee9": {
			"parent": "feedfeedfeedfeed",
			"name": "play_sfx_explosion",
			"factory": "[modules:/zaudioevententity.class].pc_entitytype",
			"blueprint": "[modules:/zaudioevententity.class].pc_entityblueprint",
			"properties": {
				"m_pMainEvent": {
					"type": "ZRuntimeResourceID",
					"value": {
						"resource": "[assembly:/sound/wwise/exportedwwisedata/events/props_events/prop_device_ica_modular/sfx_prop_device_audiodistraction/play_sfx_prop_device_musicdistraction.wwiseevent].pc_wwisebank",
						"flag": "5F"
					}
				},
				"m_pEmitter": {
					"type": "SEntityTemplateReference",
					"value": "feedfeedfeedfeea"
				},
				"m_eCullingBehavior": {
					"type": "AudioEventCullingBehavior",
					"value": "NoAutoSendStop"
				}
			}
		},
		"feedfeedfeedfee0": {
			"parent": "feedfeedfeedfeed",
			"name": "TimerRandom",
			"factory": "[assembly:/_pro/design/logic.template?/timerrandom.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/logic.template?/timerrandom.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_bEnabled": {
					"type": "bool",
					"value": false
				},
				"m_fMinTime": {
					"type": "float32",
					"value": 0.1
				},
				"m_fMaxTime": {
					"type": "float32",
					"value": 0.3
				}
			},
			"events": {
				"TimeOut": {
					"Trigger": [
						"feedfeedfeedfeec"
					],
					"Activate": [
						"feedfeedfeedfeeb"
					],
					"Detach": [
						"feedfeedfeedfeeb"
					],
					"Send": [
						"feedfeedfeedfee9"
					],
					"Stop": [
						"feedfeedfeedfee0"
					]
				}
			}
		}
	},
	"propertyOverrides": [],
	"overrideDeletes": [],
	"pinConnectionOverrides": [],
	"pinConnectionOverrideDeletes": [],
	"externalScenes": [],
	"subType": "brick",
	"quickEntityVersion": 3.1,
	"extraFactoryDependencies": [],
	"extraBlueprintDependencies": [],
	"comments": []
}
)";
    static TResourcePtr<ZTemplateEntityFactory> s_PropRTFactory;
    static TResourcePtr<ZTemplateEntityBlueprintFactory> s_PropRTBpFactory;

    if (!s_PropRTFactory || !s_PropRTBpFactory)
    {
        Logger::Debug(TAG "Loading factory from Qn.");
        if (!SDK()->LoadQnEntity(s_PropJson, s_PropRTBpFactory, s_PropRTFactory))
        {
            Logger::Debug(TAG "Could not load QN entity.");
            return false;
        }
    }
    else
    {
        Logger::Debug(TAG "Using cached factory.");
    }

    p_PropRTFactory = s_PropRTFactory;
    p_PropRTBpFactory = s_PropRTBpFactory;
    return true;
}