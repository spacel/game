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

#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#include <project_defines.h>
#include "spacelgame.h"

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

MainMenu::MainMenu(Context *context, ClientSettings *config, SpacelGame *main) :
		GenericMenu(context, config),
		m_main(main),
		m_enable_menu_music(m_config->getBool(BSETTING_ENABLE_MUSIC))
{
	m_ui_elem = GetSubsystem<UI>()->GetRoot();
	m_ui_elem->SetDefaultStyle(m_cache->GetResource<XMLFile>("UI/MainMenuStyle.xml"));
	m_window_menu = new Window(context_);
	m_title = new Text(context_);
	m_music_button = new Button(context_);
	m_error_bubble_timer  = new Timer();
	except_unsubscribe.Push("E_KEYDOWN");
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
	m_music_button->SetStyle(m_enable_menu_music ? "SoundButton": "SoundButtonOff");
	PlayMusic(m_enable_menu_music);
}

void MainMenu::Background()
{
	Texture2D *logoTexture = m_cache->GetResource<Texture2D>("Textures/cwd_up.png");

	if (!logoTexture) {
		URHO3D_LOGERROR("Background texture not loaded");
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

	SubscribeToEvent(E_UIMOUSECLICK, URHO3D_HANDLER(MainMenu, HandleControlClicked));
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
		case KEY_ESC:
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
				default: break;
			}
			break;
		case KEY_F12:
			TakeScreenshot();
			break;
		default:
			break;
	}
}

void MainMenu::HandleControlClicked(StringHash, VariantMap &eventData)
{
	// Get control that was clicked
	//UIElement *clicked = static_cast<UIElement *>(eventData[UIMouseClick::P_ELEMENT].GetPtr());
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

	SubscribeToEvent(newgame, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleNewGamePressed ));
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

	CreateMainMenuLineEdit("seed", "Seed : ", 0, universename->GetPosition().y_ + universename->GetSize().y_ + MAINMENU_BUTTON_SPACE);

	Button *generateSeed = CreateMainMenuButton("Generate seed", "ButtonInLine", "TextButtonInLine");
	generateSeed->SetPosition( -20,  universename->GetPosition().y_ + universename->GetSize().y_ + MAINMENU_BUTTON_SPACE);
	generateSeed->SetHorizontalAlignment(HA_RIGHT);

	Button *create = CreateMainMenuButton("Create", "ButtonInLine", "TextButtonInLine");
	create->SetPosition(0 + 50,  m_window_menu->GetSize().y_ - create->GetSize().y_ - MAINMENU_BUTTON_SPACE);
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
	bool universe_creation = false;

	// Universe creation
	LineEdit *le = static_cast<LineEdit *>(m_window_menu->GetChild("universe_name", true));
	if (le != nullptr) {
		universe_name = le->GetText().Replaced(':', '_').Replaced('.', '_')
			.Replaced(' ', '_');

		universe_creation = true;
	}
	else {
		// @TODO find a way to retrieve the universe_name selected for loading
	}

	const String path_universe = GetSubsystem<FileSystem>()->GetAppPreferencesDir(
			"spacel", "universe") + universe_name;

	if (universe_creation) {
		if (!universe_name.Empty()) {
			if (GetSubsystem<FileSystem>()->DirExists(path_universe)) {
				URHO3D_LOGERRORF("Universe %s already exists", universe_name.CString());
				//TODO : Send parameter variables
				ShowErrorBubble(
						Urho3D::ToString("Universe %s already exists", universe_name.CString()));
			}
		}
		else {
			ShowErrorBubble("Universe name is empty");
			URHO3D_LOGERROR("Universe name is empty");
			return;
		}
	}
	else {
		ShowErrorBubble("You must select a universe.");
		URHO3D_LOGERROR("No universe selected when loading.");
		return;
	}

	if (!GetSubsystem<FileSystem>()->CreateDir(path_universe)) {
		GetSubsystem<FileSystem>()->CreateDir(path_universe);
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

	m_universes_listview = new ListView(context_);
	m_window_menu->AddChild(m_universes_listview);
	m_universes_listview->SetStyle("ListView");
	m_universes_listview->SetName("list_view_universe");
	m_universes_listview->SetMultiselect(false);
	m_universes_listview->SetHighlightMode(HM_ALWAYS);

	// TODO: WIP
	if (!list_universe.Empty()) {
		for (Vector<String>::Iterator it = list_universe.Begin() ; it != list_universe.End(); ++it) {
			if (it->Compare(".") != 0 && it->Compare("..") != 0) {
				Text *text = new Text(context_);
				m_universes_listview->AddItem(text);
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
	m_preview->SetPosition(-m_preview->GetSize().x_ -35, m_window_menu->GetPosition().y_ + 15);
	m_preview->SetHorizontalAlignment(HA_RIGHT);
	m_preview->SetPriority(-90);

	// TODO: Information universe (WIP)
	Text *text = new Text(context_);
	m_window_menu->AddChild(text);
	text->SetStyle("Text");
	text->SetPosition(-text->GetSize().x_ -35, m_window_menu->GetPosition().y_ + m_preview->GetSize().y_ + 25);
	text->SetHorizontalAlignment(HA_RIGHT);
	text->SetSize(20, 50);
	text->SetText("Age du serveur : ");


	Button *launch = CreateMainMenuButton("Launch", "ButtonInLine", "TextButtonInLine");
	launch->SetPosition(0 + 50,  m_window_menu->GetSize().y_ - launch->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	launch->SetHorizontalAlignment(HA_LEFT);

	Button *back = CreateMainMenuButton("Cancel", "ButtonInLine", "TextButtonInLine");
	back->SetPosition(0 - 50, m_window_menu->GetSize().y_ - back->GetSize().y_ - MAINMENU_BUTTON_SPACE);
	back->SetHorizontalAlignment(HA_RIGHT);

	//SubscribeToEvent(listview_univers, E_ITEMSELECTED, URHO3D_HANDLER(MainMenu, HandleUniversSelectionItemClick));
	SubscribeToEvent(m_universes_listview, E_ITEMCLICKED, URHO3D_HANDLER(MainMenu, HandleUniversSelectionItemClick));
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
	back->SetPosition(0,  back->GetSize().y_ + 65);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
}

void MainMenu::HandleSoundsPressed(StringHash, VariantMap &eventData)
{
	m_menu_id = MAINMENUID_SETTINGS_SOUND;
	m_window_menu->RemoveAllChildren();
	UnsubscribeFromAllEventsExcept(except_unsubscribe, true);

	SetTitle("Sound");

	Button *back = CreateMainMenuButton("Back");
	back->SetPosition(0, back->GetSize().y_ + 65);

	SubscribeToEvent(back, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleSettingsPressed));
}

void MainMenu::HandleGenerateSeedPressed(StringHash eventType, VariantMap &eventData)
{
	String seed_str = std::to_string(Rand()).c_str();
	static_cast<LineEdit *>(m_window_menu->GetChild("Seed : ", true))->SetText(seed_str);
}

void MainMenu::HandleUpdate(StringHash, VariantMap &eventData)
{
	m_menu_background->SetSize(GetSubsystem<UI>()->GetRoot()->GetSize().x_,
		GetSubsystem<UI>()->GetRoot()->GetSize().y_);

	if (m_enable_error_bubble_timer &&
			m_error_bubble_timer->GetMSec(false) >= m_config->getFloat(FLOATSETTINGS_TIMER_ERROR_BUBBLE)) {
		if (Window * error_bubble_window = dynamic_cast<Window *>(
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
	m_music_button->SetStyle(m_enable_menu_music ? "SoundButton": "SoundButtonOff");
	PlayMusic(m_enable_menu_music);
}

void MainMenu::HandleUniversSelectionItemClick(StringHash, VariantMap &eventData)
{
	m_universes_listview->SetSelection(eventData["Selection"].GetUInt());
}

void MainMenu::ShowErrorBubble(const String &message, ...)
{
	m_enable_error_bubble_timer = true;
	m_error_bubble_timer->Reset();

	Window * error_bubble_window = dynamic_cast<Window *>(m_window_menu->GetChild("error_window_bubble", true));
	if (!error_bubble_window) {
		error_bubble_window = new Window(context_);
		error_bubble_window->SetStyle("ErrorBubble");
		error_bubble_window->SetName("error_window_bubble");
		error_bubble_window->SetPosition(0, 15);
		m_window_menu->AddChild(error_bubble_window);

		Text *error_txt = new Text(context_);
		error_txt->SetStyle("ErrorBubbleText");
		error_txt->SetName("error_bubble_text");
		error_txt->SetText(m_l10n->Get(message));
		error_bubble_window->AddChild(error_txt);
	} else {
		static_cast<Text *>(m_window_menu->GetChild("error_bubble_text", true))->SetText(message);
	}
	error_bubble_window->SetVisible(true);
}

inline void MainMenu::SetTitle(const String &t)
{
	m_title->SetText(m_l10n->Get(t));
}

Button *MainMenu::CreateMainMenuButton(const String &label, const String &button_style,
		const String &label_style)
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
}
