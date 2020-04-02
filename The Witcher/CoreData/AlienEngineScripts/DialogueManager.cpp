#include "DialogueManager.h"
#include "EventManager.h"
#include "..\..\ComponentText.h"

DialogueManager::DialogueManager() : Alien()
{
}

DialogueManager::~DialogueManager()
{
}

void DialogueManager::Start()
{
	eventManager = (EventManager*)GetComponentScript("EventManager");
	audioEmitter = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);
	text = (ComponentText*)GameObject::FindWithName("SubtitlesText")->GetComponent(ComponentType::UI_TEXT);
 
	LoadJSONDialogues(); 
}

void DialogueManager::LoadJSONDialogues()
{
	// Credits to Yessica
	std::string json_path = std::string("Configuration/Subtitles/InGameDialogues.json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());
	JSONfilepack* jsonDoc = JSONfilepack::GetJSON(json_path.c_str());
	if (jsonDoc)
	{
		JSONArraypack* dialogues = jsonDoc->GetArray("dialogues");
		if (dialogues == nullptr)
		{
			LOG("No dialogues array in fucking dialogues JSON"); 
			return;
		}
		
		do
		{
			LOG("Loading new dialogue data..."); 
			std::string eventName = dialogues->GetString("eventName");
			std::string subtitles = dialogues->GetString("subtitles");
			float time = dialogues->GetNumber("time");

			dialogueData.push_back(std::make_tuple(eventName, subtitles, time));

		} while (dialogues->GetAnotherNode());
	}
	else
		LOG("Couldn't open fucking dialogues JSON"); 
	

	JSONfilepack::FreeJSON(jsonDoc);
}

void DialogueManager::Update()
{
	if ((currentDialogue.subtitlesTime.currentTime += Time::GetDT()) >= currentDialogue.subtitlesTime.totalTime)
	{
		text->SetEnable(false); 
		currentDialogue.Reset(); 
	}

	// Debug
	if (Input::GetKeyDown(SDL_SCANCODE_1))
	{
		Dialogue dialogue; 
		dialogue.audioData.eventName = "Hit_Sword"; 
		dialogue.subtitlesText = "Subtitles subtitles a a a subtitles e e e"; 
		dialogue.priority = "Boss"; 

		InputNewDialogue(dialogue); 
	}; 


	if (Input::GetKeyDown(SDL_SCANCODE_2))
	{
		Dialogue dialogue;
		dialogue.audioData.eventName = "PlayGhouls";
		dialogue.subtitlesText = "Subtitlessssssssss 2 ANOTHER TEXT subtitles";
		dialogue.priority = "Enemies";

		InputNewDialogue(dialogue);
	};


}

bool DialogueManager::InputNewDialogue(Dialogue& dialogue)
{
	// TODO: We don't have a way of knowing the sound's duration, so we won't take into account the priority
	// since we don't know when to reset the current dialogue
	
	/*if ((currentDialogue.audioData.eventName != "noName") && (eventManager->eventPriorities.at(currentDialogue.priority) < eventManager->eventPriorities.at(dialogue.priority)))
	{
		LOG("Dialogue with less priority than the current one will be discarded...");
		return false;
	}; */

	
	OverrideDialogue(dialogue); 

	return true;
}

bool DialogueManager::InputNewDialogue(int index)
{
	assert((index <= (dialogueData.size() - 1)) && "Invalid dialogue index"); 
	if (index > (dialogueData.size() - 1))
	{
		LOG("Invalid dialogue index"); 
		return false; 
	}

	Dialogue dialogue; 
	dialogue.audioData.eventName = std::get<0>(dialogueData.at(index)); 
	dialogue.subtitlesText = std::get<1>(dialogueData.at(index));
	dialogue.subtitlesTime.totalTime = std::get<2>(dialogueData.at(index));
	
	OverrideDialogue(dialogue); 

	return true;
}


void DialogueManager::OverrideDialogue(Dialogue& newDialogue)
{
	// Stop playing 
	audioEmitter->StopSoundByName(currentDialogue.audioData.eventName.c_str()); 
	LOG("Stopped playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str()); 

	// Set Data --> TODO: other members in "audioData"
	currentDialogue.audioData.eventName = std::string(newDialogue.audioData.eventName.c_str()); 
	currentDialogue.priority = std::string(newDialogue.priority.c_str());
	currentDialogue.subtitlesText = std::string(newDialogue.subtitlesText.c_str());

	// Set Subtitles 
	if(text->IsEnabled() == false)
		text->SetEnable(true);
	//text->Reset(); 
	text->SetText(newDialogue.subtitlesText.c_str()); 
	

	// Play new
	//audioEmitter->SetSwitchState(newDialogue.audioData.groupID, newDialogue.audioData.stateID); 
	audioEmitter->StartSound(currentDialogue.audioData.eventName.c_str());
	LOG("Started playing dialogue with event name: %s", currentDialogue.audioData.eventName.c_str());
}
