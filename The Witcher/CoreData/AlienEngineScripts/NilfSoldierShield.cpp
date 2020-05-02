#include "NilfSoldierShield.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"

NilfSoldierShield::NilfSoldierShield() : NilfgaardSoldier()
{
}

NilfSoldierShield::~NilfSoldierShield()
{
}

void NilfSoldierShield::UpdateEnemy()
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

	case NilfgaardSoldierState::AUXILIAR:
		if (stats["BlockRange"].GetValue() < distance)
		{
			state = NilfgaardSoldierState::IDLE;
			animator->PlayState("Idle");
		}
		Block();
		break;

	case NilfgaardSoldierState::STUNNED:
		if (Time::GetGameTime() - current_stun_time > stun_time)
		{
			state = NilfgaardSoldierState::IDLE;
		}
		break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		animator->PlayState("Death");
		audio_emitter->StartSound("SoldierDeath");
		state = NilfgaardSoldierState::DEAD;
		break;
	}
	}
}

void NilfSoldierShield::Action()
{
	int rand_num = Random::GetRandomIntBetweenTwo(0, 1);
	if (rand_num == 0)
	{
		animator->PlayState("Block");
		current_time = Time::GetGameTime();
		is_blocked = true;
		state = NilfgaardSoldierState::AUXILIAR;
	}
	else
	{
		animator->PlayState("Attack");
		state = NilfgaardSoldierState::ATTACK;
	}
}

void NilfSoldierShield::Block()
{
	float b_time = (has_been_attacked) ? block_attack_time : block_time;
	if (Time::GetGameTime() - current_time > b_time)
	{
		if (stats["AttackRange"].GetValue() < distance)
		{
			state = NilfgaardSoldierState::IDLE;
			animator->PlayState("Idle");
		}
		else
		{
			state = NilfgaardSoldierState::ATTACK;
			animator->PlayState("Attack");
		}

		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
	else if (break_shield_attack >= max_break_shield_attack)
	{
		state = NilfgaardSoldierState::HIT;
		animator->PlayState("Hit");
		has_been_attacked = false;
		break_shield_attack = 0;
		is_blocked = false;
	}
}

bool NilfSoldierShield::CheckPlayerForward()
{
	float angle = transform->forward.AngleBetween(player_controllers[current_player]->transform->forward) * RADTODEG;
	LOG("Angle: %f", angle);
	if (angle > 100 && angle < 200)
		return true;
	else
		return false;
}

void NilfSoldierShield::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != NilfgaardSoldierState::DEAD) {

		if (is_blocked && CheckPlayerForward())
		{
			has_been_attacked = true;
			current_time = Time::GetGameTime();
			break_shield_attack++;
			particles["ClinckEmitter"]->Restart();
			audio_emitter->StartSound("SoldierBlock");
		}
		else
		{
			PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
			if (player)
			{
				float dmg_received = player->attacks->GetCurrentDMG();
				player->OnHit(this, GetDamaged(dmg_received, player));

				if (state == NilfgaardSoldierState::DYING)
					player->OnEnemyKill();

				HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
			}
		}
	}
}