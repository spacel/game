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

#include "mainmenu.h"
#include <common/engine/generators.h>
#include <common/engine/space.h>
#include <common/time_utils.h>

#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Input/Input.h>
#include <project_defines.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Slider.h>
#include <iostream>

using namespace Urho3D;

namespace spacel {

enum MainMenuIds
{
	MAINMENUID_MASTER = 0,
	MAINMENUID_SINGLEPLAYER,
	MAINMENUID_SINGLEPLAYER_NEWGAME,
	MAINMENUID_SINGLEPLAYER_LOADGAME,
	MAINMENUID_MULTIPLAYER,
	MAINMENUID_SETTINGS,
	MAINMENUID_SETTINGS_GRAPHICS,
	MAINMENUID_SETTINGS_SOUND,
};

#define MAINMENU_BUTTON_SPACE 20

MainMenu::MainMenu(Context *context, ClientSettings *config, SpacelGame *main):
		GenericMenu(context, config),
		m_main(main),
		m_enable_menu_music(m_config->getBool(BSETTING_ENABLE_MUSIC))
{
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/MainMenuStyle.xml"));
	m_window_menu = new Window(context_);
	m_title = new Text(context_);
	m_universe_infos = new Text(context_);
	m_music_button = new Button(context_);
	m_error_bubble_timer = new Timer();
	except_unsubscribe.Push("E_KEYDOWN");
	m_modal_window = nullptr;
}

MainMenu::~MainMenu()
{
	UnsubscribeFromAllEvents();
	m_window_menu->RemoveAllChildren();
	m_window_menu->Remove();
	delete m_error_bubble_timer;
}

void MainMenu::Start()
{
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));
	Background();
	Title();
	VariantMap v;
	HandleMasterMenu(StringHash(), v);

	// Main menu music
	m_ui_elem->AddChild(m_music_button);
	m_music_button->SetStyle(m_enable_menu_music ? "SoundButton" : "SoundButtonOff");
	PlayMusic(m_enable_menu_music);
	GetSubsystem<Input>()->SetMouseVisible(true);
}

void MainMenu::Background()
{
	Texture2D *logoTexture = m_cache->GetResource<Texture2D>("Textures/background_menu.png");

	if (!logoTexture) {
		URHO3D_LOGERROR("Background menu texture not loaded");
		return;
	}

	m_menu_background = m_ui_elem->CreateChild<Sprite>();
	m_menu_background->SetTexture(logoTexture);
	m_menu_background->SetSize(m_ui_elem->GetSize().x_, m_ui_elem->GetSize().y_);
	m_menu_background->SetPriority(-100);
}

void MainMenu::Title()
{
	m_title->SetStyle("Title");
	m_title->SetText(PROJECT_LABEL);
	m_title->SetPosition(0, 50);
	m_ui_elem->AddChild(m_title);
}

void MainMenu::HandleMasterMenu(StringHash, VariantMap &)
{
	m_menu_id = MAINMENUID_MASTER;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	m_title->SetText(PROJECT_LABEL);
	m_ui_elem->AddChild(m_window_menu);
	m_window_menu->SetStyle("Window");
	m_window_menu->SetName("Main Menu");

	Button *singleplayer = CreateMainMenuButton("Play");
	singleplayer->SetPosition(0, singleplayer->GetSize().y_ + 65);

	Button *multiplayer = CreateMainMenuButton("Multiplayer");
	multiplayer->SetPosition(0, singleplayer->GetPosition().y_ + singleplayer->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *settings = CreateMainMenuButton("Settings");
	settings->SetPosition(0, multiplayer->GetPosition().y_ + multiplayer->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *exit = CreateMainMenuButton("Exit");
	exit->SetPosition(0, settings->GetPosition().y_ + settings->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	SubscribeToEvent(m_music_button, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMusicPressed));
	SubscribeToEvent(exit, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleClosePressed));
	SubscribeToEvent(singleplayer, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSingleplayerPressed));
	SubscribeToEvent(multiplayer, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMultiplayerPressed));
	SubscribeToEvent(settings, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MainMenu, HandleUpdate));
}

void MainMenu::HandleClosePressed(StringHash, VariantMap &eventData)
{
	m_engine->Exit();
}

void MainMenu::HandleKeyDown(StringHash, VariantMap &eventData)
{
	using namespace KeyDown;
	// Check for pressing ESC. Note the m_engine member variable for convenience access to the Engine object
	int key = eventData[P_KEY].GetInt();

	switch (key) {
		case KEY_ESCAPE:
			switch (m_menu_id) {
				case MAINMENUID_MASTER:
					m_engine->Exit();
					break;
				case MAINMENUID_SETTINGS:
				case MAINMENUID_SINGLEPLAYER:
				case MAINMENUID_MULTIPLAYER:
					HandleMasterMenu(StringHash(), eventData);
					break;
				case MAINMENUID_SINGLEPLAYER_NEWGAME:
				case MAINMENUID_SINGLEPLAYER_LOADGAME:
					HandleSingleplayerPressed(StringHash(), eventData);
					break;
				case MAINMENUID_SETTINGS_GRAPHICS:
				case MAINMENUID_SETTINGS_SOUND:
					HandleSettingsPressed(StringHash(), eventData);
					break;
				default:
					break;
			}
			break;
		case KEY_DELETE:
			if (m_menu_id == MAINMENUID_SINGLEPLAYER_LOADGAME) {
				DeleteUniverse();
			}
			break;
		case KEY_F12:
			TakeScreenshot();
			break;
		default:
			break;
	}
}

void MainMenu::HandleSingleplayerPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SINGLEPLAYER;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Singleplayer");

	Button *newgame = CreateMainMenuButton("New game");
	newgame->SetPosition(0, newgame->GetSize().y_ + 65);

	Button *loadgame = CreateMainMenuButton("Load game");
	loadgame->SetPosition(0, newgame->GetPosition().y_ + loadgame->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, loadgame->GetPosition().y_ + back->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	SubscribeToEvent(newgame, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleNewGamePressed));
	SubscribeToEvent(loadgame, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleLoadGamePressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleNewGamePressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SINGLEPLAYER_NEWGAME;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("New universe");

	LineEdit *universename = CreateMainMenuLineEdit("universe_name", "Universe Name: ", 0, 65);
	universename->SetMaxLength(32);

	LineEdit *universeseed = CreateMainMenuLineEdit("create_universe_seed", "Seed: ", 0,
		universename->GetPosition().y_ + universename->GetSize().y_ + MAINMENU_BUTTON_SPACE);
	universeseed->SetMaxLength(20);

	Button *generateSeed = CreateMainMenuButton("Generate seed", "ButtonInLine", "TextButtonInLine");
	generateSeed->SetPosition(-20, universename->GetPosition().y_ + universename->GetSize().y_ + MAINMENU_BUTTON_SPACE);
	generateSeed->SetHorizontalAlignment(HA_RIGHT);

	Button *create = CreateMainMenuButton("Create", "ButtonInLine", "TextButtonInLine");
	create->SetPosition(0 + 50, m_window_menu->GetSize().y_ - create->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	create->SetHorizontalAlignment(HA_LEFT);

	Button *back = CreateMainMenuButton("Cancel", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 50, m_window_menu->GetSize().y_ - back->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	back->SetHorizontalAlignment(HA_RIGHT);

	SubscribeToEvent(generateSeed, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleGenerateSeedPressed));
	SubscribeToEvent(create, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleLaunchGamePressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSingleplayerPressed));
}

void MainMenu::HandleLaunchGamePressed(StringHash, VariantMap &eventData)
{
	// @TODO Find the better way to load & creator universe here
	String universe_name = "";
	String universe_seed = "";
	uint64_t seed = 0;
	bool universe_creation = false;

	// Universe creation
	LineEdit *name_le = dynamic_cast<LineEdit *>(m_window_menu->GetChild("universe_name", true));
	LineEdit *seed_le = dynamic_cast<LineEdit *>(m_window_menu->GetChild("create_universe_seed", true));
	if (name_le != nullptr) {
		universe_name = name_le->GetText();
		universe_seed = seed_le->GetText();
		URHO3D_LOGDEBUGF("User wants to create universe %s", universe_name.CString());
		universe_creation = true;
	}
	else {
		ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild("loading_univerise_listview", true));
		assert(lv);

		if (lv->GetSelectedItem() != nullptr) {
			universe_name = lv->GetSelectedItem()->GetName();
			engine::UniverseGenerator::SetSeed(engine::Universe::instance()->GetUniverseSeed());
			URHO3D_LOGDEBUGF("User wants to load universe %s", universe_name.CString());
		}
		// @TODO find a way to retrieve the universe_name selected for loading
	}

	universe_name = universe_name.Replaced(':', '_').Replaced('.', '_')
			.Replaced(' ', '_').ToLower();

	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir(
			"spacel", "universe") + universe_name;

	if (universe_creation) {
		if (!universe_name.Empty()) {
			if (GetSubsystem<FileSystem>()->DirExists(path_universe)) {
				URHO3D_LOGERRORF("Universe %s already exists", universe_name.CString());
				//TODO : Send parameter variables
				ShowErrorBubble(
						Urho3D::ToString(m_l10n->Get("Universe %s already exists").CString(), universe_name.CString()));
				return;
			}
		}
		else {
			ShowErrorBubble("Universe name is empty");
			URHO3D_LOGERROR("Universe name is empty");
			return;
		}

		if (universe_seed.Empty()) {
			seed = engine::UniverseGenerator::generate_seed();
		} else {
			seed  = std::stoull(seed_le->GetText().CString());
		}
		engine::UniverseGenerator::SetSeed(seed);

		if (!GetSubsystem<FileSystem>()->DirExists(path_universe)) {
			GetSubsystem<FileSystem>()->CreateDir(path_universe);
		}

		engine::DatabaseSQLite3 game_database(std::string(path_universe.CString()));
		game_database.CreateUniverse(std::string(universe_name.CString()), seed);
	} else if (universe_name.Empty()) {
		ShowErrorBubble("You must select a universe.");
		URHO3D_LOGERROR("No universe selected when loading.");
		return;
	}

	m_main->ChangeGameGlobalUI(GLOBALUI_LOADINGSCREEN, (void *)universe_name.CString());
}

void MainMenu::HandleLoadGamePressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SINGLEPLAYER_LOADGAME;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Load universe");

	Vector<String> list_universe;
	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "universe");
	GetSubsystem<FileSystem>()->ScanDir(list_universe, path_universe, "*", SCAN_DIRS, false);

	ListView *universes_listview = new ListView(context_);
	m_window_menu->AddChild(universes_listview);
	universes_listview->SetStyle("ListView");
	universes_listview->SetName("loading_univerise_listview");
	universes_listview->SetSize(m_window_menu->GetSize().x_ / 2, m_window_menu->GetSize().y_ - 125);

	if (!list_universe.Empty()) {
		for (Vector<String>::Iterator it = list_universe.Begin(); it != list_universe.End(); ++it) {
			if (it->Compare(".") != 0 && it->Compare("..") != 0) {
				Text *text = new Text(context_);
				universes_listview->AddItem(text);
				text->SetStyle("ListViewText");
				text->SetName(*it);
				text->SetText(*it);
			}
		}
	}

	// TODO: Preview image (WIP)
	Texture2D *PreviewTexture = m_cache->GetResource<Texture2D>("Textures/cwd.png");
	if (!PreviewTexture) {
		PreviewTexture = m_cache->GetResource<Texture2D>("Textures/no_preview.png");
		if (!PreviewTexture) {
			URHO3D_LOGERROR("No_Preview texture not loaded");
			return;
		}
	}

	m_preview = m_window_menu->CreateChild<Sprite>();
	m_preview->SetTexture(PreviewTexture);
	m_preview->SetSize(150, 150);
	m_preview->SetPosition(universes_listview->GetSize().x_ + (universes_listview->GetSize().x_ / 2),
		m_window_menu->GetPosition().y_ + MAINMENU_BUTTON_SPACE);
	m_preview->SetPriority(-90);

	// TODO: Information universe (WIP)
	m_window_menu->AddChild(m_universe_infos);
	m_universe_infos->SetStyle("TextUnivInfo");
	m_universe_infos->SetPosition(universes_listview->GetSize().x_ + 50, m_preview->GetPosition().y_ + m_preview->GetSize().y_ + 50);
	m_universe_infos->SetText(m_l10n->Get("Universe age: ") + "\n" + m_l10n->Get("Seed: "));

	Button *launch = CreateMainMenuButton("Launch", "ButtonInLine", "TextButtonInLine");
	launch->SetPosition(0 + 50, m_window_menu->GetSize().y_ - launch->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	launch->SetHorizontalAlignment(HA_LEFT);

	Button *back = CreateMainMenuButton("Cancel", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 50, m_window_menu->GetSize().y_ - back->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	back->SetHorizontalAlignment(HA_RIGHT);

	SubscribeToEvent(universes_listview, E_ITEMCLICKED, URHO3D_HANDLER(MainMenu, HandleInfosUniverseClicked));
	SubscribeToEvent(universes_listview, E_ITEMDOUBLECLICKED, URHO3D_HANDLER(MainMenu, HandleLaunchGamePressed));
	SubscribeToEvent(launch, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleLaunchGamePressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSingleplayerPressed));
}

void MainMenu::HandleMultiplayerPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_MULTIPLAYER;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Multiplayer");

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, back->GetSize().y_ + 65);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleSettingsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Settings");

	Button *graphics = CreateMainMenuButton("Graphics");
	graphics->SetPosition(0, graphics->GetSize().y_ + 65);

	Button *sound = CreateMainMenuButton("Sound");
	sound->SetPosition(0, graphics->GetPosition().y_ + graphics->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, sound->GetPosition().y_ + sound->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	SubscribeToEvent(graphics, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleGraphicsPressed));
	SubscribeToEvent(sound, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSoundsPressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleGraphicsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_GRAPHICS;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Graphics");

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, back->GetSize().y_ + 65);

	Text *text_full_screen = new Text(context_);
	//full_screen->AddChild(text_full_screen);
	text_full_screen->SetPosition(0, back->GetPosition().y_ + back->GetPosition().y_ + MAINMENU_BUTTON_SPACE);
	text_full_screen->SetStyle("Text");
	text_full_screen->SetText(m_l10n->Get("Full screen"));

	CheckBox *full_screen = new CheckBox(context_);
	full_screen->SetPosition(text_full_screen->GetPosition().x_ + 150,
		back->GetPosition().y_ + back->GetPosition().y_ + MAINMENU_BUTTON_SPACE);
	full_screen->SetName("CheckBox");
	full_screen->SetChecked(m_config->getBool(BSETTING_FULLSCREEN));

	m_window_menu->AddChild(full_screen);
	m_window_menu->AddChild(text_full_screen);

	full_screen->SetStyleAuto();

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(full_screen, E_TOGGLED, URHO3D_HANDLER(MainMenu, HandleFullScreenPressed));
}

void MainMenu::HandleFullScreenPressed(StringHash eventType, VariantMap &eventData)
{
	m_config->setBool(BSETTING_FULLSCREEN, !m_config->getBool(BSETTING_FULLSCREEN));
}

void MainMenu::HandleSoundsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_SOUND;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Sound");
	// master sound
	Slider *slider_sound_master = CreateSliderWithLabels("Master", "Sound Master: ", 0, MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_MASTER_GAIN);

	// sound music
	Slider *slider_sound_music = CreateSliderWithLabels("Music", "Sound Music: ", 0,
		slider_sound_master->GetPosition().y_ + slider_sound_master->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_MUSIC_GAIN);

	// sound effect
	Slider *slider_sound_effect = CreateSliderWithLabels("Effect", "Sound Effect: ", 0,
		slider_sound_music->GetPosition().y_ + slider_sound_music->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_EFFECT_GAIN);

	// sound ambient
	Slider *slider_sound_ambient = CreateSliderWithLabels("Ambient", "Sound Ambient: ", 0,
		slider_sound_effect->GetPosition().y_ + slider_sound_effect->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_AMBIENT_GAIN);

	// sound voice
	Slider *slider_sound_voice = CreateSliderWithLabels("Voice", "Sound Voice: ", 0,
		slider_sound_ambient->GetPosition().y_ + slider_sound_ambient->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_VOICE_GAIN);

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, slider_sound_voice->GetSize().y_ + slider_sound_voice->GetPosition().y_ + MAINMENU_BUTTON_SPACE);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(slider_sound_master, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_music, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_effect, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_ambient, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_voice, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
}

void MainMenu::HandleSoundsVolume(StringHash eventType, VariantMap &eventData)
{
	Slider *slider_sound_music = static_cast<Slider *>(eventData[SliderChanged::P_ELEMENT].GetPtr());
	String name = slider_sound_music->GetName();
	Text *value_sound_music = static_cast<Text *>(m_window_menu->GetChild("value" + name, false));
	value_sound_music->SetText(Urho3D::ToString("%f %%", floor(slider_sound_music->GetValue())));

	GetSubsystem<Audio>()->SetMasterGain(name, slider_sound_music->GetValue() / 100);

	if (name == "Master") {
		m_config->setFloat(FLOATSETTINGS_SOUND_MASTER_GAIN, slider_sound_music->GetValue());
	}
	else if (name == "Music") {
		m_config->setFloat(FLOATSETTINGS_SOUND_MUSIC_GAIN, slider_sound_music->GetValue());
	}
	else if (name == "Effect") {
		m_config->setFloat(FLOATSETTINGS_SOUND_EFFECT_GAIN, slider_sound_music->GetValue());
	}
	else if (name == "Ambient") {
		m_config->setFloat(FLOATSETTINGS_SOUND_AMBIENT_GAIN, slider_sound_music->GetValue());
	}
	else if (name == "Voice") {
		m_config->setFloat(FLOATSETTINGS_SOUND_VOICE_GAIN, slider_sound_music->GetValue());
	}

}

void MainMenu::HandleGenerateSeedPressed(StringHash eventType, VariantMap &eventData)
{
	String seed_str = ToString(std::to_string(engine::UniverseGenerator::generate_seed()).c_str());
	static_cast<LineEdit *>(m_window_menu->GetChild("create_universe_seed", true))->SetText(seed_str);
}

void MainMenu::HandleUpdate(StringHash, VariantMap &eventData)
{
	m_menu_background->SetSize(m_ui_elem->GetSize().x_, m_ui_elem->GetSize().y_);

	if (m_enable_error_bubble_timer &&
		m_error_bubble_timer->GetMSec(false) >= m_config->getFloat(FLOATSETTINGS_TIMER_ERROR_BUBBLE)) {
		if (Window *error_bubble_window = dynamic_cast<Window *>(
			m_window_menu->GetChild("error_window_bubble", true))) {
			error_bubble_window->SetVisible(false);
		}

		m_enable_error_bubble_timer = false;
	}
}

void MainMenu::HandleMusicPressed(StringHash, VariantMap &eventData)
{
	m_enable_menu_music = !m_enable_menu_music;
	m_config->setBool(BSETTING_ENABLE_MUSIC, m_enable_menu_music);
	m_music_button->SetStyle(m_enable_menu_music ? "SoundButton" : "SoundButtonOff");
	PlayMusic(m_enable_menu_music);
}

void MainMenu::HandleInfosUniverseClicked(StringHash, VariantMap &eventData)
{
	ListView *lv = static_cast<ListView *>(eventData[ItemSelected::P_ELEMENT].GetPtr());
	assert(lv);

	String universe_name = lv->GetSelectedItem()->GetName();
	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir(
			"spacel", "universe") + universe_name;

	engine::DatabaseSQLite3 game_database(std::string(path_universe.CString()));
	game_database.LoadUniverse(std::string(universe_name.CString()));

	m_universe_infos->SetText(m_l10n->Get("Universe age: ") +
		ToString("%s", timestamp_to_string(engine::Universe::instance()->GetUniverseBirth()).c_str()) + "\n" +
		m_l10n->Get("Seed: ") +
		ToString("%s", std::to_string(engine::Universe::instance()->GetUniverseSeed()).c_str()));
}

void MainMenu::HandleDeleteUniversePressed(StringHash eventType, VariantMap &eventData)
{
	if (eventData[MessageACK::P_OK].GetBool()) {
		ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild("loading_univerise_listview", true));
		assert(lv);
		String universe_name = lv->GetSelectedItem()->GetName();
		Vector<String> list_files_universe;
		const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir(
			"spacel", "universe") + universe_name;
		GetSubsystem<FileSystem>()->ScanDir(list_files_universe, path_universe, "*", SCAN_FILES | SCAN_DIRS | SCAN_HIDDEN, true);

		if (!list_files_universe.Empty() && GetSubsystem<FileSystem>()->DirExists(path_universe)) {
			for (Vector<String>::Iterator it = list_files_universe.Begin(); it != list_files_universe.End(); ++it) {
				if (it->Compare(".") != 0 && it->Compare("..") != 0) {
					String file = ToString("/%s", it->CString());
					GetSubsystem<FileSystem>()->Delete(path_universe + file);
				}
			}

			/*if (GetSubsystem<FileSystem>()->Delete(path_universe)) {
				ShowErrorBubble(ToString(m_l10n->Get("Universe %s deleted").CString(), universe_name.CString()));
				URHO3D_LOGINFOF("Universe %s deleted", universe_name.CString());
			}*/
		}

		lv->RemoveChild(lv->GetSelectedItem());
		lv->UpdateLayout();
	}

	if (m_modal_window != nullptr) {
		m_modal_window->UnsubscribeFromEvent(E_MESSAGEACK);
		m_modal_window->UnsubscribeFromEvent(E_MODALCHANGED);
		m_modal_window = nullptr;
	}
}

void MainMenu::DeleteUniverse()
{
	ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild("loading_univerise_listview", true));
	assert(lv);
	if (lv->GetSelectedItem() == nullptr) {
		return;
	}
	String universe_name = lv->GetSelectedItem()->GetName();

	if (m_modal_window == nullptr) {
		m_modal_window = GetSubsystem<UI>()->LoadLayout(
			m_cache->GetResource<XMLFile>("UI/ModalWindow.xml"));
		m_ui_elem->AddChild(m_modal_window);
		engine::ui::ModalWindow *modal_window = dynamic_cast<engine::ui::ModalWindow *>(m_modal_window.Get());
		assert(modal_window);
		modal_window->InitComponents("Delete universe", ToString(
			m_l10n->Get("Do you want really delete universe: %s ?").CString(),
			universe_name.CString()), false);

		SubscribeToEvent(m_modal_window, E_MESSAGEACK,
			URHO3D_HANDLER(MainMenu, HandleDeleteUniversePressed));
	}
}

void MainMenu::ShowErrorBubble(const String &message)
{
	m_enable_error_bubble_timer = true;
	m_error_bubble_timer->Reset();

	Window *error_bubble_window = dynamic_cast<Window *>(m_window_menu->GetChild("error_window_bubble", true));
	if (!error_bubble_window) {
		error_bubble_window = new Window(context_);
		m_window_menu->AddChild(error_bubble_window);
		error_bubble_window->SetStyle("ErrorBubble");
		error_bubble_window->SetName("error_window_bubble");
	}
	static_cast<Text *>(m_window_menu->GetChild("error_bubble_text", true))->SetText(message);
	error_bubble_window->SetVisible(true);
}

inline void MainMenu::SetTitle(const String &t)
{
	m_title->SetText(m_l10n->Get(t));
}

Button *MainMenu::CreateMainMenuButton(const String &label, const String &button_style, const String &label_style)
{
	Button *b = new Button(context_);
	b->SetStyle(button_style);
	b->SetHorizontalAlignment(HA_CENTER);
	m_window_menu->AddChild(b);
	CreateButtonLabel(b, label, label_style);

	return b;
}

LineEdit *MainMenu::CreateMainMenuLineEdit(const String &name, const String &label, const int x, const int y)
{
	LineEdit *le = new LineEdit(context_);
	le->SetName(name);
	le->SetStyle("LineEdit");
	le->SetPosition(x + 30, y);
	m_window_menu->AddChild(le);
	CreateLineEditLabel(le, label);

	return le;
}

Text *MainMenu::CreateText(const String &text, const String &name, const String &style)
{
	Text *t = new Text(context_);
	t->SetText(m_l10n->Get(text));
	t->SetName(name);
	t->SetStyle(style);
	return t;
}

Slider *MainMenu::CreateSlider(const String &name,
							   const float value, const float range, const String &style)
{
	Slider *s = new Slider(context_);
	m_window_menu->AddChild(s);
	s->SetName(name);
	s->SetRange(range);
	s->SetValue(value);
	s->SetStyle(style);
	s->SetEditable(true);
	s->SetSelected(true);
	return s;
}

Slider *MainMenu::CreateSliderWithLabels(const String &name, const String &label,
										 const int x, const int y, const int setting)
{
	Text *text_sound = CreateText(label);
	text_sound->SetPosition(x, y);
	m_window_menu->AddChild(text_sound);

	Text *value_sound = CreateText(Urho3D::ToString("%f %%", floor(m_config->getFloat(setting))), "value" + name);
	value_sound->SetPosition(text_sound->GetSize().x_, text_sound->GetPosition().y_);
	m_window_menu->AddChild(value_sound);

	Slider *slider_sound = CreateSlider(name, m_config->getFloat(setting));
	m_window_menu->AddChild(slider_sound);
	slider_sound->SetPosition(-10, text_sound->GetPosition().y_);

	return slider_sound;
}
}
