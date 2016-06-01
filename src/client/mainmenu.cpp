/*
 * This file is part of Spacel game.
 *
 * Copyright 2016, Loic Blot <loic.blot@unix-experience.fr>
 * Copyright 2016, Jeremy Lomoro <jeremy.lomoro@tuxsrv.fr>
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
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include "mainmenu.h"
#include "spacelgame.h"

using namespace Urho3D;

namespace spacel {

MainMenu::MainMenu(Context *context) :
		GenericMenu(context),
		m_main(true),
		m_music_active(true)
{
	m_cache = GetSubsystem<ResourceCache>();
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/MainMenuStyle.xml"));
	m_l10n = GetSubsystem<Localization>();
	m_window_menu = new Window(context_);
	m_title = new Text(context_);
}

void MainMenu::Start()
{
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));
	Background();
	Title();
	Menu();
	Music(true);
}

void MainMenu::Background()
{
	Texture2D *logoTexture = m_cache->GetResource<Texture2D>("Textures/cwd_up.png");

	if (!logoTexture) {
		log_->Write(LOG_ERROR, "Background texture not loaded");
		return;
	}

	backgroundSprite_ = m_ui_elem->CreateChild<Sprite>();
	backgroundSprite_->SetTexture(logoTexture);
	backgroundSprite_->SetSize(m_ui_elem->GetSize().x_, m_ui_elem->GetSize().y_);
	backgroundSprite_->SetPriority(-100);
}

void MainMenu::Title()
{
	m_title->SetStyle("Title");
	m_title->SetText("Spacel Game");
	m_title->SetHorizontalAlignment(HA_CENTER);
	m_title->SetVerticalAlignment(VA_TOP);
	m_ui_elem->AddChild(m_title);
}

void MainMenu::Menu()
{
	Localization *l10n = GetSubsystem<Localization>();

	m_title->SetText("Spacel Game");
	m_ui_elem->AddChild(m_window_menu);
	m_window_menu->SetStyle("Window");
	m_window_menu->SetName("Menu principal");
	m_window_menu->SetAlignment(HA_CENTER, VA_CENTER);
	m_window_menu->SetOpacity(0.75f);

	Button *singleplayer = new Button(context_);
	singleplayer->SetStyle("Button");
	singleplayer->SetPosition(0, m_window_menu->GetPosition().y_ + singleplayer->GetSize().y_ + 65);
	singleplayer->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(singleplayer);
	CreateButtonLabel(singleplayer, l10n->Get("Play"));

	Button *multiplayer = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	multiplayer->SetStyle("Button");
	multiplayer->SetPosition(0, singleplayer->GetPosition().y_ + singleplayer->GetSize().y_ + s_space_button);
	multiplayer->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(multiplayer);
	CreateButtonLabel(multiplayer, l10n->Get("Multiplayer"));

	Button *settings = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	settings->SetStyle("Button");
	settings->SetPosition(0, multiplayer->GetPosition().y_ + multiplayer->GetSize().y_ + s_space_button);
	settings->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(settings);
	CreateButtonLabel(settings, l10n->Get("Settings"));

	Button *exit = new Button(context_);
	exit->SetStyle("Button");
	exit->SetPosition(0, settings->GetPosition().y_ + settings->GetSize().y_ + s_space_button);
	exit->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(exit);
	CreateButtonLabel(exit, l10n->Get("Exit"));

	Button *music = new Button(context_);
	m_ui_elem->AddChild(music);
	if (m_music_active)
		music->SetStyle("SoundButton");
	else
		music->SetStyle("SoundButtonOff");
	music->SetAlignment(HA_RIGHT, VA_BOTTOM);

	SubscribeToEvent(music, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMusicPressed));
	SubscribeToEvent(exit, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleClosePressed));
	SubscribeToEvent(settings, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MainMenu, HandleUpdate));
}

void MainMenu::HandleClosePressed(StringHash eventType, VariantMap &eventData)
{
	engine_->Exit();
}

void MainMenu::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
	using namespace KeyDown;
	UI *ui = GetSubsystem<UI>();
	// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
	int key = eventData[P_KEY].GetInt();
	switch (key) {
		case KEY_ESC:
		if (!isMain()) {
			m_window_menu->RemoveAllChildren();
			m_main = true;
			Menu();
		}
	}
}

void MainMenu::HandleSettingsPressed(StringHash eventType, VariantMap &eventData)
{
	m_window_menu->RemoveAllChildren();
	m_title->SetText("Spacel " + m_l10n->Get("Settings"));
	m_main = false;

	Button *graphics = new Button(context_);
	graphics->SetStyle("Button");
	graphics->SetPosition(0, m_window_menu->GetPosition().y_ + graphics->GetSize().y_ + 65);
	graphics->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(graphics);
	CreateButtonLabel(graphics, m_l10n->Get("Graphics"));

	Button *sound = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	sound->SetStyle("Button");
	sound->SetPosition(0, graphics->GetPosition().y_ + graphics->GetSize().y_ + s_space_button);
	sound->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(sound);
	CreateButtonLabel(sound, m_l10n->Get("Sound"));

	Button *back = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	back->SetStyle("Button");
	back->SetPosition(0, sound->GetPosition().y_ + sound->GetSize().y_ + s_space_button);
	back->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(back);
	CreateButtonLabel(back, m_l10n->Get("Back"));

	SubscribeToEvent(graphics, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleGraphicsPressed));
	SubscribeToEvent(sound, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSoundsPressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleExitPressed));
}

void MainMenu::HandleGraphicsPressed(StringHash eventType, VariantMap &eventData)
{
	m_window_menu->RemoveAllChildren();
	m_title->SetText("Spacel " + m_l10n->Get("Graphics"));
	m_main = false;

	Button *back = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	back->SetStyle("Button");
	back->SetPosition(0,  m_window_menu->GetPosition().y_ + back->GetSize().y_ + 65);
	back->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(back);
	CreateButtonLabel(back, m_l10n->Get("Back"));

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
}

void MainMenu::HandleSoundsPressed(StringHash eventType, VariantMap &eventData)
{
	m_window_menu->RemoveAllChildren();
	m_title->SetText("Spacel " + m_l10n->Get("Sound"));
	m_main = false;

	Button *back = new Button(context_);
	// Note, must be part of the UI system before SetSize calls!
	back->SetStyle("Button");
	back->SetPosition(0,  m_window_menu->GetPosition().y_ + back->GetSize().y_ + 65);
	back->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(back);
	CreateButtonLabel(back, m_l10n->Get("Back"));

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
}

void MainMenu::HandleExitPressed(StringHash eventType, VariantMap &eventData)
{
	m_window_menu->RemoveAllChildren();
	m_main = true;
	Menu();
}

void MainMenu::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
	backgroundSprite_->SetSize(GetSubsystem<UI>()->GetRoot()->GetSize().x_,
							   GetSubsystem<UI>()->GetRoot()->GetSize().y_);
}

void MainMenu::HandleMusicPressed(StringHash eventType, VariantMap &eventData)
{
	if (m_music_active)
		Music(false);
	else
		Music(true);
}

void MainMenu::Music(const bool active)
{
	m_music_active = active;
	log_->WriteRaw("Changement état de la musique");
}

}
