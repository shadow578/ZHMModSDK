#include "ZFollowHitmanBehaviourEffect.h"

#include <Glacier/ZModule.h>
#include <Glacier/ZEntity.h>
#include <Glacier/ZSpatialEntity.h>
#include <Glacier/ZHitman5.h>
#include <Glacier/SExternalReferences.h>
#include <Glacier/CompileReflection.h>
#include <Glacier/TArray.h>

#include "EffectRegistry.h"
#include "Helpers/Utils.h"
#include "Helpers/ZQuickEntityLoader.h"

#define TAG "[ZFollowHitmanBehaviourEffectBase] "

#pragma region g_FollowBehaviourProp
// [assembly:/_pro/chaosmod/follow_player_behaviour_helper.entitytemplate].pc_entitytype
// [assembly:/_pro/chaosmod/follow_player_behaviour_helper.entitytemplate].pc_entityblueprint
static ZQuickEntityLoader g_FollowBehaviourProp(
    "ZFollowHitmanBehaviourEffect_Prop",
    R"(
{
	"tempHash": "00C11B1B29B636AB",
	"tbluHash": "00DDBC7C994B3F05",
	"rootEntity": "0000",
	"entities": {
		"0000": {
			"parent": null,
			"name": "DramaSituation_Staking_Player_ROOT",
			"factory": "[modules:/zspatialentity.class].pc_entitytype",
			"blueprint": "[modules:/zspatialentity.class].pc_entityblueprint",
			"propertyAliases": {
				"m_aActors": [
					{
						"originalEntity": "0001",
						"originalProperty": "m_aActors"
					},
					{
						"originalEntity": "0003",
						"originalProperty": "m_aActors"
					}
				],
				"m_bEnabled": [
					{
						"originalEntity": "0001",
						"originalProperty": "m_bEnabled"
					},
					{
						"originalEntity": "0001",
						"originalProperty": "m_bIsRunning"
					}
				],
				"m_bIsOnCooldown": [
					{
						"originalEntity": "0001",
						"originalProperty": "m_bIsOnCooldown"
					}
				]
			}
		},
		"0001": {
			"parent": "0000",
			"name": "DramaSituation_Staking_Player",
			"factory": "[assembly:/_pro/design/actor/spsystem.template?/dramasituation.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/actor/spsystem.template?/dramasituation.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_rDrama": {
					"type": "SEntityTemplateReference",
					"value": "0004"
				},
				"m_aActors": {
					"type": "TArray<SEntityTemplateReference>",
					"value": []
				},
				"m_eReentryBehavior": {
					"type": "ZDramaSituationEntity.EReentryBehavior",
					"value": "RB_RESTART"
				},
				"m_nPriority": {
					"type": "int32",
					"value": 0
				},
				"m_bEnabled": {
					"type": "bool",
					"value": false
				}
			}
		},
		"0002": {
			"parent": "0001",
			"name": "FollowHitmanBehavior",
			"factory": "[assembly:/_pro/design/actor/spsystem.template?/followhitmanbehavior.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/actor/spsystem.template?/followhitmanbehavior.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_pActor": {
					"type": "SEntityTemplateReference",
					"value": "0003"
				},
				"m_fMaxTetherRange": {
					"type": "float32",
					"value": 11.0
				},
				"m_fMinTetherRange": {
					"type": "float32",
					"value": 10.0
				},
				"m_fMaxSightDistance": {
					"type": "float32",
					"value": 1000.0
				},
				"m_fMaxIdleTime": {
					"type": "float32",
					"value": 1.0
				}
			}
		},
		"0003": {
			"parent": "0001",
			"name": "Role_FollowHitman",
			"factory": "[assembly:/_pro/design/actor/spsystem.template?/role.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/actor/spsystem.template?/role.entitytemplate].pc_entityblueprint",
			"properties": {
				"m_aActors": {
					"type": "TArray<SEntityTemplateReference>",
					"value": []
				},
				"m_rActor": {
					"type": "SEntityTemplateReference",
					"value": null
				},
				"m_bRememberInitialBehavior": {
					"type": "bool",
					"value": true
				}
			}
		},
		"0004": {
			"parent": "0001",
			"name": "Screenplay_FollowHitman",
			"factory": "[assembly:/_pro/design/actor/spsystem.template?/screenplay.entitytemplate].pc_entitytype",
			"blueprint": "[assembly:/_pro/design/actor/spsystem.template?/screenplay.entitytemplate].pc_entityblueprint",
			"properties": {
				"Roles": {
					"type": "TArray<SEntityTemplateReference>",
					"value": [
						"0003"
					]
				}
			},
			"events": {
				"OnStart": {
					"Start": [
						"0002"
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
)"
);
#pragma endregion


void ZFollowHitmanBehaviourEffectBase::OnEngineInitialized()
{
    g_FollowBehaviourProp.LoadAsync();
}

bool ZFollowHitmanBehaviourEffectBase::Available()
{
    return IChaosEffect::Available() &&
        // if Available() returns false, OnEngineInitialized() will not be called, so 
        // we need to ensure we only check load status once it was started
        (!g_FollowBehaviourProp.Loading() || g_FollowBehaviourProp.Ready());
}


void ZFollowHitmanBehaviourEffectBase::Start()
{
    if (!EnsureBehaviourEntity())
    {
        return;
    }

    const auto s_aAllActors = Utils::GetActors(false, false);
    auto s_aAllActorsRef = TArray<ZEntityRef>();

    for (auto* s_pActor : s_aAllActors)
    {
        ZEntityRef s_Ref;
        s_pActor->GetID(s_Ref);
        if (s_Ref)
        {
            s_aAllActorsRef.push_back(s_Ref);
        }
    }

    m_BehaviourEntity.SetProperty("m_aActors", s_aAllActorsRef);
    m_BehaviourEntity.SetProperty("m_bIsOnCooldown", false);
    m_BehaviourEntity.SetProperty("m_bEnabled", true);
}

void ZFollowHitmanBehaviourEffectBase::Stop()
{

}

void ZFollowHitmanBehaviourEffectBase::OnClearScene()
{
    IChaosEffect::OnClearScene();
    m_BehaviourEntity = {};
}

bool ZFollowHitmanBehaviourEffectBase::EnsureBehaviourEntity()
{
    if (m_BehaviourEntity)
    {
        return true;
    }

    const auto s_Scene = Globals::Hitman5Module->m_pEntitySceneContext->m_pScene;
    if (!s_Scene)
    {
        Logger::Debug(TAG "Cannot spawn prop, no scene loaded.");
        return false;
    }

    if (!g_FollowBehaviourProp.Ready())
    {
        Logger::Debug(TAG "prop factory not ready.");
        return false;
    }

    TResourcePtr<ZTemplateEntityFactory> s_PropRTFactory = g_FollowBehaviourProp.GetFactory();
    TResourcePtr<ZTemplateEntityBlueprintFactory> s_PropRTBpFactory = g_FollowBehaviourProp.GetBlueprintFactory();

    SExternalReferences s_ExternalRefs;

    Functions::ZEntityManager_NewEntity->Call(
        Globals::EntityManager,
        m_BehaviourEntity,
        "ZFollowBehaviour_Prop",
        s_PropRTFactory,
        s_Scene.m_ref,
        s_ExternalRefs,
        -1
    );

    if (!m_BehaviourEntity)
    {
        Logger::Debug(TAG "Spawning entity failed.");
        return false;
    }

    Logger::Debug(TAG "Spawned entity successfully.");
    return true;
}


REGISTER_CHAOS_EFFECT(ZFollowHitmanBehaviourEffectBase);