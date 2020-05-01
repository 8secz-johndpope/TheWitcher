#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API Bonfire : public Alien {

public:

	Bonfire();
	virtual ~Bonfire();
	
	void Start();

	void SetBonfireUsed(PlayerController* player);

	bool HaveThisPlayerUsedThis(PlayerController* player);

public:
	// GameObject
	GameObject* ui_bonfire = nullptr;
	GameObject* checkpoint = nullptr;

	bool is_active = true;

private:
	PlayerController* first_player = nullptr;
};

ALIEN_FACTORY Bonfire* CreateBonfire() {
	Bonfire* alien = new Bonfire();
	// To show in inspector here

	return alien;
} 
