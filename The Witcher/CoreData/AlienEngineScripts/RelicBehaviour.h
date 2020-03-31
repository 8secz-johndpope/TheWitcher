#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class Effect;

enum (Relic_Type,
	BASE,
	ATTACK,
	DASH,
	NONE_TYPE
	);
enum (Relic_Effect,
	FIRE,
	ICE,
	EARTH,
	LIGHTNING,
	POISON,
	NATURE,
	SEA,
	DJINN,
	STRIGAS,
	RAGE,
	RANGE,
	NONE_EFFECT
	);

class Relic {
public:
	Relic();
	virtual ~Relic();

	virtual void OnPickUp(PlayerController* player);

	std::string name = "";
	std::string description = "";
	Relic_Effect relic_effect = Relic_Effect::NONE_EFFECT;
	std::vector<Effect*> effects;

};

class AttackRelic : public Relic {
public:
	AttackRelic();
	virtual ~AttackRelic();

	void OnPickUp(PlayerController* player) override;

	std::string attack_name = "";

};

class DashRelic : public Relic {
public:
	DashRelic();
	virtual ~DashRelic();

	void OnPickUp(PlayerController* player) override;

};

class ALIEN_ENGINE_API RelicBehaviour : public Alien {

public:
	
public:

	RelicBehaviour();
	virtual ~RelicBehaviour();

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;

public:

	std::string name = "";
	std::string description = "";
	Relic_Type relic_type = Relic_Type::NONE_TYPE;
	Relic_Effect relic_effect = Relic_Effect::NONE_EFFECT;
	Relic* relic = nullptr;

};

ALIEN_FACTORY RelicBehaviour* CreateRelicBehaviour() {
	RelicBehaviour* relic_behaviour = new RelicBehaviour();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_STRING(relic_behaviour->name);
	SHOW_IN_INSPECTOR_AS_STRING(relic_behaviour->description);
	SHOW_IN_INSPECTOR_AS_ENUM(Relic_Type, relic_behaviour->relic_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Relic_Effect, relic_behaviour->relic_effect);
	

	return relic_behaviour;
}