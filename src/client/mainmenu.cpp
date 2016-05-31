#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include "mainmenu.h"

using namespace Urho3D;

namespace spacel {

MainMenu::MainMenu(Context* context):
	UIElement(context)
{
	cache = GetSubsystem<ResourceCache>();
	uiElem = GetSubsystem<UI>()->GetRoot();
	uiElem->SetDefaultStyle(cache->GetResource<XMLFile>("UI/MainMenuStyle.xml"));
}

void MainMenu::Start()
{
	Background();
	Title();
	Menu();
	Music(true);
}

void MainMenu::Background()
{
	Texture2D *logoTexture = cache->GetResource<Texture2D>("Textures/cwd_up.png");

	if (!logoTexture) {
		log_->Write(LOG_ERROR, "Background texture not loaded");
		return;
	}

	backgroundSprite_ = uiElem->CreateChild<Sprite>();
	backgroundSprite_->SetTexture(logoTexture);
	backgroundSprite_->SetSize(uiElem->GetSize().x_, uiElem->GetSize().y_);
	backgroundSprite_->SetPriority(-100);
}

void MainMenu::Title()
{
	Text *title = new Text(context_);
	title->SetStyle("Title");
	title->SetText("Spacel Game.");
	title->SetHorizontalAlignment(HA_CENTER);
	title->SetVerticalAlignment(VA_TOP);
	uiElem->AddChild(title);
}

void MainMenu::Menu()
{

	Window* window_menu = new Window(context_);
	uiElem->AddChild(window_menu);
	window_menu->SetStyle("Window");
	window_menu->SetName("Menu principal");
	window_menu->SetAlignment(HA_CENTER, VA_CENTER);
	window_menu->SetOpacity(0.75f);

	Button* singleplayer = new Button(context_);
	singleplayer->SetStyle("Button");
	singleplayer->SetPosition(0, window_menu->GetPosition().y_ + singleplayer->GetSize().y_ + 65);
	singleplayer->SetHorizontalAlignment(HA_CENTER);
	window_menu->AddChild(singleplayer);

	Text* textSinglePlayer = new Text(context_);
	singleplayer->AddChild(textSinglePlayer);
	textSinglePlayer->SetStyle("TextButton");
	textSinglePlayer->SetText("Jouer");

	Button* multiplayer = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	multiplayer->SetStyle("Button");
	multiplayer->SetPosition(0, singleplayer->GetPosition().y_ + singleplayer->GetSize().y_ + space_button);
	multiplayer->SetHorizontalAlignment(HA_CENTER);
	window_menu->AddChild(multiplayer);

	Text* textMultiplayer = new Text(context_);
	multiplayer->AddChild(textMultiplayer);
	textMultiplayer->SetStyle("TextButton");
	textMultiplayer->SetText("Multiplayer");

	Button* settings = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	settings->SetStyle("Button");
	settings->SetPosition(0, multiplayer->GetPosition().y_ + multiplayer->GetSize().y_ + space_button);
	settings->SetHorizontalAlignment(HA_CENTER);
	window_menu->AddChild(settings);

	Text* textSettings = new Text(context_);
	settings->AddChild(textSettings);
	textSettings->SetStyle("TextButton");
	textSettings->SetText("Options");

	Button* exit = new Button(context_);
	exit->SetStyle("Button");
	exit->SetPosition(0, settings->GetPosition().y_ + settings->GetSize().y_ + space_button);
	exit->SetHorizontalAlignment(HA_CENTER);
	window_menu->AddChild(exit);

	Text* textExit = new Text(context_);
	exit->AddChild(textExit);
	textExit->SetStyle("TextButton");
	textExit->SetText("Quitter");

	SubscribeToEvent(exit,E_RELEASED,URHO3D_HANDLER(MainMenu,HandleClosePressed));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MainMenu,HandleUpdate));
}

void MainMenu::HandleClosePressed(StringHash eventType, VariantMap& eventData)
{
	//TODO : Quitter proprement
	engine_->Exit();
}

void MainMenu::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	backgroundSprite_->SetSize(GetSubsystem<UI>()->GetRoot()->GetSize().x_, GetSubsystem<UI>()->GetRoot()->GetSize().y_);
}

void MainMenu::Music(bool active)
{
	Button* music = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	GetSubsystem<UI>()->GetRoot()->AddChild(music);
	music->SetStyle("SoundButton");
	music->SetAlignment(HA_RIGHT, VA_BOTTOM);

	/*if (!scene_->GetChild("Music") && active) {
		Sound* music = cache->GetResource<Sound>("Music/Gnawa-Spirit.ogg");
		// Set the song to loop
		music->SetLooped(true);

		// Create a scene node and a sound source for the music
		Node* musicNode = scene_->CreateChild("Music");
		SoundSource* musicSource = musicNode->CreateComponent<SoundSource>();
		// Set the sound type to music so that master volume control works correctly
		musicSource->SetSoundType(SOUND_MUSIC);
		musicSource->Play(music);
	} else if (scene_->GetChild("Music") && !active) {
		scene_->GetChild("Music")->Remove();
	}
	if (active)
		music->SetStyle("SoundButton");
	else
		music->SetStyle("SoundButtonOff");*/
}

}
