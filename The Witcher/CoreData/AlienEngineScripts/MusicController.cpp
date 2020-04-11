#include "MusicController.h"

MusicController::MusicController() : Alien()
{
}

MusicController::~MusicController()
{
}

void MusicController::Start()
{
	emitter = (ComponentAudioEmitter*)this->GetComponent(ComponentType::A_EMITTER);
	emitter->SetState("Interactive_Music_Lvl1", "Quiet");
}

void MusicController::Update()
{
	if (is_combat && has_changed)
	{
		LOG("HOLA");
		emitter->SetState("Interactive_Music_Lvl1", "Combat");
		has_changed = !has_changed;
	}
	else if(!is_combat && has_changed)
	{
		LOG("ADIOH");
		emitter->SetState("Interactive_Music_Lvl1", last_music.c_str());
		has_changed = !has_changed;
	}
}