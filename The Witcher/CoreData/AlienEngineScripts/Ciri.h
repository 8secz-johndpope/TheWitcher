#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Boss.h"

class ALIEN_ENGINE_API Ciri : public Boss {
public:
	float combo_range = 3.0f;
	float mini_scream_range = 5.0f;
	float mini_scream_damage = 5.0f;
	float mini_scream_force = 7.0f;
	float scream_range = 15.0f;
	float3 dash_direction = float3::zero();
	float dash_speed = 50.0f;
	float dash_time = 0.1f;
	float dash_timer = 0.0f;
public:

	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;
	float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero()) override;

	void SetActionProbabilities() override;
	void SetActionVariables() override;
	bool IsOnAction() override;

	void LaunchAction() override;

	void LaunchDashAction();
	void LaunchComboAction();
	void LaunchMiniScreamAction();

	ActionState UpdateAction() override;

	ActionState UpdateDashAction();
	ActionState UpdateComboAction();
	ActionState UpdateMiniScreamAction();

	void EndAction(GameObject* go_ended) override;
	
	void EndDashAction(GameObject* go_ended);
	void EndComboAction(GameObject* go_ended);
	void EndMiniScreamAction(GameObject* go_ended);

	void OnAnimationEnd(const char* name);

	void OnDrawGizmosSelected();

};

ALIEN_FACTORY Ciri* CreateCiri() {
	Ciri* ciri = new Ciri();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Boss::BossState, ciri->state);
	SHOW_VOID_FUNCTION(Ciri::ActivateCollider, ciri);
	SHOW_VOID_FUNCTION(Ciri::DeactivateCollider, ciri);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ciri->scream_range);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ciri->mini_scream_range);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ciri->dash_speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ciri->dash_time);

	return ciri;
} 