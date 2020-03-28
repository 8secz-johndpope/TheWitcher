#include "EnemyManager.h"
#include "Enemy.h"
#include "NilfgaardSoldier.h"

void EnemyManager::Start()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->StartEnemy();
	}
	CreateEnemy(EnemyType::NILFGAARD_SOLDIER, { 0,0,0 }, (ExtraEnumType)NilfgaardSoldier::NilfgaardType::SPEAR);
	CreateEnemy(EnemyType::NILFGAARD_SOLDIER, { 0,0,0 }, (ExtraEnumType)NilfgaardSoldier::NilfgaardType::LARGE_SWORD);
}

void EnemyManager::Update()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->UpdateEnemy();
	}
}

void EnemyManager::CleanUp()
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		(*item)->CleanUpEnemy();
		(*item)->game_object->ToDelete();
	}
	enemies.clear();
}

void EnemyManager::CreateEnemy(EnemyType type, const float3& position, ExtraEnumType extra_type)
{
	switch (type)
	{
	case EnemyType::GHOUL: {
		break; }
	case EnemyType::NILFGAARD_SOLDIER: {
		switch ((NilfgaardSoldier::NilfgaardType)extra_type)
		{
		case NilfgaardSoldier::NilfgaardType::SPEAR: {
			LOG("SPEAR");
			break; }
		case NilfgaardSoldier::NilfgaardType::ARCHER: {
			LOG("ARCHER");
			break; }
		case NilfgaardSoldier::NilfgaardType::LARGE_SWORD: {
			LOG("LARGE_SWORD");
			break; }
		case NilfgaardSoldier::NilfgaardType::SWORD_SHIELD: {
			LOG("SWORD_SHIELD");
			break; }
		default: {
			LOG("Niflgard type wrong")
			break; }
		}
		break; }
	default:
		break;
	}
}

void EnemyManager::AddEnemy(Enemy* enemy)
{
	if (enemy != nullptr) {
		enemies.push_back(enemy);
	}
}

void EnemyManager::DeleteEnemy(Enemy* enemy)
{
	for (auto item = enemies.begin(); item != enemies.end(); ++item) {
		if (*item == enemy) {
			enemy->CleanUpEnemy();
			enemy->game_object->ToDelete();
			enemies.erase(item);
			break;
		}
	}
}

