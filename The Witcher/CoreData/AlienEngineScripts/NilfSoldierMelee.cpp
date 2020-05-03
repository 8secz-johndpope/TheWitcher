#include "NilfSoldierMelee.h"
#include "EnemyManager.h"

NilfSoldierMelee::NilfSoldierMelee() : NilfgaardSoldier()
{
}

NilfSoldierMelee::~NilfSoldierMelee()
{
}

void NilfSoldierMelee::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue())
			state = NilfgaardSoldierState::MOVE;
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
		break;

	case NilfgaardSoldierState::STUNNED:
		if (Time::GetGameTime() - current_stun_time > stun_time)
		{
			state = NilfgaardSoldierState::IDLE;
		}
		break;

	case NilfgaardSoldierState::HIT:
	{
		velocity += velocity * knock_slow * Time::GetDT();
		character_ctrl->Move(velocity * Time::GetDT());
	}
	break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		animator->PlayState("Death");
		audio_emitter->StartSound("SoldierDeath");
		state = NilfgaardSoldierState::DEAD;
		break;
	}
	}
}

void NilfSoldierMelee::Action()
{
	animator->PlayState("Attack");
	animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
	state = NilfgaardSoldierState::ATTACK;
}