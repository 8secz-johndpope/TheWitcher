#include "Scale_Win.h"
#include "Spawner.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Scores_Data.h"

Scale_Win::Scale_Win() : Alien()
{
}

Scale_Win::~Scale_Win()
{
}

void Scale_Win::Start()
{
	// Scales
	left_scale = game_object->GetChild("Left_Scale");
	right_scale = game_object->GetChild("Right_Scale");
	rigid_body1 = left_scale->GetComponent<ComponentRigidBody>();
	rigid_body2 = right_scale->GetComponent<ComponentRigidBody>();

	// Spawners
	spawner_l = GameObject::FindWithName("Left_Spawner")->GetComponent<Spawner>();
	spawner_r = GameObject::FindWithName("Right_Spawner")->GetComponent<Spawner>();

	// Texts
	score_text_1 = GameObject::FindWithName("Score_Player_1")->GetComponent<ComponentText>();
	score_text_2 = GameObject::FindWithName("Score_Player_2")->GetComponent<ComponentText>();
	score_text_1->SetText("0");
	score_text_2->SetText("0");

	for (int i = 1; i <= Scores_Data::player1_kills; ++i)
	{
		float random_time = Random::GetRandomFloatBetweenTwo(0.25f, 0.5f);
		float random_pos_x = Random::GetRandomFloatBetweenTwo(-3.5f, 3.5f);
		float random_pos_z = Random::GetRandomFloatBetweenTwo(-3.5f, 3.5f);

		Invoke([this, random_pos_x, random_pos_z]() -> void
			{
				spawner_l->Spawn(TO_SPAWN::HEAD, float3(spawner_l->transform->GetGlobalPosition().x + random_pos_x,
					spawner_l->transform->GetGlobalPosition().y,
					spawner_l->transform->GetGlobalPosition().z + random_pos_z));
			}
			,
				random_time* i);
	}

	for (int i = 1; i <= Scores_Data::player2_kills; ++i)
	{
		float random_time = Random::GetRandomFloatBetweenTwo(0.25f, 0.5f);
		float random_pos_x = Random::GetRandomFloatBetweenTwo(-3.5f, 3.5f);
		float random_pos_z = Random::GetRandomFloatBetweenTwo(-3.5f, 3.5f);

		Invoke([this, random_pos_x, random_pos_z]() -> void
			{
				spawner_r->Spawn(TO_SPAWN::HEAD, float3(spawner_r->transform->GetGlobalPosition().x + random_pos_x,
					spawner_r->transform->GetGlobalPosition().y,
					spawner_r->transform->GetGlobalPosition().z + random_pos_z));
			}
			,
				random_time * i);
	}
}

void Scale_Win::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_LCTRL))
	{
		spawner_l->Spawn(TO_SPAWN::HEAD, spawner_l->transform->GetLocalPosition());
	}

	if (Input::GetKeyDown(SDL_SCANCODE_RCTRL))
	{
		spawner_r->Spawn(TO_SPAWN::HEAD, spawner_r->transform->GetLocalPosition());
	}

	if (player1_points != current_points1)
	{
		int points1 = Maths::Lerp(current_points1, player1_points, (Time::GetGameTime() - time) / time_to_scale);
		if (((Time::GetGameTime() - time) / time_to_scale) >= 1)
		{
			points1 = player1_points;
			current_points1 = player1_points;
		}
		score_text_1->SetText(std::to_string(points1).c_str());
	}

	if (player2_points != current_points2)
	{
		int points2 = Maths::Lerp(current_points2, player2_points, (Time::GetGameTime() - time) / time_to_scale);
		if (((Time::GetGameTime() - time) / time_to_scale) >= 1)
		{
			points2 = player2_points;
			current_points2 = player2_points;
		}
		score_text_2->SetText(std::to_string(points2).c_str());
	}

	if (!in_place)
	{
		left_scale->transform->SetGlobalPosition(float3(7.5f, Maths::Lerp(original_position1, desired_position1, (Time::GetGameTime() - time) / time_to_scale), 0));
		right_scale->transform->SetGlobalPosition(float3(-7.5f, Maths::Lerp(original_position2, desired_position2, (Time::GetGameTime() - time) / time_to_scale), 0));

		// Delete this when physics updated rigid body position with GO
		rigid_body1->SetPosition(left_scale->transform->GetLocalPosition());
		rigid_body2->SetPosition(right_scale->transform->GetLocalPosition());
		// ------------------------------------------

		if (Time::GetGameTime() > time + time_to_scale)
		{ 
			in_place = true;
		}
	}
}

void Scale_Win::CalculateInclination()
{
	float ratio = 0.0f;

	if (Maths::Max(player1_points, player2_points) == 0)
	{
		ratio = 1;
	}
	else 
	{
		ratio = (float)Maths::Min(player1_points, player2_points) / (float)Maths::Max(player1_points, player2_points);
	}

	float value = 0.0f;

	if (ratio <= 0.5f)
	{
		value = 1;
	}
	else if (ratio == 1)
	{
		value = 0;
	}
	else
	{
		value = 1 - ((ratio - 0.5f) * 2);
	}

	original_position1 = left_scale->transform->GetLocalPosition().y;
	original_position2 = right_scale->transform->GetLocalPosition().y;

	if (Maths::Min(player1_points, player2_points) == player1_points)
	{
		desired_position1 = max_Y * value;
		desired_position2 = -max_Y * value;
	}
	else
	{
		desired_position1 = -max_Y * value;
		desired_position2 = max_Y * value;
	}

	in_place = false;
	time = Time::GetGameTime();
}
