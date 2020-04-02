#include "MainMenu_Buttons.h"
#include "Extra_Menus.h"

MainMenu_Buttons::MainMenu_Buttons() : Alien()
{
}

MainMenu_Buttons::~MainMenu_Buttons()
{
}

void MainMenu_Buttons::NewGame()
{
	SceneManager::LoadScene("newTRIGGER");
}

void MainMenu_Buttons::ExitGame()
{
	AlienEngine::QuitApp();
}

void MainMenu_Buttons::Controls()
{
	GameObject::FindWithName("Extra_Menus")->SetEnable(true);
	((Extra_Menus*)GameObject::FindWithName("Extra_Menus")->GetComponentScript("Extra_Menus"))->MenuSpawn(Extra_Menus::MENU::CONTROLS);
	GameObject::FindWithName("Main_Menu")->SetEnable(false);
}

void MainMenu_Buttons::Settings()
{
}

void MainMenu_Buttons::Muffin()
{
	AlienEngine::OpenURL("https://github.com/OverPowered-Team");
}

