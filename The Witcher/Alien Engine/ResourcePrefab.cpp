#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentLightDirectional.h"
#include "ComponentTransform.h"
#include "PanelHierarchy.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "ModuleCamera3D.h"
#include "Time.h"
#include "ComponentDeformableMesh.h"

#include "mmgr/mmgr.h"

ResourcePrefab::ResourcePrefab() : Resource()
{
	type = ResourceType::RESOURCE_PREFAB;
}

ResourcePrefab::~ResourcePrefab()
{
	prefab_references.clear();
}

bool ResourcePrefab::CreateMetaData(GameObject* object, const char* folder, u64 force_id)
{
	std::vector<std::string> files;
	std::vector<std::string> dir;
	if (folder == nullptr) {
		path = std::string(ASSETS_PREFAB_FOLDER + std::string(object->GetName()) + ".alienPrefab");
		App->file_system->DiscoverFiles(ASSETS_PREFAB_FOLDER, files, dir);
	}
	else {
		path = std::string(std::string(folder) + std::string(object->GetName()) + ".alienPrefab");
		App->file_system->DiscoverFiles(folder, files, dir);
	}

	if (force_id != 0) {
		ID = force_id;
	}
	else {
		ID = App->resources->GetRandomID();
	}

	if (!files.empty()) {
		uint num_file = 0;
		for (uint i = 0; i < files.size(); ++i) {
			if (App->StringCmp(files[i].data(), App->file_system->GetBaseFileNameWithExtension(path.data()).data())) {
				++num_file;
				if (folder == nullptr) {
					path = std::string(ASSETS_PREFAB_FOLDER + std::string(object->GetName()) + " (" + std::to_string(num_file) + ")" + ".alienPrefab");
				}
				else {
					path = std::string(std::string(folder) + std::string(object->GetName()) + " (" + std::to_string(num_file) + ")" + ".alienPrefab");
				}
				i = -1;
			}
		}
	}

	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);
	json_serialize_to_file_pretty(value, path.data());

	if (value != nullptr && json_object != nullptr) {
		// save ID in assets
		JSONfilepack* prefab_scene = new JSONfilepack(path.data(), json_object, value);
		prefab_scene->StartSave();

		SetName(App->file_system->GetBaseFileName(path.data()).data());

		// save prefab in library
		meta_data_path = path;

		object->SetPrefab(ID);

		JSONArraypack* game_objects = prefab_scene->InitNewArray("Prefab.GameObjects");

		game_objects->SetAnotherNode();

		App->objects->SaveGameObject(object, game_objects, 1);

		prefab_scene->FinishSave();
		delete prefab_scene;

		App->resources->AddResource(this);

		std::string meta_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";
		JSON_Value* meta_value = json_value_init_object();
		JSON_Object* meta_object = json_value_get_object(meta_value);
		json_serialize_to_file_pretty(meta_value, meta_path.data());

		if (meta_value != nullptr && meta_object != nullptr) {
			JSONfilepack* meta = new JSONfilepack(meta_path.data(), meta_object, meta_value);
			meta->StartSave();
			meta->SetString("Meta.ID", std::to_string(ID).data());
			meta->FinishSave();
			delete meta;

			meta_data_path = LIBRARY_PREFABS_FOLDER + std::to_string(ID) + ".alienPrefab";
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}
	else {
		LOG_ENGINE("Could not load scene, fail when creating the file");
	}

	return true;
}

bool ResourcePrefab::ReadBaseInfo(const char* assets_file_path)
{
	path = std::string(assets_file_path);

	// TODO: change when loading game
	ID = App->resources->GetIDFromAlienPath(std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data());

	if (ID != 0) {
		meta_data_path = LIBRARY_PREFABS_FOLDER + std::to_string(ID) + ".alienPrefab";
		if (!App->file_system->Exists(meta_data_path.data())) {
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
		else {
			struct stat fileMeta;
			struct stat fileAssets;
			if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
				if (fileAssets.st_mtime > fileMeta.st_mtime) {
					remove(meta_data_path.data());
					App->file_system->Copy(path.data(), meta_data_path.data());
				}
			}
		}

		SetName(App->file_system->GetBaseFileName(path.data()).data());
		App->resources->AddResource(this);
	}

	return true;
}

void ResourcePrefab::ReadLibrary(const char* meta_data)
{
	meta_data_path = std::string(meta_data);

	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));

	App->resources->AddResource(this);
}

bool ResourcePrefab::DeleteMetaData()
{
	if (App->objects->prefab_scene && App->objects->prefab_opened == this) {
		App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
		App->objects->prefab_opened = nullptr;
		App->objects->prefab_scene = false;
		App->objects->enable_instancies = true;
		App->objects->SwapReturnZ(true, true);
	}
	remove(meta_data_path.data());
	App->objects->GetRoot(true)->UnpackAllPrefabsOf(ID);
	return true;
}

void ResourcePrefab::Save(GameObject* prefab_root)
{
	remove(meta_data_path.data());
	remove(path.data());
	JSON_Value* prefab_value = json_value_init_object();
	JSON_Object* prefab_object = json_value_get_object(prefab_value);
	json_serialize_to_file_pretty(prefab_value, path.data());
	prefab_root->SetPrefab(ID);
	if (prefab_value != nullptr && prefab_object != nullptr) {
		JSONfilepack* prefab = new JSONfilepack(path.data(), prefab_object, prefab_value);
		prefab->StartSave();
		JSONArraypack* game_objects = prefab->InitNewArray("Prefab.GameObjects");

		game_objects->SetAnotherNode();

		App->objects->SaveGameObject(prefab_root, game_objects, 1);

		prefab->FinishSave();

		App->file_system->Copy(path.data(), meta_data_path.data());
		delete prefab;
	}
	if (App->objects->prefab_scene) {
		App->objects->prefab_opened = nullptr;
		App->objects->prefab_scene = false;
		App->objects->SwapReturnZ(true, true);
		App->objects->LoadScene("Library/save_prefab_scene.alienScene", false);
		App->objects->enable_instancies = true;
		remove("Library/save_prefab_scene.alienScene");
	}
	App->objects->ignore_cntrlZ = true;
	App->objects->in_cntrl_Z = true;
	std::vector<GameObject*> objs;
	App->objects->GetRoot(true)->GetObjectWithPrefabID(ID, &objs);
	if (!objs.empty()) {
		std::vector<GameObject*>::iterator item = objs.begin();
		for (; item != objs.end(); ++item) {
			if (*item != nullptr && !(*item)->prefab_locked && (*item) != prefab_root) {
				GameObject* parent = (*item)->parent;
				std::vector<GameObject*>::iterator iter = parent->children.begin();
				for (; iter != parent->children.end(); ++iter) {
					if (*iter == (*item)) {
						(*item)->ToDelete();
						float3 pos = static_cast<ComponentTransform*>((*item)->GetComponent(ComponentType::TRANSFORM))->GetLocalPosition();
						ConvertToGameObjects(parent, iter - parent->children.begin(), pos, false);
						break;
					}
				}
			}
		}
	}
	App->objects->in_cntrl_Z = false;
	App->objects->ignore_cntrlZ = false;
}

void ResourcePrefab::OpenPrefabScene()
{
	if (Time::IsInGameState()) {
		App->ui->panel_hierarchy->popup_no_open_prefab = true;
		return;
	}
	if (App->objects->prefab_scene) {
		return;
	}
	App->objects->prefab_opened = this;
	App->objects->enable_instancies = false;
	App->objects->SwapReturnZ(false, false);
	App->objects->prefab_scene = true;
	App->objects->SaveScene(nullptr, "Library/save_prefab_scene.alienScene");
	App->objects->DeselectObjects();
	App->objects->CreateRoot();
	ConvertToGameObjects(App->objects->GetRoot(true));
}

void ResourcePrefab::ConvertToGameObjects(GameObject* parent, int list_num, float3 pos, bool set_selected)
{
	JSON_Value* value = json_parse_file(meta_data_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* prefab = new JSONfilepack(meta_data_path.data(), object, value);

		JSONArraypack* game_objects = prefab->GetArray("Prefab.GameObjects");

		std::vector<GameObject*> objects_created;

		for (uint i = 0; i < game_objects->GetArraySize(); ++i) {
			GameObject* obj = new GameObject(true);
			u64 parentID = std::stoull(game_objects->GetString("ParentID"));
			if (parentID != 0) {
				std::vector<GameObject*>::iterator objects = objects_created.begin();
				for (; objects != objects_created.end(); ++objects) {
					if ((*objects)->ID == parentID) {
						obj->LoadObject(game_objects, *objects);
						break;
					}
				}
			}
			else {
				obj->LoadObject(game_objects, App->objects->GetRoot(false));
			}
			objects_created.push_back(obj);
			game_objects->GetAnotherNode();
		}
		GameObject* obj = parent->children.back();
		if (list_num != -1) {
			parent->children.pop_back();
			parent->children.insert(parent->children.begin() + list_num, obj);
		}
		for each (GameObject * obj in objects_created) //not sure where to place this, need to link skeletons to meshes after all go's have been created
		{
			ComponentDeformableMesh* def_mesh = obj->GetComponent<ComponentDeformableMesh>();
			if (def_mesh)
				def_mesh->AttachSkeleton();
		}
		App->objects->ReAttachUIScriptEvents();
		obj->ResetIDs();
		obj->SetPrefab(ID);
		ComponentTransform* transform = (ComponentTransform*)(obj)->GetComponent(ComponentType::TRANSFORM);
		transform->SetLocalPosition(pos.x, pos.y, pos.z);
		if (set_selected) {
			App->objects->SetNewSelectedObject(obj);
			App->camera->fake_camera->Look(parent->children.back()->GetBB().CenterPoint());
			App->camera->reference = parent->children.back()->GetBB().CenterPoint();
		}

		delete prefab;
	}
	else {
		LOG_ENGINE("Error loading prefab %s", path.data());
	}
}
