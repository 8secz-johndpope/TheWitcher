#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CameraMovement;
class MusicController;

class ALIEN_ENGINE_API TriggerMusicInteractive : public Alien {
public:
	enum (Music,
		NONE,
		QUIET,
		COMBAT,
		NORMAL,
		MOUNTAIN,
		LAKE
		);
	TriggerMusicInteractive();
	virtual ~TriggerMusicInteractive();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	std::string GetNameByEnum(Music mat);

public:
	Music interactive = Music::NONE;
	GameObject* camera = nullptr;
	CameraMovement* cam_script = nullptr;
	ComponentAudioEmitter* emitter = nullptr;
	MusicController* m_controller = nullptr;
	GameObject* p1 = nullptr;
	GameObject* p2 = nullptr;
	int player_counter = 0;
	float timer = 0.f;
	bool first_time = true;
	std::string music_state;
};
ALIEN_FACTORY TriggerMusicInteractive* CreateTriggerMusicInteractive() {
	TriggerMusicInteractive* alien = new TriggerMusicInteractive();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TriggerMusicInteractive::Music, alien->interactive);
	SHOW_TOOLTIP("Pick NONE to pass the string whatever state you like.");
	SHOW_IN_INSPECTOR_AS_STRING(alien->music_state);
	
	return alien;
}
