#include "CameraMovement.h"

CameraMovement::CameraMovement() : Alien()
{
}

CameraMovement::~CameraMovement()
{
}

void CameraMovement::Start()
{
    //Get the players in the scene
    transform->SetGlobalPosition(CalculateCameraPos(hor_angle, vert_angle, distance));
    LookAtMidPoint();
    SearchAndAssignPlayers();
    trg_offset = transform->GetGlobalPosition() - CalculateMidPoint();
}

void CameraMovement::Update()
{    
    switch (state) {
    case CameraState::DYNAMIC: {
        transform->SetGlobalPosition(CalculateMidPoint() + trg_offset);
        break;
    }
    case CameraState::MOVING_TO_DYNAMIC:
    {
        current_transition_time += Time::GetDT();
        float3 trg_pos = CalculateMidPoint() + trg_offset;
        float3 curr_pos = transform->GetGlobalPosition();
        float min_dist = 0.1f;
        if ((trg_pos - curr_pos).Length() < min_dist) {
            LOG("Finished transition");
            transform->SetGlobalPosition(trg_pos);
            LookAtMidPoint();
            state = CameraState::DYNAMIC;
        }
        else {
			//INFO: This is more like an accelerated movement than a lerp since we're using the current position as the starting point

            float time_percent = (current_transition_time / curr_transition.transition_time);//A value from 0 to 1, 0 meaning it has just started and 1 meaning it has finished
			
            transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent));//Faster on the beggining
			
            //TODO: We could add the option to configure smoothness (cuadratic t, cubic t, etc.)
            //transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent * time_percent));//Less fast on the beggining
			//transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent * time_percent * time_percent));//Even less fast on the beggining

            LookAtMidPoint();
        }
        break;
    }
    }
}

void CameraMovement::LookAtMidPoint()
{
    float3 midPoint = CalculateMidPoint();

    float3 front = float3::unitZ(); //front of the object
    float3 direction = (midPoint - transform->GetGlobalPosition()).Normalized();
    
    Quat rot1 = RotationBetweenVectors(front, direction);

    float3 desiredUp = float3::unitY();
    float3 right = Cross(direction, desiredUp);
    desiredUp = Cross(right, direction);

    float3 newUp = rot1 * float3(0.0f, 1.0f, 0.0f);
    Quat rot2 = RotationBetweenVectors(newUp, desiredUp);

    transform->SetGlobalRotation(rot2 * rot1);
}

float3 CameraMovement::CalculateCameraPos(const float& ang1, const float& ang2, const float& dst)
{
    float angle1 = math::DegToRad(ang1);
    float angle2 = math::DegToRad(ang2);
    
    return float3(cos(angle1) * cos(angle2), sin(angle2), sin(angle1) * cos(angle2)) * dst;
}

void CameraMovement::OnDrawGizmos()
{
    if (players.size() < 2) {
        SearchAndAssignPlayers();
    }

    float3 mid_point = CalculateMidPoint();
    Gizmos::DrawWireSphere(mid_point, .15f, Color::Cyan(), 0.5F); // mid point

    float3 cam_pos = CalculateCameraPos(hor_angle, vert_angle, distance);
    Gizmos::DrawLine(mid_point, cam_pos, Color::Red()); // line mid -> future camera pos

    Gizmos::DrawWireSphere(transform->GetGlobalPosition(), 0.15f, Color::Green());
    Gizmos::DrawLine(mid_point, transform->GetGlobalPosition(), Color::Green());
}

void CameraMovement::SearchAndAssignPlayers()
{
    GameObject** get_players = nullptr;
    uint size;
    size = GameObject::FindGameObjectsWithTag("Player", &get_players);
    for (int i = 0; i < size; i++) {
        GameObject* g = get_players[i];
        std::pair<GameObject*, PlayerState> p(g, PlayerState::NONE);
        players.insert(p);
    }
    GameObject::FreeArrayMemory((void***)&get_players);
    num_curr_players = size;
}

float3 CameraMovement::CalculateMidPoint()
{
    float3 mid_pos(0,0,0);
    for (std::map<GameObject*, PlayerState>::iterator it = players.begin(); it != players.end(); ++it)
    {
        mid_pos += it->first->transform->GetGlobalPosition();
    }
    if (players.size() == 0)
        return mid_pos;
    return mid_pos / players.size();
}

Quat CameraMovement::RotationBetweenVectors(math::float3& start, math::float3& dest) // Include in MathGeoLib
{
    start.Normalize();
    dest.Normalize();

    float cosTheta = Dot(start, dest);
    float3 rotationAxis;

    if (cosTheta < -1 + 0.001f) {
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = Cross(float3(0.0f, 0.0f, 1.0f), start);
        if (Length(rotationAxis) < 0.01f) // bad luck, they were parallel, try again!
            rotationAxis = Cross(float3(1.0f, 0.0f, 0.0f), start);

        rotationAxis.Normalize();
        return Quat(rotationAxis, math::DegToRad(180.f));
    }

    rotationAxis = Cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return Quat(
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs,
        s * 0.5f
    );
}