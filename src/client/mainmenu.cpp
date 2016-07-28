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
#include <iostream>
#include <common/engine/generators.h>
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
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/DropDownList.h>

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
	MAINMENUID_SETTINGS_KEYBOARD,
};

// UI components defined in this menu
#define MAINMENU_ELEMENT_UNIVERSE_NAME "universe_name"
#define MAINMENU_ELEMENT_CREATE_UNIVERSE_SEED "create_universe_seed"
#define MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW "loading_universe_listview"
#define MAINMENU_ELEMENT_RESOLUTION "resolution"
#define MAINMENU_ELEMENT_ERROR_WINDOW_BUBBLE "error_window_bubble"
#define MAINMENU_ELEMENT_DELETE_UNIVERSE "delete_universe"
#define MAINMENU_ELEMENT_ERROR_BUBBLE_TEXT "error_bubble_text"

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
	singleplayer->SetPosition(0, MAINMENU_BUTTON_SPACE * 2);

	Button *multiplayer = CreateMainMenuButton("Multiplayer");
	multiplayer->SetPosition(0, singleplayer->GetPosition().y_ + singleplayer->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *settings = CreateMainMenuButton("Settings");
	settings->SetPosition(0, multiplayer->GetPosition().y_ + multiplayer->GetSize().y_ + MAINMENU_BUTTON_SPACE * 2);

	Button *exit = CreateMainMenuButton("Exit");
	exit->SetPosition(0, settings->GetPosition().y_ + settings->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	m_window_menu->SetSize(exit->GetSize().x_ + MAINMENU_BUTTON_SPACE * 2, exit->GetPosition().y_ + exit->GetSize().y_ + MAINMENU_BUTTON_SPACE * 2);

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
				case MAINMENUID_SETTINGS_KEYBOARD:
					HandleSettingsPressed(StringHash(), eventData);
					break;
				default:
					break;
			}
			break;
		case KEY_DELETE:
			if (m_menu_id == MAINMENUID_SINGLEPLAYER_LOADGAME) {
				HandleDeleteUniverse(StringHash(), eventData);
				UnsubscribeFromEvent(E_KEYDOWN);
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
	newgame->SetPosition(0, MAINMENU_BUTTON_SPACE * 2);

	Button *loadgame = CreateMainMenuButton("Load game");
	loadgame->SetPosition(0, newgame->GetPosition().y_ + loadgame->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, loadgame->GetPosition().y_ + back->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	m_window_menu->SetSize(back->GetSize().x_ + MAINMENU_BUTTON_SPACE * 2, back->GetPosition().y_ + back->GetSize().y_ + MAINMENU_BUTTON_SPACE * 2);

	SubscribeToEvent(newgame, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleNewGamePressed));
	SubscribeToEvent(loadgame, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleLoadGamePressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleNewGamePressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SINGLEPLAYER_NEWGAME;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);
	m_window_menu->SetSize(800, 600);

	SetTitle("New universe");

	LineEdit *universename = CreateMainMenuLineEdit(MAINMENU_ELEMENT_UNIVERSE_NAME, "Universe Name: ", -15, 30 + (MAINMENU_BUTTON_SPACE * 2));
	universename->SetMaxLength(32);

	LineEdit *universeseed = CreateMainMenuLineEdit("create_universe_seed", "Seed: ", -15,
		universename->GetPosition().y_ + universename->GetSize().y_ + MAINMENU_BUTTON_SPACE);
	universeseed->SetMaxLength(20);

	Button *generateSeed = CreateMainMenuButton("Generate seed", "ButtonInLine", "TextButtonInLine");
	generateSeed->SetPosition(-MAINMENU_BUTTON_SPACE, universeseed->GetPosition().y_);
	generateSeed->SetHorizontalAlignment(HA_RIGHT);

	Button *create = CreateMainMenuButton("Create", "ButtonInLine", "TextButtonInLine");
	create->SetPosition(0 + 100, -MAINMENU_BUTTON_SPACE * 2);
	create->SetHorizontalAlignment(HA_LEFT);
	create->SetVerticalAlignment(VA_BOTTOM);

	Button *back = CreateMainMenuButton("Cancel", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 100, -MAINMENU_BUTTON_SPACE * 2);
	back->SetHorizontalAlignment(HA_RIGHT);
	back->SetVerticalAlignment(VA_BOTTOM);

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
	LineEdit *name_le = dynamic_cast<LineEdit *>(
		m_window_menu->GetChild(MAINMENU_ELEMENT_UNIVERSE_NAME, true));
	LineEdit *seed_le = dynamic_cast<LineEdit *>(
		m_window_menu->GetChild(MAINMENU_ELEMENT_CREATE_UNIVERSE_SEED, true));
	if (name_le != nullptr) {
		universe_name = name_le->GetText();
		universe_seed = seed_le->GetText();
		URHO3D_LOGDEBUGF("User wants to create universe %s", universe_name.CString());
		universe_creation = true;
	}
	else {
		ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild(MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW, true));
		assert(lv);

		if (lv->GetSelectedItem() != nullptr) {
			universe_name = lv->GetSelectedItem()->GetName();
			engine::UniverseGenerator::SetSeed(engine::Universe::instance()->GetUniverseSeed());
			URHO3D_LOGDEBUGF("User wants to load universe %s", universe_name.CString());
		}
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
		ShowErrorBubble(m_l10n->Get("You must select a universe."));
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
	m_window_menu->SetSize(800, 600);

	SetTitle("Load universe");

	Vector<String> list_universe;
	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir("spacel", "universe");
	GetSubsystem<FileSystem>()->ScanDir(list_universe, path_universe, "*", SCAN_DIRS, false);

	ListView *universes_listview = new ListView(context_);
	m_window_menu->AddChild(universes_listview);
	universes_listview->SetStyle("ListView");
	universes_listview->SetName(MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW);
	universes_listview->SetSize(m_window_menu->GetSize().x_ / 2, m_window_menu->GetSize().y_ - 150);

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
	m_preview->SetPosition(universes_listview->GetSize().x_ + 125,
		30 + (MAINMENU_BUTTON_SPACE * 2));
	m_preview->SetPriority(-90);

	UpdateUniverseInfos();

	Button *launch = CreateMainMenuButton("Launch", "ButtonInLine", "TextButtonInLine");
	launch->SetPosition(0 + 100, -MAINMENU_BUTTON_SPACE * 2);
	launch->SetHorizontalAlignment(HA_LEFT);
	launch->SetVerticalAlignment(VA_BOTTOM);

	Button *delete_univers = CreateMainMenuButton("Delete universe", "ButtonInLineInactive", "TextButtonInLine");
	delete_univers->SetName(MAINMENU_ELEMENT_DELETE_UNIVERSE);
	delete_univers->SetPosition(0 - 100, -100);
	delete_univers->SetHorizontalAlignment(HA_RIGHT);
	delete_univers->SetVerticalAlignment(VA_BOTTOM);

	Button *back = CreateMainMenuButton("Back", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 100, -MAINMENU_BUTTON_SPACE * 2);
	back->SetHorizontalAlignment(HA_RIGHT);
	back->SetVerticalAlignment(VA_BOTTOM);

	SubscribeToEvent(universes_listview, E_ITEMCLICKED, URHO3D_HANDLER(MainMenu, HandleInfosUniverseClicked));
	SubscribeToEvent(universes_listview, E_ITEMDOUBLECLICKED, URHO3D_HANDLER(MainMenu, HandleLaunchGamePressed));
	SubscribeToEvent(launch, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleLaunchGamePressed));
	SubscribeToEvent(delete_univers, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleDeleteUniverse));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSingleplayerPressed));
}

void MainMenu::HandleMultiplayerPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_MULTIPLAYER;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Multiplayer");

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, MAINMENU_BUTTON_SPACE * 2);

	m_window_menu->SetSize(back->GetSize().x_ + MAINMENU_BUTTON_SPACE * 2,
		back->GetPosition().y_ + back->GetSize().y_ + MAINMENU_BUTTON_SPACE * 2);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleSettingsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Settings");

	Button *graphics = CreateMainMenuButton("Graphics");
	graphics->SetPosition(0, MAINMENU_BUTTON_SPACE * 2);

	Button *sound = CreateMainMenuButton("Sound");
	sound->SetPosition(0, graphics->GetPosition().y_ + graphics->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *control = CreateMainMenuButton("Control");
	control->SetPosition(0, sound->GetPosition().y_ + sound->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, control->GetPosition().y_ + control->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	m_window_menu->SetSize(back->GetSize().x_ + MAINMENU_BUTTON_SPACE * 2,
		back->GetPosition().y_ + back->GetSize().y_ + MAINMENU_BUTTON_SPACE * 2);

	SubscribeToEvent(graphics, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleGraphicsPressed));
	SubscribeToEvent(sound, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSoundsPressed));
	SubscribeToEvent(control, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleControlPressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleMasterMenu));
}

void MainMenu::HandleGraphicsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_GRAPHICS;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);
	m_window_menu->SetSize(800, 600);

	SetTitle("Graphics");

	Text *text_full_screen = new Text(context_);
	m_window_menu->AddChild(text_full_screen);
	text_full_screen->SetPosition(MAINMENU_BUTTON_SPACE * 2, 30 + (MAINMENU_BUTTON_SPACE * 2));
	text_full_screen->SetStyle("Text");
	text_full_screen->SetText(m_l10n->Get("Full screen"));

	CheckBox *full_screen = new CheckBox(context_);
	m_window_menu->AddChild(full_screen);
	full_screen->SetStyleAuto();
	full_screen->SetPosition(text_full_screen->GetPosition().x_ + 150,
		30 + (MAINMENU_BUTTON_SPACE * 2));
	full_screen->SetName("CheckBox");
	full_screen->SetChecked(m_config->getBool(BSETTING_FULLSCREEN));

	Text *text_resolution = new Text(context_);
	m_window_menu->AddChild(text_resolution);
	text_resolution->SetPosition(MAINMENU_BUTTON_SPACE * 2, full_screen->GetPosition().y_ + full_screen->GetSize().y_ + MAINMENU_BUTTON_SPACE);
	text_resolution->SetStyle("Text");
	text_resolution->SetText(m_l10n->Get("Resolution"));

	DropDownList *resolution = new DropDownList(context_);
	m_window_menu->AddChild(resolution);
	resolution->SetStyleAuto();
	resolution->SetPosition(text_resolution->GetPosition().x_ + 150, text_resolution->GetPosition().y_);
	resolution->SetName(MAINMENU_ELEMENT_RESOLUTION);

	PODVector<IntVector2> list_coord = GetSubsystem<Graphics>()->GetResolutions();
	for (const auto &coord: list_coord) {
		Text *text = new Text(context_);
		resolution->AddItem(text);
		text->SetStyle("Text");
		text->SetText(Urho3D::ToString("%d x %d",coord.x_, coord.y_));
	}

	Button *apply = CreateMainMenuButton("Apply", "ButtonInLine", "TextButtonInLine");
	apply->SetPosition(0 + 100, -MAINMENU_BUTTON_SPACE * 2);
	apply->SetHorizontalAlignment(HA_LEFT);
	apply->SetVerticalAlignment(VA_BOTTOM);

	Button *back = CreateMainMenuButton("Back", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 100, -MAINMENU_BUTTON_SPACE * 2);
	back->SetHorizontalAlignment(HA_RIGHT);
	back->SetVerticalAlignment(VA_BOTTOM);

	SubscribeToEvent(apply, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleApplyGraphicsPressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(full_screen, E_TOGGLED, URHO3D_HANDLER(MainMenu, HandleFullScreenPressed));
}

void MainMenu::HandleFullScreenPressed(StringHash, VariantMap &eventData)
{
	m_config->setBool(BSETTING_FULLSCREEN, !m_config->getBool(BSETTING_FULLSCREEN));
}

void MainMenu::HandleApplyGraphicsPressed(StringHash, VariantMap &eventData)
{
	if ((GetSubsystem<Graphics>()->GetFullscreen() && !m_config->getBool(BSETTING_FULLSCREEN)) ||
		(!GetSubsystem<Graphics>()->GetFullscreen() && m_config->getBool(BSETTING_FULLSCREEN)))  {
		GetSubsystem<Graphics>()->ToggleFullscreen();
	}

	DropDownList *resolution = static_cast<DropDownList *>(m_window_menu->GetChild(MAINMENU_ELEMENT_RESOLUTION, true));

	PODVector<IntVector2> list_coord = GetSubsystem<Graphics>()->GetResolutions();
	uint32_t selection_id = resolution->GetSelection();
	if (selection_id >= 0 && selection_id < list_coord.Size()) {
		Urho3D::IntVector2 coord = list_coord.At(selection_id);
		GetSubsystem<Graphics>()->SetMode(coord.x_, coord.y_);
		m_config->setU32(U32SETTING_WINDOW_WIDTH, coord.x_);
		m_config->setU32(U32SETTING_WINDOW_HEIGHT, coord.y_);
	}
}

void MainMenu::HandleSoundsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_SOUND;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);
	m_window_menu->SetSize(800, 600);

	SetTitle("Sound");

	// master sound
	Slider *slider_sound_master = CreateSliderWithLabels("Master", "Sound Master: ", MAINMENU_BUTTON_SPACE * 2,
		MAINMENU_BUTTON_SPACE, FLOATSETTINGS_SOUND_MASTER_GAIN);

	// sound music
	Slider *slider_sound_music = CreateSliderWithLabels("Music", "Sound Music: ", MAINMENU_BUTTON_SPACE * 2,
		slider_sound_master->GetPosition().y_ + slider_sound_master->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_MUSIC_GAIN);

	// sound effect
	Slider *slider_sound_effect = CreateSliderWithLabels("Effect", "Sound Effect: ", MAINMENU_BUTTON_SPACE * 2,
		slider_sound_music->GetPosition().y_ + slider_sound_music->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_EFFECT_GAIN);

	// sound ambient
	Slider *slider_sound_ambient = CreateSliderWithLabels("Ambient", "Sound Ambient: ", MAINMENU_BUTTON_SPACE * 2,
		slider_sound_effect->GetPosition().y_ + slider_sound_effect->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_AMBIENT_GAIN);

	// sound voice
	Slider *slider_sound_voice = CreateSliderWithLabels("Voice", "Sound Voice: ", MAINMENU_BUTTON_SPACE * 2,
		slider_sound_ambient->GetPosition().y_ + slider_sound_ambient->GetSize().y_ + MAINMENU_BUTTON_SPACE,
		FLOATSETTINGS_SOUND_VOICE_GAIN);

	Button *back = CreateMainMenuButton("Back", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0, -MAINMENU_BUTTON_SPACE * 2);
	back->SetHorizontalAlignment(HA_CENTER);
	back->SetVerticalAlignment(VA_BOTTOM);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
	SubscribeToEvent(slider_sound_master, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_music, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_effect, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_ambient, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
	SubscribeToEvent(slider_sound_voice, E_DRAGMOVE, URHO3D_HANDLER(MainMenu, HandleSoundsVolume));
}

void MainMenu::HandleControlPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_KEYBOARD;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);
	m_window_menu->SetSize(800, 600);

	SetTitle("Control");

	Button *apply = CreateMainMenuButton("Apply", "ButtonInLine", "TextButtonInLine");
	apply->SetPosition(0 + 100, -MAINMENU_BUTTON_SPACE * 2);
	apply->SetHorizontalAlignment(HA_LEFT);
	apply->SetVerticalAlignment(VA_BOTTOM);

	Button *back = CreateMainMenuButton("Back", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 100, -MAINMENU_BUTTON_SPACE * 2);
	back->SetHorizontalAlignment(HA_RIGHT);
	back->SetVerticalAlignment(VA_BOTTOM);

	SubscribeToEvent(apply, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleApplyGraphicsPressed));
	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
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
	static_cast<LineEdit *>(m_window_menu->GetChild(MAINMENU_ELEMENT_CREATE_UNIVERSE_SEED, true))->SetText(seed_str);
}

void MainMenu::HandleUpdate(StringHash, VariantMap &eventData)
{
	m_menu_background->SetSize(m_ui_elem->GetSize().x_, m_ui_elem->GetSize().y_);

	if (m_enable_error_bubble_timer &&
		m_error_bubble_timer->GetMSec(false) >= m_config->getFloat(FLOATSETTINGS_TIMER_ERROR_BUBBLE)) {
		if (Window *error_bubble_window = dynamic_cast<Window *>(
			m_window_menu->GetChild(MAINMENU_ELEMENT_ERROR_WINDOW_BUBBLE, true))) {
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

	Button *b = static_cast<Button *>(m_window_menu->GetChild(MAINMENU_ELEMENT_DELETE_UNIVERSE, false));
	b->SetStyle("ButtonInLine");

	String universe_name = lv->GetSelectedItem()->GetName();
	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir(
			"spacel", "universe") + universe_name;

	engine::DatabaseSQLite3 game_database(std::string(path_universe.CString()));
	game_database.LoadUniverse(std::string(universe_name.CString()));

	UpdateUniverseInfos(engine::Universe::instance()->GetUniverseBirth(),
		engine::Universe::instance()->GetUniverseSeed());
}

void MainMenu::HandleDeleteUniversePressed(StringHash, VariantMap &eventData)
{
	if (eventData[MessageACK::P_OK].GetBool()) {
		ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild(MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW, true));
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

			if (GetSubsystem<FileSystem>()->Delete(path_universe)) {
				ShowErrorBubble(ToString(m_l10n->Get("Universe %s deleted").CString(), universe_name.CString()));
				URHO3D_LOGINFOF("Universe %s deleted", universe_name.CString());
			}
		}

		lv->RemoveItem(lv->GetSelectedItem());
	}

	UpdateUniverseInfos();

	Button *b = static_cast<Button *>(m_window_menu->GetChild(MAINMENU_ELEMENT_DELETE_UNIVERSE, false));
	b->SetStyle("ButtonInLineInactive");

	if (m_modal_window != nullptr) {
		m_modal_window->UnsubscribeFromEvent(E_MESSAGEACK);
		m_modal_window->UnsubscribeFromEvent(E_MODALCHANGED);
		m_modal_window = nullptr;
	}

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));
}

void MainMenu::HandleDeleteUniverse(StringHash, VariantMap &eventData)
{
	ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild(MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW, true));
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

	Window *error_bubble_window = dynamic_cast<Window *>(m_window_menu->GetChild(MAINMENU_ELEMENT_ERROR_WINDOW_BUBBLE, true));
	if (!error_bubble_window) {
		error_bubble_window = new Window(context_);
		m_window_menu->AddChild(error_bubble_window);
		error_bubble_window->SetStyle("ErrorBubble");
		error_bubble_window->SetName(MAINMENU_ELEMENT_ERROR_WINDOW_BUBBLE);
	}
	static_cast<Text *>(m_window_menu->GetChild(MAINMENU_ELEMENT_ERROR_BUBBLE_TEXT, true))->SetText(message);
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
	le->SetPosition(x, y);
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
	return s;
}

Slider *MainMenu::CreateSliderWithLabels(const String &name, const String &label,
										 const int x, const int y, const int setting)
{
	Text *text_sound = CreateText(label);
	text_sound->SetPosition(x, y);
	m_window_menu->AddChild(text_sound);

	Text *value_sound = CreateText(Urho3D::ToString("%f %%", floor(m_config->getFloat(setting))), "value" + name);
	value_sound->SetPosition(text_sound->GetWidth() + 50, text_sound->GetPosition().y_);
	m_window_menu->AddChild(value_sound);

	Slider *slider_sound = CreateSlider(name, m_config->getFloat(setting));
	m_window_menu->AddChild(slider_sound);
	slider_sound->SetPosition(-MAINMENU_BUTTON_SPACE * 2, text_sound->GetPosition().y_);
	slider_sound->SetHorizontalAlignment(HA_RIGHT);

	return slider_sound;
}

void MainMenu::UpdateUniverseInfos(const uint32_t &birth, const uint64_t &seed)
{
	ListView *lv = dynamic_cast<ListView *>(m_window_menu->GetChild(MAINMENU_ELEMENT_LOAD_UNIVERSE_LISTVIEW, true));
	assert(lv);

	String birth_str = (birth ? ToString("%s",
		timestamp_to_string(birth).c_str()) : String::EMPTY);
	String seed_str = (seed ? ToString("%s",
		std::to_string(seed).c_str()) : String::EMPTY);

	m_window_menu->AddChild(m_universe_infos);
	m_universe_infos->SetStyle("TextUnivInfo");
	m_universe_infos->SetPosition(lv->GetSize().x_ + 50, m_window_menu->GetPosition().y_ + 250);
	m_universe_infos->SetText(m_l10n->Get("Universe age: ") + birth_str + "\n" +
		m_l10n->Get("Seed: ") + seed_str);

}
}
