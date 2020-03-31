#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGuizmos/ImGuizmo.h"

class ComponentCamera;
class GameObject;
class OctreeNode;
class Viewport;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	void Move(const float3& Movement);
	bool CleanUp();

	void Movement();
	void Rotation(float dt);
	void Focus();
	void Zoom();

	void CreateRay();

	void CreateObjectsHitMap(std::vector<std::pair<float, GameObject*>>* hits, GameObject* go, const LineSegment &ray);
	void CreateObjectsHitMap(std::vector<std::pair<float, GameObject*>>* hits, OctreeNode* node, const LineSegment &ray);
	bool TestTrianglesIntersections(GameObject* object, const LineSegment& ray);
	static bool SortByDistance(const std::pair<float, GameObject*> pair1, const std::pair<float, GameObject*> pair2);
	
	void PanelConfigOption();

public:
	
	bool is_scene_hovered = false;
	bool is_scene_focused = false;
	LineSegment ray;
	float camera_speed = 30.0f;
	float camera_zoom_speed = 100.0f;
	float camera_mouse_speed = 10.0f;

	ComponentCamera* fake_camera = nullptr;

	float3 reference = float3::zero();

	Viewport* scene_viewport = nullptr;
	Viewport* selected_viewport = nullptr;

private:

	Frustum* frustum = nullptr;

	float speed = 0.f;
	float zoom_speed = 0.f;
	float mouse_speed = 0.f;

	GameObject* looking_at = nullptr;
	
	SDL_Cursor* cursor = nullptr;

	bool start_lerp = false;
	float3 point_to_look = float3::zero();
};