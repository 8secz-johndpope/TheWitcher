#include "VagoneteMove.h"

Quat VagoneteInputs::playerRotation = Quat::identity();
float VagoneteInputs::inclination4player = 0.0F;
float VagoneteInputs::speedInclination = 0.0F;

VagoneteMove::VagoneteMove()
{
}

VagoneteMove::~VagoneteMove()
{
	for (auto item = players.begin(); item != players.end(); ++item) {
		delete* item;
		*item = nullptr;
	}
	players.clear();
}

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();

	players.push_back(new VagoneteInputs(PlayerController::PlayerType::GERALT));
	players.push_back(new VagoneteInputs(PlayerController::PlayerType::YENNEFER));
}

void VagoneteMove::Update()
{
	for (auto item = players.begin(); item != players.end(); ++item) {
		(*item)->UpdateInputs();
	}

	float3 currentPos = curve->curve.ValueAt(actual_pos);
	float3 nextPos = curve->curve.ValueAt(actual_pos + speed * Time::GetDT() * 5);

	//Pitch (slope)
	float3 railVector = (currentPos - nextPos).Normalized();
	Quat rot = Quat::LookAt(float3::unitX(), railVector, float3::unitY(), float3::unitY());

	//Inclination (normals + players)
	float3 inclinationVector = curve->curve.NormalAt(actual_pos).Normalized();
	Quat inclinationRot = Quat::RotateFromTo(float3::unitY(), inclinationVector);
	inclinationRot.Inverse();
	rot = rot * inclinationRot;

	transform->SetLocalRotation(rot * VagoneteInputs::playerRotation);
	transform->SetLocalPosition(currentPos);

	actual_pos += speed * Time::GetDT();
	VagoneteInputs::playerRotation = Quat::identity();

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}

VagoneteInputs::VagoneteInputs(PlayerController::PlayerType type)
{
	switch (type)
	{
	case PlayerController::PlayerType::GERALT: {
		controllerIndex = 1;
		keyboardInput.inclinationLeft = SDL_SCANCODE_A;
		keyboardInput.inclinationRight = SDL_SCANCODE_D;
		keyboardInput.cover = SDL_SCANCODE_S;
		keyboardInput.jump = SDL_SCANCODE_W;
		break; }
	case PlayerController::PlayerType::YENNEFER: {
		controllerIndex = 2;
		keyboardInput.inclinationLeft = SDL_SCANCODE_LEFT;
		keyboardInput.inclinationRight = SDL_SCANCODE_RIGHT;
		keyboardInput.cover = SDL_SCANCODE_DOWN;
		keyboardInput.jump = SDL_SCANCODE_UP;
		break; }
	default: {
		break; }
	}

	state = State::IDLE;
}

void VagoneteInputs::UpdateInputs()
{
	Inclination();
}

void VagoneteInputs::Inclination()
{
	if (Input::GetKeyRepeat(keyboardInput.inclinationLeft)) {
		currentInclination -= speedInclination * Time::GetDT();
	}
	else if (Input::GetKeyRepeat(keyboardInput.inclinationRight)) {
		currentInclination += speedInclination * Time::GetDT();
	}
	else {
		if (currentInclination < 0) {
			currentInclination += speedInclination * Time::GetDT();
			if (currentInclination > 0) {
				currentInclination = 0;
			}
		}
		else if (currentInclination > 0) {
			currentInclination -= speedInclination * Time::GetDT();
			if (currentInclination < 0) {
				currentInclination = 0;
			}
		}
	}

	if (currentInclination != 0) {
		currentInclination = Maths::Clamp(currentInclination, -inclination4player, inclination4player);
		playerRotation = playerRotation * Quat::RotateX(currentInclination * Maths::Deg2Rad());
	}
}
