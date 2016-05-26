/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
 *
 * Spacel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spacel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spacel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include <common/porting.h>
#include "settings.h"

using namespace Urho3D;

namespace spacel {

class App : public Application
{
public:
	SharedPtr<Text> text_;
	SharedPtr<Scene> scene_;
	SharedPtr<Node> cameraNode_;

	static const uint8_t space_button = 20;

	App(Context* context): Application(context)
	{
	}

	virtual void Setup()
	{
		m_config = new ClientSettings(context_);

		m_config->load(fs::path_config + DIR_DELIM + "client.json");

		// Called before engine initialization. engineParameters_ member
		// variable can be modified here
		engineParameters_["WindowTitle"] = "Spacel Game";
		engineParameters_["FullScreen"] = m_config->getBool(BSETTING_FULLSCREEN);
		engineParameters_["WindowWidth"] = m_config->getU32(U32SETTING_WINDOW_WIDTH);
		engineParameters_["WindowHeight"] = m_config->getU32(U32SETTING_WINDOW_HEIGHT);
		engineParameters_["WindowResizable"] = m_config->getBool(BSETTING_RESIZABLE_WINDOW);
		engineParameters_["Sound"] = m_config->getBool(BSETTING_ENABLE_SOUND);
		GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
	}

	virtual void Start()
	{
		// Called after engine initialization. Setup application & subscribe to events here
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(App, HandleKeyDown));

		ResourceCache* cache=GetSubsystem<ResourceCache>();

		// Let's use the default style that comes with Urho3D.
		GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

		// Let's create some text to display.
		text_=new Text(context_);
		// Text will be updated later in the E_UPDATE handler. Keep readin'.
		text_->SetText("Spacel Game");
		text_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),40);
		text_->SetColor(Color(.3,0,.3));
		text_->SetHorizontalAlignment(HA_CENTER);
		text_->SetVerticalAlignment(VA_TOP);
		GetSubsystem<UI>()->GetRoot()->AddChild(text_);

		Window* window_menu = new Window(context_);
		GetSubsystem<UI>()->GetRoot()->AddChild(window_menu);
		window_menu->SetName("Menu principal");
		window_menu->SetAlignment(HA_CENTER, VA_CENTER);
		window_menu->SetStyle("Window");

		Text* textSinglePlayer = new Text(context_);
		textSinglePlayer->SetText("Jouer");
		textSinglePlayer->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);
		textSinglePlayer->SetAlignment(HA_CENTER, VA_CENTER);

		Button* singleplayer = new Button(context_);
		// Note, must be part of the UI system before SetSize calls!
		GetSubsystem<UI>()->GetRoot()->AddChild(singleplayer);
		singleplayer->SetStyle("Button");
		singleplayer->SetPosition(0, window_menu->GetPosition().y_ + singleplayer->GetSize().y_ + 15);
		singleplayer->SetHorizontalAlignment(HA_CENTER);
		singleplayer->AddChild(textSinglePlayer);

		Text* textMultiplayer = new Text(context_);
		textMultiplayer->SetText("Multiplayer");
		textMultiplayer->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);
		textMultiplayer->SetAlignment(HA_CENTER, VA_CENTER);

		Button* multiplayer = new Button(context_);
		// Note, must be part of the UI system before SetSize calls!
		GetSubsystem<UI>()->GetRoot()->AddChild(multiplayer);
		multiplayer->SetStyle("Button");
		multiplayer->SetPosition(0, singleplayer->GetPosition().y_ + singleplayer->GetSize().y_ + space_button);
		multiplayer->SetHorizontalAlignment(HA_CENTER);
		multiplayer->AddChild(textMultiplayer);

		Text* textSettings = new Text(context_);
		textSettings->SetText("Options");
		textSettings->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);
		textSettings->SetAlignment(HA_CENTER, VA_CENTER);

		Button* settings = new Button(context_);
		// Note, must be part of the UI system before SetSize calls!
		GetSubsystem<UI>()->GetRoot()->AddChild(settings);
		settings->SetStyle("Button");
		settings->SetPosition(0, multiplayer->GetPosition().y_ + multiplayer->GetSize().y_ + space_button);
		settings->SetHorizontalAlignment(HA_CENTER);
		settings->AddChild(textSettings);

		Text* textExit = new Text(context_);
		textExit->SetText("Quitter");
		textExit->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),30);
		textExit->SetAlignment(HA_CENTER, VA_CENTER);

		Button* exit = new Button(context_);
		// Note, must be part of the UI system before SetSize calls!
		GetSubsystem<UI>()->GetRoot()->AddChild(exit);
		exit->SetStyle("Button");
		exit->SetPosition(0, settings->GetPosition().y_ + settings->GetSize().y_ + space_button);
		exit->SetHorizontalAlignment(HA_CENTER);
		exit->AddChild(textExit);

		window_menu->AddChild(singleplayer);
		window_menu->AddChild(multiplayer);
		window_menu->AddChild(settings);
		window_menu->AddChild(exit);

		SubscribeToEvent(exit,E_RELEASED,URHO3D_HANDLER(App,HandleClosePressed));

		// Let's setup a scene to render.
		scene_=new Scene(context_);
		// Let the scene have an Octree component!
		scene_->CreateComponent<Octree>();
		// Let's add an additional scene component for fun.
		scene_->CreateComponent<DebugRenderer>();

		// Let's put some sky in there.
		// Again, if the engine can't find these resources you need to check
		// the "ResourcePrefixPath". These files come with Urho3D.
		Node* skyNode = scene_->CreateChild("Sky");
		skyNode->SetScale(500.0f); // The scale actually does not matter
		Skybox* skybox = skyNode->CreateComponent<Skybox>();
		skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
		skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

		// We need a camera from which the viewport can render.
		cameraNode_=scene_->CreateChild("Camera");
		Camera* camera=cameraNode_->CreateComponent<Camera>();
		camera->SetFarClip(2000);

		Renderer* renderer=GetSubsystem<Renderer>();
		SharedPtr<Viewport> viewport(new Viewport(context_,scene_,cameraNode_->GetComponent<Camera>()));
		renderer->SetViewport(0,viewport);

		if (!scene_->GetChild("Music")) {
			ResourceCache* cache = GetSubsystem<ResourceCache>();
			Sound* music = cache->GetResource<Sound>("Music/Gnawa-Spirit.ogg");
			// Set the song to loop
			music->SetLooped(true);

			// Create a scene node and a sound source for the music
			Node* musicNode = scene_->CreateChild("Music");
			SoundSource* musicSource = musicNode->CreateComponent<SoundSource>();
			// Set the sound type to music so that master volume control works correctly
			musicSource->SetSoundType(SOUND_MUSIC);
			musicSource->Play(music);
		}
	}

	virtual void Stop()
	{
		m_config->save(fs::path_config + DIR_DELIM + "client.json");
		delete m_config;
	}

	void HandleKeyDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;
		// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
		int key = eventData[P_KEY].GetInt();
		if (key == KEY_ESC)
			engine_->Exit();
	}

	void HandleClosePressed(StringHash eventType,VariantMap& eventData)
	{
		engine_->Exit();
	}
private:
	ClientSettings* m_config;
};

}

URHO3D_DEFINE_APPLICATION_MAIN(spacel::App)
