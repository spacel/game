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

#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/MessageBox.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Slider.h>

#include <common/engine/databases/database-sqlite3.h>
#include "genericmenu.h"
#include "spacelgame.h"
#include "ui/ModalWindow.h"

using namespace Urho3D;

namespace spacel {

class SpacelGame;

class MainMenu: public GenericMenu  {
URHO3D_OBJECT(MainMenu, GenericMenu);

public:
	MainMenu(Context *context, ClientSettings *config, SpacelGame *main);
	~MainMenu();
	void Start();

private:
	// Handlers
	void HandleClosePressed(StringHash eventType, VariantMap &eventData);
	void HandleKeyDown(StringHash eventType, VariantMap &eventData);
	void HandleLaunchGamePressed(StringHash eventType, VariantMap &eventData);
	void HandleSingleplayerPressed(StringHash eventType, VariantMap &eventData);
	void HandleNewGamePressed(StringHash, VariantMap &eventData);
	void HandleLoadGamePressed(StringHash, VariantMap &eventData);
	void HandleMultiplayerPressed(StringHash, VariantMap &eventData);
	void HandleSettingsPressed(StringHash eventType, VariantMap &eventData);
	void HandleGraphicsPressed(StringHash eventType, VariantMap &eventData);
	void HandleApplyGraphicsPressed(StringHash eventType, VariantMap &eventData);
	void HandleSoundsPressed(StringHash eventType, VariantMap &eventData);
	void HandleFullScreenPressed(StringHash eventType, VariantMap &eventData);
	void HandleSoundsVolume(StringHash eventType, VariantMap &eventData);
	void HandleControlPressed(StringHash eventType, VariantMap &eventData);
	void HandleUpdate(StringHash eventType, VariantMap &eventData);
	void HandleMusicPressed(StringHash eventType, VariantMap &eventData);
	void HandleInfosUniverseClicked(StringHash, VariantMap &eventData);
	void HandleDeleteUniversePressed(StringHash eventType, VariantMap &eventData);
	void HandleGenerateSeedPressed(StringHash eventType, VariantMap &eventData);
	void HandleDeleteUniverse(StringHash eventType, VariantMap &eventData);
	void Background();
	void Title();
	void HandleMasterMenu(StringHash, VariantMap &);
	void ShowErrorBubble(const String &message);
	void UpdateUniverseInfos(const uint32_t &birth = 0, const uint64_t &seed = 0);

	// Helpers
	Button *CreateMainMenuButton(const String &label,
			const String &button_style = "Button",
			const String &label_style = "TextButton");
	Urho3D::LineEdit *CreateMainMenuLineEdit(const String &name,
			const String &label,
			const int x,
			const int y);
	Text *CreateText(const String &text, const String &name = "Text",
			const String &style = "Text");
	Slider *CreateSlider(const String &name,
			const float value = 100, const float range = 100, const String &style = "Slider");
	Slider *CreateSliderWithLabels(const String &name, const String &label,
			const int x, const int y, const int SETTING);
	void SetTitle(const String &t);

	// Attributes
	SpacelGame *m_main;
	uint8_t m_menu_id = 0;
	SharedPtr<UIElement> m_ui_elem;
	SharedPtr<Window> m_window_menu;
	SharedPtr<Text> m_title;
	SharedPtr<Text> m_universe_infos;
	SharedPtr<Button> m_music_button;
	SharedPtr<UIElement> m_modal_window;
	bool m_enable_menu_music;
	Timer *m_error_bubble_timer;
	bool m_enable_error_bubble_timer = false;
	SharedPtr<Sprite> m_menu_background;
	SharedPtr<Image> m_preview_image;
	SharedPtr<Sprite> m_preview;
	PODVector<StringHash> except_unsubscribe;
};

}
